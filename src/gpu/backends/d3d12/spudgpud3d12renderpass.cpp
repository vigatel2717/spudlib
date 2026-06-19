
#if SPUDGPU_COMPILE_D3D12_API

#include "spudgpud3d12.hpp"

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

void spudgpu_cmd_begin_render_pass(
    spudgpu_command_list cmd,
    const spudgpu_render_pass_begin_desc *desc) {
	if (!cmd || !desc || !desc->color_attachment)
		return;

	spudgpu_image_view_d3d12 *colorView =
	    (spudgpu_image_view_d3d12 *)desc->color_attachment;
	spudgpu_image_view_d3d12 *depthView =
	    (spudgpu_image_view_d3d12 *)desc->depth_attachment;

	ID3D12Device *device = colorView->_image->_device->_d3d_device.Get();
	ID3D12GraphicsCommandList4 *cmdList = cmd->_d3d_cmd_list.Get();
	UINT rtvInc = device->GetDescriptorHandleIncrementSize(
	    D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Create a transient non-shader-visible RTV heap for this render pass.
	// CPU descriptors are recorded by value into the command list, so the
	// heap may be released immediately after BeginRenderPass returns.
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.NumDescriptors = 1;
		heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap))))
			return;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
	    rtvHeap->GetCPUDescriptorHandleForHeapStart();
	device->CreateRenderTargetView(
	    colorView->_image->_d3d_resource.Get(),
	    &colorView->_d3d_view_desc._rtv,
	    rtvHandle);

	// Color attachment render pass desc.
	D3D12_RENDER_PASS_RENDER_TARGET_DESC rtDesc         = {};
	rtDesc.cpuDescriptor                                = rtvHandle;
	rtDesc.BeginningAccess.Type                         =
	    D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
	rtDesc.BeginningAccess.Clear.ClearValue.Format      =
	    colorView->_d3d_view_desc._rtv.Format;
	rtDesc.BeginningAccess.Clear.ClearValue.Color[0]    = desc->clear_color[0];
	rtDesc.BeginningAccess.Clear.ClearValue.Color[1]    = desc->clear_color[1];
	rtDesc.BeginningAccess.Clear.ClearValue.Color[2]    = desc->clear_color[2];
	rtDesc.BeginningAccess.Clear.ClearValue.Color[3]    = desc->clear_color[3];
	rtDesc.EndingAccess.Type                            =
	    D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

	// Optional depth/stencil attachment.
	D3D12_RENDER_PASS_DEPTH_STENCIL_DESC dsDesc    = {};
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	bool hasDepth = (depthView != nullptr);

	if (hasDepth) {
		UINT dsvInc = device->GetDescriptorHandleIncrementSize(
		    D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

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

		dsDesc.cpuDescriptor                                              = dsvHandle;
		dsDesc.DepthBeginningAccess.Type                                  =
		    D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		dsDesc.DepthBeginningAccess.Clear.ClearValue.Format               = depthFmt;
		dsDesc.DepthBeginningAccess.Clear.ClearValue.DepthStencil.Depth   = desc->clear_depth;
		dsDesc.StencilBeginningAccess.Type                                =
		    D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		dsDesc.StencilBeginningAccess.Clear.ClearValue.Format             = depthFmt;
		dsDesc.StencilBeginningAccess.Clear.ClearValue.DepthStencil.Stencil =
		    (UINT8)desc->clear_stencil;
		dsDesc.DepthEndingAccess.Type                                     =
		    D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
		dsDesc.StencilEndingAccess.Type                                   =
		    D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
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
	    1, &rtDesc,
	    hasDepth ? &dsDesc : nullptr,
	    D3D12_RENDER_PASS_FLAG_NONE);

	// rtvHeap and dsvHeap are released here — safe because D3D12 records CPU
	// descriptor handles by value into the command list at BeginRenderPass time.
}

void spudgpu_cmd_end_render_pass(spudgpu_command_list cmd) {
	if (!cmd)
		return;
	cmd->_d3d_cmd_list->EndRenderPass();
}

} // extern "C"

#endif // SPUDGPU_COMPILE_D3D12_API
