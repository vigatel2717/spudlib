
#ifndef SPUDMEMORY_H
#define SPUDMEMORY_H

/*
 * Much of this code is inspired by a YouTuber named Magicalbat
 * He hates malloc() and free() with a passion
 * Checkout his memory arena code video here:
 * https://www.youtube.com/watch?v=jgiMagdjA1s
 */

#include <spudcore.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define SMEM_KiB(n) ((uint64_t)(n) << 10)
#define SMEM_MiB(n) ((uint64_t)(n) << 20)
#define SMEM_GiB(n) ((uint64_t)(n) << 30)

#define SMEM_ALIGN_UP_POW2(n, p) (((uint64_t)(n) + ((uint64_t)(p) - 1)) & (~(uint64_t)(p) - 1))
#define SMEM_ARENA_BASE_POS (sizeof(smem_arena_t))
#define SMEM_ARENA_ALIGN (sizeof(void *))

typedef struct smem_arena_t *smem_arena;

SPUDRESULT smem_arena_create(
    uint64_t reserve_size,
    uint64_t commit_size,
    smem_arena *out_arena);

void smem_arena_destroy(smem_arena arena);
uint64_t smem_arena_get_reserve_size(smem_arena arena);
uint64_t smem_arena_get_commit_size(smem_arena arena);

SPUDRESULT smem_arena_push(
    smem_arena arena,
    uint64_t size,
    bool non_zero,
    void **out_ptr);
SPUDRESULT smem_arena_pop(
    smem_arena arena,
    uint64_t size);
SPUDRESULT smem_arena_pop_to(
    smem_arena arena,
    uint64_t pos);
void smem_arena_clear(smem_arena arena);

#define SMEM_PUSH_STRUCT(arena, ptr) smem_arena_push(arena, sizeof(*ptr), false, ptr)
#define SMEM_PUSH_STRUCT_NZ(arena, ptr) smem_arena_push(arena, sizeof(*ptr), true, ptr)
#define SMEM_PUSH_ARRAY(arena, n, ptr) smem_arena_push(arena, sizeof(*ptr) * (n), false, ptr)
#define SMEM_PUSH_ARRAY_NZ(arena, n, ptr) smem_arena_push(arena, sizeof(*ptr) * (n), true, ptr)

uint32_t smem_plat_get_pagesize(void);

void *smem_plat_reserve(uint64_t size);
bool smem_plat_commit(
    void *ptr,
    uint64_t size);
bool smem_plat_decommit(
    void *ptr,
    uint64_t size);
bool smem_plat_release(
    void *ptr,
    uint64_t size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SPUDMEMORY_H
