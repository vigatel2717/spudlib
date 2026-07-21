
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

#if __cplusplus
}
#endif

#endif // SPUDPERF_H
