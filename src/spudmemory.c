
#include "spudmemory.h"
#include "spudcore.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct smem_arena_t {
	uint64_t reserve_size;
	uint64_t commit_size;
	uint64_t pos;
	uint64_t commit_pos; // Size
} smem_arena_t;

SPUDRESULT smem_arena_create(uint64_t reserve_size, uint64_t commit_size, smem_arena *out_arena) {
	if (!out_arena)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	if (reserve_size == 0 && commit_size == 0)
		return SPUDRESULT_ZERO_SIZE;
	uint32_t page_size    = smem_plat_get_pagesize();
	reserve_size          = SMEM_ALIGN_UP_POW2(reserve_size, page_size);
	commit_size           = SMEM_ALIGN_UP_POW2(commit_size, page_size);
	smem_arena_t *pResult = (smem_arena_t *)smem_plat_reserve(reserve_size);
	if (!smem_plat_commit(pResult, commit_size)) {
		return SPUDRESULT_SMEM_PLAT_COMMIT_FAIL;
	}
	pResult->reserve_size = reserve_size;
	pResult->commit_size  = commit_size;
	pResult->pos          = SMEM_ARENA_BASE_POS;
	pResult->commit_pos   = commit_size;
	*out_arena            = pResult;
	return SPUD_SUCCESS;
}
void smem_arena_destroy(smem_arena arena) { free(arena); }

uint64_t smem_arena_get_reserve_size(smem_arena arena) { return arena ? arena->reserve_size : 0; }
uint64_t smem_arena_get_commit_size(smem_arena arena) { return arena ? arena->commit_size : 0; }

SPUDRESULT smem_arena_push(smem_arena arena, uint64_t size, bool non_zero, void **out_ptr) {
	if (!arena)
		return SPUDRESULT_SMEM_INVALID_ARENA;
	if (!out_ptr)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	if (size == 0)
		return SPUD_SUCCESS;
	uint64_t pos_aligned = SMEM_ALIGN_UP_POW2(arena->pos, SMEM_ARENA_ALIGN);
	uint64_t new_pos     = pos_aligned + size;
	if (new_pos > arena->reserve_size)
		return SPUDRESULT_SMEM_INSUFFICIENT_CAPACITY;
	if (new_pos > arena->commit_pos) {
		uint64_t new_commit_pos = new_pos;
		new_commit_pos += arena->commit_size - 1;
		new_commit_pos -= new_commit_pos % arena->commit_size;
		new_commit_pos = new_commit_pos < arena->reserve_size ? new_commit_pos : arena->reserve_size;
		uint8_t *mem         = (uint8_t *)arena + arena->commit_pos;
		uint64_t commit_size = new_commit_pos - arena->commit_pos;
		if (!smem_plat_commit(mem, commit_size)) {
			return SPUDRESULT_SMEM_PLAT_COMMIT_FAIL;
		}
		arena->commit_pos=new_commit_pos;
	}
	arena->pos   = new_pos;
	uint8_t *out = (uint8_t *)arena + pos_aligned;
	if (!non_zero)
		memset(out, 0, size);
	*out_ptr = out;
	return SPUD_SUCCESS;
}
SPUDRESULT smem_arena_pop(smem_arena arena, uint64_t size) {
	if (!arena)
		return SPUDRESULT_SMEM_INVALID_ARENA;
	if (size == 0)
		return SPUD_SUCCESS;
	uint64_t max_size = arena->pos - SMEM_ARENA_BASE_POS;
	size = size < max_size ? size : max_size;
	arena->pos -= size;
	return SPUD_SUCCESS;
}
SPUDRESULT smem_arena_pop_to(smem_arena arena, uint64_t pos) {
	if (!arena)
		return SPUDRESULT_SMEM_INVALID_ARENA;
	uint64_t size = pos < arena->pos ? arena->pos - pos : 0;
	return smem_arena_pop(arena, size);
}
void smem_arena_clear(smem_arena arena) { smem_arena_pop_to(arena, SMEM_ARENA_BASE_POS); }

#if SPUDLIB_PLATFORM_WINDOWS
#include <Windows.h>
uint32_t smem_plat_get_pagesize(void) {
	SYSTEM_INFO sysinfo = {0};
	GetSystemInfo(&sysinfo);
	return sysinfo.dwPageSize;
}

void *smem_plat_reserve(uint64_t size) { return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE); }
bool smem_plat_commit(void *ptr, uint64_t size) {
	void *result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
	return result != NULL;
}
bool smem_plat_decommit(void *ptr, uint64_t size) { return VirtualFree(ptr, size, MEM_DECOMMIT); }
bool smem_plat_release(void *ptr, uint64_t size) { return VirtualFree(ptr, size, MEM_RELEASE); }
#endif

#ifdef __cplusplus
}
#endif
