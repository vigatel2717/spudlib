
#if SPUDGPU_COMPILE_D3D12_API

#include "spudgpud3d12.hpp"

extern "C" {

SPUDRESULT spudgpu_create_swap_chain(
    spudgpu_device device,
    const spudgpu_swap_chain_desc *desc,
    spudgpu_swap_chain *out_swap_chain) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!desc)
		return SPUDRESULT_NULL_DESC;
	if (!desc->queue)
		return SPUDRESULT_GPU_INVALID_COMMAND_QUEUE;
	if (!out_swap_chain)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	spudgpu_swap_chain_d3d12 *pResult =
	    (spudgpu_swap_chain_d3d12 *)calloc(1, sizeof(spudgpu_swap_chain_d3d12));
	pResult->_device = device;
	pResult->_desc   = *desc;

	HWND hwnd = desc->surface->_hwnd;
	// The DXGI swap chain is bound to this specific queue for its lifetime -
	// spudgpu_swap_chain_desc::queue, not a hardcoded "the graphics queue"
	// guess, since DXGI can't rebind a swap chain to a different queue later.
	ID3D12CommandQueue *cmdQueue = desc->queue->_d3d_cmd_queue.Get();

	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width                 = desc->width;
	scDesc.Height                = desc->height;
	scDesc.Format                = spudgpu_d3d12_get_dxgi_format(desc->format);
	scDesc.Stereo                = FALSE;
	scDesc.SampleDesc            = {1, 0};
	scDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount           = desc->buffer_count;
	scDesc.Scaling               = DXGI_SCALING_STRETCH;
	scDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.AlphaMode             = DXGI_ALPHA_MODE_UNSPECIFIED;
	scDesc.Flags                 = 0;

	bool isExclusiveFullscreen =
	    (desc->fullscreen_mode == SPUDGPU_FULLSCREEN_MODE_FULLSCREEN);
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};
	fsDesc.RefreshRate                     = {0, 1};
	fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	fsDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
	fsDesc.Windowed         = FALSE;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgiSwapChain1;
	HRESULT hr = device->_instance->_dxgi_factory->CreateSwapChainForHwnd(
	    cmdQueue, hwnd, &scDesc, isExclusiveFullscreen ? &fsDesc : nullptr,
	    nullptr, &dxgiSwapChain1);
	if (FAILED(hr)) {
		free(pResult);
		return SPUDRESULT_API_SPECIFIC_FAILURE;
	}
	hr = dxgiSwapChain1.As(&pResult->_dxgi_swap_chain);
	if (FAILED(hr)) {
		free(pResult);
		return SPUDRESULT_API_SPECIFIC_FAILURE;
	}

	pResult->_back_buffer_images =
	    new spudgpu_image_d3d12[desc->buffer_count]();
	pResult->_back_buffer_image_views =
	    new spudgpu_image_view_d3d12[desc->buffer_count]();

	DXGI_FORMAT fmt = spudgpu_d3d12_get_dxgi_format(desc->format);

	for (uint32_t i = 0; i < desc->buffer_count; ++i) {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		hr = pResult->_dxgi_swap_chain->GetBuffer(i, IID_PPV_ARGS(&resource));
		if (FAILED(hr)) {
			delete[] pResult->_back_buffer_image_views;
			delete[] pResult->_back_buffer_images;
			free(pResult);
			return SPUDRESULT_API_SPECIFIC_FAILURE;
		}

		spudgpu_image_d3d12 &img = pResult->_back_buffer_images[i];
		resource.As(&img._d3d_resource);
		img._device            = device;
		img._desc.format       = desc->format;
		img._desc.width        = desc->width;
		img._desc.height       = desc->height;
		img._desc.depth        = 1;
		img._desc.mip_levels   = 1;
		img._desc.array_layers = 1;
		img._desc.type         = SPUDGPU_IMAGE_TYPE_2D;
		img._desc.usage        = SPUDGPU_IMAGE_USAGE_COLOR_ATTACHMENT;
		img._d3d_resource_desc = img._d3d_resource->GetDesc();

		spudgpu_image_view_d3d12 &view = pResult->_back_buffer_image_views[i];
		view._image                    = &img;
		view._desc.type                = SPUDGPU_IMAGE_VIEW_TYPE_2D;
		view._desc.parent_image        = &img;
		// view._desc.format                             = desc->format;
		view._d3d_view_desc._rtv.Format        = fmt;
		view._d3d_view_desc._rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		view._d3d_view_desc._rtv.Texture2D.MipSlice   = 0;
		view._d3d_view_desc._rtv.Texture2D.PlaneSlice = 0;
	}

	*out_swap_chain = pResult;
	return SPUD_SUCCESS;
}

void spudgpu_destroy_swap_chain(spudgpu_swap_chain swap_chain) {
	if (!swap_chain)
		return;
	uint32_t count = swap_chain->_desc.buffer_count;
	if (swap_chain->_back_buffer_images) {
		for (uint32_t i = 0; i < count; ++i)
			swap_chain->_back_buffer_images[i]._d3d_resource.Reset();
		delete[] swap_chain->_back_buffer_images;
	}
	if (swap_chain->_back_buffer_image_views)
		delete[] swap_chain->_back_buffer_image_views;
	swap_chain->_dxgi_swap_chain.Reset();
	free(swap_chain);
}

SPUDRESULT spudgpu_get_swap_chain_desc(
    spudgpu_swap_chain swap_chain, spudgpu_swap_chain_desc *out_desc) {
	if (!swap_chain)
		return SPUDRESULT_GPU_INVALID_SWAP_CHAIN;
	if (!out_desc)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	*out_desc = swap_chain->_desc;
	return SPUD_SUCCESS;
}

uint32_t spudgpu_swap_chain_acquire_next_image(spudgpu_swap_chain swap_chain) {
	if (!swap_chain)
		return 0;
	return swap_chain->_dxgi_swap_chain->GetCurrentBackBufferIndex();
}

void spudgpu_swap_chain_present(spudgpu_swap_chain swap_chain) {
	if (!swap_chain)
		return;
	UINT syncInterval =
	    (swap_chain->_desc.present_mode == SPUDGPU_PRESENT_MODE_IMMEDIATE) ? 0
	                                                                       : 1;
	swap_chain->_dxgi_swap_chain->Present(syncInterval, 0);
}

spudgpu_image_view spudgpu_get_swap_chain_image_view(
    spudgpu_swap_chain swap_chain, uint32_t image_index) {
	if (!swap_chain || image_index >= swap_chain->_desc.buffer_count)
		return nullptr;
	return &swap_chain->_back_buffer_image_views[image_index];
}
}

#endif // SPUDGPU_COMPILE_D3D12_API
