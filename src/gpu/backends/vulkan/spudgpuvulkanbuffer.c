#if SPUDGPU_COMPILE_VULKAN_API

#include <vulkan/vulkan.h>
#include "spudgpu.h"
#include "spudgpuvulkan.h"
#include <stdlib.h>

uint32_t spudgpuvulkan___find_memory_type_internal(
    VkPhysicalDevice physicalDevice,
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return (uint32_t) -1;
    //throw std::runtime_error("SpudGPU Vulkan: failed to find suitable memory type!");
}

void spudgpuvulkan___buffer_usage_flags_internal(
    uint32_t __spud_gpu_buffer_usage,
    VkBufferUsageFlags *output) {
    VkBufferUsageFlags *const outputValue = output;
    if (__spud_gpu_buffer_usage & SPUDGPU_BUFFER_USAGE_VERTEX)
        *outputValue |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (__spud_gpu_buffer_usage & SPUDGPU_BUFFER_USAGE_INDEX)
        *outputValue |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (__spud_gpu_buffer_usage & SPUDGPU_BUFFER_USAGE_UNIFORM)
        *outputValue |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if (__spud_gpu_buffer_usage & SPUDGPU_BUFFER_USAGE_STORAGE)
        *outputValue |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
}

void spudgpuvulkan___memory_property_flags_internal(
    uint32_t __spud_gpu_memory_flags,
    VkMemoryPropertyFlags *output) {
    VkMemoryPropertyFlags *const outputValue = output;
    if (__spud_gpu_memory_flags & SPUDGPU_MEMORY_FLAGS_DEVICE_LOCAL)
        *outputValue |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    if (__spud_gpu_memory_flags & SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE)
        *outputValue |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    if (__spud_gpu_memory_flags & SPUDGPU_MEMORY_FLAGS_HOST_COHERENT)
        *outputValue |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    if (__spud_gpu_memory_flags & SPUDGPU_MEMORY_FLAGS_HOST_CACHED)
        *outputValue |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
}

#if __cplusplus
extern "C" {



#endif

spudgpu_buffer spudgpu_create_buffer(spudgpu_device device, const spudgpu_buffer_desc *desc) {
    if (!(desc && device)) return nullptr;

    // Validate desc
    {
        /*if (desc.gpu_address_location == 0)
            throw std::runtime_error(
                "gpu_buffer_vulkan::gpu_buffer_vulkan: invalid GPU address location: " + std::to_string(
                    desc.gpu_address_location));*/

        if (desc->size == 0)
            //throw std::runtime_error(
            //   "SpudGPU create_buffer: invalid size: " + std::to_string(desc->size));
            return nullptr;

        if (desc->usage == 0)
            //throw std::runtime_error(
            //  "SpudGPU create_buffer: invalid usage: SPUDGPU_BUFFER_USAGE_NONE");
            return nullptr;
    }

    // Create the result struct
    spudgpu_buffer_vulkan result = {0};
    memcpy(&result._device, (spudgpu_device_vulkan *) device, sizeof(spudgpu_device_vulkan));
    result._desc = *desc;

    // Get native Vulkan device handles
    auto vk_device = result._device._logical_device_vk;
    auto vk_physical_device = result._device._physical_device_vk;

    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.pNext = NULL;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = desc->size;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // Covert SPUDGPU_BUFFER_USAGE to native Vulkan usage
    spudgpuvulkan___buffer_usage_flags_internal(desc->usage, &bufferInfo.usage);

    // Create buffer
    if (vkCreateBuffer(vk_device, &bufferInfo, nullptr, &result._buffer_vk) != VK_SUCCESS) {
        //throw std::runtime_error("SpudGPU Vulkan: failed to create buffer!");
        return nullptr;
    }

    // Allocate memory
    {
        // TODO: Vulkan Memory Property Flags
        VkMemoryPropertyFlags properties = 0;
        spudgpuvulkan___memory_property_flags_internal(desc->memory_flags, &properties);

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(vk_device, result._buffer_vk, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {0};
        allocInfo.pNext = NULL;
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = spudgpuvulkan___find_memory_type_internal(
            vk_physical_device, memRequirements.memoryTypeBits,
            properties);

        if (vkAllocateMemory(vk_device, &allocInfo, nullptr, &result._memory_vk) != VK_SUCCESS) {
            //throw std::runtime_error("SpudGPU Vulkan: failed to allocate buffer memory!");
            vkDestroyBuffer(vk_device, result._buffer_vk, nullptr);
            return nullptr;
        }

        vkBindBufferMemory(vk_device, result._buffer_vk, result._memory_vk, 0);
    }

    // Retrieve the GPU Address point for gpu_buffer_desc
    {
        VkBufferDeviceAddressInfoKHR info = {0};
        info.pNext = NULL;
        info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.buffer = result._buffer_vk;
        result._desc.gpu_address_location = vkGetBufferDeviceAddress(vk_device, &info);
    }

    // If all successful, return a memcpy'ed heap pointer
    spudgpu_buffer_vulkan *pResult = malloc(sizeof(spudgpu_buffer_vulkan));
    memcpy(pResult, &result, sizeof(spudgpu_buffer_vulkan));
    return (spudgpu_buffer) pResult;
}

spudgpu_buffer_desc spudgpu_get_buffer_desc(spudgpu_buffer buffer) {
    if (!buffer) return (spudgpu_buffer_desc){0};
    return ((spudgpu_buffer_vulkan *) buffer)->_desc;
}

void spudgpu_destroy_buffer(spudgpu_device device, spudgpu_buffer buffer) {
    if (!(device && buffer)) return;
    spudgpu_buffer_vulkan *vkBuffer = (spudgpu_buffer_vulkan *) buffer;
    vkDestroyBuffer(vkBuffer->_device._logical_device_vk, vkBuffer->_buffer_vk, nullptr);
    vkFreeMemory(vkBuffer->_device._logical_device_vk, vkBuffer->_memory_vk, nullptr);
    free(vkBuffer);
}

spudgpu_buffer_view spudgpu_create_buffer_view(spudgpu_buffer buffer, const spudgpu_buffer_view_desc *desc) {
    if (!(buffer && desc)) return nullptr;

    spudgpu_buffer_vulkan *vkBuffer = (spudgpu_buffer_vulkan *) buffer;

    // Validate that the view range fits within the parent buffer
    if (desc->offset_from_parent_buffer + desc->size > vkBuffer->_desc.size)
        return nullptr;

    spudgpu_buffer_view_vulkan result = {0};
    result._desc = *desc;
    result._desc.parent_buffer = buffer;
    result._parent_buffer = *vkBuffer;
    result._buffer_view_vk = VK_NULL_HANDLE; // Only needed for texel buffers


    // If all successful, return a memcpy'ed heap pointer
    spudgpu_buffer_vulkan *pResult = malloc(sizeof(spudgpu_buffer_view_vulkan));
    memcpy(pResult, &result, sizeof(spudgpu_buffer_view_vulkan));
    return (spudgpu_buffer_view) pResult;
}

spudgpu_buffer_view_desc spudgpu_get_buffer_view_desc(spudgpu_buffer_view buffer_view) {
    if (!buffer_view) return (spudgpu_buffer_view_desc){0};
    return ((spudgpu_buffer_view_vulkan *) buffer_view)->_desc;
}

void spudgpu_destroy_buffer_view(spudgpu_buffer_view buffer_view) {
    if (!buffer_view) return;
    spudgpu_buffer_view_vulkan *vkBufferView = (spudgpu_buffer_view_vulkan *) buffer_view;
    vkDestroyBufferView(vkBufferView->_parent_buffer._device._logical_device_vk, vkBufferView->_buffer_view_vk,
                        nullptr);
    free(vkBufferView);
}

bool spudgpu_map_buffer(
    spudgpu_buffer buffer,
    uint64_t offset,
    uint64_t size,
    void **ppData) {
    if (!(buffer && ppData)) return false;
    spudgpu_buffer_vulkan *vkBuffer = (spudgpu_buffer_vulkan *) buffer;

    // Guard: must have been allocated with HOST_VISIBLE
    if (!(vkBuffer->_desc.memory_flags & SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE))
        return false;

    VkDeviceSize mapSize = (size == 0) ? vkBuffer->_desc.size : size;

    VkResult result = vkMapMemory(
        vkBuffer->_device._logical_device_vk,
        vkBuffer->_memory_vk,
        (VkDeviceSize) offset,
        mapSize,
        0, // flags — reserved, must be 0
        ppData);

    return result == VK_SUCCESS;
}

void spudgpu_unmap_buffer(spudgpu_buffer buffer) {
    if (!buffer) return;
    spudgpu_buffer_vulkan *vkBuffer = (spudgpu_buffer_vulkan *) buffer;
    vkUnmapMemory(
        vkBuffer->_device._logical_device_vk,
        vkBuffer->_memory_vk);
}

void spudgpu_flush_buffer(
    spudgpu_buffer buffer,
    uint64_t offset,
    uint64_t size) {
    if (!buffer) return;
    spudgpu_buffer_vulkan *vkBuffer = (spudgpu_buffer_vulkan *) buffer;

    VkMappedMemoryRange range = {0};
    range.pNext = NULL;
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = vkBuffer->_memory_vk;
    range.offset = (VkDeviceSize) offset;
    range.size = (size == 0) ? VK_WHOLE_SIZE : (VkDeviceSize) size;

    vkFlushMappedMemoryRanges(
        vkBuffer->_device._logical_device_vk,
        1, &range);
}

void spudgpu_invalidate_buffer(
    spudgpu_buffer buffer,
    uint64_t offset,
    uint64_t size) {
    if (!buffer) return;
    spudgpu_buffer_vulkan *vkBuffer = (spudgpu_buffer_vulkan *) buffer;

    VkMappedMemoryRange range = {0};
    range.pNext = NULL;
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = vkBuffer->_memory_vk;
    range.offset = (VkDeviceSize) offset;
    range.size = (size == 0) ? VK_WHOLE_SIZE : (VkDeviceSize) size;

    vkInvalidateMappedMemoryRanges(
        vkBuffer->_device._logical_device_vk,
        1, &range);
}

#if __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_VULKAN_API
