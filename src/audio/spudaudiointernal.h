#ifndef SPUDLIB_SPUDAUDIOINTERNAL_H
#define SPUDLIB_SPUDAUDIOINTERNAL_H

#include "spudaudio.h"

#if __cplusplus
extern "C" {
#endif

// Backend-independent SPUDAUDIO_STREAM_DESC checks shared by every
// backend's probe and create. Doesn't check `callback` (the probe ignores
// it) or whether buffer_frames == 0 is required/forbidden (that's a
// per-backend fact) — only the buffer >= period half.
SPUDRESULT spudaudio_validate_stream_desc(const SPUDAUDIO_STREAM_DESC *desc);

// True when the format carries a channel layout (positions[0] isn't
// UNSPECIFIED). Only meaningful on a format that passed validation.
bool spudaudio_format_has_layout(const SPUDAUDIO_FORMAT *format);

// Last labeled (speaker) position; FL..this is the speaker range.
#define SPUDAUDIO_CHANNEL_POSITION_LAST_SPEAKER SPUDAUDIO_CHANNEL_POSITION_BRC

// The layout rules from spudaudio.h: all UNSPECIFIED, or all specified
// with no duplicates except NA, and MONO only on a single channel.
bool spudaudio_validate_positions(const SPUDAUDIO_FORMAT *format);

#if __cplusplus
}
#endif

#endif // SPUDLIB_SPUDAUDIOINTERNAL_H
