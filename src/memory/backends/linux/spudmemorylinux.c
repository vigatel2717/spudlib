
#if SPUDLIB_PLATFORM_LINUX

#include "spudmemory.h"

#include <sys/mman.h>
#include <unistd.h>

#if __cplusplus
extern "C" {
#endif

uint32_t smem_plat_get_pagesize(void) {
	return (uint32_t)sysconf(_SC_PAGESIZE);
}

void *smem_plat_reserve(uint64_t size) {
	void *ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	return ptr == MAP_FAILED ? NULL : ptr;
}

bool smem_plat_commit(void *ptr, uint64_t size) {
	return mprotect(ptr, size, PROT_READ | PROT_WRITE) == 0;
}

bool smem_plat_decommit(void *ptr, uint64_t size) {
	// Drop the physical pages but keep the reservation, matching Windows'
	// MEM_DECOMMIT semantics (VirtualAlloc'd address range stays reserved).
	if (madvise(ptr, size, MADV_DONTNEED) != 0)
		return false;
	return mprotect(ptr, size, PROT_NONE) == 0;
}

bool smem_plat_release(void *ptr, uint64_t size) {
	return munmap(ptr, size) == 0;
}

#if __cplusplus
}
#endif

#endif // SPUDLIB_PLATFORM_LINUX
