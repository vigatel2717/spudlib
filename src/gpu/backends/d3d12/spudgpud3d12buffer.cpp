
#if SPUDGPU_COMPILE_D3D12_API

#include "spudgpud3d12.hpp"

static CD3DX12_RESOURCE_DESC spudgpu_d3d12_create_resource_desc_from_buffer(
    const spudgpu_buffer_desc *desc) {
	D3D12_RESOURCE_FLAGS d3dResourceFlags =
	    spudgpu_d3d12_get_buffer_resource_flags(desc->usage, desc->buffer_flags);
	CD3DX12_RESOURCE_DESC result =
	    CD3DX12_RESOURCE_DESC::Buffer(desc->size, d3dResourceFlags);
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
		return SPUD_SUCCESS;

	spudgpu_buffer_d3d12 *pResult =
	    (spudgpu_buffer_d3d12 *)calloc(1, sizeof(spudgpu_buffer_d3d12));
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
		free(pResult);
		return SPUDRESULT_API_SPECIFIC_FAILURE;
	}
	pResult->_d3d_gpu_address = pResult->_d3d_resource->GetGPUVirtualAddress();

	*out_buffer = pResult;
	return SPUD_SUCCESS;
}
void spudgpu_destroy_buffer(spudgpu_buffer buffer) {
	if (!buffer)
		return;
	buffer->_d3d_resource.Reset();
	free(buffer);
}
SPUDRESULT
spudgpu_get_buffer_desc(spudgpu_buffer buffer, spudgpu_buffer_desc *out_desc) {
	if (!buffer)
		return SPUDRESULT_GPU_INVALID_BUFFER;
	if (out_desc)
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
		return SPUD_SUCCESS;

	spudgpu_buffer_view_d3d12 *pResult = (spudgpu_buffer_view_d3d12 *)calloc(
	    1, sizeof(spudgpu_buffer_view_d3d12));
	pResult->_buffer = buffer;
	pResult->_desc   = *desc;

	switch (buffer->_desc.usage) {
	case SPUDGPU_BUFFER_USAGE_VERTEX:
		pResult->_d3d_view._vb.BufferLocation =
		    buffer->_d3d_gpu_address + desc->offset_from_parent_buffer;
		pResult->_d3d_view._vb.StrideInBytes  = desc->stride;
		pResult->_d3d_view._vb.SizeInBytes    = desc->size;
		break;
	case SPUDGPU_BUFFER_USAGE_INDEX:
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
		break;
	case SPUDGPU_BUFFER_USAGE_UNIFORM:
		pResult->_d3d_view._cb.BufferLocation =
		    buffer->_d3d_gpu_address + desc->offset_from_parent_buffer;
		pResult->_d3d_view._cb.SizeInBytes    = desc->size;
		break;
	// case SPUDGPU_BUFFER_USAGE_STORAGE:
	//	pResult->_d3d_view._so.BufferLocation = desc->offset_from_parent_buffer;
	//  TODO : D3D12_STREAM_OUTPUT_BUFFER_VIEW Buffer Filled Size Location
	// pResult->_d3d_view._so.BufferFilledSizeLocation = desc->size;
	//	pResult->_d3d_view._so.SizeInBytes = desc->size;
	//	break;
	default:
		free(pResult);
		return SPUDRESULT_GPU_INVALID_BUFFER_USAGE;
	}

	*out_buffer_view = pResult;
	return SPUD_SUCCESS;
}
void spudgpu_destroy_buffer_view(spudgpu_buffer_view buffer) {
	// Nothing to do in D3D12 here.
	// if (!buffer) return;
	free(buffer);
}
SPUDRESULT spudgpu_get_buffer_view_desc(
    spudgpu_buffer_view view, spudgpu_buffer_view_desc *out_desc) {
	if (!view)
		return SPUDRESULT_GPU_INVALID_BUFFER_VIEW;
	if (out_desc)
		*out_desc = view->_desc;
	return SPUD_SUCCESS;
}

SPUDRESULT spudgpu_map_buffer(
    spudgpu_buffer buffer, uint64_t offset, uint64_t size, void **ppData) {
	if (!buffer)
		return SPUDRESULT_GPU_INVALID_BUFFER;
	if (size == 0)
		return SPUDRESULT_ZERO_SIZE;
	CD3DX12_RANGE d3dRange = CD3DX12_RANGE(offset, offset + size);
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
	// SPUD_SUCCESS unconditionally, including for a null buffer — matches the
	// Vulkan backend's null-buffer behavior for cross-backend parity.
	return SPUD_SUCCESS;
}
}

#endif // SPUDGPU_COMPILE_D3D12_API
