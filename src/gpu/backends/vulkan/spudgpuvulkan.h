//
// Created by nathanmoore on 5/18/26.
//

#ifndef SPUDLIB_SPUDGPUVULKAN_H
#define SPUDLIB_SPUDGPUVULKAN_H

#include "spudgpu.h"
#include <vulkan/vulkan.h>

typedef struct spudgpu_instance_vulkan {
    VkInstance _instance_vk;
    // Effectively this -- spudgpu_device_vulkan *_device_pointer_array[device_count];
    // We need to keep track of newly allocated spudgpu_device's to prevent memory leaks.
    // Eventually we will get rid of SPUDGPU_DEVICE_LIST.
    uint64_t *_devices_pointer_array;
    uint32_t _devices_count;
} spudgpu_instance_vulkan;

typedef struct spudgpu_device_vulkan {
    spudgpu_instance_vulkan _instance;
    VkPhysicalDevice _physical_device_vk;
    VkDevice _logical_device_vk;
    VkPhysicalDeviceProperties _properties_vk;
    VkPhysicalDeviceFeatures _features_vk;
    uint32_t _graphics_queue_family_index;
} spudgpu_device_vulkan;

typedef struct spudgpu_command_queue_vulkan {
    VkQueue _queue_vk;
} spudgpu_command_queue_vulkan;

typedef struct spudgpu_command_allocator_vulkan {
    spudgpu_device_vulkan _device;
    VkCommandPool _command_pool_vk;
    uint32_t _queue_family_index;
} spudgpu_command_allocator_vulkan;

typedef struct spudgpu_command_list_vulkan {
    spudgpu_command_allocator_vulkan _allocator;
    VkCommandBuffer _command_buffer_vk;

    // Transient framebuffer created by begin_render_pass, destroyed by end_render_pass.
    VkFramebuffer _transient_framebuffer_vk;
    VkDevice _transient_framebuffer_device_vk;
} spudgpu_command_list_vulkan;

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

typedef struct spudgpu_surface_vulkan {
    spudgpu_instance_vulkan _instance;
    VkSurfaceKHR _surface_vk;
} spudgpu_surface_vulkan;

typedef struct spudgpu_swap_chain_vulkan {
    spudgpu_swap_chain_desc _desc;
    spudgpu_device_vulkan _device;
    VkSurfaceKHR _surface_vk;
    VkSwapchainKHR _swapchain_vk;
    VkFormat _format_vk;
    VkExtent2D _extent_vk;
    VkImage *_swapchain_images_vk;
    uint32_t _swapchain_images_count;
    spudgpu_image_view_vulkan *_swapchain_image_views_vk;
    uint32_t _swapchain_image_views_count;

    VkSemaphore *_image_available_semaphores_vk; // One per frame-in-flight
    VkSemaphore *_render_finished_semaphores_vk; // One per frame-in-flight
    VkFence *_in_flight_fences_vk; // One per frame-in-flight
    uint32_t _max_frames_in_flight;
    uint32_t _current_frame; // Cycles 0..max_frames_in_flight-1
    uint32_t _current_image_index; // Set by acquire, read by present
} spudgpu_swap_chain_vulkan;

typedef struct spudgpu_shader_module_vulkan {
    spudgpu_device_vulkan _device;
    VkShaderModule _shader_module_vk;
    VkShaderStageFlagBits _stage_vk;
} spudgpu_shader_module_vulkan;


typedef struct spudgpu_shader_pipeline_vulkan {
    spudgpu_device_vulkan _device;
    spudgpu_shader_pipeline_desc _desc;
    VkPipeline _pipeline_vk;
    VkPipelineLayout _pipeline_layout_vk;
    VkRenderPass _render_pass_vk; // Owned by this pipeline
} spudgpu_shader_pipeline_vulkan;

typedef struct spudgpu_compute_pipeline_vulkan {
    spudgpu_device_vulkan _device;
    spudgpu_compute_pipeline_desc _desc;
    VkPipeline _pipeline_vk;
    VkPipelineLayout _pipeline_layout_vk;
} spudgpu_compute_pipeline_vulkan;

typedef struct spudgpu_descriptor_set_layout_vulkan {
    spudgpu_device_vulkan _device;
    spudgpu_descriptor_set_layout_desc _desc;
    VkDescriptorSetLayout _layout_vk;
} spudgpu_descriptor_set_layout_vulkan;

typedef struct spudgpu_descriptor_pool_vulkan {
    spudgpu_device_vulkan _device;
    spudgpu_descriptor_pool_desc _desc;
    VkDescriptorPool _pool_vk;
} spudgpu_descriptor_pool_vulkan;

typedef struct spudgpu_descriptor_set_vulkan {
    spudgpu_descriptor_pool_vulkan _pool;
    VkDescriptorSet _set_vk;
} spudgpu_descriptor_set_vulkan;

VkFormat convert_spud_to_vulkan_format(SPUDGPU_FORMAT format);

#endif //SPUDLIB_SPUDGPUVULKAN_H
