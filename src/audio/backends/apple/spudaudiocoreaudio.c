#if SPUDAUDIO_COMPILE_COREAUDIO

/*
 * CoreAudio backend for macOS, on the HAL (AudioObject* / AudioDevice*).
 *
 * The HAL is macOS-only: iOS, iPadOS and watchOS have no AudioObject API
 * and get their own backend later (AVAudioSession + RemoteIO), sharing the
 * platform-neutral pieces in spudaudioapple.{h,c}. This whole file is
 * SPUDAUDIO_COMPILE_COREAUDIO, which spudaudio.h only allows with
 * SPUDAUDIO_PLATFORM_MACOS.
 *
 * Model:
 * - A SpudAudio device is a HAL device in one direction (scope): one HAL
 *   device with both input and output streams enumerates once per
 *   direction, under the same UID.
 * - Streams are HAL IOProcs. The HAL's own real-time thread calls them, so
 *   the thread priority must be SPUDAUDIO_THREAD_PRIORITY_PLATFORM.
 * - Clients see each device stream's virtual format, which for mixable
 *   streams is always Float32 at the device's nominal rate. So F32 is the
 *   only sample format, the rate is the device's (spudaudio_set_device_
 *   sample_rate changes it), and the channel count is the device's.
 *   ALLOW_OS_CONVERSION isn't built yet — that needs an AudioConverter or
 *   AUHAL — so the flag is refused with NOT_IMPLEMENTED_YET rather than
 *   ignored.
 * - A device can expose several HAL streams in one direction (a buffer
 *   each in the IOProc's AudioBufferList); they're presented as one
 *   SpudAudio channel range, rearranged (never converted) through a
 *   scratch buffer.
 * - The period is kAudioDevicePropertyBufferFrameSize, which the HAL keeps
 *   per process per device: two SpudAudio streams on the same device in
 *   one process share it.
 * - EXCLUSIVE is hog mode.
 */

#include "spudaudioapple.h"

#include <Block.h>
#include <CoreAudio/CoreAudio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <dispatch/dispatch.h>
#include <math.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if __cplusplus
extern "C" {
#endif

struct spudaudio_device_t {
#if _DEBUG
	const char *debug_name;
#endif
	AudioObjectID id;
	SPUDAUDIO_DIRECTION direction;
	char persistent_id[256]; // kAudioDevicePropertyDeviceUID
	struct spudaudio_device_t *next; // instance-owned list
};

struct spudaudio_instance_t {
#if _DEBUG
	const char *debug_name;
#endif
	struct spudaudio_device_t *devices; // every device ever enumerated
	spudaudio_device *enumerated[2];    // latest array per direction
};

struct spudaudio_stream_t {
#if _DEBUG
	const char *debug_name;
#endif
	AudioObjectID device;
	SPUDAUDIO_DIRECTION direction;
	SPUDAUDIO_STREAM_CONFIG config;
	SPUDAUDIO_STREAM_CALLBACK callback;
	void *user_data;
	AudioDeviceIOProcID proc;
	bool hogged;
	mach_timebase_info_data_t timebase;
	float *scratch;   // max_callback_frames x channels
	float **planes;   // planar: one pointer per channel into scratch

	// Device-change listeners (alive, nominal rate) run on this private
	// serial queue, so destroy can drain it after removing them.
	dispatch_queue_t listener_queue;
	AudioObjectPropertyListenerBlock listener;
	bool listening;

	atomic_uint state; // SPUDAUDIO_STREAM_STATE
	atomic_int error;  // SPUDRESULT
	atomic_uint_least64_t underflow_count;
	atomic_uint_least64_t overflow_count;

	// Which thread is inside the callback right now, for the start/stop
	// re-entrancy guard. The HAL's IOProc thread isn't known in advance.
	atomic_bool in_callback;
	pthread_t callback_thread;

	// IOProc-only state.
	bool have_origin;
	double origin_sample_time;   // device sample time of the first frame since start
	double expected_sample_time; // where the next buffer should start
	uint64_t position_frames;    // fallback when a timestamp has no sample time
};

// --------------------------------------------------------------------------
// HAL helpers
// --------------------------------------------------------------------------

static AudioObjectPropertyScope ca_scope(SPUDAUDIO_DIRECTION d) {
	return d == SPUDAUDIO_DIRECTION_OUTPUT ? kAudioObjectPropertyScopeOutput : kAudioObjectPropertyScopeInput;
}

static OSStatus ca_get(AudioObjectID obj, AudioObjectPropertySelector sel, AudioObjectPropertyScope scope, UInt32 size, void *out) {
	AudioObjectPropertyAddress a = {sel, scope, kAudioObjectPropertyElementMain};
	return AudioObjectGetPropertyData(obj, &a, 0, NULL, &size, out);
}

static OSStatus ca_set(AudioObjectID obj, AudioObjectPropertySelector sel, AudioObjectPropertyScope scope, UInt32 size, const void *in) {
	AudioObjectPropertyAddress a = {sel, scope, kAudioObjectPropertyElementMain};
	return AudioObjectSetPropertyData(obj, &a, 0, NULL, size, in);
}

// Variable-size property, malloc'd; NULL if absent or empty.
static void *ca_get_alloc(AudioObjectID obj, AudioObjectPropertySelector sel, AudioObjectPropertyScope scope, UInt32 *out_size) {
	AudioObjectPropertyAddress a = {sel, scope, kAudioObjectPropertyElementMain};
	UInt32 size                  = 0;
	if (!AudioObjectHasProperty(obj, &a) || AudioObjectGetPropertyDataSize(obj, &a, 0, NULL, &size) != noErr || size == 0)
		return NULL;
	void *data = malloc(size);
	if (data && AudioObjectGetPropertyData(obj, &a, 0, NULL, &size, data) != noErr) {
		free(data);
		data = NULL;
	}
	if (out_size)
		*out_size = size;
	return data;
}

static void ca_get_string(AudioObjectID obj, AudioObjectPropertySelector sel, char *out, size_t out_size) {
	out[0]       = '\0';
	CFStringRef s = NULL;
	if (ca_get(obj, sel, kAudioObjectPropertyScopeGlobal, sizeof(CFStringRef), &s) == noErr && s) {
		CFStringGetCString(s, out, (CFIndex)out_size, kCFStringEncodingUTF8);
		CFRelease(s);
	}
}

static SPUDRESULT ca_result(OSStatus status) {
	switch (status) {
	case noErr:
		return SPUD_SUCCESS;
	case kAudioHardwareBadDeviceError:
	case kAudioHardwareBadObjectError:
	case kAudioHardwareBadStreamError:
		return SPUDRESULT_SAUD_DEVICE_LOST;
	case kAudioDevicePermissionsError: // another process holds hog mode
		return SPUDRESULT_SAUD_EXCLUSIVE_UNAVAILABLE;
	case kAudioHardwareIllegalOperationError:
	case kAudioDeviceUnsupportedFormatError:
		return SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED;
	default:
		return SPUDRESULT_API_SPECIFIC_FAILURE;
	}
}

// Total channels in one direction, summed over the device's HAL streams,
// and whether they arrive as a single interleaved buffer.
static uint32_t ca_channel_count(AudioObjectID dev, SPUDAUDIO_DIRECTION dir, bool *out_single_buffer) {
	AudioBufferList *list = (AudioBufferList *)ca_get_alloc(dev, kAudioDevicePropertyStreamConfiguration, ca_scope(dir), NULL);
	uint32_t channels     = 0;
	if (list) {
		for (UInt32 b = 0; b < list->mNumberBuffers; ++b)
			channels += list->mBuffers[b].mNumberChannels;
		if (out_single_buffer)
			*out_single_buffer = list->mNumberBuffers == 1;
		free(list);
	}
	return channels;
}

static uint32_t ca_nominal_rate(AudioObjectID dev) {
	Float64 rate = 0;
	ca_get(dev, kAudioDevicePropertyNominalSampleRate, kAudioObjectPropertyScopeGlobal, sizeof(rate), &rate);
	return (uint32_t)lround(rate);
}

// The device's own format in one direction, as clients see it (the HAL
// virtual format: Float32 at the nominal rate), with its preferred channel
// layout as positions.
static void ca_device_format(AudioObjectID dev, SPUDAUDIO_DIRECTION dir, SPUDAUDIO_FORMAT *out) {
	memset(out, 0, sizeof(*out));
	bool single         = true;
	out->sample_format  = SPUDAUDIO_SAMPLE_FORMAT_F32;
	out->sample_rate    = ca_nominal_rate(dev);
	out->channel_count  = ca_channel_count(dev, dir, &single);
	out->interleaved    = single || out->channel_count == 1;
	if (out->channel_count > SPUDAUDIO_MAX_CHANNELS)
		out->channel_count = SPUDAUDIO_MAX_CHANNELS;
	AudioChannelLayout *layout = (AudioChannelLayout *)ca_get_alloc(dev, kAudioDevicePropertyPreferredChannelLayout, ca_scope(dir), NULL);
	spudaudio_apple_layout_to_positions(layout, out);
	free(layout);
}

static void ca_sleep_ms(long ms) {
	struct timespec t = {ms / 1000, (ms % 1000) * 1000000L};
	nanosleep(&t, NULL);
}

// --------------------------------------------------------------------------
// Instance / devices
// --------------------------------------------------------------------------

SPUDRESULT spudaudio_create_instance(
    SPUDAUDIO_NATIVE_API api,
    spudaudio_instance *out_instance) {
	if (!out_instance)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	*out_instance = NULL;
	if (api != SPUDAUDIO_NATIVE_API_COREAUDIO)
		return SPUDRESULT_INVALID_API;
	struct spudaudio_instance_t *inst = (struct spudaudio_instance_t *)calloc(1, sizeof(*inst));
	if (!inst)
		return SPUDRESULT_OUT_OF_MEMORY;
	*out_instance = inst;
	return SPUD_SUCCESS;
}

SPUDRESULT spudaudio_destroy_instance(spudaudio_instance instance) {
	if (!instance)
		return SPUDRESULT_SAUD_INVALID_INSTANCE;
	struct spudaudio_device_t *dev = instance->devices;
	while (dev) {
		struct spudaudio_device_t *next = dev->next;
		free(dev);
		dev = next;
	}
	free(instance->enumerated[0]);
	free(instance->enumerated[1]);
	free(instance);
	return SPUD_SUCCESS;
}

SPUDAUDIO_NATIVE_API spudaudio_get_native_api(spudaudio_instance instance) {
	return instance ? SPUDAUDIO_NATIVE_API_COREAUDIO : SPUDAUDIO_NATIVE_API_NONE;
}

// Stable handles across enumerations: keyed by UID + direction.
static struct spudaudio_device_t *ca_find_or_add_device(
    spudaudio_instance instance,
    AudioObjectID id,
    const char *uid,
    SPUDAUDIO_DIRECTION direction) {
	for (struct spudaudio_device_t *d = instance->devices; d; d = d->next)
		if (d->direction == direction && strcmp(d->persistent_id, uid) == 0) {
			d->id = id; // the HAL may renumber a device that came back
			return d;
		}
	struct spudaudio_device_t *d = (struct spudaudio_device_t *)calloc(1, sizeof(*d));
	if (!d)
		return NULL;
	d->id        = id;
	d->direction = direction;
	strncpy(d->persistent_id, uid, sizeof(d->persistent_id) - 1);
	d->next           = instance->devices;
	instance->devices = d;
	return d;
}

SPUDRESULT spudaudio_enumerate_devices(
    spudaudio_instance instance,
    SPUDAUDIO_DIRECTION direction,
    spudaudio_device **out_devices,
    uint32_t *out_device_count) {
	if (!out_devices || !out_device_count)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	*out_devices      = NULL;
	*out_device_count = 0;
	if (!instance)
		return SPUDRESULT_SAUD_INVALID_INSTANCE;
	if (direction != SPUDAUDIO_DIRECTION_OUTPUT && direction != SPUDAUDIO_DIRECTION_INPUT)
		return SPUDRESULT_DESC_INVALID_PARAMETERS;

	UInt32 size        = 0;
	AudioObjectID *ids = (AudioObjectID *)ca_get_alloc(kAudioObjectSystemObject, kAudioHardwarePropertyDevices, kAudioObjectPropertyScopeGlobal, &size);
	uint32_t count     = ids ? size / (uint32_t)sizeof(AudioObjectID) : 0;
	spudaudio_device *list = count ? (spudaudio_device *)calloc(count, sizeof(spudaudio_device)) : NULL;
	if (count && !list) {
		free(ids);
		return SPUDRESULT_OUT_OF_MEMORY;
	}

	uint32_t written = 0;
	SPUDRESULT r     = SPUD_SUCCESS;
	for (uint32_t i = 0; i < count; ++i) {
		if (ca_channel_count(ids[i], direction, NULL) == 0)
			continue; // no streams in this direction
		char uid[256];
		ca_get_string(ids[i], kAudioDevicePropertyDeviceUID, uid, sizeof(uid));
		struct spudaudio_device_t *d = ca_find_or_add_device(instance, ids[i], uid, direction);
		if (!d) {
			r = SPUDRESULT_OUT_OF_MEMORY;
			break;
		}
		list[written++] = d;
	}
	free(ids);
	if (SPUDFAIL(r)) {
		free(list);
		return r;
	}
	free(instance->enumerated[direction]);
	instance->enumerated[direction] = list;
	*out_devices                    = list;
	*out_device_count               = written;
	return SPUD_SUCCESS;
}

SPUDRESULT spudaudio_get_device_properties(
    spudaudio_device device,
    SPUDAUDIO_DEVICE_PROPERTIES *out_properties) {
	if (!out_properties)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	memset(out_properties, 0, sizeof(*out_properties));
	if (!device)
		return SPUDRESULT_SAUD_INVALID_DEVICE;

	UInt32 alive = 0;
	if (ca_get(device->id, kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal, sizeof(alive), &alive) != noErr || !alive)
		return SPUDRESULT_SAUD_DEVICE_LOST;

	ca_get_string(device->id, kAudioObjectPropertyName, out_properties->name, sizeof(out_properties->name));
	memcpy(out_properties->persistent_id, device->persistent_id, sizeof(out_properties->persistent_id));
	out_properties->direction          = device->direction;
	out_properties->supports_exclusive = true; // hog mode
	ca_device_format(device->id, device->direction, &out_properties->native_format);

	static const struct {
		AudioObjectPropertySelector selector;
		SPUDAUDIO_DIRECTION direction;
		SPUDAUDIO_DEFAULT_ROLES role;
	} defaults[] = {
	    {kAudioHardwarePropertyDefaultOutputDevice, SPUDAUDIO_DIRECTION_OUTPUT, SPUDAUDIO_DEFAULT_ROLE_GENERAL},
	    {kAudioHardwarePropertyDefaultSystemOutputDevice, SPUDAUDIO_DIRECTION_OUTPUT, SPUDAUDIO_DEFAULT_ROLE_SYSTEM_SOUNDS},
	    {kAudioHardwarePropertyDefaultInputDevice, SPUDAUDIO_DIRECTION_INPUT, SPUDAUDIO_DEFAULT_ROLE_GENERAL},
	};
	for (size_t i = 0; i < sizeof(defaults) / sizeof(defaults[0]); ++i) {
		if (defaults[i].direction != device->direction)
			continue;
		AudioObjectID id = kAudioObjectUnknown;
		if (ca_get(kAudioObjectSystemObject, defaults[i].selector, kAudioObjectPropertyScopeGlobal, sizeof(id), &id) == noErr && id == device->id)
			out_properties->default_roles |= defaults[i].role;
	}
	return SPUD_SUCCESS;
}

/*
 * Device-wide, so explicit. Validated against the device's available rates
 * first; the HAL applies a new nominal rate asynchronously, so this waits
 * (bounded) until the device reports it.
 */
SPUDRESULT spudaudio_set_device_sample_rate(spudaudio_device device, uint32_t sample_rate) {
	if (!device)
		return SPUDRESULT_SAUD_INVALID_DEVICE;
	if (sample_rate == 0)
		return SPUDRESULT_DESC_INVALID_PARAMETERS;

	UInt32 size             = 0;
	AudioValueRange *ranges = (AudioValueRange *)ca_get_alloc(device->id, kAudioDevicePropertyAvailableNominalSampleRates, kAudioObjectPropertyScopeGlobal, &size);
	bool available          = false;
	for (uint32_t i = 0; ranges && i < size / sizeof(AudioValueRange); ++i)
		if (sample_rate >= ranges[i].mMinimum && sample_rate <= ranges[i].mMaximum)
			available = true;
	free(ranges);
	if (!available)
		return SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED;
	if (ca_nominal_rate(device->id) == sample_rate)
		return SPUD_SUCCESS;

	Float64 rate      = sample_rate;
	OSStatus status   = ca_set(device->id, kAudioDevicePropertyNominalSampleRate, kAudioObjectPropertyScopeGlobal, sizeof(rate), &rate);
	if (status != noErr)
		return ca_result(status);
	for (int waited = 0; waited < 2000; waited += 10) {
		if (ca_nominal_rate(device->id) == sample_rate)
			return SPUD_SUCCESS;
		ca_sleep_ms(10);
	}
	return SPUDRESULT_API_SPECIFIC_FAILURE; // the device never reported the new rate
}

// --------------------------------------------------------------------------
// Timing / probe
// --------------------------------------------------------------------------

static bool ca_period_range(AudioObjectID dev, uint32_t *out_min, uint32_t *out_max) {
	AudioValueRange range = {0, 0};
	if (ca_get(dev, kAudioDevicePropertyBufferFrameSizeRange, kAudioObjectPropertyScopeGlobal, sizeof(range), &range) != noErr)
		return false;
	*out_min = (uint32_t)ceil(range.mMinimum);
	*out_max = (uint32_t)floor(range.mMaximum);
	return true;
}

// kAudioDevicePropertyUsesVariableBufferFrameSizes: when present, buffers
// vary slightly and its value is the largest the IOProc will be given.
static uint32_t ca_variable_max_frames(AudioObjectID dev) {
	AudioObjectPropertyAddress a = {kAudioDevicePropertyUsesVariableBufferFrameSizes, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain};
	UInt32 max_frames            = 0;
	if (AudioObjectHasProperty(dev, &a))
		ca_get(dev, a.mSelector, a.mScope, sizeof(max_frames), &max_frames);
	return max_frames;
}

SPUDRESULT spudaudio_get_timing_caps(
    spudaudio_device device,
    const SPUDAUDIO_FORMAT *format,
    SPUDAUDIO_STREAM_FLAGS flags,
    SPUDAUDIO_TIMING_CAPS *out_caps) {
	(void)flags; // hog mode doesn't change the HAL's buffer-size range
	if (!out_caps)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	memset(out_caps, 0, sizeof(*out_caps));
	if (!format)
		return SPUDRESULT_NULL_DESC;
	if (!device)
		return SPUDRESULT_SAUD_INVALID_DEVICE;
	if (!ca_period_range(device->id, &out_caps->min_period_frames, &out_caps->max_period_frames))
		return SPUDRESULT_SAUD_DEVICE_LOST;
	UInt32 current = 0;
	ca_get(device->id, kAudioDevicePropertyBufferFrameSize, kAudioObjectPropertyScopeGlobal, sizeof(current), &current);
	out_caps->period_mode               = SPUDAUDIO_PERIOD_MODE_EXACT;
	out_caps->period_granularity_frames = 1;
	out_caps->default_period_frames     = current;
	out_caps->variable_callback_frames  = ca_variable_max_frames(device->id) != 0;
	// min/max_buffer_frames stay 0: no buffer separate from the period.
	return SPUD_SUCCESS;
}

static SPUDRESULT ca_check_thread(const SPUDAUDIO_THREAD_DESC *t) {
	// The HAL's own real-time IO thread runs the IOProc.
	return t->priority == SPUDAUDIO_THREAD_PRIORITY_PLATFORM ? SPUD_SUCCESS : SPUDRESULT_SAUD_THREAD_PRIORITY_UNSUPPORTED;
}

/*
 * The one check behind probe and create. Format, period and buffer are
 * properties of the device the HAL reports directly, so they're compared
 * exactly here; the suggestion for each is the device's own value. Hog
 * mode is checked, not taken — taking it would silence every other app on
 * the device for the probe's duration; create takes it.
 */
static SPUDRESULT ca_check_desc(const SPUDAUDIO_STREAM_DESC *desc, SPUDAUDIO_STREAM_CONFIG *cfg) {
	const AudioObjectID dev = desc->device->id;
	UInt32 alive            = 0;
	if (ca_get(dev, kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal, sizeof(alive), &alive) != noErr || !alive)
		return SPUDRESULT_SAUD_DEVICE_LOST;

	// Refused, not ignored: ignoring it would answer FORMAT_NOT_SUPPORTED
	// as if conversion had been tried.
	if (desc->flags & SPUDAUDIO_STREAM_FLAG_ALLOW_OS_CONVERSION)
		return SPUDRESULT_NOT_IMPLEMENTED_YET;

	// --- Format.
	SPUDAUDIO_FORMAT device_format;
	ca_device_format(dev, desc->device->direction, &device_format);
	cfg->format = desc->format;
	if (desc->format.sample_format != SPUDAUDIO_SAMPLE_FORMAT_F32) {
		cfg->format.sample_format = SPUDAUDIO_SAMPLE_FORMAT_F32;
		return SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED;
	}
	if (desc->format.sample_rate != device_format.sample_rate) {
		cfg->format.sample_rate = device_format.sample_rate; // or spudaudio_set_device_sample_rate
		return SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED;
	}
	if (desc->format.channel_count != device_format.channel_count) {
		cfg->format.channel_count = device_format.channel_count;
		memcpy(cfg->format.positions, device_format.positions, sizeof(cfg->format.positions));
		return SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED;
	}
	// A layout must be the device's own (its preferred layout is the
	// user's speaker setup in Audio MIDI Setup — not something to change).
	if (spudaudio_format_has_layout(&desc->format) &&
	    memcmp(desc->format.positions, device_format.positions, desc->format.channel_count) != 0) {
		memcpy(cfg->format.positions, device_format.positions, sizeof(cfg->format.positions));
		return SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED;
	}

	// --- Period.
	uint32_t min_period = 0, max_period = 0;
	if (!ca_period_range(dev, &min_period, &max_period))
		return SPUDRESULT_SAUD_DEVICE_LOST;
	cfg->period_mode = SPUDAUDIO_PERIOD_MODE_EXACT;
	if (desc->period_frames < min_period || desc->period_frames > max_period) {
		cfg->period_frames = desc->period_frames < min_period ? min_period : max_period;
		return SPUDRESULT_SAUD_PERIOD_OUT_OF_RANGE;
	}
	cfg->period_frames = desc->period_frames;

	// --- Buffer: none separate from the period.
	if (desc->buffer_frames != 0) {
		cfg->buffer_frames = 0;
		return SPUDRESULT_SAUD_BUFFER_OUT_OF_RANGE;
	}

	// --- Exclusive: hog mode must be free (-1) or already ours.
	if (desc->flags & SPUDAUDIO_STREAM_FLAG_EXCLUSIVE) {
		pid_t owner = -1;
		if (ca_get(dev, kAudioDevicePropertyHogMode, kAudioObjectPropertyScopeGlobal, sizeof(owner), &owner) != noErr)
			return SPUDRESULT_SAUD_EXCLUSIVE_UNAVAILABLE;
		if (owner != -1 && owner != getpid())
			return SPUDRESULT_SAUD_EXCLUSIVE_UNAVAILABLE;
	}

	uint32_t variable        = ca_variable_max_frames(dev);
	cfg->max_callback_frames = variable > desc->period_frames ? variable : desc->period_frames;
	return SPUD_SUCCESS;
}

SPUDRESULT spudaudio_probe_stream(
    const SPUDAUDIO_STREAM_DESC *desc,
    SPUDAUDIO_STREAM_CONFIG *out_config) {
	if (!out_config)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	memset(out_config, 0, sizeof(*out_config));
	SPUDRESULT r = spudaudio_validate_stream_desc(desc);
	if (SPUDFAIL(r))
		return r;
	r = ca_check_thread(&desc->thread);
	if (SPUDFAIL(r))
		return r;
	return ca_check_desc(desc, out_config);
}

// --------------------------------------------------------------------------
// IOProc
// --------------------------------------------------------------------------

/*
 * Runs on the HAL's real-time IO thread. No allocation, no locks: the
 * scratch buffer and planes were sized at create for max_callback_frames.
 *
 * Glitches: the HAL keeps the device clock running through an overload and
 * skips the IO cycle, so a skipped cycle shows up as the timestamp's
 * sample time jumping past where the previous buffer ended. That gap is
 * the DISCONTINUITY, and the position jumps by exactly the frames missed.
 */
static OSStatus ca_ioproc(
    AudioObjectID device,
    const AudioTimeStamp *now,
    const AudioBufferList *input,
    const AudioTimeStamp *input_time,
    AudioBufferList *output,
    const AudioTimeStamp *output_time,
    void *client) {
	(void)device, (void)now;
	struct spudaudio_stream_t *s = (struct spudaudio_stream_t *)client;
	s->callback_thread           = pthread_self();
	atomic_store(&s->in_callback, true);

	const bool out              = s->direction == SPUDAUDIO_DIRECTION_OUTPUT;
	const AudioBufferList *list = out ? output : input;
	const AudioTimeStamp *time  = out ? output_time : input_time;
	const uint32_t channels     = s->config.format.channel_count;
	float *interleaved          = s->config.format.interleaved ? s->scratch : NULL;
	void *frames_ptr            = s->config.format.interleaved ? (void *)s->scratch : (void *)s->planes;

	if (atomic_load(&s->state) != SPUDAUDIO_STREAM_STATE_RUNNING || !list || list->mNumberBuffers == 0 || list->mBuffers[0].mNumberChannels == 0) {
		// Stopping, or ended (device lost / rate changed): output silence.
		if (out && output)
			for (UInt32 b = 0; b < output->mNumberBuffers; ++b)
				if (output->mBuffers[b].mData)
					memset(output->mBuffers[b].mData, 0, output->mBuffers[b].mDataByteSize);
		atomic_store(&s->in_callback, false);
		return noErr;
	}

	uint32_t frames = list->mBuffers[0].mDataByteSize / (list->mBuffers[0].mNumberChannels * (uint32_t)sizeof(float));
	if (frames > s->config.max_callback_frames)
		frames = s->config.max_callback_frames; // never past the promised bound

	SPUDAUDIO_CALLBACK_INFO info = {0};
	if (time && (time->mFlags & kAudioTimeStampSampleTimeValid)) {
		if (!s->have_origin) {
			s->have_origin          = true;
			s->origin_sample_time   = time->mSampleTime;
			s->expected_sample_time = time->mSampleTime;
		}
		if (time->mSampleTime > s->expected_sample_time + 0.5) {
			info.flags |= SPUDAUDIO_CALLBACK_FLAG_DISCONTINUITY;
			atomic_fetch_add(out ? &s->underflow_count : &s->overflow_count, 1);
		}
		info.device_position_frames = (uint64_t)(time->mSampleTime - s->origin_sample_time);
		s->expected_sample_time     = time->mSampleTime + frames;
	} else {
		info.device_position_frames = s->position_frames;
	}
	s->position_frames = info.device_position_frames + frames;
	if (time && (time->mFlags & kAudioTimeStampHostTimeValid)) {
		info.host_time_ns = spudaudio_apple_host_time_to_ns(time->mHostTime, &s->timebase);
		info.flags |= SPUDAUDIO_CALLBACK_FLAG_HOST_TIME_VALID;
	}

	if (out) {
		s->callback(s, frames_ptr, frames, &info, s->user_data);
		spudaudio_apple_copy_to_buffer_list(output, frames, channels, interleaved, s->planes);
	} else {
		spudaudio_apple_copy_from_buffer_list(input, frames, channels, interleaved, s->planes);
		s->callback(s, frames_ptr, frames, &info, s->user_data);
	}

	atomic_store(&s->in_callback, false);
	return noErr;
}

// --------------------------------------------------------------------------
// Streams
// --------------------------------------------------------------------------

static void ca_set_terminal(struct spudaudio_stream_t *s, SPUDRESULT r) {
	atomic_store(&s->error, (int)r);
	atomic_store(&s->state, r == SPUDRESULT_SAUD_DEVICE_LOST ? SPUDAUDIO_STREAM_STATE_DEVICE_LOST : SPUDAUDIO_STREAM_STATE_ERROR);
}

static bool ca_is_terminal(unsigned int state) {
	return state == SPUDAUDIO_STREAM_STATE_DEVICE_LOST || state == SPUDAUDIO_STREAM_STATE_ERROR;
}

// Runs on the stream's listener queue: the device vanished, or its nominal
// rate changed (this process or any other) so the granted format is stale.
// Either ends the stream; the device is stopped so the IOProc stops too.
static void ca_on_device_change(struct spudaudio_stream_t *s, UInt32 count, const AudioObjectPropertyAddress *addresses) {
	for (UInt32 i = 0; i < count && !ca_is_terminal(atomic_load(&s->state)); ++i) {
		if (addresses[i].mSelector == kAudioDevicePropertyDeviceIsAlive) {
			UInt32 alive = 0;
			if (ca_get(s->device, kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal, sizeof(alive), &alive) != noErr || !alive)
				ca_set_terminal(s, SPUDRESULT_SAUD_DEVICE_LOST);
		} else if (addresses[i].mSelector == kAudioDevicePropertyNominalSampleRate) {
			if (ca_nominal_rate(s->device) != s->config.format.sample_rate)
				ca_set_terminal(s, SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED);
		}
	}
	if (ca_is_terminal(atomic_load(&s->state)))
		AudioDeviceStop(s->device, s->proc);
}

static const AudioObjectPropertyAddress ca_watched[] = {
    {kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain},
    {kAudioDevicePropertyNominalSampleRate, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain},
};

static void ca_drain(void *context) { (void)context; }

// Hog mode toggles: setting it takes ownership if free, releases it if
// ours (the value written is ignored by the HAL).
static void ca_release_hog(struct spudaudio_stream_t *s) {
	pid_t owner = -1;
	if (s->hogged && ca_get(s->device, kAudioDevicePropertyHogMode, kAudioObjectPropertyScopeGlobal, sizeof(owner), &owner) == noErr && owner == getpid()) {
		pid_t value = -1;
		ca_set(s->device, kAudioDevicePropertyHogMode, kAudioObjectPropertyScopeGlobal, sizeof(value), &value);
	}
	s->hogged = false;
}

static void ca_free_stream(struct spudaudio_stream_t *s) {
	if (s->listening) {
		for (size_t i = 0; i < sizeof(ca_watched) / sizeof(ca_watched[0]); ++i)
			AudioObjectRemovePropertyListenerBlock(s->device, &ca_watched[i], s->listener_queue, s->listener);
		// Removal doesn't wait for a listener already queued: running an
		// empty task on the same serial queue does.
		dispatch_sync_f(s->listener_queue, NULL, ca_drain);
	}
	if (s->proc)
		AudioDeviceDestroyIOProcID(s->device, s->proc);
	ca_release_hog(s);
	if (s->listener)
		Block_release(s->listener);
	if (s->listener_queue)
		dispatch_release(s->listener_queue);
	free(s->planes);
	free(s->scratch);
	free(s);
}

SPUDRESULT spudaudio_create_stream(
    const SPUDAUDIO_STREAM_DESC *desc,
    spudaudio_stream *out_stream) {
	if (!out_stream)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	*out_stream  = NULL;
	SPUDRESULT r = spudaudio_validate_stream_desc(desc);
	if (SPUDFAIL(r))
		return r;
	if (!desc->callback)
		return SPUDRESULT_DESC_INVALID_PARAMETERS;
	r = ca_check_thread(&desc->thread);
	if (SPUDFAIL(r))
		return r;

	struct spudaudio_stream_t *s = (struct spudaudio_stream_t *)calloc(1, sizeof(*s));
	if (!s)
		return SPUDRESULT_OUT_OF_MEMORY;
	s->device    = desc->device->id;
	s->direction = desc->device->direction;
	s->callback  = desc->callback;
	s->user_data = desc->user_data;
	atomic_init(&s->state, SPUDAUDIO_STREAM_STATE_STOPPED);
	atomic_init(&s->error, SPUD_SUCCESS);
	atomic_init(&s->underflow_count, 0);
	atomic_init(&s->overflow_count, 0);
	atomic_init(&s->in_callback, false);
	mach_timebase_info(&s->timebase);

	r = ca_check_desc(desc, &s->config);

	// Period: set this process's IO buffer size for the device and read
	// it back — the HAL can still refuse or adjust a value in range.
	if (SPUD_SUCCESS == r) {
		UInt32 frames   = desc->period_frames;
		OSStatus status = ca_set(s->device, kAudioDevicePropertyBufferFrameSize, kAudioObjectPropertyScopeGlobal, sizeof(frames), &frames);
		UInt32 granted  = 0;
		if (status == noErr)
			status = ca_get(s->device, kAudioDevicePropertyBufferFrameSize, kAudioObjectPropertyScopeGlobal, sizeof(granted), &granted);
		if (status != noErr)
			r = ca_result(status);
		else if (granted != desc->period_frames)
			r = SPUDRESULT_SAUD_PERIOD_OUT_OF_RANGE;
	}

	// Exclusive: take hog mode and confirm it's ours.
	if (SPUD_SUCCESS == r && (desc->flags & SPUDAUDIO_STREAM_FLAG_EXCLUSIVE)) {
		pid_t owner = -1;
		ca_get(s->device, kAudioDevicePropertyHogMode, kAudioObjectPropertyScopeGlobal, sizeof(owner), &owner);
		if (owner != getpid()) {
			pid_t value = getpid();
			ca_set(s->device, kAudioDevicePropertyHogMode, kAudioObjectPropertyScopeGlobal, sizeof(value), &value);
			ca_get(s->device, kAudioDevicePropertyHogMode, kAudioObjectPropertyScopeGlobal, sizeof(owner), &owner);
		}
		s->hogged = owner == getpid();
		if (!s->hogged)
			r = SPUDRESULT_SAUD_EXCLUSIVE_UNAVAILABLE;
	}

	if (SPUD_SUCCESS == r) {
		const uint32_t channels = s->config.format.channel_count;
		const uint32_t frames   = s->config.max_callback_frames;
		s->scratch              = (float *)calloc((size_t)frames * channels, sizeof(float));
		s->planes               = (float **)calloc(channels, sizeof(float *));
		if (!s->scratch || !s->planes)
			r = SPUDRESULT_OUT_OF_MEMORY;
		else
			for (uint32_t ch = 0; ch < channels; ++ch)
				s->planes[ch] = s->scratch + (size_t)ch * frames;
	}

	if (SPUD_SUCCESS == r)
		r = ca_result(AudioDeviceCreateIOProcID(s->device, ca_ioproc, s, &s->proc));

	if (SPUD_SUCCESS == r) {
		s->listener_queue = dispatch_queue_create("spudaudio.coreaudio.stream", DISPATCH_QUEUE_SERIAL);
		s->listener       = Block_copy(^(UInt32 count, const AudioObjectPropertyAddress *addresses) {
            ca_on_device_change(s, count, addresses);
        });
		s->listening      = true;
		for (size_t i = 0; i < sizeof(ca_watched) / sizeof(ca_watched[0]) && SPUD_SUCCESS == r; ++i)
			r = ca_result(AudioObjectAddPropertyListenerBlock(s->device, &ca_watched[i], s->listener_queue, s->listener));
	}

	if (SPUDFAIL(r)) {
		ca_free_stream(s);
		return r;
	}
	*out_stream = s;
	return SPUD_SUCCESS;
}

static bool ca_on_callback_thread(struct spudaudio_stream_t *s) {
	return atomic_load(&s->in_callback) && pthread_equal(pthread_self(), s->callback_thread);
}

SPUDRESULT spudaudio_stream_start(spudaudio_stream stream) {
	if (!stream)
		return SPUDRESULT_SAUD_INVALID_STREAM;
	if (ca_on_callback_thread(stream))
		return SPUDRESULT_GENERAL_FAILURE;
	unsigned int state = atomic_load(&stream->state);
	if (state == SPUDAUDIO_STREAM_STATE_RUNNING)
		return SPUD_SUCCESS;
	if (ca_is_terminal(state))
		return (SPUDRESULT)atomic_load(&stream->error);

	stream->have_origin     = false;
	stream->position_frames = 0;
	// RUNNING before the device starts: the IOProc outputs silence
	// unless the stream is RUNNING.
	atomic_store(&stream->state, SPUDAUDIO_STREAM_STATE_RUNNING);
	OSStatus status = AudioDeviceStart(stream->device, stream->proc);
	if (status != noErr) {
		SPUDRESULT r = ca_result(status);
		if (r == SPUDRESULT_SAUD_DEVICE_LOST)
			ca_set_terminal(stream, r);
		else
			atomic_store(&stream->state, SPUDAUDIO_STREAM_STATE_STOPPED);
		return r;
	}
	return SPUD_SUCCESS;
}

// AudioDeviceStop is synchronous when called off the IO thread: once it
// returns, the IOProc isn't running and won't be called again.
SPUDRESULT spudaudio_stream_stop(spudaudio_stream stream) {
	if (!stream)
		return SPUDRESULT_SAUD_INVALID_STREAM;
	if (ca_on_callback_thread(stream))
		return SPUDRESULT_GENERAL_FAILURE;
	AudioDeviceStop(stream->device, stream->proc);
	unsigned int running = SPUDAUDIO_STREAM_STATE_RUNNING;
	atomic_compare_exchange_strong(&stream->state, &running, SPUDAUDIO_STREAM_STATE_STOPPED);
	return SPUD_SUCCESS;
}

void spudaudio_destroy_stream(spudaudio_stream stream) {
	if (!stream)
		return;
	spudaudio_stream_stop(stream);
	ca_free_stream(stream);
}

SPUDRESULT spudaudio_stream_get_config(
    spudaudio_stream stream,
    SPUDAUDIO_STREAM_CONFIG *out_config) {
	if (!out_config)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	if (!stream)
		return SPUDRESULT_SAUD_INVALID_STREAM;
	*out_config = stream->config;
	return SPUD_SUCCESS;
}

SPUDRESULT spudaudio_stream_get_status(
    spudaudio_stream stream,
    SPUDAUDIO_STREAM_STATUS *out_status) {
	if (!out_status)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	if (!stream)
		return SPUDRESULT_SAUD_INVALID_STREAM;
	out_status->state           = (SPUDAUDIO_STREAM_STATE)atomic_load(&stream->state);
	out_status->error           = (SPUDRESULT)atomic_load(&stream->error);
	out_status->underflow_count = atomic_load(&stream->underflow_count);
	out_status->overflow_count  = atomic_load(&stream->overflow_count);
	return SPUD_SUCCESS;
}

// The HAL's own breakdown: device latency + safety offset + the IO buffer
// + the first stream's latency, all in frames at the nominal rate.
SPUDRESULT spudaudio_stream_get_latency_frames(
    spudaudio_stream stream,
    uint32_t *out_latency_frames) {
	if (!out_latency_frames)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	*out_latency_frames = 0;
	if (!stream)
		return SPUDRESULT_SAUD_INVALID_STREAM;
	const AudioObjectPropertyScope scope = ca_scope(stream->direction);
	UInt32 device_latency = 0, safety = 0, buffer = 0, stream_latency = 0;
	OSStatus status = ca_get(stream->device, kAudioDevicePropertyLatency, scope, sizeof(device_latency), &device_latency);
	if (status == noErr)
		status = ca_get(stream->device, kAudioDevicePropertySafetyOffset, scope, sizeof(safety), &safety);
	if (status == noErr)
		status = ca_get(stream->device, kAudioDevicePropertyBufferFrameSize, kAudioObjectPropertyScopeGlobal, sizeof(buffer), &buffer);
	if (status != noErr)
		return ca_result(status);
	AudioStreamID *streams = (AudioStreamID *)ca_get_alloc(stream->device, kAudioDevicePropertyStreams, scope, NULL);
	if (streams) {
		ca_get(streams[0], kAudioStreamPropertyLatency, kAudioObjectPropertyScopeGlobal, sizeof(stream_latency), &stream_latency);
		free(streams);
	}
	*out_latency_frames = device_latency + safety + buffer + stream_latency;
	return SPUD_SUCCESS;
}

SPUDRESULT spudaudio_set_device_event_callback(
    spudaudio_instance instance,
    SPUDAUDIO_DEVICE_EVENT_CALLBACK callback,
    void *user_data) {
	(void)callback;
	(void)user_data;
	if (!instance)
		return SPUDRESULT_SAUD_INVALID_INSTANCE;
	return SPUDRESULT_NOT_IMPLEMENTED_YET;
}

#if __cplusplus
}
#endif

#else
// Not built without SPUDAUDIO_COMPILE_COREAUDIO (CMake leaves this file
// out); this keeps a stray build of it a valid, empty translation unit.
typedef int spudaudio_coreaudio_not_compiled;
#endif // SPUDAUDIO_COMPILE_COREAUDIO
