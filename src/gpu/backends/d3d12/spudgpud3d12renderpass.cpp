
#if SPUDGPU_COMPILE_D3D12_API

#include "spudgpud3d12.hpp"
#include <cstdio>
#include <vector>

// Maps SPUDGPU_IMAGE_LAYOUT to D3D12_RESOURCE_STATES.
static D3D12_RESOURCE_STATES spudgpu_d3d12_layout_to_state(SPUDGPU_IMAGE_LAYOUT layout) {
	switch (layout) {
	case SPUDGPU_IMAGE_LAYOUT_UNDEFINED:
	case SPUDGPU_IMAGE_LAYOUT_GENERAL:
		return D3D12_RESOURCE_STATE_COMMON;
	case SPUDGPU_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		return D3D12_RESOURCE_STATE_RENDER_TARGET;
	case SPUDGPU_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		return D3D12_RESOURCE_STATE_DEPTH_WRITE;
	case SPUDGPU_IMAGE_LAYOUT_SHADER_READ_ONLY:
		return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
		       D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	case SPUDGPU_IMAGE_LAYOUT_TRANSFER_SRC:
		return D3D12_RESOURCE_STATE_COPY_SOURCE;
	case SPUDGPU_IMAGE_LAYOUT_TRANSFER_DST:
		return D3D12_RESOURCE_STATE_COPY_DEST;
	case SPUDGPU_IMAGE_LAYOUT_PRESENT_SRC:
		return D3D12_RESOURCE_STATE_PRESENT;
	default:
		return D3D12_RESOURCE_STATE_COMMON;
	}
}

static void spudgpu_d3d12_resource_barrier(
    ID3D12GraphicsCommandList *cmdList,
    ID3D12Resource *resource,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout) {
	D3D12_RESOURCE_STATES stateBefore = spudgpu_d3d12_layout_to_state(old_layout);
	D3D12_RESOURCE_STATES stateAfter  = spudgpu_d3d12_layout_to_state(new_layout);
	if (stateBefore == stateAfter)
		return;

	D3D12_RESOURCE_BARRIER barrier           = {};
	barrier.Type                             = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags                            = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource             = resource;
	barrier.Transition.StateBefore           = stateBefore;
	barrier.Transition.StateAfter            = stateAfter;
	barrier.Transition.Subresource           = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	cmdList->ResourceBarrier(1, &barrier);
}

extern "C" {

void spudgpu_cmd_image_barrier(
    spudgpu_command_list cmd,
    spudgpu_image image,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout) {
	if (!cmd || !image)
		return;
	spudgpu_d3d12_resource_barrier(
	    cmd->_d3d_cmd_list.Get(),
	    image->_d3d_resource.Get(),
	    old_layout, new_layout);
}

void spudgpu_cmd_image_barrier_view(
    spudgpu_command_list cmd,
    spudgpu_image_view image_view,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout) {
	if (!cmd || !image_view || !image_view->_image)
		return;
	spudgpu_d3d12_resource_barrier(
	    cmd->_d3d_cmd_list.Get(),
	    image_view->_image->_d3d_resource.Get(),
	    old_layout, new_layout);
}

void spudgpu_cmd_image_barrier_subresource(
    spudgpu_command_list cmd,
    spudgpu_image image,
    const spudgpu_image_view_desc_subresource_range *range,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout) {
	if (!cmd || !image || !range)
		return;

	D3D12_RESOURCE_STATES stateBefore = spudgpu_d3d12_layout_to_state(old_layout);
	D3D12_RESOURCE_STATES stateAfter  = spudgpu_d3d12_layout_to_state(new_layout);
	if (stateBefore == stateAfter)
		return;

	UINT mipLevels = image->_desc.mip_levels;
	UINT arraySize = image->_desc.array_layers;

	std::vector<D3D12_RESOURCE_BARRIER> barriers;
	barriers.reserve((size_t)range->mip_level_count * range->array_layer_count);

	for (UINT layer = 0; layer < range->array_layer_count; ++layer) {
		for (UINT mip = 0; mip < range->mip_level_count; ++mip) {
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource   = image->_d3d_resource.Get();
			barrier.Transition.StateBefore = stateBefore;
			barrier.Transition.StateAfter  = stateAfter;
			barrier.Transition.Subresource = D3D12CalcSubresource(
			    (UINT)(range->base_mip_level + mip),
			    (UINT)(range->base_array_layer + layer),
			    0, mipLevels, arraySize);
			barriers.push_back(barrier);
		}
	}

	if (!barriers.empty())
		cmd->_d3d_cmd_list->ResourceBarrier((UINT)barriers.size(), barriers.data());
}

// Builds a D3D12_PLACED_SUBRESOURCE_FOOTPRINT describing the buffer-side
// layout for one array layer of a buffer<->image copy region. Uses the
// caller-supplied row/image length as-is (0 = tightly packed) rather than
// forcing D3D12's 256-byte row-pitch alignment — a caller that wants a
// portable, correctly-aligned staging buffer should size it via
// spudgpu_get_image_buffer_copy_size first and pass that pitch in.
static D3D12_PLACED_SUBRESOURCE_FOOTPRINT spudgpu_d3d12_make_footprint(
    SPUDGPU_FORMAT format,
    const spudgpu_image_buffer_copy_desc *desc) {
	UINT bytesPerTexel = spudgpu_format_bit_count(format) / 8;

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
	footprint.Offset             = desc->buffer_offset;
	footprint.Footprint.Format   = spudgpu_d3d12_get_dxgi_format(format);
	footprint.Footprint.Width    = desc->width;
	footprint.Footprint.Height   = desc->height;
	footprint.Footprint.Depth    = desc->depth ? desc->depth : 1;
	footprint.Footprint.RowPitch = (desc->buffer_row_length ? desc->buffer_row_length : desc->width) * bytesPerTexel;
	return footprint;
}

void spudgpu_cmd_copy_buffer_to_image(
    spudgpu_command_list cmd,
    spudgpu_buffer src_buffer,
    spudgpu_image dst_image,
    const spudgpu_image_buffer_copy_desc *desc) {
	if (!cmd || !src_buffer || !dst_image || !desc)
		return;

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint =
	    spudgpu_d3d12_make_footprint(dst_image->_desc.format, desc);

	UINT layerCount        = desc->array_layer_count ? desc->array_layer_count : 1;
	UINT bufferImageHeight = desc->buffer_image_height ? desc->buffer_image_height : desc->height;
	UINT64 layerPitchBytes = (UINT64)footprint.Footprint.RowPitch * bufferImageHeight;

	for (UINT layer = 0; layer < layerCount; ++layer) {
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT layerFootprint = footprint;
		layerFootprint.Offset += (UINT64)layer * layerPitchBytes;

		UINT dstSubresource = D3D12CalcSubresource(
		    desc->mip_level, desc->base_array_layer + layer, 0,
		    dst_image->_desc.mip_levels, dst_image->_desc.array_layers);

		CD3DX12_TEXTURE_COPY_LOCATION srcLoc(src_buffer->_d3d_resource.Get(), layerFootprint);
		CD3DX12_TEXTURE_COPY_LOCATION dstLoc(dst_image->_d3d_resource.Get(), dstSubresource);

		cmd->_d3d_cmd_list->CopyTextureRegion(
		    &dstLoc, desc->image_x, desc->image_y, desc->image_z, &srcLoc, nullptr);
	}
}

void spudgpu_cmd_copy_image_to_buffer(
    spudgpu_command_list cmd,
    spudgpu_image src_image,
    spudgpu_buffer dst_buffer,
    const spudgpu_image_buffer_copy_desc *desc) {
	if (!cmd || !src_image || !dst_buffer || !desc)
		return;

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint =
	    spudgpu_d3d12_make_footprint(src_image->_desc.format, desc);

	UINT layerCount        = desc->array_layer_count ? desc->array_layer_count : 1;
	UINT bufferImageHeight = desc->buffer_image_height ? desc->buffer_image_height : desc->height;
	UINT64 layerPitchBytes = (UINT64)footprint.Footprint.RowPitch * bufferImageHeight;

	D3D12_BOX srcBox;
	srcBox.left   = desc->image_x;
	srcBox.top    = desc->image_y;
	srcBox.front  = desc->image_z;
	srcBox.right  = desc->image_x + desc->width;
	srcBox.bottom = desc->image_y + desc->height;
	srcBox.back   = desc->image_z + (desc->depth ? desc->depth : 1);

	for (UINT layer = 0; layer < layerCount; ++layer) {
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT layerFootprint = footprint;
		layerFootprint.Offset += (UINT64)layer * layerPitchBytes;

		UINT srcSubresource = D3D12CalcSubresource(
		    desc->mip_level, desc->base_array_layer + layer, 0,
		    src_image->_desc.mip_levels, src_image->_desc.array_layers);

		CD3DX12_TEXTURE_COPY_LOCATION srcLoc(src_image->_d3d_resource.Get(), srcSubresource);
		CD3DX12_TEXTURE_COPY_LOCATION dstLoc(dst_buffer->_d3d_resource.Get(), layerFootprint);

		cmd->_d3d_cmd_list->CopyTextureRegion(&dstLoc, 0, 0, 0, &srcLoc, &srcBox);
	}
}

// GetCopyableFootprints reports D3D12's actual required (already-aligned)
// row pitch and total buffer size for the given mip level — unlike the raw
// copy commands above, this always returns a portably-correct value.
void spudgpu_get_image_buffer_copy_size(
    spudgpu_image image,
    uint32_t mip_level,
    uint64_t *out_row_pitch,
    uint64_t *out_total_size) {
	if (!image)
		return;

	D3D12_RESOURCE_DESC resDesc = image->_d3d_resource_desc;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
	UINT64 totalBytes = 0;

	image->_device->_d3d_device->GetCopyableFootprints(
	    &resDesc, mip_level, 1, 0, &footprint, nullptr, nullptr, &totalBytes);

	if (out_row_pitch)
		*out_row_pitch = footprint.Footprint.RowPitch;
	if (out_total_size)
		*out_total_size = totalBytes;
}

void spudgpu_cmd_blit_image(
    spudgpu_command_list cmd,
    spudgpu_image src_image,
    spudgpu_image dst_image,
    const spudgpu_image_blit_desc *desc) {
	if (!cmd || !src_image || !dst_image || !desc)
		return;

	UINT srcWidth  = desc->src_x1 - desc->src_x0;
	UINT srcHeight = desc->src_y1 - desc->src_y0;
	UINT srcDepth  = desc->src_z1 - desc->src_z0;
	UINT dstWidth  = desc->dst_x1 - desc->dst_x0;
	UINT dstHeight = desc->dst_y1 - desc->dst_y0;
	UINT dstDepth  = desc->dst_z1 - desc->dst_z0;

	if (srcWidth != dstWidth || srcHeight != dstHeight || srcDepth != dstDepth) {
		// D3D12 has no native filtered/scaling blit equivalent to vkCmdBlitImage
		// — CopyTextureRegion requires matching src/dst extents. A scaling blit
		// would need a shader-based resample pass, which isn't implemented here.
		printf("spudgpu: spudgpu_cmd_blit_image with differing src/dst extents is not supported on D3D12 (needs a shader-based resample pass)\n");
		return;
	}

	D3D12_BOX srcBox;
	srcBox.left   = desc->src_x0;
	srcBox.top    = desc->src_y0;
	srcBox.front  = desc->src_z0;
	srcBox.right  = desc->src_x1;
	srcBox.bottom = desc->src_y1;
	srcBox.back   = desc->src_z1;

	UINT layerCount = desc->src_array_layer_count ? desc->src_array_layer_count : 1;
	for (UINT layer = 0; layer < layerCount; ++layer) {
		UINT srcSubresource = D3D12CalcSubresource(
		    desc->src_mip_level, desc->src_base_array_layer + layer, 0,
		    src_image->_desc.mip_levels, src_image->_desc.array_layers);
		UINT dstSubresource = D3D12CalcSubresource(
		    desc->dst_mip_level, desc->dst_base_array_layer + layer, 0,
		    dst_image->_desc.mip_levels, dst_image->_desc.array_layers);

		CD3DX12_TEXTURE_COPY_LOCATION srcLoc(src_image->_d3d_resource.Get(), srcSubresource);
		CD3DX12_TEXTURE_COPY_LOCATION dstLoc(dst_image->_d3d_resource.Get(), dstSubresource);

		cmd->_d3d_cmd_list->CopyTextureRegion(
		    &dstLoc, desc->dst_x0, desc->dst_y0, desc->dst_z0, &srcLoc, &srcBox);
	}
}

static D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE spudgpu_d3d12_load_op_to_beginning_access(SPUDGPU_LOAD_OP op) {
	switch (op) {
	case SPUDGPU_LOAD_OP_CLEAR:
		return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
	case SPUDGPU_LOAD_OP_DONT_CARE:
		return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
	case SPUDGPU_LOAD_OP_LOAD:
	default:
		return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
	}
}

static D3D12_RENDER_PASS_ENDING_ACCESS_TYPE spudgpu_d3d12_store_op_to_ending_access(SPUDGPU_STORE_OP op) {
	switch (op) {
	case SPUDGPU_STORE_OP_DONT_CARE:
		return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
	case SPUDGPU_STORE_OP_STORE:
	default:
		return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
	}
}

// spudgpu_cmd_begin_rendering / spudgpu_cmd_end_rendering — dynamic
// rendering. D3D12's BeginRenderPass/EndRenderPass never needed a
// precompiled render-pass object or pipeline-compatibility contract to begin
// with (unlike classic Vulkan render passes), so this maps onto it directly:
// per-attachment beginning/ending access is passed inline, and the pipeline
// is bound separately at draw time via spudgpu_cmd_bind_pipeline.
void spudgpu_cmd_begin_rendering(
    spudgpu_command_list cmd,
    const spudgpu_rendering_begin_desc *desc) {
	if (!cmd || !desc)
		return;
	if(!desc->color_attachment_count || !desc->color_attachments)
		return;
	if (!desc->color_attachments[0].image_view)
		return;

	UINT colorCount = desc->color_attachment_count;
	if (colorCount > SPUDGPU_MAX_COLOR_ATTACHMENTS)
		colorCount = SPUDGPU_MAX_COLOR_ATTACHMENTS;

	spudgpu_image_view_d3d12 *firstColorView =
	    (spudgpu_image_view_d3d12 *)desc->color_attachments[0].image_view;
	ID3D12Device *device = firstColorView->_image->_device->_d3d_device.Get();
	ID3D12GraphicsCommandList4 *cmdList = cmd->_d3d_cmd_list.Get();

	// Create a transient non-shader-visible RTV heap for this pass. CPU
	// descriptors are recorded by value into the command list, so the heap
	// may be released immediately after BeginRenderPass returns.
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.NumDescriptors = colorCount;
		heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap))))
			return;
	}
	UINT rtvInc = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStart = rtvHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_RENDER_PASS_RENDER_TARGET_DESC rtDescs[SPUDGPU_MAX_COLOR_ATTACHMENTS] = {};
	for (UINT i = 0; i < colorCount; ++i) {
		const spudgpu_color_attachment_desc &src = desc->color_attachments[i];
		spudgpu_image_view_d3d12 *colorView = (spudgpu_image_view_d3d12 *)src.image_view;

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvStart;
		rtvHandle.ptr += (SIZE_T)i * rtvInc;
		device->CreateRenderTargetView(
		    colorView->_image->_d3d_resource.Get(),
		    &colorView->_d3d_view_desc._rtv,
		    rtvHandle);

		rtDescs[i].cpuDescriptor                              = rtvHandle;
		rtDescs[i].BeginningAccess.Type                       = spudgpu_d3d12_load_op_to_beginning_access(src.load_op);
		rtDescs[i].BeginningAccess.Clear.ClearValue.Format    = colorView->_d3d_view_desc._rtv.Format;
		rtDescs[i].BeginningAccess.Clear.ClearValue.Color[0]  = src.clear_color[0];
		rtDescs[i].BeginningAccess.Clear.ClearValue.Color[1]  = src.clear_color[1];
		rtDescs[i].BeginningAccess.Clear.ClearValue.Color[2]  = src.clear_color[2];
		rtDescs[i].BeginningAccess.Clear.ClearValue.Color[3]  = src.clear_color[3];
		rtDescs[i].EndingAccess.Type                          = spudgpu_d3d12_store_op_to_ending_access(src.store_op);
	}

	// Optional depth/stencil attachment.
	D3D12_RENDER_PASS_DEPTH_STENCIL_DESC dsDesc = {};
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	bool hasDepth = (desc->depth_attachment.image_view != nullptr);

	if (hasDepth) {
		spudgpu_image_view_d3d12 *depthView =
		    (spudgpu_image_view_d3d12 *)desc->depth_attachment.image_view;

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		heapDesc.NumDescriptors = 1;
		heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&dsvHeap))))
			return;

		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle =
		    dsvHeap->GetCPUDescriptorHandleForHeapStart();
		device->CreateDepthStencilView(
		    depthView->_image->_d3d_resource.Get(),
		    &depthView->_d3d_view_desc._dsv,
		    dsvHandle);

		DXGI_FORMAT depthFmt = depthView->_d3d_view_desc._dsv.Format;

		dsDesc.cpuDescriptor                                                = dsvHandle;
		dsDesc.DepthBeginningAccess.Type                                    =
		    spudgpu_d3d12_load_op_to_beginning_access(desc->depth_attachment.depth_load_op);
		dsDesc.DepthBeginningAccess.Clear.ClearValue.Format                 = depthFmt;
		dsDesc.DepthBeginningAccess.Clear.ClearValue.DepthStencil.Depth     = desc->depth_attachment.clear_depth;
		dsDesc.StencilBeginningAccess.Type                                  =
		    spudgpu_d3d12_load_op_to_beginning_access(desc->depth_attachment.stencil_load_op);
		dsDesc.StencilBeginningAccess.Clear.ClearValue.Format               = depthFmt;
		dsDesc.StencilBeginningAccess.Clear.ClearValue.DepthStencil.Stencil =
		    (UINT8)desc->depth_attachment.clear_stencil;
		dsDesc.DepthEndingAccess.Type                                       =
		    spudgpu_d3d12_store_op_to_ending_access(desc->depth_attachment.depth_store_op);
		dsDesc.StencilEndingAccess.Type                                     =
		    spudgpu_d3d12_store_op_to_ending_access(desc->depth_attachment.stencil_store_op);
	}

	// Set the viewport and scissor to the full render area.
	D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width    = (float)desc->width;
	viewport.Height   = (float)desc->height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	cmdList->RSSetViewports(1, &viewport);

	D3D12_RECT scissor = {};
	scissor.left   = 0;
	scissor.top    = 0;
	scissor.right  = (LONG)desc->width;
	scissor.bottom = (LONG)desc->height;
	cmdList->RSSetScissorRects(1, &scissor);

	cmdList->BeginRenderPass(
	    colorCount, rtDescs,
	    hasDepth ? &dsDesc : nullptr,
	    D3D12_RENDER_PASS_FLAG_NONE);

	// rtvHeap and dsvHeap are released here — safe because D3D12 records CPU
	// descriptor handles by value into the command list at BeginRenderPass time.
}

void spudgpu_cmd_end_rendering(spudgpu_command_list cmd) {
	if (!cmd)
		return;
	cmd->_d3d_cmd_list->EndRenderPass();
}

} // extern "C"

#endif // SPUDGPU_COMPILE_D3D12_API
