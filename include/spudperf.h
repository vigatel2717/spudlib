/**
 * @file spudperf.h
 * @brief SpudPerf: the platform's clocks and the process's memory counters.
 *
 * Each call reads one value from the operating system or the CPU and returns
 * it. Nothing is measured, averaged or kept between calls, and no call takes
 * a handle: there is no state to own. Every function may be called from any
 * thread.
 *
 * No function here returns a SPUDRESULT. The clocks cannot fail, and
 * spudperf_get_ram_usage() reports a failed read as 0.
 */

#ifndef SPUDPERF_H
#define SPUDPERF_H

#include "stdint.h"

#if __cplusplus
extern "C" {
#endif

/**
 * @brief Reads the CPU's free-running hardware counter.
 *
 * - x86 and x86-64: the time stamp counter (`rdtsc`).
 * - 64-bit ARM: the generic timer's virtual count (`CNTVCT_EL0`).
 *
 * Both are fixed-frequency counters, not a count of core clock cycles, and
 * the frequency is not reported here and differs between machines. The value
 * is good for comparing two readings taken on the same machine, and nothing
 * else: it is not a time in any unit. For a time, use
 * spudperf_get_monotonic_time_ns().
 *
 * @return The counter's current value, in ticks of that counter.
 */
uint64_t spudperf_catch_current_clock_cycle();

/**
 * @brief Reads how much physical memory the calling process is using.
 *
 * What the platform counts differs:
 *
 * | Platform | Current | Peak |
 * |---|---|---|
 * | Windows | Working set size | Peak working set size |
 * | Linux | `VmRSS` in `/proc/self/status` | `VmHWM` in `/proc/self/status` |
 * | Apple | Physical footprint (`phys_footprint`) | Peak resident size (`resident_size_peak`) |
 *
 * On Apple platforms the two are different measures, so the peak is not
 * necessarily the largest value the current figure has had.
 *
 * Either parameter may be NULL to skip that value.
 *
 * @param[out] current_ram_usage Receives the memory in use now, in bytes, or
 *                               0 if the platform's query failed. May be
 *                               NULL.
 * @param[out] peak_ram_usage    Receives the most memory the process has
 *                               used, in bytes, or 0 if the platform's query
 *                               failed. May be NULL.
 */
void spudperf_get_ram_usage(
    uint64_t *current_ram_usage,
    uint64_t *peak_ram_usage);

/**
 * @brief Reads a monotonic clock, in milliseconds.
 *
 * The epoch is arbitrary, so only the difference between two readings means
 * anything. The clock never goes backwards and is not the time of day.
 *
 * - Windows: `QueryPerformanceCounter`.
 * - Linux and Apple: `CLOCK_MONOTONIC`.
 *
 * On Apple platforms this is not the clock
 * spudperf_get_monotonic_time_ns() reads: `CLOCK_MONOTONIC` keeps counting
 * while the system sleeps and `CLOCK_UPTIME_RAW` does not, so the two drift
 * apart across a sleep. Take both readings of a difference from the same
 * function.
 *
 * @return Milliseconds since the clock's epoch.
 */
uint64_t spudperf_get_current_time_milliseconds();

/**
 * @brief Reads the host's monotonic clock, in nanoseconds.
 *
 * The epoch is arbitrary, so only the difference between two readings means
 * anything. This is the clock the platform audio APIs timestamp against, so
 * a reading is directly comparable with
 * SPUDAUDIO_CALLBACK_INFO::host_time_ns:
 *
 * | Platform | Clock | The audio timestamp on the same clock |
 * |---|---|---|
 * | Windows | `QueryPerformanceCounter` | WASAPI `u64QPCPosition` |
 * | Apple | `CLOCK_UPTIME_RAW` (`mach_absolute_time`) | CoreAudio `AudioTimeStamp.mHostTime` |
 * | Linux | `CLOCK_MONOTONIC` | PipeWire `pw_time.now`, ALSA `SND_PCM_TSTAMP_TYPE_MONOTONIC` |
 *
 * On Apple platforms this clock stops while the system sleeps.
 *
 * @return Nanoseconds since the clock's epoch.
 */
uint64_t spudperf_get_monotonic_time_ns(void);

#if __cplusplus
}
#endif

#endif // SPUDPERF_H
