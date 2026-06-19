
#if SPUDGPU_COMPILE_D3D12_API

#include "spudgpud3d12.hpp"

// Maps SPUDGPU_DESCRIPTOR_TYPE to D3D12_DESCRIPTOR_RANGE_TYPE.
// Returns -1 for sampler (handled separately) and -2 for unknown.
static int spudgpu_d3d12_descriptor_range_type(SPUDGPU_DESCRIPTOR_TYPE type) {
	switch (type) {
	case SPUDGPU_DESCRIPTOR_TYPE_UNIFORM_BUFFER:       return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	case SPUDGPU_DESCRIPTOR_TYPE_STORAGE_BUFFER:       return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	case SPUDGPU_DESCRIPTOR_TYPE_SAMPLED_IMAGE:        return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	case SPUDGPU_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV half; sampler tracked separately
	case SPUDGPU_DESCRIPTOR_TYPE_STORAGE_IMAGE:        return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	case SPUDGPU_DESCRIPTOR_TYPE_SAMPLER:              return -1; // sampler heap
	default:                                           return -2;
	}
}

static bool spudgpu_d3d12_is_sampler_type(SPUDGPU_DESCRIPTOR_TYPE type) {
	return type == SPUDGPU_DESCRIPTOR_TYPE_SAMPLER ||
	       type == SPUDGPU_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
}

extern "C" {

SPUDRESULT spudgpu_create_descriptor_set_layout(
    spudgpu_device device,
    const spudgpu_descriptor_set_layout_desc *desc,
    spudgpu_descriptor_set_layout *out_layout) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!desc)
		return SPUDRESULT_NULL_DESC;
	if (desc->binding_count > SPUDGPU_MAX_DESCRIPTOR_BINDINGS_PER_SET)
		return SPUDRESULT_GPU_TOO_MANY_DESCRIPTOR_BINDINGS;
	if (!out_layout)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	spudgpu_descriptor_set_layout_d3d12 *pResult =
	    (spudgpu_descriptor_set_layout_d3d12 *)calloc(
	        1, sizeof(spudgpu_descriptor_set_layout_d3d12));
	pResult->_device = device;
	pResult->_desc   = *desc;

	// Pre-compute per-binding offsets into the set's heap range.
	uint32_t csv_cursor     = 0;
	uint32_t sampler_cursor = 0;
	for (uint32_t i = 0; i < desc->binding_count; ++i) {
		const spudgpu_descriptor_binding_desc &b = desc->bindings[i];
		spudgpu_d3d12_binding_slot &slot         = pResult->_slots[i];

		bool needsSampler = spudgpu_d3d12_is_sampler_type(b.descriptor_type);
		bool needsCsv     = (b.descriptor_type != SPUDGPU_DESCRIPTOR_TYPE_SAMPLER);

		if (needsCsv) {
			slot.cbv_srv_uav_offset = csv_cursor;
			csv_cursor += b.count;
		}
		if (needsSampler) {
			slot.sampler_offset = sampler_cursor;
			sampler_cursor += b.count;
		}
		slot.is_sampler = (b.descriptor_type == SPUDGPU_DESCRIPTOR_TYPE_SAMPLER);
	}
	pResult->_cbv_srv_uav_count = csv_cursor;
	pResult->_sampler_count     = sampler_cursor;

#if _DEBUG
	pResult->_debug_name = desc->debug_name;
#endif

	*out_layout = pResult;
	return SPUD_SUCCESS;
}

void spudgpu_destroy_descriptor_set_layout(spudgpu_descriptor_set_layout layout) {
	free(layout);
}

SPUDRESULT spudgpu_create_descriptor_pool(
    spudgpu_device device,
    const spudgpu_descriptor_pool_desc *desc,
    spudgpu_descriptor_pool *out_pool) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!desc)
		return SPUDRESULT_NULL_DESC;
	if (desc->pool_size_count > SPUDGPU_MAX_DESCRIPTOR_POOL_SIZES)
		return SPUDRESULT_GPU_TOO_MANY_DESCRIPTOR_POOLS;
	if (!out_pool)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	// Sum capacities per heap type.
	uint32_t csvCapacity     = 0;
	uint32_t samplerCapacity = 0;
	for (uint32_t i = 0; i < desc->pool_size_count; ++i) {
		SPUDGPU_DESCRIPTOR_TYPE t = desc->pool_sizes[i].descriptor_type;
		uint32_t count            = desc->pool_sizes[i].count;
		if (t == SPUDGPU_DESCRIPTOR_TYPE_SAMPLER) {
			samplerCapacity += count;
		} else {
			csvCapacity += count;
			if (t == SPUDGPU_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
				samplerCapacity += count; // also needs sampler slots
		}
	}

	spudgpu_descriptor_pool_d3d12 *pResult =
	    new spudgpu_descriptor_pool_d3d12();
	pResult->_device               = device;
	pResult->_desc                 = *desc;
	pResult->_cbv_srv_uav_capacity = csvCapacity;
	pResult->_sampler_capacity     = samplerCapacity;
	pResult->_cbv_srv_uav_cursor   = 0;
	pResult->_sampler_cursor       = 0;

	ID3D12Device *d3dDevice = device->_d3d_device.Get();

	if (csvCapacity > 0) {
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.NumDescriptors = csvCapacity;
		heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NodeMask       = 0;
		if (FAILED(d3dDevice->CreateDescriptorHeap(
		        &heapDesc, IID_PPV_ARGS(&pResult->_cbv_srv_uav_heap)))) {
			delete pResult;
			return SPUDRESULT_API_SPECIFIC_FAILURE;
		}
		pResult->_cbv_srv_uav_increment =
		    d3dDevice->GetDescriptorHandleIncrementSize(
		        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	if (samplerCapacity > 0) {
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		heapDesc.NumDescriptors = samplerCapacity;
		heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NodeMask       = 0;
		if (FAILED(d3dDevice->CreateDescriptorHeap(
		        &heapDesc, IID_PPV_ARGS(&pResult->_sampler_heap)))) {
			delete pResult;
			return SPUDRESULT_API_SPECIFIC_FAILURE;
		}
		pResult->_sampler_increment =
		    d3dDevice->GetDescriptorHandleIncrementSize(
		        D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	}

#if _DEBUG
	pResult->_debug_name = desc->debug_name;
#endif

	*out_pool = pResult;
	return SPUD_SUCCESS;
}

void spudgpu_reset_descriptor_pool(spudgpu_descriptor_pool pool) {
	if (!pool)
		return;
	// Reset allocation cursors; existing descriptor set handles become invalid.
	pool->_cbv_srv_uav_cursor = 0;
	pool->_sampler_cursor     = 0;
}

void spudgpu_destroy_descriptor_pool(spudgpu_descriptor_pool pool) {
	if (!pool)
		return;
	spudgpu_descriptor_pool_d3d12 *p = (spudgpu_descriptor_pool_d3d12 *)pool;
	p->_cbv_srv_uav_heap.Reset();
	p->_sampler_heap.Reset();
	delete p;
}

SPUDRESULT spudgpu_allocate_descriptor_sets(
    spudgpu_device device,
    const spudgpu_descriptor_set_alloc_desc *desc,
    spudgpu_descriptor_set *out_sets) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!desc)
		return SPUDRESULT_NULL_DESC;
	if (!desc->pool)
		return SPUDRESULT_GPU_INVALID_DESCRIPTOR_POOL;
	if (!desc->set_count)
		return SPUDRESULT_GPU_ZERO_DESCRIPTOR_SET_LAYOUTS;
	if (desc->set_count > SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS)
		return SPUDRESULT_GPU_TOO_MANY_DESCRIPTOR_SET_LAYOUTS;

	spudgpu_descriptor_pool_d3d12 *pool =
	    (spudgpu_descriptor_pool_d3d12 *)desc->pool;

	for (uint32_t i = 0; i < desc->set_count; ++i) {
		if (!desc->layouts[i])
			return SPUDRESULT_NULL_DESC;

		spudgpu_descriptor_set_layout_d3d12 *layout =
		    (spudgpu_descriptor_set_layout_d3d12 *)desc->layouts[i];

		// Check pool has capacity.
		if (pool->_cbv_srv_uav_cursor + layout->_cbv_srv_uav_count >
		    pool->_cbv_srv_uav_capacity)
			return SPUDRESULT_GPU_INTERNAL_DESCRIPTOR_SET_ALLOCATION_FAIL;
		if (pool->_sampler_cursor + layout->_sampler_count >
		    pool->_sampler_capacity)
			return SPUDRESULT_GPU_INTERNAL_DESCRIPTOR_SET_ALLOCATION_FAIL;

		spudgpu_descriptor_set_d3d12 *pSet =
		    (spudgpu_descriptor_set_d3d12 *)calloc(
		        1, sizeof(spudgpu_descriptor_set_d3d12));
		pSet->_pool             = pool;
		pSet->_layout           = layout;
		pSet->_cbv_srv_uav_base = pool->_cbv_srv_uav_cursor;
		pSet->_sampler_base     = pool->_sampler_cursor;

		pool->_cbv_srv_uav_cursor += layout->_cbv_srv_uav_count;
		pool->_sampler_cursor     += layout->_sampler_count;

		out_sets[i] = (spudgpu_descriptor_set)pSet;
	}

	return SPUD_SUCCESS;
}

void spudgpu_update_descriptor_sets(
    spudgpu_device device,
    const spudgpu_write_descriptor_set *writes,
    uint32_t write_count) {
	if (!device || !writes || !write_count)
		return;

	ID3D12Device *d3dDevice = device->_d3d_device.Get();

	for (uint32_t w = 0; w < write_count; ++w) {
		const spudgpu_write_descriptor_set &wr = writes[w];
		if (!wr.dst_set)
			continue;

		spudgpu_descriptor_set_d3d12 *set =
		    (spudgpu_descriptor_set_d3d12 *)wr.dst_set;
		spudgpu_descriptor_set_layout_d3d12 *layout = set->_layout;
		spudgpu_descriptor_pool_d3d12 *pool         = set->_pool;

		// Find the binding slot index in the layout matching dst_binding.
		uint32_t slotIdx = UINT32_MAX;
		for (uint32_t b = 0; b < layout->_desc.binding_count; ++b) {
			if (layout->_desc.bindings[b].binding == wr.dst_binding) {
				slotIdx = b;
				break;
			}
		}
		if (slotIdx == UINT32_MAX)
			continue;

		const spudgpu_d3d12_binding_slot &slot = layout->_slots[slotIdx];
		SPUDGPU_DESCRIPTOR_TYPE dtype          = wr.descriptor_type;

		// Write CBV/SRV/UAV descriptors.
		if (dtype != SPUDGPU_DESCRIPTOR_TYPE_SAMPLER && pool->_cbv_srv_uav_heap) {
			D3D12_CPU_DESCRIPTOR_HANDLE heapStart =
			    pool->_cbv_srv_uav_heap->GetCPUDescriptorHandleForHeapStart();
			uint32_t baseSlot = set->_cbv_srv_uav_base +
			                    slot.cbv_srv_uav_offset +
			                    wr.dst_array_element;
			uint32_t inc = pool->_cbv_srv_uav_increment;

			for (uint32_t d = 0; d < wr.descriptor_count; ++d) {
				D3D12_CPU_DESCRIPTOR_HANDLE handle;
				handle.ptr = heapStart.ptr + (SIZE_T)(baseSlot + d) * inc;

				if (dtype == SPUDGPU_DESCRIPTOR_TYPE_UNIFORM_BUFFER &&
				    wr.buffer_info) {
					spudgpu_buffer_d3d12 *buf =
					    (spudgpu_buffer_d3d12 *)wr.buffer_info->buffer;
					D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
					cbvDesc.BufferLocation =
					    buf->_d3d_gpu_address + wr.buffer_info->offset;
					// CBV size must be 256-byte aligned.
					uint64_t range = wr.buffer_info->range
					                     ? wr.buffer_info->range
					                     : (buf->_desc.size - wr.buffer_info->offset);
					cbvDesc.SizeInBytes = (UINT)((range + 255) & ~255ULL);
					d3dDevice->CreateConstantBufferView(&cbvDesc, handle);

				} else if (dtype == SPUDGPU_DESCRIPTOR_TYPE_STORAGE_BUFFER &&
				           wr.buffer_info) {
					spudgpu_buffer_d3d12 *buf =
					    (spudgpu_buffer_d3d12 *)wr.buffer_info->buffer;
					uint64_t range = wr.buffer_info->range
					                     ? wr.buffer_info->range
					                     : (buf->_desc.size - wr.buffer_info->offset);
					D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
					uavDesc.Format                    = DXGI_FORMAT_R32_TYPELESS;
					uavDesc.ViewDimension             = D3D12_UAV_DIMENSION_BUFFER;
					uavDesc.Buffer.FirstElement       =
					    wr.buffer_info->offset / sizeof(uint32_t);
					uavDesc.Buffer.NumElements        = (UINT)(range / sizeof(uint32_t));
					uavDesc.Buffer.Flags              = D3D12_BUFFER_UAV_FLAG_RAW;
					d3dDevice->CreateUnorderedAccessView(
					    buf->_d3d_resource.Get(), nullptr, &uavDesc, handle);

				} else if ((dtype == SPUDGPU_DESCRIPTOR_TYPE_SAMPLED_IMAGE ||
				            dtype == SPUDGPU_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) &&
				           wr.image_info) {
					spudgpu_image_view_d3d12 *view =
					    (spudgpu_image_view_d3d12 *)wr.image_info->image_view;
					D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc =
					    view->_d3d_view_desc._srv;
					d3dDevice->CreateShaderResourceView(
					    view->_image->_d3d_resource.Get(), &srvDesc, handle);

				} else if (dtype == SPUDGPU_DESCRIPTOR_TYPE_STORAGE_IMAGE &&
				           wr.image_info) {
					spudgpu_image_view_d3d12 *view =
					    (spudgpu_image_view_d3d12 *)wr.image_info->image_view;
					D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc =
					    view->_d3d_view_desc._uav;
					d3dDevice->CreateUnorderedAccessView(
					    view->_image->_d3d_resource.Get(), nullptr, &uavDesc, handle);
				}
			}
		}

		// Write sampler descriptors (SAMPLER or COMBINED_IMAGE_SAMPLER).
		// Samplers aren't backed by spudgpu_buffer/image_info yet — placeholder.
		// When a sampler object type is added, fill this in.
	}
}

void spudgpu_cmd_bind_descriptor_sets(
    spudgpu_command_list cmd,
    spudgpu_shader_pipeline pipeline,
    uint32_t first_set,
    const spudgpu_descriptor_set *sets,
    uint32_t set_count) {
	if (!cmd || !sets || !set_count)
		return;
	if (set_count > SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS)
		return;

	ID3D12GraphicsCommandList *cmdList = cmd->_d3d_cmd_list.Get();

	// All sets in a call must share the same pool (same descriptor heaps).
	spudgpu_descriptor_set_d3d12 *firstSet =
	    (spudgpu_descriptor_set_d3d12 *)sets[0];
	spudgpu_descriptor_pool_d3d12 *pool = firstSet->_pool;

	// Bind the descriptor heaps; D3D12 allows at most one of each type.
	ID3D12DescriptorHeap *heaps[2];
	UINT heapCount = 0;
	if (pool->_cbv_srv_uav_heap)
		heaps[heapCount++] = pool->_cbv_srv_uav_heap.Get();
	if (pool->_sampler_heap)
		heaps[heapCount++] = pool->_sampler_heap.Get();
	if (heapCount)
		cmdList->SetDescriptorHeaps(heapCount, heaps);

	// One root parameter per descriptor set: a descriptor table starting at
	// the set's base GPU handle. Root parameter index = first_set + i.
	if (pool->_cbv_srv_uav_heap) {
		D3D12_GPU_DESCRIPTOR_HANDLE heapStart =
		    pool->_cbv_srv_uav_heap->GetGPUDescriptorHandleForHeapStart();
		uint32_t inc = pool->_cbv_srv_uav_increment;
		for (uint32_t i = 0; i < set_count; ++i) {
			spudgpu_descriptor_set_d3d12 *set =
			    (spudgpu_descriptor_set_d3d12 *)sets[i];
			D3D12_GPU_DESCRIPTOR_HANDLE table;
			table.ptr = heapStart.ptr + (UINT64)set->_cbv_srv_uav_base * inc;
			cmdList->SetGraphicsRootDescriptorTable(first_set + i, table);
		}
	}
}

void spudgpu_cmd_bind_descriptor_sets_compute(
    spudgpu_command_list cmd,
    spudgpu_compute_pipeline pipeline,
    uint32_t first_set,
    const spudgpu_descriptor_set *sets,
    uint32_t set_count) {
	if (!cmd || !sets || !set_count)
		return;
	if (set_count > SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS)
		return;

	ID3D12GraphicsCommandList *cmdList = cmd->_d3d_cmd_list.Get();

	spudgpu_descriptor_set_d3d12 *firstSet =
	    (spudgpu_descriptor_set_d3d12 *)sets[0];
	spudgpu_descriptor_pool_d3d12 *pool = firstSet->_pool;

	ID3D12DescriptorHeap *heaps[2];
	UINT heapCount = 0;
	if (pool->_cbv_srv_uav_heap)
		heaps[heapCount++] = pool->_cbv_srv_uav_heap.Get();
	if (pool->_sampler_heap)
		heaps[heapCount++] = pool->_sampler_heap.Get();
	if (heapCount)
		cmdList->SetDescriptorHeaps(heapCount, heaps);

	if (pool->_cbv_srv_uav_heap) {
		D3D12_GPU_DESCRIPTOR_HANDLE heapStart =
		    pool->_cbv_srv_uav_heap->GetGPUDescriptorHandleForHeapStart();
		uint32_t inc = pool->_cbv_srv_uav_increment;
		for (uint32_t i = 0; i < set_count; ++i) {
			spudgpu_descriptor_set_d3d12 *set =
			    (spudgpu_descriptor_set_d3d12 *)sets[i];
			D3D12_GPU_DESCRIPTOR_HANDLE table;
			table.ptr = heapStart.ptr + (UINT64)set->_cbv_srv_uav_base * inc;
			cmdList->SetComputeRootDescriptorTable(first_set + i, table);
		}
	}
}

} // extern "C"

#endif // SPUDGPU_COMPILE_D3D12_API
