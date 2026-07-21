
#include "spudperf.h"

#if SPUDLIB_PLATFORM_WIN32
#include <windows.h>
#include <Psapi.h>
#elif SPUDLIB_PLATFORM_LINUX
#include <stdio.h>
#endif

#if __cplusplus
extern "C" {
#endif

uint64_t spudperf_catch_current_clock_cycle() {
#if defined(_MSC_VER)
	return __rdtsc();
#else
	uint32_t lo, hi;
	__asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
	return ((uint64_t)hi << 32) | lo;
#endif
}

void spudperf_get_ram_usage(
    uint64_t *current_ram_usage, uint64_t *peak_ram_usage) {
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

#if __cplusplus
}
#endif
