
#if SPUDGPU_COMPILE_VULKAN_API

#include <vulkan/vulkan.h>
#include "spudgpu.h"
#include "spudgpuvulkan.h"
#include <stdlib.h>
#include <string.h>

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

SPUDRESULT spudgpu_create_buffer(
    spudgpu_device device,
    const spudgpu_buffer_desc *desc,
    spudgpu_buffer *out_buffer) {
    if (!device) return SPUDRESULT_GPU_INVALID_DEVICE;
    if (!desc) return SPUDRESULT_NULL_DESC;
    if (!out_buffer) return SPUD_SUCCESS;

    // Validate desc
    {
        /*if (desc.gpu_address_location == 0)
            throw std::runtime_error(
                "gpu_buffer_vulkan::gpu_buffer_vulkan: invalid GPU address location: " + std::to_string(
                    desc.gpu_address_location));*/

        if (desc->size == 0)
            //throw std::runtime_error(
            //   "SpudGPU create_buffer: invalid size: " + std::to_string(desc->size));
            return SPUDRESULT_GPU_ZERO_BUFFER_SIZE;

        if (desc->usage == 0)
            //throw std::runtime_error(
            //  "SpudGPU create_buffer: invalid usage: SPUDGPU_BUFFER_USAGE_NONE");
            return SPUDRESULT_GPU_INVALID_BUFFER_USAGE;
    }

    // Create the result struct
    spudgpu_buffer_vulkan result = {0};
    result._device = *device;
    result._desc = *desc;

    // Get native Vulkan device handles
    VkDevice vk_device = result._device._logical_device_vk;
    VkPhysicalDevice vk_physical_device = result._device._physical_device_vk;

    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.pNext = NULL;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = desc->size;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // Covert SPUDGPU_BUFFER_USAGE to native Vulkan usage
    spudgpuvulkan___buffer_usage_flags_internal(desc->usage, &bufferInfo.usage);

    // Every spudgpu_buffer gets a valid desc.gpu_address_location after
    // creation (see spudgpu.h), mirroring D3D12's always-available
    // GetGPUVirtualAddress — so every buffer needs this usage bit.
    bufferInfo.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

    // Create buffer
    if (vkCreateBuffer(vk_device, &bufferInfo, NULL, &result._buffer_vk) != VK_SUCCESS) {
        //throw std::runtime_error("SpudGPU Vulkan: failed to create buffer!");
        return SPUDRESULT_API_SPECIFIC_FAILURE;
    }

    // Allocate memory
    {
        // TODO: Vulkan Memory Property Flags
        VkMemoryPropertyFlags properties = 0;
        spudgpuvulkan___memory_property_flags_internal(desc->memory_flags, &properties);

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(vk_device, result._buffer_vk, &memRequirements);

        // Required whenever the buffer was created with
        // VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT (see bufferInfo.usage
        // above) — vkGetBufferDeviceAddress below requires the backing
        // memory to have been allocated with this flag set.
        VkMemoryAllocateFlagsInfo allocFlagsInfo = {0};
        allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

        VkMemoryAllocateInfo allocInfo = {0};
        allocInfo.pNext = &allocFlagsInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = spudgpuvulkan___find_memory_type_internal(
            vk_physical_device, memRequirements.memoryTypeBits,
            properties);

        if (vkAllocateMemory(vk_device, &allocInfo, NULL, &result._memory_vk) != VK_SUCCESS) {
            //throw std::runtime_error("SpudGPU Vulkan: failed to allocate buffer memory!");
            vkDestroyBuffer(vk_device, result._buffer_vk, NULL);
            return SPUDRESULT_API_SPECIFIC_FAILURE;
        }

        if (vkBindBufferMemory(vk_device, result._buffer_vk, result._memory_vk, 0) != VK_SUCCESS) {
            vkDestroyBuffer(vk_device, result._buffer_vk, NULL);
            vkFreeMemory(vk_device, result._memory_vk, NULL);
            return SPUDRESULT_API_SPECIFIC_FAILURE;
        }
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
    *out_buffer = pResult;
    return SPUD_SUCCESS;
}

SPUDRESULT spudgpu_get_buffer_desc(
    spudgpu_buffer buffer,
    spudgpu_buffer_desc *out_desc) {
    if (!buffer) return SPUDRESULT_GPU_INVALID_BUFFER;
    if (out_desc) *out_desc = buffer->_desc;
    return SPUD_SUCCESS;
}

void spudgpu_destroy_buffer(spudgpu_buffer buffer) {
    if (!buffer) return;
    vkDestroyBuffer(buffer->_device._logical_device_vk, buffer->_buffer_vk, NULL);
    vkFreeMemory(buffer->_device._logical_device_vk, buffer->_memory_vk, NULL);
    free(buffer);
}

SPUDRESULT spudgpu_create_buffer_view(
    spudgpu_buffer buffer,
    const spudgpu_buffer_view_desc *desc,
    spudgpu_buffer_view *out_view) {
    if (!buffer) return SPUDRESULT_GPU_INVALID_BUFFER;
    if (!desc) return SPUDRESULT_NULL_DESC;
    if (!out_view) return SPUD_SUCCESS;

	// Validate desc
	if (desc->size == 0)
		return SPUDRESULT_GPU_ZERO_BUFFER_SIZE;

	// Validate that the view range fits within the parent buffer
	if (desc->offset_from_parent_buffer + desc->size > buffer->_desc.size)
		return SPUDRESULT_GPU_BUFFER_OR_IMAGE_VIEW_RANGE_OUT_OF_SCOPE;

	spudgpu_buffer_view_vulkan result = {0};
    result._desc = *desc;
    result._desc.parent_buffer = buffer;
    result._buffer_view_vk = VK_NULL_HANDLE; // Only needed for texel buffers

    /*
     * Vulkan buffer views are not needed for anything other than a texel buffer.
     * This is because buffer handles + offsets are passed directly into Vulkan commands.
     * Unlike D3D12 which has explicit D3D12*_VIEW handles.
     */

    // If all successful, return a memcpy'ed heap pointer
    spudgpu_buffer_view pResult = malloc(sizeof(spudgpu_buffer_view_vulkan));
    memcpy(pResult, &result, sizeof(spudgpu_buffer_view_vulkan));
    *out_view = pResult;
    return SPUD_SUCCESS;
}

SPUDRESULT spudgpu_get_buffer_view_desc(
    spudgpu_buffer_view buffer_view,
    spudgpu_buffer_view_desc *out_desc) {
    if (!buffer_view) return SPUDRESULT_GPU_INVALID_BUFFER_VIEW;
    if (out_desc) *out_desc = buffer_view->_desc;
    return SPUD_SUCCESS;
}

void spudgpu_destroy_buffer_view(spudgpu_buffer_view buffer_view) {
    if (!buffer_view) return;
	vkDestroyBufferView(
	    buffer_view->_desc.parent_buffer->_device._logical_device_vk,
	    buffer_view->_buffer_view_vk, NULL);
	free(buffer_view);
}

SPUDRESULT spudgpu_map_buffer(
    spudgpu_buffer buffer,
    uint64_t offset,
    uint64_t size,
    void **ppData) {
    if (!buffer) return SPUDRESULT_GPU_INVALID_BUFFER;
    //if (!size) return SPUDRESULT_GPU_ZERO_BUFFER_SIZE;
    if (offset + size > buffer->_desc.size) return SPUDRESULT_GPU_MAP_OUT_OF_RANGE;

    // Guard: must have been allocated with HOST_VISIBLE
    if (!(buffer->_desc.memory_flags & SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE))
        return SPUDRESULT_GPU_INVALID_MEMORY_FLAGS;

    VkDeviceSize mapSize = (size == 0) ? buffer->_desc.size : size;

    if (vkMapMemory(
        buffer->_device._logical_device_vk,
        buffer->_memory_vk,
        (VkDeviceSize) offset,
        mapSize,
        0, // flags — reserved, must be 0
        ppData) != VK_SUCCESS)
        return SPUDRESULT_API_SPECIFIC_FAILURE;

	return SPUD_SUCCESS;
}

void spudgpu_unmap_buffer(spudgpu_buffer buffer) {
    if (!buffer) return;
    vkUnmapMemory(
        buffer->_device._logical_device_vk,
        buffer->_memory_vk);
}

void spudgpu_flush_buffer(
    spudgpu_buffer buffer,
    uint64_t offset,
    uint64_t size) {
    if (!buffer) return;
    
    VkMappedMemoryRange range = {0};
    range.pNext = NULL;
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = buffer->_memory_vk;
    range.offset = (VkDeviceSize) offset;
    range.size = (size == 0) ? VK_WHOLE_SIZE : (VkDeviceSize) size;

    vkFlushMappedMemoryRanges(
        buffer->_device._logical_device_vk,
        1, &range);
}

SPUDRESULT spudgpu_invalidate_buffer(
    spudgpu_buffer buffer,
    uint64_t offset,
    uint64_t size) {
    if (!buffer) return SPUD_SUCCESS;

    VkMappedMemoryRange range = {0};
    range.pNext = NULL;
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = buffer->_memory_vk;
    range.offset = (VkDeviceSize) offset;
    range.size = (size == 0) ? VK_WHOLE_SIZE : (VkDeviceSize) size;

    if (vkInvalidateMappedMemoryRanges(
        buffer->_device._logical_device_vk,
        1, &range) != VK_SUCCESS )
        return SPUDRESULT_API_SPECIFIC_FAILURE;
    
    return SPUD_SUCCESS;
}

#if __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_VULKAN_API
