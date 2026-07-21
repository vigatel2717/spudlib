
#if SPUDLIB_PLATFORM_WIN32

#include "spudmemory.h"

#include <Windows.h>

#if __cplusplus
extern "C" {
#endif

uint32_t smem_plat_get_pagesize(void) {
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return (uint32_t)info.dwPageSize;
}

void *smem_plat_reserve(uint64_t size) {
	return VirtualAlloc(NULL, (SIZE_T)size, MEM_RESERVE, PAGE_NOACCESS);
}

bool smem_plat_commit(void *ptr, uint64_t size) {
	return VirtualAlloc(ptr, (SIZE_T)size, MEM_COMMIT, PAGE_READWRITE) != NULL;
}

bool smem_plat_decommit(void *ptr, uint64_t size) {
	return VirtualFree(ptr, (SIZE_T)size, MEM_DECOMMIT) != 0;
}

bool smem_plat_release(void *ptr, uint64_t size) {
	(void)size; // MEM_RELEASE requires the original base pointer and a size of 0.
	return VirtualFree(ptr, 0, MEM_RELEASE) != 0;
}

#if __cplusplus
}
#endif

#endif // SPUDLIB_PLATFORM_WIN32
