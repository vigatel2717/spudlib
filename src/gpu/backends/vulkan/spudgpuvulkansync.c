
#if SPUDGPU_COMPILE_VULKAN_API

#include "spudgpuvulkan.h"
#include <stdlib.h>
#include <stdio.h>

#if __cplusplus
extern "C" {
#endif

spudgpu_fence spudgpu_create_fence(spudgpu_device device, bool signaled_on_creation) {
    if (!device) return NULL;
    spudgpu_device_vulkan *dev = (spudgpu_device_vulkan *) device;

    VkFenceCreateInfo info = {0};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (signaled_on_creation) info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    spudgpu_fence_vulkan *fence = calloc(1, sizeof(spudgpu_fence_vulkan));
    if (!fence) return NULL;

    VkResult r = vkCreateFence(dev->_logical_device_vk, &info, NULL, &fence->_fence_vk);
    if (r != VK_SUCCESS) {
        printf("spudgpu: vkCreateFence failed (%d)\n", r);
        free(fence);
        return NULL;
    }
    fence->_device_vk = dev->_logical_device_vk;
    return (spudgpu_fence) fence;
}

void spudgpu_destroy_fence(spudgpu_device device, spudgpu_fence fence) {
    if (!fence) return;
    spudgpu_fence_vulkan *f = (spudgpu_fence_vulkan *) fence;
    vkDestroyFence(f->_device_vk, f->_fence_vk, NULL);
    free(f);
}

bool spudgpu_wait_for_fences(
    spudgpu_device device,
    spudgpu_fence *fences,
    uint32_t fence_count,
    bool wait_all,
    uint64_t timeout_ns) {
    if (!fences || fence_count == 0) return false;
    spudgpu_fence_vulkan *first = (spudgpu_fence_vulkan *) fences[0];

    VkFence vk_fences[fence_count];
    for (uint32_t i = 0; i < fence_count; i++) {
        spudgpu_fence_vulkan *f = (spudgpu_fence_vulkan *) fences[i];
        vk_fences[i] = f->_fence_vk;
    }

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

    spudgpu_device_vulkan *vk_dev = (spudgpu_device_vulkan *) device;

    VkFence vk_fences[fence_count];
    for (uint32_t i = 0; i < fence_count; i++) {
        spudgpu_fence_vulkan *f = (spudgpu_fence_vulkan *) fences[i];
        vk_fences[i] = f->_fence_vk;
    }

    vkResetFences(vk_dev->_logical_device_vk, fence_count, vk_fences);
}

bool spudgpu_get_fence_status(spudgpu_fence fence) {
    if (!fence) return false;
    spudgpu_fence_vulkan *f = (spudgpu_fence_vulkan *) fence;
    return vkGetFenceStatus(f->_device_vk, f->_fence_vk) == VK_SUCCESS;
}

spudgpu_semaphore spudgpu_create_semaphore(spudgpu_device device) {
    if (!device) return NULL;
    spudgpu_device_vulkan *dev = (spudgpu_device_vulkan *) device;

    VkSemaphoreCreateInfo info = {0};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    spudgpu_semaphore_vulkan *sem = calloc(1, sizeof(spudgpu_semaphore_vulkan));
    if (!sem) return NULL;

    VkResult r = vkCreateSemaphore(dev->_logical_device_vk, &info, NULL, &sem->_semaphore_vk);
    if (r != VK_SUCCESS) {
        printf("spudgpu: vkCreateSemaphore failed (%d)\n", r);
        free(sem);
        return NULL;
    }
    sem->_device_vk = dev->_logical_device_vk;
    return (spudgpu_semaphore) sem;
}

void spudgpu_destroy_semaphore(spudgpu_semaphore semaphore) {
    if (!semaphore) return;
    spudgpu_semaphore_vulkan *sem = (spudgpu_semaphore_vulkan *) semaphore;
    vkDestroySemaphore(sem->_device_vk, sem->_semaphore_vk, NULL);
    free(sem);
}

#if __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_VULKAN_API
