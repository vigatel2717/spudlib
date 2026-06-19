
#if SPUDGPU_COMPILE_VULKAN_API

#include "spudgpuvulkan.h"
#include <stdlib.h>
#include <stdio.h>

#if __cplusplus
extern "C" {
#endif

SPUDRESULT spudgpu_create_fence(
    spudgpu_device device,
    bool signaled_on_creation,
    spudgpu_fence *out_fence) {
    if (!device) return SPUDRESULT_GPU_INVALID_DEVICE;
    if (!out_fence) return SPUD_SUCCESS;
    
    VkFenceCreateInfo info = {0};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (signaled_on_creation) info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    spudgpu_fence_vulkan *fence = calloc(1, sizeof(spudgpu_fence_vulkan));
    if (!fence) return SPUDRESULT_GENERAL_FAILURE;

    VkResult r = vkCreateFence(device->_logical_device_vk, &info, NULL, &fence->_fence_vk);
    if (r != VK_SUCCESS) {
        printf("spudgpu: vkCreateFence failed (%d)\n", r);
        free(fence);
        return SPUDRESULT_API_SPECIFIC_FAILURE;
    }
    fence->_device_vk = device->_logical_device_vk;
    *out_fence = fence;
    return SPUD_SUCCESS;
}

void spudgpu_destroy_fence(spudgpu_fence fence) {
    if (!fence) return;
    vkDestroyFence(fence->_device_vk, fence->_fence_vk, NULL);
    free(fence);
}

bool spudgpu_wait_for_fences(
    spudgpu_device device,
    spudgpu_fence *fences,
    uint32_t fence_count,
    bool wait_all,
    uint64_t timeout_ns) {
    if (!fences || fence_count == 0) return false;
    spudgpu_fence_vulkan *first = fences[0];

    VkFence vk_fences[fence_count];
    for (uint32_t i = 0; i < fence_count; i++)
        vk_fences[i] = fences[i]->_fence_vk;

    VkResult r = vkWaitForFences(
        first->_device_vk,
        fence_count,
        vk_fences,
        wait_all ? VK_TRUE : VK_FALSE,
        timeout_ns);
    return r == VK_SUCCESS;
}

void spudgpu_reset_fences(spudgpu_device device, spudgpu_fence *fences, uint32_t fence_count) {
    if (!device || (!fences || fence_count == 0)) return;

    VkFence vk_fences[fence_count];
    for (uint32_t i = 0; i < fence_count; i++) 
        vk_fences[i] = fences[i]->_fence_vk;

    vkResetFences(device->_logical_device_vk, fence_count, vk_fences);
}

bool spudgpu_get_fence_status(spudgpu_fence fence) {
    if (!fence) return false;
    return vkGetFenceStatus(fence->_device_vk, fence->_fence_vk) == VK_SUCCESS;
}

SPUDRESULT spudgpu_create_semaphore(
    spudgpu_device device,
    spudgpu_semaphore *out_semaphore) {
    if (!device) return SPUDRESULT_GPU_INVALID_DEVICE;
    if (!out_semaphore) return SPUD_SUCCESS;

    VkSemaphoreCreateInfo info = {0};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    spudgpu_semaphore_vulkan *sem = calloc(1, sizeof(spudgpu_semaphore_vulkan));
    if (!sem) return SPUDRESULT_GENERAL_FAILURE;

    VkResult r = vkCreateSemaphore(device->_logical_device_vk, &info, NULL, &sem->_semaphore_vk);
    if (r != VK_SUCCESS) {
        printf("spudgpu: vkCreateSemaphore failed (%d)\n", r);
        free(sem);
        return SPUDRESULT_API_SPECIFIC_FAILURE;
    }
    sem->_device_vk = device->_logical_device_vk;
    *out_semaphore = sem;
    return SPUD_SUCCESS;
}

void spudgpu_destroy_semaphore(spudgpu_semaphore semaphore) {
    if (!semaphore) return;
    vkDestroySemaphore(semaphore->_device_vk, semaphore->_semaphore_vk, NULL);
    free(semaphore);
}

#if __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_VULKAN_API
