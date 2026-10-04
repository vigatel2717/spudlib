#if SPUDAUDIO_COMPILE_ALSA

#include "../../spudaudiointernal.h"

#include <alsa/asoundlib.h>

#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <sys/eventfd.h>
#include <unistd.h>

#if __cplusplus
extern "C" {
#endif

/*
 * ALSA backend. PipeWire will be a second backend in this same instance
 * model; until then SPUDAUDIO_NATIVE_API_PIPEWIRE reports
 * SPUDRESULT_NOT_IMPLEMENTED_YET.
 *
 * ALSA has no exclusive/shared flag: exclusivity is which PCM name the
 * caller picks (hw:... is exclusive, dmix/pipewire/default are shared),
 * and the enumerated names already expose that choice. A request with
 * SPUDAUDIO_STREAM_FLAG_EXCLUSIVE is therefore refused rather than
 * silently ignored.
 */

struct spudaudio_device_t {
#if _DEBUG
	const char *debug_name;
#endif
	SPUDAUDIO_DIRECTION direction;
	char persistent_id[256]; // the ALSA PCM name, e.g. "hw:CARD=PCH,DEV=0"
	char name[128];          // DESC hint, lines joined; else the PCM name
	struct spudaudio_device_t *next; // instance-owned list
};

/*
 * A created stream: an open PCM with hw_params/sw_params installed and
 * prepared, not started. Status fields are written by the audio thread
 * (once it exists) and read from any thread, hence C11 atomics.
 */
struct spudaudio_stream_t {
#if _DEBUG
	const char *debug_name;
#endif
	snd_pcm_t *pcm;
	// The PCM's channel map before this stream applied its layout (NULL if
	// it had none / no layout requested) — restored on destroy, since some
	// drivers keep the map as a persistent control.
	snd_pcm_chmap_t *original_chmap;
	SPUDAUDIO_DIRECTION direction;
	SPUDAUDIO_STREAM_CONFIG config;
	SPUDAUDIO_STREAM_CALLBACK callback;
	void *user_data;
	uint32_t sample_bytes;
	uint32_t frame_bytes;
	uint8_t *scratch; // one period; interleaved, or one plane per channel
	void **planes;    // planar: channel_count pointers into scratch

	SPUDAUDIO_THREAD_PRIORITY priority;
	int32_t sched_fifo_priority;

	// Audio thread. Only start/stop/destroy touch these (the caller
	// serializes those), except startup_result, written by the thread
	// before it posts `startup`.
	pthread_t thread;
	bool has_thread;
	// Set by the audio thread itself before its first callback (the
	// creator's copy of `thread` can land after the thread has already
	// called back), cleared after join.
	pthread_t audio_thread_self;
	atomic_bool audio_thread_known;
	int stop_fd; // eventfd; written by stop, polled by the thread
	sem_t startup;
	bool startup_initialized;
	SPUDRESULT startup_result;

	// Audio-thread-only state.
	uint64_t frames_transferred; // stream index of the next frame
	bool pending_discontinuity;  // flag the next callback

	atomic_uint state; // SPUDAUDIO_STREAM_STATE
	atomic_int error;  // SPUDRESULT
	atomic_uint_least64_t underflow_count;
	atomic_uint_least64_t overflow_count;
};

struct spudaudio_instance_t {
#if _DEBUG
	const char *debug_name;
#endif
	struct spudaudio_device_t *devices; // every PCM ever enumerated
	spudaudio_device *enumerated[2];    // latest array per direction
};

// --------------------------------------------------------------------------
// Helpers
// --------------------------------------------------------------------------

static SPUDRESULT alsa_result(int err) {
	switch (-err) {
	case 0:
		return SPUD_SUCCESS;
	case EBUSY:
		return SPUDRESULT_SAUD_EXCLUSIVE_UNAVAILABLE;
	case ENODEV:
	case ENOENT:
		return SPUDRESULT_SAUD_DEVICE_LOST;
	case ENOMEM:
		return SPUDRESULT_OUT_OF_MEMORY;
	default:
		return SPUDRESULT_API_SPECIFIC_FAILURE;
	}
}

/*
 * The un-suffixed SND_PCM_FORMAT_* names are native-endian; packed 24-bit
 * only has explicit-endian names, so it's picked by byte order.
 *
 * S24_32_MSB -> SND_PCM_FORMAT_S32: ALSA has no name for 24 bits high in
 * 32, but those bytes *are* an S32 sample whose low byte is zero, so the
 * device receives exactly the intended signal. Read back, an ALSA S32
 * device reports as S32 — SpudAudio can't tell it only uses 24 bits.
 */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define ALSA_FORMAT_S24_PACKED SND_PCM_FORMAT_S24_3LE
#else
#define ALSA_FORMAT_S24_PACKED SND_PCM_FORMAT_S24_3BE
#endif

static snd_pcm_format_t alsa_format(SPUDAUDIO_SAMPLE_FORMAT f) {
	switch (f) {
	case SPUDAUDIO_SAMPLE_FORMAT_S16:
		return SND_PCM_FORMAT_S16;
	case SPUDAUDIO_SAMPLE_FORMAT_S24_PACKED:
		return ALSA_FORMAT_S24_PACKED;
	case SPUDAUDIO_SAMPLE_FORMAT_S24_32_MSB:
		return SND_PCM_FORMAT_S32;
	case SPUDAUDIO_SAMPLE_FORMAT_S24_32_LSB:
		return SND_PCM_FORMAT_S24;
	case SPUDAUDIO_SAMPLE_FORMAT_S32:
		return SND_PCM_FORMAT_S32;
	case SPUDAUDIO_SAMPLE_FORMAT_F32:
		return SND_PCM_FORMAT_FLOAT;
	default:
		return SND_PCM_FORMAT_UNKNOWN;
	}
}

static SPUDAUDIO_SAMPLE_FORMAT alsa_format_back(snd_pcm_format_t f) {
	if (f == SND_PCM_FORMAT_S16)
		return SPUDAUDIO_SAMPLE_FORMAT_S16;
	if (f == ALSA_FORMAT_S24_PACKED)
		return SPUDAUDIO_SAMPLE_FORMAT_S24_PACKED;
	if (f == SND_PCM_FORMAT_S24)
		return SPUDAUDIO_SAMPLE_FORMAT_S24_32_LSB;
	if (f == SND_PCM_FORMAT_S32)
		return SPUDAUDIO_SAMPLE_FORMAT_S32;
	if (f == SND_PCM_FORMAT_FLOAT)
		return SPUDAUDIO_SAMPLE_FORMAT_F32;
	return SPUDAUDIO_SAMPLE_FORMAT_UNKNOWN;
}

/*
 * Channel positions <-> snd_pcm_chmap_t. SpudAudio's labels are ALSA's
 * full set plus LFE2, so every ALSA label maps both ways; LFE2 has no ALSA
 * label and makes a layout unsupported. AUX is SND_CHMAP_UNKNOWN (read
 * back as AUX0, AUX1, ...) and NA is SND_CHMAP_NA.
 */
static const unsigned int alsa_chmap_of_position[SPUDAUDIO_CHANNEL_POSITION_LAST_SPEAKER + 1] = {
    [SPUDAUDIO_CHANNEL_POSITION_MONO] = SND_CHMAP_MONO,
    [SPUDAUDIO_CHANNEL_POSITION_FL]   = SND_CHMAP_FL,
    [SPUDAUDIO_CHANNEL_POSITION_FR]   = SND_CHMAP_FR,
    [SPUDAUDIO_CHANNEL_POSITION_FC]   = SND_CHMAP_FC,
    [SPUDAUDIO_CHANNEL_POSITION_LFE]  = SND_CHMAP_LFE,
    [SPUDAUDIO_CHANNEL_POSITION_RL]   = SND_CHMAP_RL,
    [SPUDAUDIO_CHANNEL_POSITION_RR]   = SND_CHMAP_RR,
    [SPUDAUDIO_CHANNEL_POSITION_FLC]  = SND_CHMAP_FLC,
    [SPUDAUDIO_CHANNEL_POSITION_FRC]  = SND_CHMAP_FRC,
    [SPUDAUDIO_CHANNEL_POSITION_RC]   = SND_CHMAP_RC,
    [SPUDAUDIO_CHANNEL_POSITION_SL]   = SND_CHMAP_SL,
    [SPUDAUDIO_CHANNEL_POSITION_SR]   = SND_CHMAP_SR,
    [SPUDAUDIO_CHANNEL_POSITION_TC]   = SND_CHMAP_TC,
    [SPUDAUDIO_CHANNEL_POSITION_TFL]  = SND_CHMAP_TFL,
    [SPUDAUDIO_CHANNEL_POSITION_TFC]  = SND_CHMAP_TFC,
    [SPUDAUDIO_CHANNEL_POSITION_TFR]  = SND_CHMAP_TFR,
    [SPUDAUDIO_CHANNEL_POSITION_TRL]  = SND_CHMAP_TRL,
    [SPUDAUDIO_CHANNEL_POSITION_TRC]  = SND_CHMAP_TRC,
    [SPUDAUDIO_CHANNEL_POSITION_TRR]  = SND_CHMAP_TRR,
    [SPUDAUDIO_CHANNEL_POSITION_RLC]  = SND_CHMAP_RLC,
    [SPUDAUDIO_CHANNEL_POSITION_RRC]  = SND_CHMAP_RRC,
    [SPUDAUDIO_CHANNEL_POSITION_FLW]  = SND_CHMAP_FLW,
    [SPUDAUDIO_CHANNEL_POSITION_FRW]  = SND_CHMAP_FRW,
    // LFE2: no ALSA label — left SND_CHMAP_UNKNOWN (0), i.e. unrepresentable.
    [SPUDAUDIO_CHANNEL_POSITION_FLH]  = SND_CHMAP_FLH,
    [SPUDAUDIO_CHANNEL_POSITION_FCH]  = SND_CHMAP_FCH,
    [SPUDAUDIO_CHANNEL_POSITION_FRH]  = SND_CHMAP_FRH,
    [SPUDAUDIO_CHANNEL_POSITION_TFLC] = SND_CHMAP_TFLC,
    [SPUDAUDIO_CHANNEL_POSITION_TFRC] = SND_CHMAP_TFRC,
    [SPUDAUDIO_CHANNEL_POSITION_TSL]  = SND_CHMAP_TSL,
    [SPUDAUDIO_CHANNEL_POSITION_TSR]  = SND_CHMAP_TSR,
    [SPUDAUDIO_CHANNEL_POSITION_LLFE] = SND_CHMAP_LLFE,
    [SPUDAUDIO_CHANNEL_POSITION_RLFE] = SND_CHMAP_RLFE,
    [SPUDAUDIO_CHANNEL_POSITION_BC]   = SND_CHMAP_BC,
    [SPUDAUDIO_CHANNEL_POSITION_BLC]  = SND_CHMAP_BLC,
    [SPUDAUDIO_CHANNEL_POSITION_BRC]  = SND_CHMAP_BRC,
};

// false when ALSA has no label for `p` (LFE2).
static bool alsa_chmap_pos(SPUDAUDIO_CHANNEL_POSITION p, unsigned int *out) {
	if (p == SPUDAUDIO_CHANNEL_POSITION_NA)
		*out = SND_CHMAP_NA;
	else if (p >= SPUDAUDIO_CHANNEL_POSITION_AUX0)
		*out = SND_CHMAP_UNKNOWN;
	else if (p <= SPUDAUDIO_CHANNEL_POSITION_LAST_SPEAKER && alsa_chmap_of_position[p] != SND_CHMAP_UNKNOWN)
		*out = alsa_chmap_of_position[p];
	else
		return false;
	return true;
}

static void alsa_chmap_to_positions(const snd_pcm_chmap_t *map, SPUDAUDIO_FORMAT *out) {
	memset(out->positions, 0, sizeof(out->positions));
	uint32_t aux = 0;
	for (unsigned int i = 0; i < map->channels && i < SPUDAUDIO_MAX_CHANNELS; ++i) {
		unsigned int pos               = map->pos[i] & SND_CHMAP_POSITION_MASK;
		SPUDAUDIO_CHANNEL_POSITION got = SPUDAUDIO_CHANNEL_POSITION_UNSPECIFIED;
		// Driver-specific numbering (SND_CHMAP_DRIVER_SPEC) isn't an ALSA
		// label, so it stays unlabeled like UNKNOWN.
		const bool labeled = !(map->pos[i] & SND_CHMAP_DRIVER_SPEC);
		if (labeled && pos == SND_CHMAP_NA)
			got = SPUDAUDIO_CHANNEL_POSITION_NA;
		else if (labeled && pos != SND_CHMAP_UNKNOWN)
			for (int p = SPUDAUDIO_CHANNEL_POSITION_MONO; p <= SPUDAUDIO_CHANNEL_POSITION_LAST_SPEAKER && !got; ++p)
				if (alsa_chmap_of_position[p] == pos)
					got = (SPUDAUDIO_CHANNEL_POSITION)p;
		out->positions[i] = got ? got : (SPUDAUDIO_CHANNEL_POSITION)(SPUDAUDIO_CHANNEL_POSITION_AUX0 + aux++);
	}
}

static bool alsa_chmap_pos_equal(unsigned int have, unsigned int want) {
	if (have & SND_CHMAP_DRIVER_SPEC)
		return want == SND_CHMAP_UNKNOWN;
	return (have & SND_CHMAP_POSITION_MASK) == want;
}

static bool alsa_chmap_equal_in_order(const snd_pcm_chmap_t *map, const unsigned int *want, unsigned int n) {
	if (map->channels != n)
		return false;
	for (unsigned int i = 0; i < n; ++i)
		if (!alsa_chmap_pos_equal(map->pos[i], want[i]))
			return false;
	return true;
}

// FIXED maps must match in order; VAR/PAIRED maps can be reordered, so a
// same-set match counts here and snd_pcm_set_chmap at install decides
// whether this particular order is reachable.
static bool alsa_chmap_query_matches(const snd_pcm_chmap_query_t *q, const unsigned int *want, unsigned int n) {
	if (q->map.channels != n)
		return false;
	if (q->type == SND_CHMAP_TYPE_FIXED || q->type == SND_CHMAP_TYPE_NONE)
		return alsa_chmap_equal_in_order(&q->map, want, n);
	bool used[SPUDAUDIO_MAX_CHANNELS] = {false};
	for (unsigned int i = 0; i < n; ++i) {
		bool found = false;
		for (unsigned int j = 0; j < n && !found; ++j)
			if (!used[j] && alsa_chmap_pos_equal(q->map.pos[j], want[i]))
				used[j] = found = true;
		if (!found)
			return false;
	}
	return true;
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
	if (api == SPUDAUDIO_NATIVE_API_PIPEWIRE)
		return SPUDRESULT_NOT_IMPLEMENTED_YET;
	if (api != SPUDAUDIO_NATIVE_API_ALSA)
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
	return instance ? SPUDAUDIO_NATIVE_API_ALSA : SPUDAUDIO_NATIVE_API_NONE;
}

// Returns the instance's existing handle for this PCM name, or adds one,
// so handles stay stable across enumerations.
static struct spudaudio_device_t *alsa_find_or_add_device(
    spudaudio_instance instance,
    const char *name,
    const char *description,
    SPUDAUDIO_DIRECTION direction) {
	for (struct spudaudio_device_t *d = instance->devices; d; d = d->next)
		if (d->direction == direction && strcmp(d->persistent_id, name) == 0)
			return d;

	struct spudaudio_device_t *d = (struct spudaudio_device_t *)calloc(1, sizeof(*d));
	if (!d)
		return NULL;
	d->direction = direction;
	strncpy(d->persistent_id, name, sizeof(d->persistent_id) - 1);
	// DESC is multi-line ("card, device\nwhat it is"); join it into one.
	strncpy(d->name, description ? description : name, sizeof(d->name) - 1);
	for (char *c = d->name; *c; ++c)
		if (*c == '\n')
			*c = ' ';
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

	void **hints = NULL;
	int err      = snd_device_name_hint(-1, "pcm", &hints);
	if (err < 0)
		return alsa_result(err);

	// IOID is "Output", "Input", or absent for a PCM that does both.
	const char *wanted_ioid = direction == SPUDAUDIO_DIRECTION_OUTPUT ? "Output" : "Input";

	uint32_t capacity = 0;
	for (void **h = hints; *h; ++h)
		++capacity;
	spudaudio_device *list = capacity ? (spudaudio_device *)calloc(capacity, sizeof(spudaudio_device)) : NULL;
	if (capacity && !list) {
		snd_device_name_free_hint(hints);
		return SPUDRESULT_OUT_OF_MEMORY;
	}

	uint32_t written = 0;
	SPUDRESULT r     = SPUD_SUCCESS;
	for (void **h = hints; *h; ++h) {
		char *name = snd_device_name_get_hint(*h, "NAME");
		char *desc = snd_device_name_get_hint(*h, "DESC");
		char *ioid = snd_device_name_get_hint(*h, "IOID");
		if (name && (!ioid || strcmp(ioid, wanted_ioid) == 0)) {
			struct spudaudio_device_t *d = alsa_find_or_add_device(instance, name, desc, direction);
			if (d)
				list[written++] = d;
			else
				r = SPUDRESULT_OUT_OF_MEMORY;
		}
		free(name);
		free(desc);
		free(ioid);
		if (SPUDFAIL(r))
			break;
	}
	snd_device_name_free_hint(hints);
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
	memcpy(out_properties->name, device->name, sizeof(out_properties->name));
	memcpy(out_properties->persistent_id, device->persistent_id, sizeof(out_properties->persistent_id));
	out_properties->direction     = device->direction;
	// ALSA's only notion of a default is the PCM named "default".
	out_properties->default_roles = strcmp(device->persistent_id, "default") == 0 ? SPUDAUDIO_DEFAULT_ROLE_GENERAL : SPUDAUDIO_DEFAULT_ROLE_NONE;
	// native_format stays zeroed: an ALSA PCM has no single native format,
	// only the hw_params space the probe explores. No exclusive flag
	// either — see the file comment.
	out_properties->supports_exclusive = false;
	return SPUD_SUCCESS;
}

// --------------------------------------------------------------------------
// Timing / probe
// --------------------------------------------------------------------------

SPUDRESULT spudaudio_get_timing_caps(
    spudaudio_device device,
    const SPUDAUDIO_FORMAT *format,
    SPUDAUDIO_STREAM_FLAGS flags,
    SPUDAUDIO_TIMING_CAPS *out_caps) {
	(void)flags;
	if (!out_caps)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	memset(out_caps, 0, sizeof(*out_caps));
	if (!format)
		return SPUDRESULT_NULL_DESC;
	if (!device)
		return SPUDRESULT_SAUD_INVALID_DEVICE;
	return SPUDRESULT_NOT_IMPLEMENTED_YET;
}

// With out_original NULL (probe), a successfully applied map is undone
// before returning. Otherwise (create) it stays applied and the previous
// map is handed back for the stream to restore on destroy.
static SPUDRESULT alsa_apply_chmap(
    snd_pcm_t *pcm,
    const unsigned int *want,
    const SPUDAUDIO_FORMAT *format,
    SPUDAUDIO_STREAM_CONFIG *out_config,
    snd_pcm_chmap_t **out_original) {
	const unsigned int n = format->channel_count;
	snd_pcm_chmap_t *map = (snd_pcm_chmap_t *)malloc(sizeof(snd_pcm_chmap_t) + n * sizeof(unsigned int));
	if (!map)
		return SPUDRESULT_OUT_OF_MEMORY;
	map->channels = n;
	memcpy(map->pos, want, n * sizeof(unsigned int));

	snd_pcm_chmap_t *original = snd_pcm_get_chmap(pcm);
	SPUDRESULT r              = SPUD_SUCCESS;
	if (snd_pcm_set_chmap(pcm, map) < 0) {
		// Refused — but a fixed map that already *is* this layout is fine.
		if (!original || !alsa_chmap_equal_in_order(original, want, n)) {
			if (original)
				alsa_chmap_to_positions(original, &out_config->format);
			else
				memset(out_config->format.positions, 0, sizeof(out_config->format.positions));
			r = SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED;
		}
	} else if (out_original) {
		*out_original = original; // kept applied; caller restores later
		original      = NULL;
	} else if (original) {
		snd_pcm_set_chmap(pcm, original);
	}
	free(original);
	free(map);
	return r;
}

/*
 * ALSA's hw_params is a constraint space the driver narrows as each
 * parameter is fixed — period/buffer limits depend on format, rate and
 * channels, and drivers can add arbitrary rules (power-of-two periods,
 * byte alignment, ...) that no min/max/step describes. So rather than
 * modelling them, the probe sets each requested value *exactly* in the
 * same order a stream would, and on the first refusal asks ALSA's own
 * solver for its nearest acceptable value (set_*_near on a scratch copy of
 * the space as it stood) — that becomes the suggestion. Finally
 * snd_pcm_hw_params installs the configuration on the real device, which
 * is the step that catches anything the refinement didn't.
 */
static SPUDRESULT alsa_configure(
    snd_pcm_t *pcm,
    const SPUDAUDIO_STREAM_DESC *desc,
    SPUDAUDIO_STREAM_CONFIG *out_config,
    snd_pcm_chmap_t **out_original_chmap) {
	snd_pcm_hw_params_t *hw;
	snd_pcm_hw_params_t *scratch;
	snd_pcm_hw_params_alloca(&hw);
	snd_pcm_hw_params_alloca(&scratch);

	int err = snd_pcm_hw_params_any(pcm, hw);
	if (err < 0)
		return alsa_result(err);

	// Resampling only happens inside ALSA plugins (plughw:, default, ...);
	// it's off unless the caller opted into OS conversion. Sample-format
	// conversion is likewise a plugin property — the device name the
	// caller chose decides whether it exists at all.
	snd_pcm_hw_params_set_rate_resample(pcm, hw, (desc->flags & SPUDAUDIO_STREAM_FLAG_ALLOW_OS_CONVERSION) ? 1 : 0);

	// --- Format: access (interleaved/planar), sample format, channels, rate.
	out_config->format = desc->format;

	snd_pcm_access_t access = desc->format.interleaved ? SND_PCM_ACCESS_RW_INTERLEAVED : SND_PCM_ACCESS_RW_NONINTERLEAVED;
	if (snd_pcm_hw_params_set_access(pcm, hw, access) < 0) {
		snd_pcm_access_t other = desc->format.interleaved ? SND_PCM_ACCESS_RW_NONINTERLEAVED : SND_PCM_ACCESS_RW_INTERLEAVED;
		if (snd_pcm_hw_params_test_access(pcm, hw, other) == 0)
			out_config->format.interleaved = !desc->format.interleaved;
		else
			memset(&out_config->format, 0, sizeof(out_config->format));
		return SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED;
	}

	snd_pcm_format_t fmt = alsa_format(desc->format.sample_format);
	if (fmt == SND_PCM_FORMAT_UNKNOWN || snd_pcm_hw_params_set_format(pcm, hw, fmt) < 0) {
		// ALSA has no "nearest" sample format; set_format_first is its own
		// pick from what the device supports. UNKNOWN if SpudAudio has no
		// equivalent (e.g. U8).
		snd_pcm_format_t first = SND_PCM_FORMAT_UNKNOWN;
		snd_pcm_hw_params_copy(scratch, hw);
		if (snd_pcm_hw_params_set_format_first(pcm, scratch, &first) >= 0)
			out_config->format.sample_format = alsa_format_back(first);
		else
			out_config->format.sample_format = SPUDAUDIO_SAMPLE_FORMAT_UNKNOWN;
		return SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED;
	}

	if (snd_pcm_hw_params_set_channels(pcm, hw, desc->format.channel_count) < 0) {
		unsigned int near = desc->format.channel_count;
		snd_pcm_hw_params_copy(scratch, hw);
		out_config->format.channel_count = snd_pcm_hw_params_set_channels_near(pcm, scratch, &near) >= 0 ? near : 0;
		return SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED;
	}

	if (snd_pcm_hw_params_set_rate(pcm, hw, desc->format.sample_rate, 0) < 0) {
		unsigned int near = desc->format.sample_rate;
		int dir           = 0;
		snd_pcm_hw_params_copy(scratch, hw);
		out_config->format.sample_rate = snd_pcm_hw_params_set_rate_near(pcm, scratch, &near, &dir) >= 0 ? near : 0;
		return SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED;
	}

	// --- Channel positions, first pass: the maps the PCM advertises. A
	// layout must match one; a PCM that advertises none can't confirm any
	// layout, so only "no layout" is accepted there.
	unsigned int want[SPUDAUDIO_MAX_CHANNELS];
	const bool has_layout = spudaudio_format_has_layout(&desc->format);
	if (has_layout) {
		for (uint32_t i = 0; i < desc->format.channel_count; ++i) {
			if (!alsa_chmap_pos(desc->format.positions[i], &want[i])) {
				// A label ALSA can't express: suggest the same format with
				// no layout rather than relabeling.
				memset(out_config->format.positions, 0, sizeof(out_config->format.positions));
				return SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED;
			}
		}
		snd_pcm_chmap_query_t **maps = snd_pcm_query_chmaps(pcm);
		bool matched                 = false;
		const snd_pcm_chmap_t *first = NULL;
		for (snd_pcm_chmap_query_t **q = maps; q && *q && !matched; ++q) {
			matched = alsa_chmap_query_matches(*q, want, desc->format.channel_count);
			if (!first && (*q)->map.channels == desc->format.channel_count)
				first = &(*q)->map;
		}
		if (!matched) {
			if (first)
				alsa_chmap_to_positions(first, &out_config->format);
			else
				memset(out_config->format.positions, 0, sizeof(out_config->format.positions));
		}
		if (maps)
			snd_pcm_free_chmaps(maps);
		if (!matched)
			return SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED;
	}

	// --- Period.
	out_config->period_mode = SPUDAUDIO_PERIOD_MODE_EXACT;
	if (snd_pcm_hw_params_set_period_size(pcm, hw, desc->period_frames, 0) < 0) {
		snd_pcm_uframes_t near = desc->period_frames;
		int dir                = 0;
		snd_pcm_hw_params_copy(scratch, hw);
		out_config->period_frames = snd_pcm_hw_params_set_period_size_near(pcm, scratch, &near, &dir) >= 0 ? (uint32_t)near : 0;
		return SPUDRESULT_SAUD_PERIOD_OUT_OF_RANGE;
	}
	out_config->period_frames = desc->period_frames;

	// --- Buffer. Required on ALSA (it has a buffer separate from the
	// period), so 0 is refused like any other unacceptable size, and the
	// solver's nearest — its smallest legal buffer, for 0 — is suggested.
	if (desc->buffer_frames == 0 || snd_pcm_hw_params_set_buffer_size(pcm, hw, desc->buffer_frames) < 0) {
		snd_pcm_uframes_t near = desc->buffer_frames;
		snd_pcm_hw_params_copy(scratch, hw);
		out_config->buffer_frames = snd_pcm_hw_params_set_buffer_size_near(pcm, scratch, &near) >= 0 ? (uint32_t)near : 0;
		return SPUDRESULT_SAUD_BUFFER_OUT_OF_RANGE;
	}
	out_config->buffer_frames = desc->buffer_frames;

	// --- Install on the real device: the authoritative step.
	err = snd_pcm_hw_params(pcm, hw);
	if (err < 0)
		return alsa_result(err);

	snd_pcm_uframes_t granted_period = 0, granted_buffer = 0;
	int dir                          = 0;
	snd_pcm_hw_params_get_period_size(hw, &granted_period, &dir);
	snd_pcm_hw_params_get_buffer_size(hw, &granted_buffer);
	out_config->period_frames = (uint32_t)granted_period;
	out_config->buffer_frames = (uint32_t)granted_buffer;
	// SpudAudio's thread hands the callback one period per transfer.
	out_config->max_callback_frames = (uint32_t)granted_period;

	if (granted_period != desc->period_frames)
		return SPUDRESULT_SAUD_PERIOD_OUT_OF_RANGE;
	if (granted_buffer != desc->buffer_frames)
		return SPUDRESULT_SAUD_BUFFER_OUT_OF_RANGE;

	// --- Channel positions, second pass: actually apply the map, which is
	// the only way to know a VAR/PAIRED map reaches this exact order.
	// Some drivers keep the map as a persistent control (HDMI's "Playback
	// Channel Map"), so the original is restored before closing — a probe
	// must not leave the device changed.
	return has_layout ? alsa_apply_chmap(pcm, want, &desc->format, out_config, out_original_chmap) : SPUD_SUCCESS;
}

// SpudAudio's own thread runs the callback, so NORMAL and SCHED_FIFO
// apply; MMCSS and PLATFORM are other backends' mechanisms.
static SPUDRESULT alsa_check_thread(const SPUDAUDIO_THREAD_DESC *t) {
	switch (t->priority) {
	case SPUDAUDIO_THREAD_PRIORITY_NORMAL:
		return SPUD_SUCCESS;
	case SPUDAUDIO_THREAD_PRIORITY_SCHED_FIFO:
		if (t->sched_fifo_priority < sched_get_priority_min(SCHED_FIFO) || t->sched_fifo_priority > sched_get_priority_max(SCHED_FIFO))
			return SPUDRESULT_DESC_INVALID_PARAMETERS;
		return SPUD_SUCCESS;
	default:
		return SPUDRESULT_SAUD_THREAD_PRIORITY_UNSUPPORTED;
	}
}

/*
 * The one path that turns a desc into an open, hw-configured PCM, used by
 * both probe and create so creation can never accept something the probe
 * would reject (or vice versa). NONBLOCK so a hw: device another process
 * holds reports -EBUSY instead of blocking until it's released; the
 * stream's audio thread will wait on the PCM's poll descriptors rather
 * than in blocking reads/writes, so it stays non-blocking.
 */
static SPUDRESULT alsa_open(
    const SPUDAUDIO_STREAM_DESC *desc,
    SPUDAUDIO_STREAM_CONFIG *out_config,
    snd_pcm_chmap_t **out_original_chmap,
    snd_pcm_t **out_pcm) {
	*out_pcm = NULL;
	if (desc->flags & SPUDAUDIO_STREAM_FLAG_EXCLUSIVE)
		return SPUDRESULT_SAUD_EXCLUSIVE_UNAVAILABLE;

	snd_pcm_t *pcm          = NULL;
	snd_pcm_stream_t stream = desc->device->direction == SPUDAUDIO_DIRECTION_OUTPUT ? SND_PCM_STREAM_PLAYBACK : SND_PCM_STREAM_CAPTURE;
	int err                 = snd_pcm_open(&pcm, desc->device->persistent_id, stream, SND_PCM_NONBLOCK);
	if (err < 0)
		return alsa_result(err);

	// Conversion only exists in plugins. These PCM types never convert, so
	// the flag is refused rather than left a no-op.
	if (desc->flags & SPUDAUDIO_STREAM_FLAG_ALLOW_OS_CONVERSION) {
		const snd_pcm_type_t type = snd_pcm_type(pcm);
		if (type == SND_PCM_TYPE_HW || type == SND_PCM_TYPE_DMIX || type == SND_PCM_TYPE_DSNOOP || type == SND_PCM_TYPE_DSHARE) {
			snd_pcm_close(pcm);
			return SPUDRESULT_SAUD_OS_CONVERSION_UNAVAILABLE;
		}
	}

	SPUDRESULT r = alsa_configure(pcm, desc, out_config, out_original_chmap);
	if (SPUDFAIL(r)) {
		snd_pcm_close(pcm);
		return r;
	}
	*out_pcm = pcm;
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
	r = alsa_check_thread(&desc->thread);
	if (SPUDFAIL(r))
		return r;

	snd_pcm_t *pcm = NULL;
	r              = alsa_open(desc, out_config, NULL, &pcm);
	if (pcm)
		snd_pcm_close(pcm);
	return r;
}

/*
 * sw_params for a created stream:
 * - start_threshold = boundary: the PCM never starts itself; it starts
 *   only when spudaudio_stream_start says so (after the first period is
 *   written, for output), so start is always the caller's decision.
 * - avail_min = one period: the PCM's poll descriptors wake the audio
 *   thread once per period, matching the callback cadence.
 * - monotonic timestamps: snd_pcm_status' htstamp becomes CLOCK_MONOTONIC,
 *   the clock spudperf_get_monotonic_time_ns and
 *   SPUDAUDIO_CALLBACK_INFO::host_time_ns use. ALSA's default is
 *   gettimeofday, which would not line up.
 * stop_threshold stays at ALSA's default (buffer size), so an underrun or
 * overrun stops the PCM with -EPIPE — reported as a DISCONTINUITY, not
 * hidden.
 */
static SPUDRESULT alsa_set_sw_params(snd_pcm_t *pcm, uint32_t period_frames) {
	snd_pcm_sw_params_t *sw;
	snd_pcm_sw_params_alloca(&sw);
	int err = snd_pcm_sw_params_current(pcm, sw);
	if (err < 0)
		return alsa_result(err);

	snd_pcm_uframes_t boundary = 0;
	if ((err = snd_pcm_sw_params_get_boundary(sw, &boundary)) < 0 ||
	    (err = snd_pcm_sw_params_set_start_threshold(pcm, sw, boundary)) < 0 ||
	    (err = snd_pcm_sw_params_set_avail_min(pcm, sw, period_frames)) < 0 ||
	    (err = snd_pcm_sw_params_set_tstamp_mode(pcm, sw, SND_PCM_TSTAMP_ENABLE)) < 0 ||
	    (err = snd_pcm_sw_params_set_tstamp_type(pcm, sw, SND_PCM_TSTAMP_TYPE_MONOTONIC)) < 0 ||
	    (err = snd_pcm_sw_params(pcm, sw)) < 0)
		return alsa_result(err);
	return SPUD_SUCCESS;
}

// --------------------------------------------------------------------------
// Streams
// --------------------------------------------------------------------------

static void alsa_set_terminal(struct spudaudio_stream_t *s, SPUDRESULT r) {
	atomic_store(&s->error, (int)r);
	atomic_store(&s->state, r == SPUDRESULT_SAUD_DEVICE_LOST ? SPUDAUDIO_STREAM_STATE_DEVICE_LOST : SPUDAUDIO_STREAM_STATE_ERROR);
}

static void alsa_free_stream(struct spudaudio_stream_t *s) {
	if (s->pcm) {
		if (s->original_chmap)
			snd_pcm_set_chmap(s->pcm, s->original_chmap);
		snd_pcm_close(s->pcm);
	}
	if (s->stop_fd >= 0)
		close(s->stop_fd);
	if (s->startup_initialized)
		sem_destroy(&s->startup);
	free(s->original_chmap);
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
	r = alsa_check_thread(&desc->thread);
	if (SPUDFAIL(r))
		return r;

	struct spudaudio_stream_t *s = (struct spudaudio_stream_t *)calloc(1, sizeof(*s));
	if (!s)
		return SPUDRESULT_OUT_OF_MEMORY;
	s->stop_fd             = -1;
	s->direction           = desc->device->direction;
	s->callback            = desc->callback;
	s->user_data           = desc->user_data;
	s->priority            = desc->thread.priority;
	s->sched_fifo_priority = desc->thread.sched_fifo_priority;
	atomic_init(&s->state, SPUDAUDIO_STREAM_STATE_STOPPED);
	atomic_init(&s->error, SPUD_SUCCESS);
	atomic_init(&s->underflow_count, 0);
	atomic_init(&s->overflow_count, 0);
	atomic_init(&s->audio_thread_known, false);

	r = alsa_open(desc, &s->config, &s->original_chmap, &s->pcm);
	if (SPUD_SUCCESS == r)
		r = alsa_set_sw_params(s->pcm, s->config.period_frames);
	if (SPUD_SUCCESS == r) {
		// hw_params already left the PCM PREPARED; this makes that
		// explicit and is a no-op if so.
		int err = snd_pcm_prepare(s->pcm);
		if (err < 0)
			r = alsa_result(err);
	}
	if (SPUD_SUCCESS == r) {
		const uint32_t channels = s->config.format.channel_count;
		const uint32_t period   = s->config.period_frames;
		s->sample_bytes         = spudaudio_sample_format_byte_size(s->config.format.sample_format);
		s->frame_bytes          = s->sample_bytes * channels;
		s->scratch              = (uint8_t *)calloc(period, s->frame_bytes);
		s->planes               = (void **)calloc(channels, sizeof(void *));
		s->stop_fd              = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
		if (!s->scratch || !s->planes)
			r = SPUDRESULT_OUT_OF_MEMORY;
		else if (s->stop_fd < 0 || sem_init(&s->startup, 0, 0) != 0)
			r = SPUDRESULT_API_SPECIFIC_FAILURE;
		else
			s->startup_initialized = true;
		if (SPUD_SUCCESS == r)
			for (uint32_t ch = 0; ch < channels; ++ch)
				s->planes[ch] = s->scratch + (size_t)ch * period * s->sample_bytes;
	}
	if (SPUDFAIL(r)) {
		alsa_free_stream(s);
		return r;
	}

	*out_stream = s;
	return SPUD_SUCCESS;
}

// --------------------------------------------------------------------------
// Audio thread
// --------------------------------------------------------------------------

static SPUDRESULT alsa_apply_priority(const struct spudaudio_stream_t *s) {
	if (s->priority != SPUDAUDIO_THREAD_PRIORITY_SCHED_FIFO)
		return SPUD_SUCCESS;
	struct sched_param param = {0};
	param.sched_priority     = s->sched_fifo_priority;
	int err                  = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
	if (err == 0)
		return SPUD_SUCCESS;
	return err == EPERM ? SPUDRESULT_SAUD_THREAD_PRIORITY_DENIED : SPUDRESULT_API_SPECIFIC_FAILURE;
}

static uint64_t alsa_ns(const snd_htimestamp_t *t) {
	return (uint64_t)t->tv_sec * 1000000000ULL + (uint64_t)t->tv_nsec;
}

/*
 * Host time of the next frame to transfer, from snd_pcm_status (htstamp is
 * CLOCK_MONOTONIC — set in alsa_set_sw_params). Output: the next frame
 * written plays after the `delay` frames already queued. Input: the next
 * frame read is the oldest unread one, captured `delay` frames ago.
 */
static void alsa_fill_host_time(const struct spudaudio_stream_t *s, SPUDAUDIO_CALLBACK_INFO *info) {
	snd_pcm_status_t *status;
	snd_pcm_status_alloca(&status);
	if (snd_pcm_status(s->pcm, status) < 0 || snd_pcm_status_get_state(status) != SND_PCM_STATE_RUNNING)
		return;
	snd_htimestamp_t stamp;
	snd_pcm_status_get_htstamp(status, &stamp);
	const uint64_t now = alsa_ns(&stamp);
	if (now == 0)
		return;
	snd_pcm_sframes_t delay = snd_pcm_status_get_delay(status);
	uint64_t offset         = delay > 0 ? (uint64_t)delay * 1000000000ULL / s->config.format.sample_rate : 0;
	if (s->direction == SPUDAUDIO_DIRECTION_OUTPUT)
		info->host_time_ns = now + offset;
	else if (offset <= now)
		info->host_time_ns = now - offset;
	else
		return;
	info->flags |= SPUDAUDIO_CALLBACK_FLAG_HOST_TIME_VALID;
}

// Moves `frames` between scratch and the PCM, finishing short transfers.
static int alsa_io(struct spudaudio_stream_t *s, uint32_t frames) {
	const bool output = s->direction == SPUDAUDIO_DIRECTION_OUTPUT;
	uint32_t done     = 0;
	while (done < frames) {
		snd_pcm_sframes_t n;
		if (s->config.format.interleaved) {
			uint8_t *at = s->scratch + (size_t)done * s->frame_bytes;
			n           = output ? snd_pcm_writei(s->pcm, at, frames - done) : snd_pcm_readi(s->pcm, at, frames - done);
		} else {
			void *at[SPUDAUDIO_MAX_CHANNELS];
			for (uint32_t ch = 0; ch < s->config.format.channel_count; ++ch)
				at[ch] = (uint8_t *)s->planes[ch] + (size_t)done * s->sample_bytes;
			n = output ? snd_pcm_writen(s->pcm, at, frames - done) : snd_pcm_readn(s->pcm, at, frames - done);
		}
		if (n == -EAGAIN) {
			snd_pcm_wait(s->pcm, 100);
			continue;
		}
		if (n < 0)
			return (int)n;
		done += (uint32_t)n;
	}
	return 0;
}

// One period through the callback: output calls it then writes, input
// reads then calls it. The callback always gets exactly one period.
static int alsa_transfer(struct spudaudio_stream_t *s, SPUDAUDIO_CALLBACK_INFO *info) {
	const uint32_t period       = s->config.period_frames;
	void *frames                = s->config.format.interleaved ? (void *)s->scratch : (void *)s->planes;
	info->device_position_frames = s->frames_transferred;
	if (s->pending_discontinuity) {
		info->flags |= SPUDAUDIO_CALLBACK_FLAG_DISCONTINUITY;
		s->pending_discontinuity = false;
	}
	int err;
	if (s->direction == SPUDAUDIO_DIRECTION_OUTPUT) {
		s->callback(s, frames, period, info, s->user_data);
		err = alsa_io(s, period);
	} else {
		err = alsa_io(s, period);
		if (err == 0)
			s->callback(s, frames, period, info, s->user_data);
	}
	if (err == 0)
		s->frames_transferred += period;
	return err;
}

// Output: prefill the whole buffer through the callback, then start. The
// start threshold is the boundary (alsa_set_sw_params), so the PCM only
// ever starts here. Prefill runs before the device clock does, so those
// callbacks carry no host time.
static SPUDRESULT alsa_start_device(struct spudaudio_stream_t *s) {
	if (s->direction == SPUDAUDIO_DIRECTION_OUTPUT) {
		uint32_t periods = s->config.buffer_frames / s->config.period_frames;
		for (uint32_t i = 0; i < (periods ? periods : 1); ++i) {
			SPUDAUDIO_CALLBACK_INFO info = {0};
			int err                      = alsa_transfer(s, &info);
			if (err < 0)
				return alsa_result(err);
		}
	}
	int err = snd_pcm_start(s->pcm);
	return err < 0 ? alsa_result(err) : SPUD_SUCCESS;
}

/*
 * -EPIPE is an xrun: the PCM stopped itself (stop_threshold). WASAPI keeps
 * a stream going through a glitch, so ALSA is brought to the same
 * behaviour: count it, advance the stream position by the frames lost
 * (estimated from when the xrun happened — snd_pcm_status' trigger
 * timestamp — to now), re-prepare and restart, and flag the next callback.
 * -ESTRPIPE is a system suspend: resume if the driver can, otherwise the
 * same re-prepare. Anything else is a real failure.
 */
static SPUDRESULT alsa_recover(struct spudaudio_stream_t *s, int err) {
	if (err == -ESTRPIPE) {
		while ((err = snd_pcm_resume(s->pcm)) == -EAGAIN)
			poll(NULL, 0, 100);
		if (err == 0) {
			s->pending_discontinuity = true;
			return SPUD_SUCCESS;
		}
	} else if (err != -EPIPE) {
		return alsa_result(err);
	}

	snd_pcm_status_t *status;
	snd_pcm_status_alloca(&status);
	if (snd_pcm_status(s->pcm, status) == 0 && snd_pcm_status_get_state(status) == SND_PCM_STATE_XRUN) {
		snd_htimestamp_t now, at;
		snd_pcm_status_get_htstamp(status, &now);
		snd_pcm_status_get_trigger_htstamp(status, &at);
		if (alsa_ns(&at) && alsa_ns(&now) > alsa_ns(&at))
			s->frames_transferred += (alsa_ns(&now) - alsa_ns(&at)) * s->config.format.sample_rate / 1000000000ULL;
	}
	if (s->direction == SPUDAUDIO_DIRECTION_OUTPUT)
		atomic_fetch_add(&s->underflow_count, 1);
	else
		atomic_fetch_add(&s->overflow_count, 1);

	err = snd_pcm_prepare(s->pcm);
	if (err < 0)
		return alsa_result(err);
	s->pending_discontinuity = true;
	return alsa_start_device(s);
}

// Transfers every whole period available, recovering from xruns.
static SPUDRESULT alsa_service(struct spudaudio_stream_t *s) {
	for (;;) {
		snd_pcm_sframes_t avail = snd_pcm_avail_update(s->pcm);
		if (avail < 0) {
			SPUDRESULT r = alsa_recover(s, (int)avail);
			if (SPUDFAIL(r))
				return r;
			continue;
		}
		if ((snd_pcm_uframes_t)avail < s->config.period_frames)
			return SPUD_SUCCESS;
		SPUDAUDIO_CALLBACK_INFO info = {0};
		alsa_fill_host_time(s, &info);
		int err = alsa_transfer(s, &info);
		if (err < 0) {
			SPUDRESULT r = alsa_recover(s, err);
			if (SPUDFAIL(r))
				return r;
		}
	}
}

static void *alsa_audio_thread(void *arg) {
	struct spudaudio_stream_t *s = (struct spudaudio_stream_t *)arg;
	s->audio_thread_self         = pthread_self();
	atomic_store(&s->audio_thread_known, true);

	// --- Startup, reported back to spudaudio_stream_start. The poll set is
	// the stop eventfd plus the PCM's own descriptors.
	struct pollfd *fds = NULL;
	int pcm_fds        = 0;
	SPUDRESULT r       = alsa_apply_priority(s);
	if (SPUD_SUCCESS == r) {
		pcm_fds = snd_pcm_poll_descriptors_count(s->pcm);
		fds     = pcm_fds > 0 ? (struct pollfd *)calloc((size_t)pcm_fds + 1, sizeof(struct pollfd)) : NULL;
		if (!fds)
			r = pcm_fds > 0 ? SPUDRESULT_OUT_OF_MEMORY : SPUDRESULT_API_SPECIFIC_FAILURE;
		else if (snd_pcm_poll_descriptors(s->pcm, fds + 1, (unsigned int)pcm_fds) != pcm_fds)
			r = SPUDRESULT_API_SPECIFIC_FAILURE;
	}
	if (SPUD_SUCCESS == r) {
		fds[0].fd     = s->stop_fd;
		fds[0].events = POLLIN;
		r             = alsa_start_device(s);
	}
	if (SPUDFAIL(r)) {
		if (r == SPUDRESULT_SAUD_DEVICE_LOST)
			alsa_set_terminal(s, r);
		snd_pcm_drop(s->pcm);
		snd_pcm_prepare(s->pcm);
		free(fds);
		s->startup_result = r;
		sem_post(&s->startup);
		return NULL;
	}
	atomic_store(&s->state, SPUDAUDIO_STREAM_STATE_RUNNING);
	s->startup_result = SPUD_SUCCESS;
	sem_post(&s->startup);

	// --- Run. snd_pcm_poll_descriptors_revents translates plugin-specific
	// wakeups (dmix timers, ...) into real PCM readiness. The timeout keeps
	// servicing a device that went quiet, which is how a vanished one
	// surfaces (-ENODEV).
	for (;;) {
		int ready = poll(fds, (nfds_t)pcm_fds + 1, 2000);
		if (ready < 0) {
			if (errno == EINTR)
				continue;
			alsa_set_terminal(s, SPUDRESULT_API_SPECIFIC_FAILURE);
			break;
		}
		if (fds[0].revents & POLLIN)
			break; // stop requested
		unsigned short revents = 0;
		if (ready > 0)
			snd_pcm_poll_descriptors_revents(s->pcm, fds + 1, (unsigned int)pcm_fds, &revents);
		if (ready == 0 || revents & (POLLIN | POLLOUT | POLLERR)) {
			r = alsa_service(s);
			if (SPUDFAIL(r)) {
				alsa_set_terminal(s, r);
				break;
			}
		}
	}

	// --- Stop: discard what's queued and leave the PCM PREPARED, ready for
	// a later start.
	snd_pcm_drop(s->pcm);
	snd_pcm_prepare(s->pcm);
	unsigned int running = SPUDAUDIO_STREAM_STATE_RUNNING;
	atomic_compare_exchange_strong(&s->state, &running, SPUDAUDIO_STREAM_STATE_STOPPED);
	free(fds);
	return NULL;
}

static bool alsa_on_audio_thread(struct spudaudio_stream_t *s) {
	return atomic_load(&s->audio_thread_known) && pthread_equal(pthread_self(), s->audio_thread_self);
}

static void alsa_join_thread(struct spudaudio_stream_t *s) {
	pthread_join(s->thread, NULL);
	s->has_thread = false;
	atomic_store(&s->audio_thread_known, false);
}

SPUDRESULT spudaudio_stream_start(spudaudio_stream stream) {
	if (!stream)
		return SPUDRESULT_SAUD_INVALID_STREAM;
	if (alsa_on_audio_thread(stream))
		return SPUDRESULT_GENERAL_FAILURE; // would deadlock
	unsigned int state = atomic_load(&stream->state);
	if (state == SPUDAUDIO_STREAM_STATE_RUNNING)
		return SPUD_SUCCESS;
	if (state == SPUDAUDIO_STREAM_STATE_DEVICE_LOST || state == SPUDAUDIO_STREAM_STATE_ERROR)
		return (SPUDRESULT)atomic_load(&stream->error);

	uint64_t drain;
	while (read(stream->stop_fd, &drain, sizeof(drain)) > 0) {
	} // clear a stop left over from the previous run
	stream->frames_transferred    = 0;
	stream->pending_discontinuity = false;
	if (pthread_create(&stream->thread, NULL, alsa_audio_thread, stream) != 0)
		return SPUDRESULT_API_SPECIFIC_FAILURE;
	stream->has_thread = true;

	while (sem_wait(&stream->startup) != 0 && errno == EINTR) {
	}
	if (SPUDFAIL(stream->startup_result)) {
		alsa_join_thread(stream);
		return stream->startup_result;
	}
	return SPUD_SUCCESS;
}

SPUDRESULT spudaudio_stream_stop(spudaudio_stream stream) {
	if (!stream)
		return SPUDRESULT_SAUD_INVALID_STREAM;
	if (alsa_on_audio_thread(stream))
		return SPUDRESULT_GENERAL_FAILURE; // would deadlock
	if (!stream->has_thread)
		return SPUD_SUCCESS; // never started, or already stopped
	const uint64_t one = 1;
	ssize_t written    = write(stream->stop_fd, &one, sizeof(one));
	(void)written; // an eventfd write only fails at counter overflow
	alsa_join_thread(stream);
	return SPUD_SUCCESS;
}

void spudaudio_destroy_stream(spudaudio_stream stream) {
	if (!stream)
		return;
	spudaudio_stream_stop(stream);
	alsa_free_stream(stream);
}

SPUDRESULT spudaudio_stream_get_config(
    spudaudio_stream stream,
    SPUDAUDIO_STREAM_CONFIG *out_config) {
	if (!out_config)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	if (!stream)
		return SPUDRESULT_SAUD_INVALID_STREAM;
	*out_config = stream->config; // fixed at creation
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

// snd_pcm_delay: output, frames until a newly written frame is heard;
// input, frames between capture and a read. alsa-lib's PCM calls are
// internally locked (its default thread-safe mode), so this is safe
// alongside the audio thread.
SPUDRESULT spudaudio_stream_get_latency_frames(
    spudaudio_stream stream,
    uint32_t *out_latency_frames) {
	if (!out_latency_frames)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	*out_latency_frames = 0;
	if (!stream)
		return SPUDRESULT_SAUD_INVALID_STREAM;
	snd_pcm_sframes_t delay = 0;
	int err                 = snd_pcm_delay(stream->pcm, &delay);
	if (err < 0)
		return alsa_result(err);
	*out_latency_frames = delay > 0 ? (uint32_t)delay : 0;
	return SPUD_SUCCESS;
}

// ALSA has no device-wide rate: each opened PCM sets its own in
// hw_params.
SPUDRESULT spudaudio_set_device_sample_rate(spudaudio_device device, uint32_t sample_rate) {
	(void)sample_rate;
	if (!device)
		return SPUDRESULT_SAUD_INVALID_DEVICE;
	return SPUDRESULT_SAUD_SAMPLE_RATE_NOT_SETTABLE;
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

#endif // SPUDAUDIO_COMPILE_ALSA
