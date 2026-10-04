
#ifndef SPUDPERF_H
#define SPUDPERF_H

#include "stdint.h"

#if __cplusplus
extern "C" {
#endif

uint64_t spudperf_catch_current_clock_cycle();
void spudperf_get_ram_usage(
    uint64_t *current_ram_usage,
    uint64_t *peak_ram_usage);

uint64_t spudperf_get_current_time_milliseconds();

// Host monotonic clock in nanoseconds, from an arbitrary epoch. This is the
// same clock the platform audio APIs timestamp against, so it's directly
// comparable with SPUDAUDIO_CALLBACK_INFO::host_time_ns:
//   Windows  QueryPerformanceCounter    (WASAPI u64QPCPosition)
//   macOS    CLOCK_UPTIME_RAW           (mach_absolute_time, CoreAudio
//                                        AudioTimeStamp.mHostTime) — stops
//                                        while the system sleeps
//   Linux    CLOCK_MONOTONIC            (PipeWire pw_time.now, ALSA
//                                        SND_PCM_TSTAMP_TYPE_MONOTONIC)
uint64_t spudperf_get_monotonic_time_ns(void);

#if __cplusplus
}
#endif

#endif // SPUDPERF_H
