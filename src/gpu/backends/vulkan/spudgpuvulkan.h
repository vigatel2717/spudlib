//
// Created by nathanmoore on 5/18/26.
//

#ifndef SPUDLIB_SPUDGPUVULKAN_H
#define SPUDLIB_SPUDGPUVULKAN_H
#include <vulkan/vulkan_core.h>

typedef struct spudgpu_instance_vulkan {
    VkInstance _instance_vk;
} spudgpu_instance_vulkan;

typedef struct spudgpu_device_vulkan {
    spudgpu_instance_vulkan _instance;
    VkPhysicalDevice _physical_device_vk;
    VkDevice _logical_device_vk;
    VkPhysicalDeviceProperties _properties_vk;
    VkPhysicalDeviceFeatures _features_vk;
} spudgpu_device_vulkan;

typedef struct spudgpu_buffer_vulkan {
    spudgpu_buffer_desc _desc;
    spudgpu_device_vulkan _device;
    VkBuffer _buffer_vk;
    VkDeviceMemory _memory_vk;
} spudgpu_buffer_vulkan;

typedef struct spudgpu_buffer_view_vulkan {
    spudgpu_buffer_view_desc _desc;
    spudgpu_buffer_vulkan _parent_buffer;
    VkBufferView _buffer_view_vk;
} spudgpu_buffer_view_vulkan;

typedef struct spudgpu_image_vulkan {
    spudgpu_image_desc _desc;
    spudgpu_device_vulkan _device;
    VkImage _image_vk;
    VkDeviceMemory _memory_vk;
    VkFormat _format_vk;
} spudgpu_image_vulkan;

typedef struct spudgpu_image_view_vulkan {
    spudgpu_image_view_desc _desc;
    spudgpu_image_vulkan _parent_image;
    VkImageView _image_view_vk;
} spudgpu_image_view_vulkan;

typedef struct spudgpu_swap_chain_vulkan {
    spudgpu_swap_chain_desc _desc;
    spudgpu_device_vulkan _device;
    VkSurfaceKHR _surface_vk;
    VkSwapchainKHR _swapchain_vk;
    VkFormat _format_vk;
    VkExtent2D _extent_vk;
    VkImage *_swapchain_images_vk;
    uint32_t _swapchain_images_count;
    VkImageView *_swapchain_image_views_vk;
    uint32_t _swapchain_image_views_count;
} spudgpu_swap_chain_vulkan;

VkFormat convert_spud_to_vulkan_format(VkFormat format);

#endif //SPUDLIB_SPUDGPUVULKAN_H
