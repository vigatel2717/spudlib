//
// Created by nathanmoore on 5/18/26.
//

#if SPUDGPU_COMPILE_VULKAN_API

#ifndef SPUDLIB_SPUDGPUVULKAN_H
#define SPUDLIB_SPUDGPUVULKAN_H

#include "spudgpu.h"
#include <vulkan/vulkan.h>

typedef struct spudgpu_instance_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkInstance _instance_vk;
    // Effectively this -- spudgpu_device_t *_device_pointer_array[device_count];
    // We need to keep track of newly allocated spudgpu_device's to prevent memory leaks.
    uint64_t *_devices_pointer_array;
    uint32_t _devices_count;
} spudgpu_instance_vulkan;

typedef struct spudgpu_device_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkDevice _logical_device_vk;
    VkPhysicalDevice _physical_device_vk;
    spudgpu_instance_vulkan _instance;
    VkPhysicalDeviceProperties _properties_vk;
    VkPhysicalDeviceFeatures _features_vk;
    uint32_t _graphics_queue_family_index;
} spudgpu_device_vulkan;

typedef struct spudgpu_command_queue_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkQueue _queue_vk;
    uint32_t _queue_family_index;
} spudgpu_command_queue_vulkan;

typedef struct spudgpu_command_allocator_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkCommandPool _command_pool_vk;
    spudgpu_device_vulkan _device;
    uint32_t _queue_family_index;
} spudgpu_command_allocator_vulkan;

typedef struct spudgpu_command_list_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkCommandBuffer _command_buffer_vk;
    spudgpu_command_allocator_vulkan _allocator;

    // Transient framebuffer created by begin_render_pass, destroyed by end_render_pass.
    VkFramebuffer _transient_framebuffer_vk;
    VkDevice _transient_framebuffer_device_vk;
} spudgpu_command_list_vulkan;

typedef struct spudgpu_buffer_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkBuffer _buffer_vk;
    VkDeviceMemory _memory_vk;
    spudgpu_buffer_desc _desc;
    spudgpu_device_vulkan _device;
} spudgpu_buffer_vulkan;

typedef struct spudgpu_buffer_view_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkBufferView _buffer_view_vk;
    spudgpu_buffer_view_desc _desc;
} spudgpu_buffer_view_vulkan;

typedef struct spudgpu_image_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkImage _image_vk;
    VkDeviceMemory _memory_vk;
    VkFormat _format_vk;
    spudgpu_image_desc _desc;
    spudgpu_device_vulkan _device;
} spudgpu_image_vulkan;

typedef struct spudgpu_image_view_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkImageView _image_view_vk;
    spudgpu_image_view_desc _desc;
} spudgpu_image_view_vulkan;

typedef struct spudgpu_surface_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkSurfaceKHR _surface_vk;
    spudgpu_instance_vulkan _instance;
} spudgpu_surface_vulkan;

typedef struct spudgpu_fence_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkFence _fence_vk;
    VkDevice _device_vk;
} spudgpu_fence_vulkan;

typedef struct spudgpu_semaphore_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkSemaphore _semaphore_vk;
    VkDevice _device_vk;
} spudgpu_semaphore_vulkan;

typedef struct spudgpu_swap_chain_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkSwapchainKHR _swapchain_vk;
    spudgpu_swap_chain_desc _desc;
    spudgpu_device_vulkan _device;
    VkSurfaceKHR _surface_vk;
    VkFormat _format_vk;
    VkExtent2D _extent_vk;
    VkImage *_swapchain_images_vk;
    uint32_t _swapchain_images_count;
    spudgpu_image_view_vulkan *_swapchain_image_views_vk;
    uint32_t _swapchain_image_views_count;

    spudgpu_semaphore_vulkan *_image_available_semaphores; // One per frame-in-flight
    spudgpu_semaphore_vulkan *_render_finished_semaphores; // One per frame-in-flight
    spudgpu_fence_vulkan     *_in_flight_fences;           // One per frame-in-flight
    uint32_t _max_frames_in_flight;
    uint32_t _current_frame; // Cycles 0..max_frames_in_flight-1
    uint32_t _current_image_index; // Set by acquire, read by present
} spudgpu_swap_chain_vulkan;

typedef struct spudgpu_shader_module_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkShaderModule _shader_module_vk;
    VkShaderStageFlagBits _stage_vk;
    spudgpu_device_vulkan _device;
} spudgpu_shader_module_vulkan;


typedef struct spudgpu_shader_pipeline_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkPipeline _pipeline_vk;
    VkPipelineLayout _pipeline_layout_vk;
    VkRenderPass _render_pass_vk; // Owned by this pipeline
    spudgpu_device_vulkan _device;
    spudgpu_shader_pipeline_desc _desc;
} spudgpu_shader_pipeline_vulkan;

typedef struct spudgpu_compute_pipeline_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkPipeline _pipeline_vk;
    VkPipelineLayout _pipeline_layout_vk;
    spudgpu_device_vulkan _device;
    spudgpu_compute_pipeline_desc _desc;
} spudgpu_compute_pipeline_vulkan;

typedef struct spudgpu_descriptor_set_layout_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkDescriptorSetLayout _layout_vk;
    spudgpu_device_vulkan _device;
    spudgpu_descriptor_set_layout_desc _desc;
} spudgpu_descriptor_set_layout_vulkan;

typedef struct spudgpu_descriptor_pool_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkDescriptorPool _pool_vk;
    spudgpu_device_vulkan _device;
    spudgpu_descriptor_pool_desc _desc;
} spudgpu_descriptor_pool_vulkan;

typedef struct spudgpu_descriptor_set_t {
#if _DEBUG
    const char *_debug_name;
#endif
    VkDescriptorSet _set_vk;
    spudgpu_descriptor_pool_vulkan _pool;
} spudgpu_descriptor_set_vulkan;

VkFormat convert_spud_to_vulkan_format(SPUDGPU_FORMAT format);

#endif //SPUDLIB_SPUDGPUVULKAN_H

#endif //SPUDGPU_COMPILE_VULKAN_API
