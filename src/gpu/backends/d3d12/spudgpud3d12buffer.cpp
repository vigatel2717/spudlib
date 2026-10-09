
#if SPUDGPU_COMPILE_D3D12_API

#include "spudgpud3d12.hpp"

static CD3DX12_RESOURCE_DESC spudgpu_d3d12_create_resource_desc_from_buffer(
    const spudgpu_buffer_desc *desc) {
	D3D12_RESOURCE_FLAGS d3dResourceFlags =
	    spudgpu_d3d12_get_buffer_resource_flags(desc->usage, desc->buffer_flags);
	// A CBV's SizeInBytes must be a 256-byte-aligned multiple
	// (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT), and CreateConstantBufferView
	// rounds up to that regardless of the resource's real size -- so the backing
	// resource itself has to be allocated at least that large or the CBV's declared
	// range runs past the end of the resource. Padding is allocation-only: the
	// caller-visible spudgpu_buffer_desc::size (returned by spudgpu_get_buffer_desc)
	// is left exactly as requested.
	uint64_t allocSize = desc->size;
	if (desc->usage == SPUDGPU_BUFFER_USAGE_UNIFORM)
		allocSize = (allocSize + 255) & ~(uint64_t)255;
	CD3DX12_RESOURCE_DESC result =
	    CD3DX12_RESOURCE_DESC::Buffer(allocSize, d3dResourceFlags);
	return result;
}

extern "C" {

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

	spudgpu_buffer_d3d12 *pResult =
	    (spudgpu_buffer_d3d12 *)malloc(sizeof(spudgpu_buffer_d3d12));
	if (!pResult)
		return SPUDRESULT_OUT_OF_MEMORY;
	pResult          = new (pResult) spudgpu_buffer_d3d12();
	pResult->_device = device;
	pResult->_desc   = *desc;

	pResult->_d3d_resource_desc =
	    spudgpu_d3d12_create_resource_desc_from_buffer(desc);
	D3D12_HEAP_FLAGS d3dHeapFlags =
	    spudgpu_d3d12_get_heap_flags(desc->heap_flags);
	D3D12_HEAP_PROPERTIES d3dHeapProperties =
	    spudgpu_d3d12_get_heap_properties_from_memory_flags(desc->memory_flags);
	D3D12_RESOURCE_STATES d3dInitialState =
	    spudgpu_d3d12_get_initial_buffer_state(desc->memory_flags);
	if (device->_d3d_device->CreateCommittedResource(
	        &d3dHeapProperties, d3dHeapFlags, &pResult->_d3d_resource_desc,
	        d3dInitialState, nullptr,
	        IID_PPV_ARGS(&pResult->_d3d_resource))) {
		pResult->~spudgpu_buffer_d3d12();
		free(pResult);
		return SPUDRESULT_API_SPECIFIC_FAILURE;
	}
	pResult->_d3d_gpu_address = pResult->_d3d_resource->GetGPUVirtualAddress();

#if _DEBUG
	if (spud_debug_name_set(pResult, desc->debug_name) != SPUD_SUCCESS) {
		spudgpu_destroy_buffer(pResult);
		return SPUDRESULT_OUT_OF_MEMORY;
	}
	pResult->_desc.debug_name = pResult->_debug_name;
#endif

	*out_buffer = pResult;
	return SPUD_SUCCESS;
}
void spudgpu_destroy_buffer(spudgpu_buffer buffer) {
	if (!buffer)
		return;
#if _DEBUG
	free((void *)buffer->_debug_name);
#endif
	buffer->~spudgpu_buffer_d3d12();
	free(buffer);
}
SPUDRESULT
spudgpu_get_buffer_desc(spudgpu_buffer buffer, spudgpu_buffer_desc *out_desc) {
	if (!buffer)
		return SPUDRESULT_GPU_INVALID_BUFFER;
	if (!out_desc)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	*out_desc = buffer->_desc;
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
	if (desc->offset_from_parent_buffer + desc->size > buffer->_desc.size)
		return SPUDRESULT_GPU_BUFFER_OR_IMAGE_VIEW_RANGE_OUT_OF_SCOPE;

	spudgpu_buffer_view_d3d12 *pResult = (spudgpu_buffer_view_d3d12 *)calloc(
	    1, sizeof(spudgpu_buffer_view_d3d12));
	if (!pResult)
		return SPUDRESULT_OUT_OF_MEMORY;
	pResult->_buffer = buffer;
	pResult->_desc   = *desc;

	// spudgpu_buffer_desc::usage is a bitmask (a buffer can legitimately carry
	// more than one usage, e.g. UNIFORM | VERTEX for a per-instance buffer
	// also read by a compute pass) -- test membership, not exact equality,
	// or a multi-usage buffer falls through to default below. VERTEX/INDEX
	// are checked first since they're what spudgpu_create_buffer_view is
	// actually for; a buffer's UNIFORM/STORAGE binding goes through
	// spudgpu_write_descriptor_set instead and never reaches here in
	// practice, but the case is kept for a caller that does.
	if (buffer->_desc.usage & SPUDGPU_BUFFER_USAGE_VERTEX) {
		pResult->_d3d_view._vb.BufferLocation =
		    buffer->_d3d_gpu_address + desc->offset_from_parent_buffer;
		pResult->_d3d_view._vb.StrideInBytes  = desc->stride;
		pResult->_d3d_view._vb.SizeInBytes    = desc->size;
	} else if (buffer->_desc.usage & SPUDGPU_BUFFER_USAGE_INDEX) {
		pResult->_d3d_view._ib.BufferLocation =
		    buffer->_d3d_gpu_address + desc->offset_from_parent_buffer;
		if (desc->stride == 4)
			pResult->_d3d_view._ib.Format = DXGI_FORMAT_R32_UINT;
		else if (desc->stride == 2)
			pResult->_d3d_view._ib.Format = DXGI_FORMAT_R16_UINT;
		else {
			free(pResult);
			return SPUDRESULT_GPU_INVALID_INDEX_STRIDE;
		}
		pResult->_d3d_view._ib.SizeInBytes = desc->size;
	} else if (buffer->_desc.usage & SPUDGPU_BUFFER_USAGE_UNIFORM) {
		pResult->_d3d_view._cb.BufferLocation =
		    buffer->_d3d_gpu_address + desc->offset_from_parent_buffer;
		pResult->_d3d_view._cb.SizeInBytes    = desc->size;
	// } else if (buffer->_desc.usage & SPUDGPU_BUFFER_USAGE_STORAGE) {
	//	pResult->_d3d_view._so.BufferLocation = desc->offset_from_parent_buffer;
	//  TODO : D3D12_STREAM_OUTPUT_BUFFER_VIEW Buffer Filled Size Location
	// pResult->_d3d_view._so.BufferFilledSizeLocation = desc->size;
	//	pResult->_d3d_view._so.SizeInBytes = desc->size;
	} else {
		free(pResult);
		return SPUDRESULT_GPU_INVALID_BUFFER_USAGE;
	}

	*out_buffer_view = pResult;
	return SPUD_SUCCESS;
}
void spudgpu_destroy_buffer_view(spudgpu_buffer_view buffer) {
	if (!buffer)
		return;
#if _DEBUG
	free((void *)buffer->_debug_name);
#endif
	free(buffer);
}
SPUDRESULT spudgpu_get_buffer_view_desc(
    spudgpu_buffer_view view, spudgpu_buffer_view_desc *out_desc) {
	if (!view)
		return SPUDRESULT_GPU_INVALID_BUFFER_VIEW;
	if (!out_desc)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	*out_desc = view->_desc;
	return SPUD_SUCCESS;
}

SPUDRESULT spudgpu_map_buffer(
    spudgpu_buffer buffer, uint64_t offset, uint64_t size, void **ppData) {
	if (!buffer)
		return SPUDRESULT_GPU_INVALID_BUFFER;
	if (offset + size > buffer->_desc.size)
		return SPUDRESULT_GPU_MAP_OUT_OF_RANGE;
	if (!ppData)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	if (!(buffer->_desc.memory_flags & SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE))
		return SPUDRESULT_GPU_INVALID_MEMORY_FLAGS;
	// 0 means "map the entire buffer" per spudgpu.h's documented contract --
	// matches the Vulkan backend's mapSize fallback (spudgpuvulkanbuffer.c).
	uint64_t mapSize       = (size == 0) ? buffer->_desc.size : size;
	CD3DX12_RANGE d3dRange = CD3DX12_RANGE(offset, offset + mapSize);
	if (FAILED(buffer->_d3d_resource->Map(0, &d3dRange, ppData)))
		return SPUDRESULT_API_SPECIFIC_FAILURE;
	return SPUD_SUCCESS;
}
void spudgpu_unmap_buffer(spudgpu_buffer buffer) {
	if (!buffer) return;
	buffer->_d3d_resource->Unmap(0, nullptr);
}

// D3D12 has no standalone flush/invalidate call the way Vulkan does
// (vkFlushMappedMemoryRanges / vkInvalidateMappedMemoryRanges) — that
// behavior is folded into Map()'s pReadRange and Unmap()'s pWrittenRange
// parameters instead, which spudgpu_map_buffer/spudgpu_unmap_buffer above
// already pass. By the time a caller could invoke either of these, the
// runtime has already guaranteed the mapped range is coherent, so both are
// correctly no-ops here.
void spudgpu_flush_buffer(spudgpu_buffer buffer, uint64_t offset, uint64_t size) {
}
SPUDRESULT spudgpu_invalidate_buffer(spudgpu_buffer buffer, uint64_t offset, uint64_t size) {
	if (!buffer)
		return SPUDRESULT_GPU_INVALID_BUFFER;
	return SPUD_SUCCESS;
}
}

#endif // SPUDGPU_COMPILE_D3D12_API
