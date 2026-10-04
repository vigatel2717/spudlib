//
// SAUD = SpudAudio
//

#ifndef SPUDLIB_SPUDAUDIO_H
#define SPUDLIB_SPUDAUDIO_H

#include "spudcore.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*
 * SpudAudio  —  thin audio endpoint I/O (WASAPI / PipeWire-ALSA / CoreAudio)
 *
 * Moves PCM frames between the caller and an audio endpoint — a translation
 * of the platform audio API, nothing more. No mixer, no decoder, no
 * resampler, no voice/sound objects, no 3D spatialisation, no volume policy.
 * All of that is the caller's problem (ApricotFields), the same way SpudGPU
 * never decides what to draw.
 *
 * - Never picks a device. The OS default endpoints are reported as a
 *   property (default_roles); the caller decides whether to use them.
 * - Never silently converts formats. If the endpoint can't take the
 *   requested format, stream creation fails with
 *   SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED. Platform-side conversion (WASAPI
 *   AUTOCONVERTPCM, ALSA plugins) is only enabled when the caller
 *   explicitly sets SPUDAUDIO_STREAM_FLAG_ALLOW_OS_CONVERSION, and refused
 *   where it can't happen — see SPUDAUDIO_STREAM_FLAGS.
 * - The data path is a callback, because that's the only model all three
 *   platforms share natively (CoreAudio has no acquire/release buffer API).
 *   On WASAPI/ALSA the backend owns the event-wait thread that drives it —
 *   plumbing, not policy.
 */

/*
 * Compile-time configuration. spudlib's CMake defines one
 * SPUDAUDIO_PLATFORM_* for the target OS and a SPUDAUDIO_COMPILE_* for each
 * native audio API built in, as PUBLIC definitions — so hosts see the same
 * values and know at compile time which SPUDAUDIO_NATIVE_API values
 * spudaudio_create_instance accepts, rather than finding
 * SPUDRESULT_INVALID_API at runtime. Anything not defined is 0.
 *
 *   SPUDAUDIO_PLATFORM_WINDOWS  SPUDAUDIO_COMPILE_WASAPI
 *   SPUDAUDIO_PLATFORM_LINUX    SPUDAUDIO_COMPILE_ALSA
 *                               (SPUDAUDIO_COMPILE_PIPEWIRE: planned)
 *   SPUDAUDIO_PLATFORM_MACOS    SPUDAUDIO_COMPILE_COREAUDIO
 *   SPUDAUDIO_PLATFORM_IOS      none yet — iOS and iPadOS
 *   SPUDAUDIO_PLATFORM_WATCHOS  none yet
 *
 * SPUDAUDIO_PLATFORM_APPLE is any of the three Apple platforms. With no
 * SPUDAUDIO_COMPILE_* at all, spudlib builds an argument-checking stub
 * whose calls return SPUDRESULT_NOT_IMPLEMENTED_YET.
 */
#ifndef SPUDAUDIO_PLATFORM_WINDOWS
#define SPUDAUDIO_PLATFORM_WINDOWS 0
#endif
#ifndef SPUDAUDIO_PLATFORM_LINUX
#define SPUDAUDIO_PLATFORM_LINUX 0
#endif
#ifndef SPUDAUDIO_PLATFORM_MACOS
#define SPUDAUDIO_PLATFORM_MACOS 0
#endif
#ifndef SPUDAUDIO_PLATFORM_IOS
#define SPUDAUDIO_PLATFORM_IOS 0
#endif
#ifndef SPUDAUDIO_PLATFORM_WATCHOS
#define SPUDAUDIO_PLATFORM_WATCHOS 0
#endif
#define SPUDAUDIO_PLATFORM_APPLE (SPUDAUDIO_PLATFORM_MACOS || SPUDAUDIO_PLATFORM_IOS || SPUDAUDIO_PLATFORM_WATCHOS)

#ifndef SPUDAUDIO_COMPILE_WASAPI
#define SPUDAUDIO_COMPILE_WASAPI 0
#endif
#ifndef SPUDAUDIO_COMPILE_ALSA
#define SPUDAUDIO_COMPILE_ALSA 0
#endif
#ifndef SPUDAUDIO_COMPILE_PIPEWIRE
#define SPUDAUDIO_COMPILE_PIPEWIRE 0
#endif
#ifndef SPUDAUDIO_COMPILE_COREAUDIO
#define SPUDAUDIO_COMPILE_COREAUDIO 0
#endif

#if SPUDAUDIO_PLATFORM_WINDOWS + SPUDAUDIO_PLATFORM_LINUX + SPUDAUDIO_PLATFORM_MACOS + SPUDAUDIO_PLATFORM_IOS + SPUDAUDIO_PLATFORM_WATCHOS > 1
#error "SpudAudio: more than one SPUDAUDIO_PLATFORM_* is defined"
#endif
#if SPUDAUDIO_COMPILE_WASAPI && !SPUDAUDIO_PLATFORM_WINDOWS
#error "SpudAudio: SPUDAUDIO_COMPILE_WASAPI needs SPUDAUDIO_PLATFORM_WINDOWS"
#endif
#if (SPUDAUDIO_COMPILE_ALSA || SPUDAUDIO_COMPILE_PIPEWIRE) && !SPUDAUDIO_PLATFORM_LINUX
#error "SpudAudio: SPUDAUDIO_COMPILE_ALSA / _PIPEWIRE need SPUDAUDIO_PLATFORM_LINUX"
#endif
#if SPUDAUDIO_COMPILE_COREAUDIO && !SPUDAUDIO_PLATFORM_MACOS
#error "SpudAudio: SPUDAUDIO_COMPILE_COREAUDIO (the HAL) needs SPUDAUDIO_PLATFORM_MACOS"
#endif

typedef struct spudaudio_instance_t *spudaudio_instance;
typedef struct spudaudio_device_t *spudaudio_device;
typedef struct spudaudio_stream_t *spudaudio_stream;

typedef uint32_t SPUDAUDIO_NATIVE_API;
enum {
	SPUDAUDIO_NATIVE_API_NONE      = 0,
	SPUDAUDIO_NATIVE_API_WASAPI    = 1,
	SPUDAUDIO_NATIVE_API_PIPEWIRE  = 2,
	SPUDAUDIO_NATIVE_API_ALSA      = 3,
	SPUDAUDIO_NATIVE_API_COREAUDIO = 4
};

typedef uint32_t SPUDAUDIO_DIRECTION;
enum {
	SPUDAUDIO_DIRECTION_OUTPUT = 0, // render / playback
	SPUDAUDIO_DIRECTION_INPUT  = 1  // capture
};

/*
 * Sample formats, all native-endian. 24-bit audio has three real layouts
 * and each one is its own format, because the same "24 in 32" name means
 * different bits on different platforms:
 *
 *   S24_PACKED  3 bytes per sample.
 *               WASAPI 24/24 PCM, ALSA SND_PCM_FORMAT_S24_3LE,
 *               CoreAudio packed 24-bit
 *   S24_32_MSB  24 bits in the high 3 bytes of a 32-bit container, low
 *               byte zero — bit-identical to an S32 sample with 24
 *               significant bits.
 *               WASAPI 32-bit container / 24 valid bits (WASAPI's only
 *               24-in-32), ALSA SND_PCM_FORMAT_S32 (no separate ALSA name),
 *               CoreAudio kAudioFormatFlagIsAlignedHigh
 *   S24_32_LSB  24 bits in the low 3 bytes of a 32-bit container, high
 *               byte ignored.
 *               ALSA SND_PCM_FORMAT_S24, CoreAudio 24 bits unaligned-high
 *               in 32. WASAPI has no equivalent.
 */
typedef uint32_t SPUDAUDIO_SAMPLE_FORMAT;
enum {
	SPUDAUDIO_SAMPLE_FORMAT_UNKNOWN    = 0,
	SPUDAUDIO_SAMPLE_FORMAT_S16        = 1,
	SPUDAUDIO_SAMPLE_FORMAT_S24_PACKED = 2,
	SPUDAUDIO_SAMPLE_FORMAT_S24_32_MSB = 3,
	SPUDAUDIO_SAMPLE_FORMAT_S24_32_LSB = 4,
	SPUDAUDIO_SAMPLE_FORMAT_S32        = 5,
	SPUDAUDIO_SAMPLE_FORMAT_F32        = 6
};

// @return Bytes per sample, or 0 for SPUDAUDIO_SAMPLE_FORMAT_UNKNOWN or an
// unrecognised value.
uint32_t spudaudio_sample_format_byte_size(SPUDAUDIO_SAMPLE_FORMAT fmt);

/*
 * Channel positions — one per channel, in channel order. An array rather
 * than WASAPI's speaker bitmask because it's the representation every
 * backend can express: ALSA (snd_pcm_chmap_t) and PipeWire take position
 * arrays natively, CoreAudio an AudioChannelLayout of per-channel labels,
 * and a mask can't describe unlabeled channels or a non-canonical order.
 *
 * The labels are the union of what the platforms use — PipeWire's set,
 * which is ALSA's full set plus LFE2, and which contains all 18 WASAPI
 * speakers and nearly all CoreAudio speaker labels. Names follow
 * PipeWire/ALSA: "R" is rear (WASAPI calls it back), and BC/BLC/BRC are
 * *bottom* positions (WASAPI's back center is RC). The 18 WASAPI speakers
 * come first, in WASAPI's bit order.
 *
 *   NA          A channel that exists but carries nothing (ALSA/PipeWire
 *               NA — common in HDMI maps). The only position that may
 *               repeat.
 *   AUX0..63    Unlabeled channels that do carry signal (ALSA
 *               SND_CHMAP_UNKNOWN, PipeWire AUXn, WASAPI channels beyond
 *               the mask, CoreAudio Discrete_n).
 *
 * A format either has no layout (every position UNSPECIFIED) or a full
 * one (none UNSPECIFIED, no duplicates except NA, MONO only for 1
 * channel).
 *
 * A label a backend can't express makes the layout unsupported there —
 * never relabeled. The probe's suggestion then drops the layout (all
 * UNSPECIFIED), the one choice every backend accepts:
 *   WASAPI     only MONO, FL..TRR, plus NA/AUX as trailing unassigned
 *              channels (WASAPI can't tell NA from AUX; both read back as
 *              AUX). Speakers must also be in ascending bit order with
 *              NA/AUX last — when only the order is wrong, the suggestion
 *              is the reordered layout instead.
 *   ALSA       everything except LFE2
 *   CoreAudio  its non-speaker labels (matrix Lt/Rt, ambisonic W/X/Y/Z,
 *              haptic) have no position and read back as AUX
 */
#define SPUDAUDIO_MAX_CHANNELS 64

typedef uint8_t SPUDAUDIO_CHANNEL_POSITION;
enum {
	SPUDAUDIO_CHANNEL_POSITION_UNSPECIFIED = 0,
	SPUDAUDIO_CHANNEL_POSITION_MONO        = 1,

	// The 18 WASAPI speakers, in dwChannelMask bit order (bit = pos - FL).
	SPUDAUDIO_CHANNEL_POSITION_FL  = 2,  // front left
	SPUDAUDIO_CHANNEL_POSITION_FR  = 3,  // front right
	SPUDAUDIO_CHANNEL_POSITION_FC  = 4,  // front center
	SPUDAUDIO_CHANNEL_POSITION_LFE = 5,  // low-frequency effects
	SPUDAUDIO_CHANNEL_POSITION_RL  = 6,  // rear left       (WASAPI back left)
	SPUDAUDIO_CHANNEL_POSITION_RR  = 7,  // rear right      (WASAPI back right)
	SPUDAUDIO_CHANNEL_POSITION_FLC = 8,  // front left of center
	SPUDAUDIO_CHANNEL_POSITION_FRC = 9,  // front right of center
	SPUDAUDIO_CHANNEL_POSITION_RC  = 10, // rear center     (WASAPI back center)
	SPUDAUDIO_CHANNEL_POSITION_SL  = 11, // side left
	SPUDAUDIO_CHANNEL_POSITION_SR  = 12, // side right
	SPUDAUDIO_CHANNEL_POSITION_TC  = 13, // top center
	SPUDAUDIO_CHANNEL_POSITION_TFL = 14, // top front left
	SPUDAUDIO_CHANNEL_POSITION_TFC = 15, // top front center
	SPUDAUDIO_CHANNEL_POSITION_TFR = 16, // top front right
	SPUDAUDIO_CHANNEL_POSITION_TRL = 17, // top rear left   (WASAPI top back left)
	SPUDAUDIO_CHANNEL_POSITION_TRC = 18, // top rear center (WASAPI top back center)
	SPUDAUDIO_CHANNEL_POSITION_TRR = 19, // top rear right  (WASAPI top back right)

	// The rest of the PipeWire/ALSA set.
	SPUDAUDIO_CHANNEL_POSITION_RLC  = 20, // rear left of center
	SPUDAUDIO_CHANNEL_POSITION_RRC  = 21, // rear right of center
	SPUDAUDIO_CHANNEL_POSITION_FLW  = 22, // front left wide
	SPUDAUDIO_CHANNEL_POSITION_FRW  = 23, // front right wide
	SPUDAUDIO_CHANNEL_POSITION_LFE2 = 24, // second LFE (PipeWire, CoreAudio)
	SPUDAUDIO_CHANNEL_POSITION_FLH  = 25, // front left high
	SPUDAUDIO_CHANNEL_POSITION_FCH  = 26, // front center high
	SPUDAUDIO_CHANNEL_POSITION_FRH  = 27, // front right high
	SPUDAUDIO_CHANNEL_POSITION_TFLC = 28, // top front left of center
	SPUDAUDIO_CHANNEL_POSITION_TFRC = 29, // top front right of center
	SPUDAUDIO_CHANNEL_POSITION_TSL  = 30, // top side left
	SPUDAUDIO_CHANNEL_POSITION_TSR  = 31, // top side right
	SPUDAUDIO_CHANNEL_POSITION_LLFE = 32, // left LFE
	SPUDAUDIO_CHANNEL_POSITION_RLFE = 33, // right LFE
	SPUDAUDIO_CHANNEL_POSITION_BC   = 34, // bottom center
	SPUDAUDIO_CHANNEL_POSITION_BLC  = 35, // bottom left center
	SPUDAUDIO_CHANNEL_POSITION_BRC  = 36, // bottom right center

	SPUDAUDIO_CHANNEL_POSITION_NA       = 63,  // present but silent; may repeat
	SPUDAUDIO_CHANNEL_POSITION_AUX0     = 64,  // AUX0 + n, n < 64
	SPUDAUDIO_CHANNEL_POSITION_AUX_LAST = 127
};

typedef struct SPUDAUDIO_FORMAT {
	SPUDAUDIO_SAMPLE_FORMAT sample_format;
	uint32_t sample_rate;   // Hz
	uint32_t channel_count; // 1..SPUDAUDIO_MAX_CHANNELS
	// Only the first channel_count entries are meaningful.
	SPUDAUDIO_CHANNEL_POSITION positions[SPUDAUDIO_MAX_CHANNELS];
	bool interleaved;       // false = planar (one buffer per channel)
} SPUDAUDIO_FORMAT;

/*
 * Instance / device enumeration
 */

/**
 * @brief Creates a SpudAudio instance bound to one native audio API.
 * @param[in] api The native API to bind to. There is no "best available"
 * value — the caller chooses.
 * @param[out] out_instance The returned spudaudio_instance.
 * @return SPUD_SUCCESS, SPUDRESULT_INVALID_API if `api` isn't compiled in
 * on this platform, or another SPUDRESULT.
 */
SPUDRESULT spudaudio_create_instance(
    SPUDAUDIO_NATIVE_API api,
    spudaudio_instance *out_instance);
SPUDRESULT spudaudio_destroy_instance(spudaudio_instance instance);

// @return The instance's native API, or SPUDAUDIO_NATIVE_API_NONE if
// instance is NULL.
SPUDAUDIO_NATIVE_API spudaudio_get_native_api(spudaudio_instance instance);

/**
 * @brief Enumerates the endpoints of one direction.
 * @param[in] instance The SpudAudio instance to enumerate through.
 * @param[in] direction Output (render) or input (capture) endpoints.
 * @param[out] out_devices Array of device handles, owned by the instance.
 * The array stays valid until the next enumeration of the same direction;
 * the handles in it stay valid until the instance is destroyed, and the
 * same endpoint keeps the same handle across enumerations.
 * @param[out] out_device_count Number of devices written to out_devices.
 */
SPUDRESULT spudaudio_enumerate_devices(
    spudaudio_instance instance,
    SPUDAUDIO_DIRECTION direction,
    spudaudio_device **out_devices,
    uint32_t *out_device_count);

/*
 * Which OS default(s) an endpoint currently is. Platforms keep several
 * defaults, not one, so this is a set of roles; a backend only ever sets
 * the roles its platform has.
 *
 *   GENERAL         WASAPI eConsole (Windows' "Default Device"), CoreAudio
 *                   kAudioHardwarePropertyDefault{Output,Input}Device,
 *                   PipeWire default.audio.{sink,source}, ALSA "default"
 *   MULTIMEDIA      WASAPI eMultimedia. Windows' settings UI moves it
 *                   together with eConsole, but the API keeps it separate.
 *   COMMUNICATIONS  WASAPI eCommunications ("Default Communication Device")
 *   SYSTEM_SOUNDS   CoreAudio kAudioHardwarePropertyDefaultSystemOutputDevice
 *                   (alerts / UI sounds; output only)
 */
typedef uint32_t SPUDAUDIO_DEFAULT_ROLES;
enum {
	SPUDAUDIO_DEFAULT_ROLE_NONE           = 0x0,
	SPUDAUDIO_DEFAULT_ROLE_GENERAL        = 0x1,
	SPUDAUDIO_DEFAULT_ROLE_MULTIMEDIA     = 0x2,
	SPUDAUDIO_DEFAULT_ROLE_COMMUNICATIONS = 0x4,
	SPUDAUDIO_DEFAULT_ROLE_SYSTEM_SOUNDS  = 0x8
};

typedef struct SPUDAUDIO_DEVICE_PROPERTIES {
	// Human-readable, UTF-8, truncated to fit.
	//   WASAPI  PKEY_Device_FriendlyName
	//   ALSA    the PCM's DESC hint (lines joined), else its name
	char name[128];
	char persistent_id[256]; // stable across runs (IMMDevice ID / ALSA PCM
	                         // name / PipeWire node name / CoreAudio UID) so
	                         // the caller can persist its choice
	SPUDAUDIO_DIRECTION direction;
	// The default roles this endpoint holds right now. Informational;
	// SpudAudio never acts on it.
	SPUDAUDIO_DEFAULT_ROLES default_roles;
	// The endpoint's own shared-mode format (WASAPI GetMixFormat). Zeroed
	// where the backend has no single native format (ALSA).
	SPUDAUDIO_FORMAT native_format;
	// Whether the backend has an exclusive mode at all (WASAPI yes, ALSA
	// no). Whether this device currently allows it is only known by
	// probing: SPUDRESULT_SAUD_EXCLUSIVE_UNAVAILABLE.
	bool supports_exclusive;
	// Period/buffer limits aren't here: every backend's limits depend on the
	// format and sharing mode — see spudaudio_get_timing_caps.
} SPUDAUDIO_DEVICE_PROPERTIES;

SPUDRESULT spudaudio_get_device_properties(
    spudaudio_device device,
    SPUDAUDIO_DEVICE_PROPERTIES *out_properties);

/*
 * Sets a device-wide sample rate, where the platform has one. This changes
 * the device for every application using it, which is why it's a separate,
 * explicit call: stream creation never changes it, and a stream format at
 * another rate is simply FORMAT_NOT_SUPPORTED with the device's current
 * rate as the suggestion.
 *
 *   CoreAudio  kAudioDevicePropertyNominalSampleRate. `rate` must be in
 *              kAudioDevicePropertyAvailableNominalSampleRates, else
 *              FORMAT_NOT_SUPPORTED. The HAL applies it asynchronously;
 *              this returns once the device reports the new rate. A device
 *              another process holds in hog mode returns
 *              EXCLUSIVE_UNAVAILABLE.
 *   WASAPI     not settable — the shared-mode rate is a user setting with
 *              no API, and exclusive streams carry their own rate.
 *   ALSA       not settable — the rate is per opened PCM (hw_params).
 * Not settable: SPUDRESULT_SAUD_SAMPLE_RATE_NOT_SETTABLE.
 *
 * A running CoreAudio stream whose device rate changes — through this call
 * or any other application — ends in SPUDAUDIO_STREAM_STATE_ERROR with
 * error SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED: its granted format no longer
 * describes the device.
 */
SPUDRESULT spudaudio_set_device_sample_rate(spudaudio_device device, uint32_t sample_rate);

/*
 * Streams
 */

/*
 * EXCLUSIVE            Sole use of the endpoint. See EXCLUSIVE_UNAVAILABLE
 *                      under spudaudio_probe_stream.
 * ALLOW_OS_CONVERSION  Lets the platform convert between the requested
 *                      format and the endpoint's. Without it the format
 *                      must match exactly. Where conversion can't exist
 *                      for this request, the flag is refused, never
 *                      ignored:
 *   WASAPI     shared mode: AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM +
 *              SRC_DEFAULT_QUALITY. Exclusive mode has no conversion:
 *              SPUDRESULT_SAUD_OS_CONVERSION_UNAVAILABLE.
 *   ALSA       conversion lives in plugins, so it comes from the PCM name
 *              the caller picked (plughw:, default, ...); the flag turns
 *              on snd_pcm_hw_params_set_rate_resample. PCMs that never
 *              convert (hw:, dmix, dsnoop, dshare) refuse it with
 *              SPUDRESULT_SAUD_OS_CONVERSION_UNAVAILABLE.
 *   CoreAudio  not implemented yet (needs AUHAL or an AudioConverter):
 *              SPUDRESULT_NOT_IMPLEMENTED_YET.
 *   PipeWire   (planned) the graph converts natively.
 */
typedef uint32_t SPUDAUDIO_STREAM_FLAGS;
enum {
	SPUDAUDIO_STREAM_FLAG_NONE                = 0x0,
	SPUDAUDIO_STREAM_FLAG_EXCLUSIVE           = 0x1,
	SPUDAUDIO_STREAM_FLAG_ALLOW_OS_CONVERSION = 0x2
};

/*
 * How binding a requested period is. This differs by backend, not just
 * the range, so it's reported rather than hidden:
 *
 *   UNKNOWN     Not determined — the zero value, e.g. in a probe
 *               suggestion that stopped before the period was checked.
 *   FIXED       Only one period exists (min == max == default).
 *               WASAPI shared via IAudioClient.
 *   EXACT       Granted exactly as requested if spudaudio_probe_stream
 *               accepts it. WASAPI shared via IAudioClient3, WASAPI
 *               exclusive, ALSA, CoreAudio.
 *   NEGOTIATED  A request only — a shared graph picks the actual period
 *               and may change it while the stream runs. PipeWire.
 *
 * In every mode, the callback's `frame_count` is the authoritative current
 * period and never exceeds SPUDAUDIO_STREAM_CONFIG::max_callback_frames.
 */
typedef uint32_t SPUDAUDIO_PERIOD_MODE;
enum {
	SPUDAUDIO_PERIOD_MODE_UNKNOWN    = 0,
	SPUDAUDIO_PERIOD_MODE_FIXED      = 1,
	SPUDAUDIO_PERIOD_MODE_EXACT      = 2,
	SPUDAUDIO_PERIOD_MODE_NEGOTIATED = 3
};

/*
 * Timing limits for one device + format + sharing mode. A guide, not a
 * guarantee: some constraints can't be expressed as min/max/step (WASAPI
 * exclusive buffer alignment, ALSA driver-specific rules) and only show up
 * when the OS is asked about a concrete request. spudaudio_probe_stream is
 * the authoritative check on every backend.
 *
 *   WASAPI     IAudioClient::GetDevicePeriod (100 ns, converted at the
 *              format's rate) / IAudioClient3::GetSharedModeEnginePeriod
 *   ALSA       snd_pcm_hw_params_get_{period,buffer}_size_{min,max} after
 *              format/rate/channels are applied
 *   CoreAudio  kAudioDevicePropertyBufferFrameSizeRange
 *   PipeWire   clock.min-quantum / clock.max-quantum / clock.quantum
 */
typedef struct SPUDAUDIO_TIMING_CAPS {
	SPUDAUDIO_PERIOD_MODE period_mode;
	uint32_t min_period_frames;
	uint32_t max_period_frames;
	// Valid periods are the multiples of granularity within [min, max].
	// WASAPI3's fundamental period; 1 where the platform reports no step.
	uint32_t period_granularity_frames;
	uint32_t default_period_frames; // OS-reported default, 0 if none (ALSA)
	// Range for SPUDAUDIO_STREAM_DESC::buffer_frames. Both 0 when the
	// backend has no buffer separate from the period (CoreAudio, PipeWire).
	uint32_t min_buffer_frames;
	uint32_t max_buffer_frames;
	// frame_count may differ from the period between callbacks (WASAPI
	// shared padding, CoreAudio variable-buffer devices, PipeWire).
	bool variable_callback_frames;
} SPUDAUDIO_TIMING_CAPS;

SPUDRESULT spudaudio_get_timing_caps(
    spudaudio_device device,
    const SPUDAUDIO_FORMAT *format,
    SPUDAUDIO_STREAM_FLAGS flags, // exclusive vs shared changes the answer
    SPUDAUDIO_TIMING_CAPS *out_caps);

/*
 * Per-callback glitch flags. A stream is single-direction, so one
 * DISCONTINUITY flag covers both cases: on an OUTPUT stream it means the
 * device ran dry (underflow) before this callback; on an INPUT stream it
 * means captured data was dropped (overflow). The same events also bump
 * SPUDAUDIO_STREAM_STATUS's counters, for callers that only poll.
 *
 *   DISCONTINUITY  WASAPI AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY /
 *                  ALSA -EPIPE xrun / PipeWire xrun / CoreAudio overload
 *   SILENT         WASAPI AUDCLNT_BUFFERFLAGS_SILENT (capture) — the OS says
 *                  the packet is silence; SpudAudio zero-fills `frames` so
 *                  the data matches the flag
 *   HOST_TIME_VALID host_time_ns is meaningful. Clear when the platform
 *                  gives no timestamp for this buffer or flags it bad
 *                  (WASAPI AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR)
 */
typedef uint32_t SPUDAUDIO_CALLBACK_FLAGS;
enum {
	SPUDAUDIO_CALLBACK_FLAG_NONE            = 0x0,
	SPUDAUDIO_CALLBACK_FLAG_DISCONTINUITY   = 0x1,
	SPUDAUDIO_CALLBACK_FLAG_SILENT          = 0x2,
	SPUDAUDIO_CALLBACK_FLAG_HOST_TIME_VALID = 0x4
};

typedef struct SPUDAUDIO_CALLBACK_INFO {
	SPUDAUDIO_CALLBACK_FLAGS flags;

	// Position of this buffer's first frame on the stream's sample clock.
	// Starts at 0 on each spudaudio_stream_start and is monotonic within a
	// run; after a DISCONTINUITY it jumps forward by the frames lost (as
	// measured by WASAPI, or estimated from the xrun's timestamp on ALSA).
	// WASAPI u64DevicePosition / IAudioClock, CoreAudio
	// AudioTimeStamp.mSampleTime, PipeWire pw_time.ticks, ALSA status.
	uint64_t device_position_frames;

	// Host monotonic clock in nanoseconds — the same clock as
	// spudperf_get_monotonic_time_ns() — at which this buffer's first frame
	// was captured (INPUT) or will reach the endpoint (OUTPUT). Only valid
	// when SPUDAUDIO_CALLBACK_FLAG_HOST_TIME_VALID is set.
	uint64_t host_time_ns;
} SPUDAUDIO_CALLBACK_INFO;

// Called on the platform's audio thread. For OUTPUT, fill `frames`; for
// INPUT, read them. `frame_count` can differ between calls (WASAPI shared
// padding, PipeWire quantum changes) — never assume it equals the
// requested period — but never exceeds the stream's max_callback_frames,
// so scratch buffers can be sized once up front. Must not block or
// allocate — that's the caller's contract, SpudAudio doesn't guard it.
// `info` is only valid for the duration of the call.
typedef void (*SPUDAUDIO_STREAM_CALLBACK)(
    spudaudio_stream stream,
    void *frames,          // interleaved: one buffer; planar: void*[channels]
    uint32_t frame_count,
    const SPUDAUDIO_CALLBACK_INFO *info,
    void *user_data);

/*
 * Scheduling for the thread that runs the stream callback. It names the
 * platform *mechanism* and the caller supplies its parameters, because any
 * mapping from abstract tiers ("audio", "pro audio") to a SCHED_FIFO number
 * or an MMCSS task would be SpudAudio deciding. UNSPECIFIED (0) is
 * rejected, so a zeroed desc never silently gets a default.
 *
 *   NORMAL      SpudAudio's own thread at OS-default scheduling.
 *               WASAPI, ALSA.
 *   MMCSS       WASAPI: AvSetMmThreadCharacteristicsW(mmcss_task) +
 *               AvSetMmThreadPriority(mmcss_relative_priority).
 *   SCHED_FIFO  ALSA: pthread_setschedparam(SCHED_FIFO,
 *               sched_fifo_priority). Needs RLIMIT_RTPRIO / CAP_SYS_NICE;
 *               rtkit is not used (it would add a D-Bus dependency).
 *   PLATFORM    The platform's own real-time thread runs the callback and
 *               SpudAudio has no thread to configure. CoreAudio (HAL IOProc)
 *               and PipeWire (data loop), once those backends exist.
 *
 * A mechanism the backend doesn't have fails probe/create with
 * SPUDRESULT_SAUD_THREAD_PRIORITY_UNSUPPORTED. An elevation the OS refuses
 * when the thread starts (no MMCSS task of that name, no RT permission)
 * fails spudaudio_stream_start with SPUDRESULT_SAUD_THREAD_PRIORITY_DENIED
 * — never silently run at a lower priority; retrying with NORMAL is the
 * caller's call.
 */
typedef uint32_t SPUDAUDIO_THREAD_PRIORITY;
enum {
	SPUDAUDIO_THREAD_PRIORITY_UNSPECIFIED = 0,
	SPUDAUDIO_THREAD_PRIORITY_NORMAL      = 1,
	SPUDAUDIO_THREAD_PRIORITY_MMCSS       = 2,
	SPUDAUDIO_THREAD_PRIORITY_SCHED_FIFO  = 3,
	SPUDAUDIO_THREAD_PRIORITY_PLATFORM    = 4
};

#define SPUDAUDIO_MAX_MMCSS_TASK_NAME 64

typedef struct SPUDAUDIO_THREAD_DESC {
	SPUDAUDIO_THREAD_PRIORITY priority;
	// MMCSS only: a task under HKLM\SOFTWARE\Microsoft\Windows NT\
	// CurrentVersion\Multimedia\SystemProfile\Tasks, e.g. "Pro Audio",
	// "Audio". UTF-8, copied at creation.
	const char *mmcss_task;
	// MMCSS only: AVRT_PRIORITY, -2 (very low) .. 2 (critical).
	int32_t mmcss_relative_priority;
	// SCHED_FIFO only: within sched_get_priority_min/max(SCHED_FIFO),
	// 1..99 on Linux.
	int32_t sched_fifo_priority;
} SPUDAUDIO_THREAD_DESC;

typedef struct SPUDAUDIO_STREAM_DESC {
	spudaudio_device device;
	SPUDAUDIO_FORMAT format;
	uint32_t period_frames; // required — no "pick something sensible" value

	// Total frames the OS queues for this stream: sets latency, while
	// period_frames sets how often the callback runs. Must be >=
	// period_frames, and is typically a whole multiple of it.
	//   WASAPI     Shared mode (IAudioClient3, Windows 10 1607+): no
	//              separate buffer, must be 0. Shared mode on older
	//              Windows: hnsBufferDuration. Exclusive mode:
	//              hnsBufferDuration, and must equal period_frames (the
	//              device double-buffers internally).
	//   ALSA       snd_pcm_hw_params buffer_size
	//   CoreAudio  no separate buffer — the I/O buffer is the period
	//   PipeWire   no client buffer — only node.latency (the period)
	// Must be 0 exactly when spudaudio_get_timing_caps reports
	// min/max_buffer_frames of 0; any mismatch fails with
	// SPUDRESULT_SAUD_BUFFER_OUT_OF_RANGE rather than being ignored.
	uint32_t buffer_frames;
	SPUDAUDIO_STREAM_FLAGS flags;
	SPUDAUDIO_THREAD_DESC thread; // required — see SPUDAUDIO_THREAD_PRIORITY
	SPUDAUDIO_STREAM_CALLBACK callback;
	void *user_data;
} SPUDAUDIO_STREAM_DESC;

// What a stream is (or would be) granted. Returned by both
// spudaudio_probe_stream and spudaudio_stream_get_config.
typedef struct SPUDAUDIO_STREAM_CONFIG {
	SPUDAUDIO_FORMAT format;
	SPUDAUDIO_PERIOD_MODE period_mode;
	uint32_t period_frames; // NEGOTIATED: the value at creation time
	uint32_t buffer_frames; // 0 on backends with no separate buffer
	// Hard upper bound on the callback's frame_count for the stream's
	// whole lifetime, in every period mode.
	//   WASAPI     IAudioClient::GetBufferSize
	//   ALSA       period_frames (SpudAudio's thread transfers per period)
	//   CoreAudio  kAudioDevicePropertyBufferFrameSize /
	//              kAudioUnitProperty_MaximumFramesPerSlice
	//   PipeWire   clock.max-quantum, or node.max-latency if lower
	uint32_t max_callback_frames;
} SPUDAUDIO_STREAM_CONFIG;

/*
 * Asks the OS whether `desc` would be accepted exactly as written, without
 * creating a stream. The one authoritative check on every backend —
 * constraints that caps can't express (WASAPI exclusive alignment, ALSA
 * driver rules) are only discoverable this way. `callback`/`user_data`
 * are ignored.
 *
 * Each backend really configures the device with the request (WASAPI
 * Initialize on a throwaway IAudioClient, ALSA snd_pcm_hw_params on a
 * freshly opened PCM) and then releases it, so the answer is the OS's,
 * not a SpudAudio model of it. If the OS grants anything other than
 * exactly what was asked, that's a rejection.
 *
 * SPUD_SUCCESS: accepted; out_config is what creation would grant.
 * FORMAT_NOT_SUPPORTED / PERIOD_OUT_OF_RANGE / BUFFER_OUT_OF_RANGE:
 *   rejected. Checking stops at the first rejection, in the order format
 *   -> period -> buffer. One exception: ALSA can only confirm channel
 *   positions once the configuration is installed, so a layout it can't
 *   apply is a FORMAT_NOT_SUPPORTED reported after period and buffer. out_config holds the OS's nearest acceptable
 *   value for every field checked so far (including the rejected one),
 *   and 0 for fields not reached or that the OS offered nothing for — so
 *   resubmitting a suggestion can surface the next constraint. It's a
 *   suggestion only: SpudAudio never applies it.
 *     WASAPI     IsFormatSupported closest match (shared); GetBufferSize
 *                after AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED (exclusive);
 *                GetSharedModeEnginePeriod range/step (IAudioClient3);
 *                whatever Initialize actually granted
 *     ALSA       snd_pcm_hw_params_set_{format_first,channels,rate,
 *                period_size,buffer_size}_near on a scratch hw_params;
 *                the first matching snd_pcm_query_chmaps entry, or the
 *                PCM's current map (snd_pcm_get_chmap), for positions
 *     CoreAudio  BufferFrameSizeRange clamp; device nominal format
 *     PipeWire   clamp to clock.min/max-quantum
 * EXCLUSIVE_UNAVAILABLE: the device is held by another client (WASAPI
 *   AUDCLNT_E_DEVICE_IN_USE / EXCLUSIVE_MODE_NOT_ALLOWED, ALSA -EBUSY), or
 *   the backend has no exclusive mode (ALSA — exclusivity there is the
 *   choice of a hw: device name, not a flag).
 * OS_CONVERSION_UNAVAILABLE: ALLOW_OS_CONVERSION was set where the
 *   platform can't convert for this request (WASAPI exclusive mode, an
 *   ALSA PCM with no conversion plugin). Retry without it.
 * NOT_IMPLEMENTED_YET: ALLOW_OS_CONVERSION on CoreAudio, until it's built.
 * DEVICE_LOST: the device disappeared since enumeration.
 *
 * Probing briefly opens the device (exclusively, in exclusive mode).
 * Acceptance isn't a reservation: creation can still fail if the device
 * changes in between.
 */
SPUDRESULT spudaudio_probe_stream(
    const SPUDAUDIO_STREAM_DESC *desc,
    SPUDAUDIO_STREAM_CONFIG *out_config);

// Same rules as the probe — both run the identical configuration path, so
// a desc the probe accepts creates exactly the probed configuration, and
// one it rejects fails here with the same error. Never rounds or
// substitutes. `callback` is required.
//
// The new stream is configured but STOPPED, and already holds the device
// the way it will while running: an exclusive-mode WASAPI stream owns the
// endpoint, an ALSA hw: PCM is busy to other processes, and an ALSA layout
// is applied to the PCM's channel map (the previous map is restored by
// spudaudio_destroy_stream).
SPUDRESULT spudaudio_create_stream(
    const SPUDAUDIO_STREAM_DESC *desc,
    spudaudio_stream *out_stream);
void spudaudio_destroy_stream(spudaudio_stream stream);

/*
 * Start: on backends with their own thread (WASAPI, ALSA), launches it,
 * applies its priority, and for OUTPUT prefills the device buffer by
 * calling the callback (those prefill calls run before the device clock
 * starts, so they carry no HOST_TIME_VALID) before starting the device.
 * On PLATFORM-thread backends (CoreAudio) it starts the device and the
 * platform's real-time thread pulls from the callback — no prefill. Returns once the stream is
 * RUNNING, or with the reason it couldn't start — priority denied, device
 * lost, ... — in which case it's left STOPPED (or in its terminal state).
 * Starting a RUNNING stream is a no-op; starting one in DEVICE_LOST/ERROR
 * returns that stored error.
 *
 * Stop: stops the device, ends the thread, and returns once no callback is
 * running or will run; the stream can be started again. On a stream that
 * already ended in DEVICE_LOST/ERROR it just reaps the thread.
 *
 * Underruns/overruns don't stop a stream: the backend recovers (WASAPI and
 * CoreAudio do so themselves; ALSA re-prepares and restarts) and the next
 * callback carries SPUDAUDIO_CALLBACK_FLAG_DISCONTINUITY.
 *
 * Start, stop and destroy must not be called from inside the stream's own
 * callback (start/stop return SPUDRESULT_GENERAL_FAILURE there), and calls
 * on one stream must not race each other — the caller serializes them.
 * Destroy stops a running stream first.
 */
SPUDRESULT spudaudio_stream_start(spudaudio_stream stream);
SPUDRESULT spudaudio_stream_stop(spudaudio_stream stream);

// What the stream was granted at creation.
SPUDRESULT spudaudio_stream_get_config(
    spudaudio_stream stream,
    SPUDAUDIO_STREAM_CONFIG *out_config);

// Current end-to-end latency. Can change while running on NEGOTIATED
// backends; for per-buffer timing use SPUDAUDIO_CALLBACK_INFO::host_time_ns.
SPUDRESULT spudaudio_stream_get_latency_frames(
    spudaudio_stream stream,
    uint32_t *out_latency_frames);

/*
 * Stream status — polled, not pushed. A dying device stops the audio
 * thread, so the stream callback can never deliver its own failure, and a
 * failure listener would run on a different platform thread per backend
 * (COM notification thread / CoreAudio listener thread / PipeWire loop /
 * SpudAudio's own thread). Instead the backend records failures atomically
 * and the caller reads them whenever it likes, e.g. once per frame.
 *
 * Nothing is lost between polls: DEVICE_LOST and ERROR are terminal and
 * stay set until the stream is destroyed, and glitches are monotonic
 * counters — diff against the previous poll.
 */
typedef uint32_t SPUDAUDIO_STREAM_STATE;
enum {
	SPUDAUDIO_STREAM_STATE_STOPPED     = 0,
	SPUDAUDIO_STREAM_STATE_RUNNING     = 1,
	SPUDAUDIO_STREAM_STATE_DEVICE_LOST = 2, // terminal: destroy and recreate
	SPUDAUDIO_STREAM_STATE_ERROR       = 3  // terminal: see `error`
};

typedef struct SPUDAUDIO_STREAM_STATUS {
	SPUDAUDIO_STREAM_STATE state;
	SPUDRESULT error;         // cause of DEVICE_LOST / ERROR, else SPUD_SUCCESS
	// Monotonic; one increment per callback that carried
	// SPUDAUDIO_CALLBACK_FLAG_DISCONTINUITY. Only the counter matching the
	// stream's direction ever moves.
	uint64_t underflow_count; // OUTPUT: device ran out of frames
	uint64_t overflow_count;  // INPUT: captured frames were dropped
} SPUDAUDIO_STREAM_STATUS;

// Lock-free; safe from any thread, including inside the stream callback.
SPUDRESULT spudaudio_stream_get_status(
    spudaudio_stream stream,
    SPUDAUDIO_STREAM_STATUS *out_status);

/*
 * Device-change notification — reported, never acted on. Streams on a
 * removed device move to SPUDAUDIO_STREAM_STATE_DEVICE_LOST (see
 * spudaudio_stream_get_status); the caller decides whether and where to
 * reopen them. Runs on an arbitrary platform thread: don't block in it.
 */
typedef uint32_t SPUDAUDIO_DEVICE_EVENT;
enum {
	SPUDAUDIO_DEVICE_EVENT_ADDED           = 0,
	SPUDAUDIO_DEVICE_EVENT_REMOVED         = 1,
	SPUDAUDIO_DEVICE_EVENT_DEFAULT_CHANGED = 2
};

// For DEFAULT_CHANGED, `roles` is the role(s) that now point at
// `persistent_id` (WASAPI reports one role per change, CoreAudio one per
// default property); it's SPUDAUDIO_DEFAULT_ROLE_NONE for ADDED/REMOVED.
typedef void (*SPUDAUDIO_DEVICE_EVENT_CALLBACK)(
    SPUDAUDIO_DEVICE_EVENT event,
    const char *persistent_id,
    SPUDAUDIO_DIRECTION direction,
    SPUDAUDIO_DEFAULT_ROLES roles,
    void *user_data);

SPUDRESULT spudaudio_set_device_event_callback(
    spudaudio_instance instance,
    SPUDAUDIO_DEVICE_EVENT_CALLBACK callback,
    void *user_data);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SPUDLIB_SPUDAUDIO_H
