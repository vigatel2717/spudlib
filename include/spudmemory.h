
#ifndef SPUDMEMORY_H
#define SPUDMEMORY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct smem_arena_t *smem_arena;

smem_arena smem_arena_create(uint64_t initial_capacity, bool autogrow);
void smem_arena_destroy(smem_arena arena);
uint64_t smem_arena_get_current_capacity(smem_arena arena);
uint64_t smem_arena_get_current_size(smem_arena arena);
bool smem_arena_is_autogrow(smem_arena arena);
void smem_arena_reserve(smem_arena arena, uint64_t add_capacity);

//typedef struct smem_ptr_t *smem_ptr;
//void smem_assign(smem_arena arena, void *obj, smem_ptr *out_assigned_ptr);

void *smem_alloc(smem_arena arena, uint64_t size);
void *smem_calloc(smem_arena arena, uint64_t size);
void smem_zero(smem_arena arena);

/*
// Save/restore position for temporary scratch allocations
typedef struct smem_arena_mark_t *smem_arena_mark;
smem_arena_mark smem_arena_save(smem_arena arena);
void smem_arena_restore(smem_arena arena, smem_arena_mark mark);

// Duplicate a string/buffer into the arena (extremely common need)
char *smem_strdup(smem_arena arena, const char *str);
void *smem_memdup(smem_arena arena, const void *ptr, uint64_t size);

// Aligned allocation (needed for SIMD, GPU upload buffers, etc.)
void *smem_alloc_aligned(smem_arena arena, uint64_t size, uint64_t alignment);
*/
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SPUDMEMORY_H
