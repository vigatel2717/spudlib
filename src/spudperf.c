
#include "spudperf.h"

#if SPUDLIB_PLATFORM_WIN32
#include <windows.h>
#include <Psapi.h>
#elif SPUDLIB_PLATFORM_LINUX
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#endif

#if __cplusplus
extern "C" {
#endif

uint64_t spudperf_catch_current_clock_cycle() {
#if defined(_MSC_VER) && !defined(_M_ARM64)
	return __rdtsc();
#elif defined(__aarch64__) || defined(__arm64__)
	// x86's TSC and ARM's generic timer are both just free-running,
	// fixed-frequency hardware counters usable for relative timing (not
	// literal core-clock cycles on either architecture) -- CNTVCT_EL0 is
	// ARM's direct equivalent and, unlike PMCCNTR_EL0, is readable from
	// userspace without special privileges.
	uint64_t val;
	__asm__ volatile("mrs %0, cntvct_el0" : "=r"(val));
	return val;
#else
	uint32_t lo, hi;
	__asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
	return ((uint64_t)hi << 32) | lo;
#endif
}

void spudperf_get_ram_usage(
    uint64_t *current_ram_usage,
    uint64_t *peak_ram_usage) {
#if SPUDLIB_PLATFORM_WIN32
	PROCESS_MEMORY_COUNTERS pmc = {0};
	pmc.cb                      = sizeof(pmc);
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	if (current_ram_usage)
		*current_ram_usage = (uint64_t)pmc.WorkingSetSize;
	if (peak_ram_usage)
		*peak_ram_usage = (uint64_t)pmc.PeakWorkingSetSize;
#elif SPUDLIB_PLATFORM_LINUX
	FILE *f = fopen("/proc/self/status", "r");
	if (!f) {
		if (current_ram_usage)
			*current_ram_usage = 0;
		if (peak_ram_usage)
			*peak_ram_usage = 0;
		return;
	}

	uint64_t rss = 0, hwm = 0;
	char line[128];
	while (fgets(line, sizeof(line), f)) {
		unsigned long kb;
		if (sscanf(line, "VmRSS: %lu kB", &kb) == 1)
			rss = (uint64_t)kb * 1024;
		else if (sscanf(line, "VmHWM: %lu kB", &kb) == 1)
			hwm = (uint64_t)kb * 1024;
	}
	fclose(f);

	if (current_ram_usage)
		*current_ram_usage = rss;
	if (peak_ram_usage)
		*peak_ram_usage = hwm;
#endif
}

uint64_t spudperf_get_current_time_milliseconds() {
#if SPUDLIB_PLATFORM_WIN32
	static LARGE_INTEGER frequency;
	if (frequency.QuadPart == 0)
		QueryPerformanceFrequency(&frequency);
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (uint64_t)((counter.QuadPart * 1000) / frequency.QuadPart);
#elif SPUDLIB_PLATFORM_LINUX
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#else
#error "Unsupported platform"
	return 0;
#endif
}

#if __cplusplus
}
#endif
