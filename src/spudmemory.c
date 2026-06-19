
#include "spudmemory.h"
#include "spudcore.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct smem_chunk_t {
	uint64_t _ptr_begin;
	uint64_t _capacity;
	uint64_t _size;
} smem_chunk_t;

typedef struct smem_arena_t {
	smem_chunk_t *_chunks;
	uint32_t _chunk_count;
	bool _autogrow;
} smem_arena_t;

static uint32_t smem_chunk_index_for_available_continuous_space(
    smem_arena arena, uint64_t space_size) {
	if (!arena)
		return SPUD_UINT32_MAX;
	for (size_t i = 0; i < arena->_chunk_count; ++i) {
		smem_chunk_t *chunk = &arena->_chunks[i];
		if ((chunk->_capacity - chunk->_size) > space_size) {
			return i; // Chunk [Index].
		}
	}
	return arena->_chunk_count; // [Index] Allocate new chunk at end of chunks.
}
static void
smem_alloc_new_chunk(smem_arena arena, uint64_t chunk_initial_capacity) {
	if (!(arena && chunk_initial_capacity))
		return;
	const uint32_t new_count = arena->_chunk_count + 1;
	arena->_chunks =
	    realloc(arena->_chunks, sizeof(smem_chunk_t) * (new_count));
	arena->_chunk_count                     = new_count;
	arena->_chunks[new_count - 1]._capacity = chunk_initial_capacity;
	arena->_chunks[new_count - 1]._ptr_begin =
	    (uint64_t)malloc((size_t)chunk_initial_capacity);
	arena->_chunks[new_count - 1]._size = 0;
}

#ifdef __cplusplus
extern "C" {
#endif
smem_arena smem_arena_create(uint64_t initial_capacity, bool autogrow) {
	smem_arena_t *result = (smem_arena_t *)calloc(1, sizeof(smem_arena_t));
	result->_autogrow    = autogrow;
	if (initial_capacity == 0) {
		result->_chunks      = NULL;
		result->_chunk_count = 0;
	} else {
		smem_chunk_t chunk = {0};
		chunk._capacity    = initial_capacity;
		chunk._ptr_begin   = (uint64_t)malloc((size_t)initial_capacity);
		chunk._size        = 0;
		result->_chunks    = (smem_chunk_t *)malloc(sizeof(smem_chunk_t));
		memcpy(result->_chunks, &chunk, sizeof(smem_chunk_t));
		result->_chunk_count = 1;
	}
	return result;
}
void smem_arena_destroy(smem_arena arena) {
	if (!arena)
		return;
	for (size_t i = 0; i < arena->_chunk_count; ++i)
		free((void *) arena->_chunks[i]._ptr_begin);
	free(arena->_chunks);
	free(arena);
}
uint64_t smem_arena_get_current_capacity(smem_arena arena) {
	if (!arena)
		return 0;
	uint64_t result = 0;
	for (uint32_t i = 0; i < arena->_chunk_count; ++i)
		result += arena->_chunks[i]._capacity;
	return result;
}
uint64_t smem_arena_get_current_size(smem_arena arena) {
	if (!arena)
		return 0;
	uint64_t result = 0;
	for (uint32_t i = 0; i < arena->_chunk_count; ++i)
		result += arena->_chunks[i]._size;
	return result;
}
bool smem_arena_is_autogrow(smem_arena arena) {
	return arena ? arena->_autogrow : false;
}
void smem_arena_reserve(smem_arena arena, uint64_t add_capacity) {
	if (!arena)
		return;
}

void *smem_alloc(smem_arena arena, uint64_t size) {
	if (!arena)
		return NULL;
	uint32_t chunk_index =
	    smem_chunk_index_for_available_continuous_space(arena, size);
	if (chunk_index == arena->_chunk_count) {
		if (!arena->_autogrow)
			return NULL;
		smem_alloc_new_chunk(arena, size);
		return NULL;
	} else {
		smem_chunk_t *chunk = &arena->_chunks[chunk_index];
		chunk->_size += size;
		return (void *) (chunk->_ptr_begin + chunk->_size);
	}
}
void *smem_calloc(smem_arena arena, uint64_t size) {
	void *result = smem_alloc(arena, size);
	memset(result, 0, size);
	return result;
}
void smem_zero(smem_arena arena) {
	if (!arena)
		return;
	for (uint32_t i = 0; i < arena->_chunk_count; ++i) {
		smem_chunk_t *chunk = &arena->_chunks[i];
		memset((void *) (chunk->_ptr_begin), 0, chunk->_capacity);
		chunk->_size = 0;
	}
}
#ifdef __cplusplus
}
#endif
