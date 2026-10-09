# SpudAudio

Thin audio endpoint I/O: WASAPI (Windows), ALSA (Linux; PipeWire planned), CoreAudio
(macOS, planned). The public API and its per-backend mappings are documented in
`include/spudaudio.h`; this file holds design notes that don't belong in the header.

## Future intent: positional (spatialized) audio

> **Status: future intent — not implemented, and not part of SpudAudio.** Spatialization
> is rendering policy, so under SpudLib's zero-policy rule it lives *above* SpudLib —
> in whatever calls it. SpudAudio's only part is
> reporting which speaker each device channel feeds
> (`SPUDAUDIO_DEVICE_PROPERTIES.native_format.positions`, and the granted
> `SPUDAUDIO_STREAM_CONFIG.format.positions`), so the layer above can render for the
> real speaker layout.

Motivating case: an application plays a sound effect from a direction relative to
where something happened in its viewport.

1. **Direction** — the clicked point relative to the camera: azimuth (left/right),
   elevation (up/down), distance. Computed by whoever owns the camera and the clicked
   geometry, above SpudLib.
2. **Spatializer** — turns that direction into a signal per output channel, mixed into
   the stream callback's buffer:

| Method | What it does | Fits when |
|---|---|---|
| **Stereo panning** (constant-power) | Click left in the viewport → louder in the left speaker | Stereo speakers/headphones. Simplest; plenty for a UI click. |
| **Binaural / HRTF** | Filters the sound the way a head and ears would, so it seems to come from a direction — including above, below, behind | Headphones, when elevation/depth should actually be audible |
| **VBAP / multichannel panning** | Distributes the sound across the nearest real speakers | 5.1 / 7.1 / Atmos setups |
| **Ambisonics** | Encodes the sound into a sound field first, then decodes it to whatever output exists | Many simultaneous sources, head tracking, or one mix for every kind of output |

For one effect per placement, **direct panning to the device's actual layout** is the
right tool: `FL FR` → stereo panning, `FL FR FC LFE RL RR SL SR` → 7.1 panning, no
layout → a fallback the caller chooses. Ambisonics earns its complexity only with many
concurrent sources or a moving listener (VR).

Two practical constraints when this gets built:

- **Latency matters more than the panning method.** A click sound must land within
  roughly 20–30 ms of the click or it feels detached — favouring WASAPI's low-latency
  shared path (`IAudioClient3`) and small periods. `SPUDAUDIO_CALLBACK_INFO::host_time_ns`
  (same clock as `spudperf_get_monotonic_time_ns`) lets the mixer schedule the sound
  against the actual click time.
- **It doubles as an accessibility feature** — hearing *where* something landed is a
  cue in its own right.

### Why ambisonics isn't a channel position

Every `SPUDAUDIO_CHANNEL_POSITION` names a speaker. Ambisonic channels (W
omnidirectional, X/Y/Z front-back/left-right/up-down; (order + 1)² channels in total)
describe a sound field and are each mixed into *every* speaker by a decoder, so
labelling them as positions would misdescribe them. CoreAudio's ambisonic labels
therefore read back as `AUX`. If ambisonic streams are ever carried as such, they need
their own per-format layout kind (speaker positions vs. ambisonic order + channel
ordering + normalization), not more position labels.
