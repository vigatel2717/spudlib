
#if SPUDGPU_COMPILE_VULKAN_API

#include <vulkan/vulkan.h>
#include "spudgpu.h"
#include "spudgpuvulkan.h"
#include <stdlib.h>

extern uint32_t spudgpuvulkan___find_memory_type_internal(
    VkPhysicalDevice physicalDevice,
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties);

extern void spudgpuvulkan___memory_property_flags_internal(
    uint32_t __spud_gpu_memory_flags,
    VkMemoryPropertyFlags *output);

void spudgpuvulkan___image_usage_flags_internal(
    uint32_t __spud_gpu_image_usage,
    VkImageUsageFlags *output) {
    VkImageUsageFlags flags = 0;

    if (__spud_gpu_image_usage & SPUDGPU_IMAGE_USAGE_SAMPLED)
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;

    if (__spud_gpu_image_usage & SPUDGPU_IMAGE_USAGE_COLOR_ATTACHMENT)
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (__spud_gpu_image_usage & SPUDGPU_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT)
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if (__spud_gpu_image_usage & SPUDGPU_IMAGE_USAGE_STORAGE)
        flags |= VK_IMAGE_USAGE_STORAGE_BIT;

    if (__spud_gpu_image_usage & SPUDGPU_IMAGE_USAGE_TRANSFER_SRC)
        flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    if (__spud_gpu_image_usage & SPUDGPU_IMAGE_USAGE_TRANSFER_DST)
        flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    // Uploading data to device-local images always requires transfer dst
    flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    *output = flags;
}

void spudgpuvulkan___image_type_internal(
    uint32_t ___spud_gpu_image_type,
    VkImageType *output) {
    VkImageType flags = 0;
    if (___spud_gpu_image_type & SPUDGPU_IMAGE_TYPE_1D)
        flags |= VK_IMAGE_TYPE_1D;
    if (___spud_gpu_image_type & SPUDGPU_IMAGE_TYPE_2D)
        flags |= VK_IMAGE_TYPE_2D;
    if (___spud_gpu_image_type & SPUDGPU_IMAGE_TYPE_3D)
        flags |= VK_IMAGE_TYPE_3D;
    *output = flags;
}

void spudgpuvulkan___image_view_type_internal(
    uint32_t ___spud_gpu_image_view_type,
    VkImageViewType *output) {
    VkImageViewType flags = 0;
    if (___spud_gpu_image_view_type & SPUDGPU_IMAGE_VIEW_TYPE_1D)
        flags |= VK_IMAGE_VIEW_TYPE_1D;
    if (___spud_gpu_image_view_type & SPUDGPU_IMAGE_VIEW_TYPE_2D)
        flags |= VK_IMAGE_VIEW_TYPE_2D;
    if (___spud_gpu_image_view_type & SPUDGPU_IMAGE_VIEW_TYPE_3D)
        flags |= VK_IMAGE_VIEW_TYPE_3D;
    if (___spud_gpu_image_view_type & SPUDGPU_IMAGE_VIEW_TYPE_CUBE)
        flags |= VK_IMAGE_VIEW_TYPE_CUBE;
    if (___spud_gpu_image_view_type & SPUDGPU_IMAGE_VIEW_TYPE_1D_ARRAY)
        flags |= VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    if (___spud_gpu_image_view_type & SPUDGPU_IMAGE_VIEW_TYPE_2D_ARRAY)
        flags |= VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    if (___spud_gpu_image_view_type & SPUDGPU_IMAGE_VIEW_TYPE_CUBE_ARRAY)
        flags |= VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    *output = flags;
}

#if __cplusplus
extern "C" {
#endif

spudgpu_image spudgpu_create_image(spudgpu_device device, const spudgpu_image_desc *desc) {
    if (!desc || !device) return nullptr;

    // Validate desc
    {
        if (!(desc->width && desc->height && desc->depth && desc->format && desc->array_layers && desc->mip_levels))
            return nullptr;
        if (desc->usage == 0) return nullptr;
    }

    // Create the result struct
    spudgpu_image_vulkan result = {0};
    result._device = *((spudgpu_device_vulkan *) device);
    result._desc = *desc;
    result._format_vk = convert_spud_to_vulkan_format(result._desc.format);

    // Get native Vulkan device handles

    auto vk_device = result._device._logical_device_vk;
    auto vk_physical_device = result._device._physical_device_vk;

    VkImageCreateInfo imageInfo = {0};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = NULL;
    imageInfo.flags = 0;
    imageInfo.mipLevels = result._desc.mip_levels;
    imageInfo.arrayLayers = result._desc.array_layers;
    imageInfo.format = result._format_vk;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    spudgpuvulkan___image_usage_flags_internal(result._desc.usage, &imageInfo.usage);
    spudgpuvulkan___image_type_internal(result._desc.type, &imageInfo.imageType);

    // Determine the Image Type and Width Height Depth
    imageInfo.extent.width = result._desc.width;
    imageInfo.extent.height = result._desc.height;
    imageInfo.extent.depth = result._desc.depth;

    if (vkCreateImage(vk_device, &imageInfo, nullptr, &result._image_vk) != VK_SUCCESS) {
        //throw std::runtime_error("Failed to create vulkan image!");
        return nullptr;
    }

    // Create the Vulkan Memory
    {
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(vk_device, result._image_vk, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {0};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = spudgpuvulkan___find_memory_type_internal(
            vk_physical_device, memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (vkAllocateMemory(vk_device, &allocInfo, nullptr, &result._memory_vk) != VK_SUCCESS) {
            //throw std::runtime_error("Failed to allocate image memory!");
            vkDestroyImage(vk_device, result._image_vk, nullptr);
            return nullptr;
        }

        vkBindImageMemory(vk_device, result._image_vk, result._memory_vk, 0);
    }

    // If all successful, return a memcpy'ed heap pointer
    spudgpu_image_vulkan *pResult = malloc(sizeof(spudgpu_image_vulkan));
    memcpy(pResult, &result, sizeof(spudgpu_image_vulkan));
    return (spudgpu_image) pResult;
}

spudgpu_image_desc spudgpu_get_image_desc(spudgpu_image image) {
    if (!image) return (spudgpu_image_desc){0};
    return ((spudgpu_image_vulkan *) image)->_desc;
}

void spudgpu_destroy_image(spudgpu_device device, spudgpu_image image) {
    if (!(device && image)) return;
    spudgpu_image_vulkan *vkimage = (spudgpu_image_vulkan *) image;
    vkDestroyImage(vkimage->_device._logical_device_vk, vkimage->_image_vk, nullptr);
    vkFreeMemory(vkimage->_device._logical_device_vk, vkimage->_memory_vk, nullptr);
    free(vkimage);
}

spudgpu_image_view spudgpu_create_image_view(spudgpu_image image, const spudgpu_image_view_desc *desc) {
    if (!desc) return (spudgpu_image_view){0};

    spudgpu_image_vulkan *vk_image = (spudgpu_image_vulkan *) image;

    spudgpu_image_view_vulkan result = {0};
    result._desc = *desc;
    result._parent_image = *vk_image;

    VkImageViewCreateInfo viewInfo = {0};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.pNext = nullptr;
    viewInfo.image = vk_image->_image_vk;
    viewInfo.format = vk_image->_format_vk;

    // Map image type to view type
    spudgpuvulkan___image_view_type_internal(result._desc.type, &viewInfo.viewType);

    // Standard RGBA component swizzle (identity)
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    // Subresource range — which mip levels and array layers this view covers
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = desc->subresource_range.base_mip_level;
    viewInfo.subresourceRange.levelCount = desc->subresource_range.mip_level_count;
    viewInfo.subresourceRange.baseArrayLayer = desc->subresource_range.base_array_layer;
    viewInfo.subresourceRange.layerCount = desc->subresource_range.array_layer_count;

    if (vkCreateImageView(
            vk_image->_device._logical_device_vk,
            &viewInfo, nullptr,
            &result._image_view_vk) != VK_SUCCESS) {
        return nullptr;
    }

    // If all successful, return a memcpy'ed heap pointer
    spudgpu_image_view_vulkan *pResult = malloc(sizeof(spudgpu_image_view_vulkan));
    memcpy(pResult, &result, sizeof(spudgpu_image_view_vulkan));
    return (spudgpu_image_view) pResult;
}

void spudgpu_destroy_image_view(spudgpu_device device, spudgpu_image_view image_view) {
    if (!(device && image_view)) return;
    spudgpu_image_view_vulkan *vkImageView = (spudgpu_image_view_vulkan *) image_view;
    vkDestroyImageView(vkImageView->_parent_image._device._logical_device_vk, vkImageView->_image_view_vk, nullptr);
    free(vkImageView);
}

spudgpu_image_view_desc spudgpu_get_image_view_desc(spudgpu_image_view image_view) {
    if (!image_view) return (spudgpu_image_view_desc){0};
    return ((spudgpu_image_view_vulkan *) image_view)->_desc;
}

#if __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_VULKAN_API
