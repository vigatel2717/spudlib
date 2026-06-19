
#if SPUDGPU_COMPILE_D3D12_API

#include "spudgpud3d12.hpp"
#include <vector>

static SPUDRESULT ___internal_spudgpu_d3d12_create_command_queues_per_family(
    spudgpu_device device,
    const D3D12_COMMAND_QUEUE_DESC *desc,
    std::array<spudgpu_command_queue, SPUD_D3D12_COMMAND_QUEUE_COUNT_PER_FAMILY>
        &arr) {
	for (size_t i = 0; i < arr.size(); ++i) {
		arr[i] = (spudgpu_command_queue_d3d12 *)calloc(
		    1, sizeof(spudgpu_command_queue_d3d12));
		if (device->_d3d_device->CreateCommandQueue(
		        desc, IID_PPV_ARGS(&arr[i]->_d3d_cmd_queue)))
			return SPUDRESULT_API_SPECIFIC_FAILURE;
	}
	return SPUD_SUCCESS;
}

static SPUDRESULT
___internal_spudgpu_d3d12_create_device_command_queues(spudgpu_device device) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask                 = 1;
	desc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;

	SPUDRESULT r = SPUD_SUCCESS;

	// Direct Queues
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	r = ___internal_spudgpu_d3d12_create_command_queues_per_family(
	    device, &desc, device->_cmd_queues_direct);
	if (r != SPUD_SUCCESS) return r;

	// Copy Queues
	desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	r = ___internal_spudgpu_d3d12_create_command_queues_per_family(
	    device, &desc, device->_cmd_queues_copy);
	if (r != SPUD_SUCCESS) return r;

	// Compute Queues
	desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	r = ___internal_spudgpu_d3d12_create_command_queues_per_family(
	    device, &desc, device->_cmd_queues_compute);
	if (r != SPUD_SUCCESS) return r;

	return r;
}

extern "C" {

SPUDRESULT spudgpu_create_instance(
    SPUDGPU_NATIVE_API native_api,
    const char *application_name,
    uint32_t application_version,
    const char *engine_name,
    uint32_t engine_version,
    spudgpu_instance *out_instance) {
	if (native_api == SPUDGPU_NATIVE_API_NONE)
		return SPUDRESULT_INVALID_API;
	if (!out_instance)
		return SPUD_SUCCESS;

	spudgpu_instance_d3d12 *pResult =
	    (spudgpu_instance_d3d12 *)calloc(1, sizeof(spudgpu_instance_d3d12));
	pResult->application_name        = application_name;
	pResult->application_version     = application_version;
	pResult->engine_name             = engine_name;
	pResult->engine_version          = engine_version;
	pResult->_gpu_devices            = nullptr;
	pResult->_gpu_device_count       = 0;
	pResult->_gpu_devices_enumerated = false;
	
	UINT dxgiFlags = 0;
#if _DEBUG
	dxgiFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	HRESULT hr = CreateDXGIFactory2(dxgiFlags, IID_PPV_ARGS(&pResult->_dxgi_factory));
	if (FAILED(hr)) {
		free(pResult);
		return SPUDRESULT_API_SPECIFIC_FAILURE;
	}

	*out_instance = pResult;
	return SPUD_SUCCESS;
}

static SPUDRESULT ___internal_spudgpu_d3d12_destroy_device(spudgpu_device device) {
	for (size_t i = 0; i < device->_cmd_queues_direct.size(); ++i)
		if (device->_cmd_queues_direct[i])
			device->_cmd_queues_direct[i]->_d3d_cmd_queue.Reset();
	for (size_t i = 0; i < device->_cmd_queues_copy.size(); ++i)
		if (device->_cmd_queues_copy[i])
			device->_cmd_queues_copy[i]->_d3d_cmd_queue.Reset();
	for (size_t i = 0; i < device->_cmd_queues_compute.size(); ++i)
		if (device->_cmd_queues_compute[i])
			device->_cmd_queues_compute[i]->_d3d_cmd_queue.Reset();
	device->_d3d_device.Reset();
	device->_dxgi_adapter.Reset();
	return SPUD_SUCCESS;
}
SPUDRESULT spudgpu_destroy_instance(spudgpu_instance instance) {
	if (!instance)
		return SPUD_SUCCESS;
	SPUDRESULT sr = SPUD_SUCCESS;
	for (size_t i = 0; i < instance->_gpu_device_count; ++i) {
		sr =
		    ___internal_spudgpu_d3d12_destroy_device(instance->_gpu_devices[i]);
		if (sr != SPUD_SUCCESS)
			return sr;
	}
	instance->_dxgi_factory.Reset();
	free(instance);
	return SPUD_SUCCESS;
}

SPUDRESULT spudgpu_enumerate_devices(
    spudgpu_instance instance,
    spudgpu_device **ppOutputDevices,
    uint32_t *pOutputDevicesCount) {
	if (!instance)
		return SPUDRESULT_GPU_INVALID_INSTANCE;
	if (instance->_gpu_devices_enumerated)
		return SPUD_SUCCESS;

	const D3D_FEATURE_LEVEL minimumFeatureLevel = D3D_FEATURE_LEVEL_12_2;

	std::vector<spudgpu_device_d3d12 *> gpuDevices =
	    std::vector<spudgpu_device_d3d12 *>();

	Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgiAdapter1 = nullptr;
	for (uint32_t adapterIndex = 0;
	     instance->_dxgi_factory->EnumAdapters1(adapterIndex, &dxgiAdapter1) !=
	     DXGI_ERROR_NOT_FOUND;
	     ++adapterIndex) {
		if (!dxgiAdapter1) // Don't mess with a null result.
			continue;
		DXGI_ADAPTER_DESC1 desc;
		dxgiAdapter1->GetDesc1(&desc);
		// Don't include a software graphics device.
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter = nullptr;
		if (FAILED(dxgiAdapter1.As(&dxgiAdapter)))
			continue;

		// Validate a possible creation of an ID3D12Device
		if (SUCCEEDED(D3D12CreateDevice(
		        dxgiAdapter.Get(), minimumFeatureLevel, __uuidof(ID3D12Device14),
		        nullptr))) {

			// Create a new SpudGPU Device
			spudgpu_device_d3d12 *gpuDevice =
			    (spudgpu_device_d3d12 *)calloc(1, sizeof(spudgpu_device_d3d12));
			if (FAILED(D3D12CreateDevice(
			        dxgiAdapter.Get(), minimumFeatureLevel,
			        IID_PPV_ARGS(&gpuDevice->_d3d_device))))
				// If a D3D12 Device creation failed,
				// just return out of this function.
				return SPUDRESULT_API_SPECIFIC_FAILURE;
			gpuDevice->_dxgi_adapter = dxgiAdapter;
			gpuDevice->_instance     = instance;
			// Create the premade command queues for use.
			SPUDRESULT sr = ___internal_spudgpu_d3d12_create_device_command_queues(gpuDevice);
			if (sr != SPUD_SUCCESS) return sr;
			gpuDevices.push_back(gpuDevice);
			continue;
		} else
			continue;
	}
	instance->_gpu_device_count = (uint32_t)gpuDevices.size();
	instance->_gpu_devices      = (spudgpu_device *)malloc(
	    sizeof(spudgpu_device) * instance->_gpu_device_count);
	memcpy(
	    instance->_gpu_devices, gpuDevices.data(),
	    sizeof(spudgpu_device) * instance->_gpu_device_count);
	instance->_gpu_devices_enumerated = true;

	*pOutputDevicesCount = instance->_gpu_device_count;
	*ppOutputDevices = instance->_gpu_devices;

	return SPUD_SUCCESS;
}

SPUDGPU_NATIVE_API spudgpu_get_native_gpu_api(spudgpu_instance instance) {
	return instance ? SPUDGPU_NATIVE_API_D3D12 : SPUDGPU_NATIVE_API_NONE;
}

SPUDRESULT spudgpu_create_surface(
    spudgpu_instance instance,
    void *window_handle,
    void *display_handle,
    spudgpu_surface *out_surface) {
	if (!instance)
		return SPUDRESULT_GPU_INVALID_INSTANCE;
	if (!window_handle)
		return SPUDRESULT_NULL_DESC;
	if (!out_surface)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	spudgpu_surface_d3d12 *pResult =
	    (spudgpu_surface_d3d12 *)calloc(1, sizeof(spudgpu_surface_d3d12));
	pResult->_hwnd     = (HWND)window_handle;
	pResult->_instance = instance;
	*out_surface       = pResult;
	return SPUD_SUCCESS;
}
void spudgpu_destroy_surface(spudgpu_surface surface) {
	free(surface);
}
}

#endif
