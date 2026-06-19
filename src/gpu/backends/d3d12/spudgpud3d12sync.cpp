
#if SPUDGPU_COMPILE_D3D12_API

#include "spudgpud3d12.hpp"

static D3D12_FENCE_FLAGS
spudgpu_d3d12_get_fence_flags(SPUDGPU_FENCE_FLAGS flags) {
	D3D12_FENCE_FLAGS result = D3D12_FENCE_FLAG_NONE;
	switch (flags) {
	case SPUDGPU_FENCE_FLAG_SHARED:
		result |= D3D12_FENCE_FLAG_SHARED;
		break;
	}
	return result;
}

extern "C" {

SPUDRESULT spudgpu_create_fence(
    spudgpu_device device,
    SPUDGPU_FENCE_FLAGS flags,
    bool signaled_on_creation,
    spudgpu_fence *out_fence) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!out_fence)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	spudgpu_fence_d3d12 *pResult =
	    (spudgpu_fence_d3d12 *)calloc(1, sizeof(spudgpu_fence_d3d12));
	pResult->_device              = device;
	pResult->flags                = flags;
	pResult->signaled_on_creation = signaled_on_creation;

	uint64_t initial_value = signaled_on_creation ? 1 : 0;
	pResult->_signal_value = initial_value;

	if (FAILED(device->_d3d_device->CreateFence(
	        initial_value, spudgpu_d3d12_get_fence_flags(flags),
	        IID_PPV_ARGS(&pResult->_d3d_fence)))) {
		free(pResult);
		return SPUDRESULT_API_SPECIFIC_FAILURE;
	}

	*out_fence = pResult;

	return SPUD_SUCCESS;
}

void spudgpu_destroy_fence(spudgpu_fence fence) {
	if (!fence)
		return;
	fence->_d3d_fence.Reset();
	free(fence);
}

uint64_t spudgpu_get_fence_value(spudgpu_fence fence) {
	if (!fence)
		return 0;
	else
		return fence->_d3d_fence->GetCompletedValue();
}
SPUDRESULT spudgpu_signal_fence(
    spudgpu_device device, spudgpu_fence fence, uint64_t value) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!fence)
		return SPUDRESULT_GPU_INVALID_FENCE;
	if (FAILED(fence->_d3d_fence->Signal(value)))
		return SPUDRESULT_API_SPECIFIC_FAILURE;
	fence->_signal_value = value;
	return SPUD_SUCCESS;
}

SPUDRESULT spudgpu_wait_for_fences(
    spudgpu_device device,
    spudgpu_fence *fences,
    uint32_t fence_count,
    bool wait_all,
    uint64_t timeout_ns) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!fences)
		return SPUDRESULT_GPU_INVALID_FENCE;
	if (fence_count == 0)
		return SPUDRESULT_ZERO_SIZE;

	if (fence_count > MAXIMUM_WAIT_OBJECTS)
		return SPUDRESULT_DESC_INVALID_PARAMETERS;

	HANDLE events[MAXIMUM_WAIT_OBJECTS];
	uint32_t events_created = 0;

	for (uint32_t i = 0; i < fence_count; i++) {
		if (!fences[i]) {
			for (uint32_t j = 0; j < events_created; j++)
				CloseHandle(events[j]);
			return SPUDRESULT_GPU_INVALID_FENCE;
		}
		events[i] = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		if (!events[i]) {
			for (uint32_t j = 0; j < events_created; j++)
				CloseHandle(events[j]);
			return SPUDRESULT_API_SPECIFIC_FAILURE;
		}
		events_created++;
		if (FAILED(
		        fences[i]->_d3d_fence->SetEventOnCompletion(
		            fences[i]->_signal_value, events[i]))) {
			for (uint32_t j = 0; j < events_created; j++)
				CloseHandle(events[j]);
			return SPUDRESULT_API_SPECIFIC_FAILURE;
		}
	}

	DWORD timeout_ms = (timeout_ns == UINT64_MAX)
	                       ? INFINITE
	                       : (DWORD)(timeout_ns / 1000000ULL);
	DWORD result     = WaitForMultipleObjects(
	    fence_count, events, wait_all ? TRUE : FALSE, timeout_ms);

	for (uint32_t i = 0; i < fence_count; i++)
		CloseHandle(events[i]);

	if (result == WAIT_TIMEOUT)
		return SPUDRESULT_GENERAL_FAILURE;
	if (result == WAIT_FAILED)
		return SPUDRESULT_API_SPECIFIC_FAILURE;
	return SPUD_SUCCESS;
}

SPUDRESULT spudgpu_create_semaphore(
    spudgpu_device device, spudgpu_semaphore *out_semaphore) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!out_semaphore)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	spudgpu_semaphore_d3d12 *pResult =
	    (spudgpu_semaphore_d3d12 *)calloc(1, sizeof(spudgpu_semaphore_d3d12));
	pResult->_device = device;

	if (FAILED(device->_d3d_device->CreateFence(
	        0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pResult->_d3d_fence)))) {
		free(pResult);
		return SPUDRESULT_API_SPECIFIC_FAILURE;
	}

	*out_semaphore = pResult;

	return SPUD_SUCCESS;
}

void spudgpu_destroy_semaphore(spudgpu_semaphore semaphore) {
	if (!semaphore)
		return;
	semaphore->_d3d_fence.Reset();
	free(semaphore);
}
}

#endif // SPUDGPU_COMPILE_D3D12_API
