#if SPUDGPU_COMPILE_VULKAN_API

#include "spudgpuvulkan.h"
#include <stdint.h>

#if __cplusplus
extern "C" {

#endif

VkInstance spudgpu_get_vk_instance(spudgpu_instance instance) {
    if (!instance) return NULL;
    return ((spudgpu_instance_vulkan *) instance)->_instance_vk;
}

VkPhysicalDevice spudgpu_get_vk_physical_device(spudgpu_device device) {
    if (!device) return NULL;
    return ((spudgpu_device_vulkan *) device)->_physical_device_vk;
}

VkDevice spudgpu_get_vk_device(spudgpu_device device) {
    if (!device) return NULL;
    return ((spudgpu_device_vulkan *) device)->_logical_device_vk;
}

VkQueue spudgpu_get_vk_queue(spudgpu_command_queue queue) {
    if (!queue) return NULL;
    return ((spudgpu_command_queue_vulkan *) queue)->_queue_vk;
}

uint32_t spudgpu_get_vk_queue_family_index(spudgpu_command_queue queue) {
    if (!queue) return UINT32_MAX;
    return ((spudgpu_command_queue_vulkan *) queue)->_queue_family_index;
}

VkCommandBuffer spudgpu_get_vk_command_buffer(spudgpu_command_list cmd) {
    if (!cmd) return NULL;
    return ((spudgpu_command_list_vulkan *) cmd)->_command_buffer_vk;
}

VkImageView spudgpu_get_vk_image_view(spudgpu_image_view view) {
    if (!view) return NULL;
    // VkImageView is a non-dispatchable handle (uint64_t in the Vulkan spec).
    // The reinterpret through uintptr_t is lossless on 64-bit targets.
    return ((spudgpu_image_view_vulkan *) view)->_image_view_vk;
}

VkFormat spudgpu_get_vk_swap_chain_format(spudgpu_swap_chain swap_chain) {
    if (!swap_chain) return 0;
    return ((spudgpu_swap_chain_vulkan *) swap_chain)->_format_vk;
}

#if __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_VULKAN_API
