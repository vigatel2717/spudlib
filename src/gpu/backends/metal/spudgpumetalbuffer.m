//
// SpudGPU Metal backend - buffers and buffer views.
// Implemented and verified against real Apple hardware.
//

#if SPUDGPU_COMPILE_METAL_API

#include "spudgpu.h"
#include "spudcore.h"
#include "spudgpumetal.h"
#include <Metal/Metal.h>

// Only two storage modes matter on Apple Silicon's unified memory - there is
// no MTLStorageModeManaged here (that mode doesn't exist on Apple Silicon at
// all; it's a discrete-GPU-era mode for genuinely separate CPU/GPU memory).
// HOST_VISIBLE's presence picks Shared vs. Private; HOST_CACHED then picks
// the CPU cache mode within Shared (same tradeoff as a D3D12 upload vs.
// readback heap). HOST_COHERENT doesn't change anything: Shared storage is
// always coherent on unified memory, so there is no non-coherent
// host-visible tier to opt in or out of - see spudgpu_flush_buffer/
// spudgpu_invalidate_buffer below.
static MTLResourceOptions spudgpumetal___internal_buffer_resource_options(SPUDGPU_MEMORY_FLAGS memory_flags) {
	if (!(memory_flags & SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE)) {
		return MTLResourceStorageModePrivate;
	}

	MTLResourceOptions options = MTLResourceStorageModeShared;
	options |= (memory_flags & SPUDGPU_MEMORY_FLAGS_HOST_CACHED)
	               ? MTLResourceCPUCacheModeDefaultCache
	               : MTLResourceCPUCacheModeWriteCombined;
	return options;
}

SPUDRESULT spudgpu_create_buffer(
    spudgpu_device device,
    const spudgpu_buffer_desc *desc,
    spudgpu_buffer *out_buffer) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!desc)
		return SPUDRESULT_NULL_DESC;
	if (!out_buffer)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	if (desc->size == 0)
		return SPUDRESULT_GPU_ZERO_BUFFER_SIZE;
	if (desc->usage == SPUDGPU_BUFFER_USAGE_NONE)
		return SPUDRESULT_GPU_INVALID_BUFFER_USAGE;

	spudgpu_device_metal *device_metal = (spudgpu_device_metal *)device;
	SPUDRESULT sr                      = SPUD_SUCCESS;

	spudgpu_buffer_metal *buffer_metal = (spudgpu_buffer_metal *)calloc(1, sizeof(spudgpu_buffer_metal));
	if (!buffer_metal) {
		sr = SPUDRESULT_GENERAL_FAILURE;
		goto failedattempt;
	}
	buffer_metal->_parent_device = device_metal;
	buffer_metal->_desc          = *desc;

	// heap_flags: SPUDGPU_HEAP_FLAG_ALLOW_SHADER_ATOMICS is a documented
	// no-op on Metal (see spudgpu.h). SPUDGPU_HEAP_FLAG_NOT_ZEROED is also a
	// no-op here - Metal never guarantees zero-initialized contents in the
	// first place (unlike Vulkan/D3D12's default-zeroed allocations), so
	// there's nothing to opt out of. SPUDGPU_HEAP_FLAG_SHARED (cross-
	// process) and SPUDGPU_HEAP_FLAG_CREATE_NOT_RESIDENT have no equivalent
	// on a plain MTLBuffer and are unused - no current caller sets them
	// (mirrors SPUDGPU_IMAGE_USAGE_PRESENTABLE's precedent). buffer_flags'
	// SPUDGPU_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS is likewise unused -
	// Metal buffers were never exclusively owned by one queue to begin with.
	buffer_metal->_buffer_mtl = [device_metal->_device_mtl
	    newBufferWithLength:desc->size
	                 options:spudgpumetal___internal_buffer_resource_options(desc->memory_flags)];
	if (!buffer_metal->_buffer_mtl) {
		sr = SPUDRESULT_API_SPECIFIC_FAILURE;
		goto failedattempt;
	}

	// Every spudgpu_buffer gets a real GPU virtual address after creation
	// regardless of backend (see spudgpu_buffer_desc::gpu_address_location),
	// mirroring D3D12's always-available GetGPUVirtualAddress - Metal's
	// equivalent is -[MTLBuffer gpuAddress], valid for any storage mode.
	buffer_metal->_desc.gpu_address_location = buffer_metal->_buffer_mtl.gpuAddress;

	*out_buffer = (spudgpu_buffer)buffer_metal;

	return sr;

failedattempt:
	free(buffer_metal);
	return sr;
}
void spudgpu_destroy_buffer(spudgpu_buffer buffer) {
	spudgpu_buffer_metal *buffer_metal = (spudgpu_buffer_metal *)buffer;
	if (buffer_metal) {
		if (buffer_metal->_buffer_mtl) {
			[buffer_metal->_buffer_mtl release];
		}
		free(buffer_metal);
	}
}
SPUDRESULT spudgpu_get_buffer_desc(
    spudgpu_buffer buffer,
    spudgpu_buffer_desc *out_desc) {
	spudgpu_buffer_metal *buffer_metal = (spudgpu_buffer_metal *)buffer;
	if (!buffer_metal)
		return SPUDRESULT_GPU_INVALID_BUFFER;
	if (!out_desc)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	*out_desc = buffer_metal->_desc;
	return SPUD_SUCCESS;
}

SPUDRESULT spudgpu_create_buffer_view(
    spudgpu_buffer buffer,
    const spudgpu_buffer_view_desc *desc,
    spudgpu_buffer_view *out_buffer_view) {
	if (!buffer)
		return SPUDRESULT_GPU_INVALID_BUFFER;
	if (!desc)
		return SPUDRESULT_NULL_DESC;
	if (!out_buffer_view)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	if (desc->size == 0)
		return SPUDRESULT_GPU_ZERO_BUFFER_SIZE;

	spudgpu_buffer_metal *parent_buffer_metal = (spudgpu_buffer_metal *)buffer;
	if (desc->offset_from_parent_buffer + desc->size > parent_buffer_metal->_desc.size)
		return SPUDRESULT_GPU_BUFFER_OR_IMAGE_VIEW_RANGE_OUT_OF_SCOPE;

	SPUDRESULT sr = SPUD_SUCCESS;

	spudgpu_buffer_view_metal *result = (spudgpu_buffer_view_metal *)calloc(1, sizeof(spudgpu_buffer_view_metal));
	if (!result) {
		sr = SPUDRESULT_GENERAL_FAILURE;
		goto failedattempt;
	}
	result->_desc               = *desc;
	result->_desc.parent_buffer = buffer; // Authoritative - see spudgpu_buffer_view_metal.

	*out_buffer_view = (spudgpu_buffer_view)result;

	return sr;

failedattempt:
	return sr;
}

void spudgpu_destroy_buffer_view(spudgpu_buffer_view buffer_view) { free(buffer_view); }
SPUDRESULT spudgpu_get_buffer_view_desc(
    spudgpu_buffer_view buffer_view,
    spudgpu_buffer_view_desc *out_desc) {
	if (!buffer_view)
		return SPUDRESULT_GPU_INVALID_BUFFER_VIEW;
	if (!out_desc)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	*out_desc = ((spudgpu_buffer_view_metal *)buffer_view)->_desc;
	return SPUD_SUCCESS;
}

SPUDRESULT spudgpu_map_buffer(
    spudgpu_buffer buffer,
    uint64_t offset,
    uint64_t size,
    void **ppData) {
	if (!buffer)
		return SPUDRESULT_GPU_INVALID_BUFFER;
	if (!ppData)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	spudgpu_buffer_metal *buffer_metal = (spudgpu_buffer_metal *)buffer;

	// Only a Shared-storage buffer (SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE) has a
	// CPU-visible pointer at all - see spudgpumetal___internal_buffer_resource_options.
	if (!(buffer_metal->_desc.memory_flags & SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE))
		return SPUDRESULT_GPU_INVALID_MEMORY_FLAGS;

	uint64_t map_size = (size == 0) ? buffer_metal->_desc.size : size;
	if (offset + map_size > buffer_metal->_desc.size)
		return SPUDRESULT_GPU_MAP_OUT_OF_RANGE;

	// Metal has no map/unmap operation to perform - a Shared-storage
	// buffer's CPU pointer is always available via -contents; "mapping" is
	// just handing back contents + offset.
	*ppData = (uint8_t *)buffer_metal->_buffer_mtl.contents + offset;

	return SPUD_SUCCESS;
}

void spudgpu_unmap_buffer(spudgpu_buffer buffer) {
	// Nothing to unmap - see spudgpu_map_buffer.
	if (!buffer)
		return;
}

// A Shared-storage MTLBuffer on unified memory is always coherent - there is
// no non-coherent host-visible tier to flush writes out of or invalidate CPU
// caches for on this backend (see
// spudgpumetal___internal_buffer_resource_options and spudgpu_create_buffer).
// Both are correctly no-ops here, not an unimplemented gap - mirrors the
// D3D12 backend, which is also a no-op for both, for its own reasons
// (Map()/Unmap()'s range parameters already cover it there).
void spudgpu_flush_buffer(
    spudgpu_buffer buffer,
    uint64_t offset,
    uint64_t size) {
	if (!buffer)
		return;
}

SPUDRESULT spudgpu_invalidate_buffer(
    spudgpu_buffer buffer,
    uint64_t offset,
    uint64_t size) {
	if (!buffer)
		return SPUDRESULT_GPU_INVALID_BUFFER;
	return SPUD_SUCCESS;
}


#endif // SPUDGPU_COMPILE_METAL_API
