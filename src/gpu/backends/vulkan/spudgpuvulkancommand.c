
#if SPUDGPU_COMPILE_VULKAN_API

#include "spudgpuvulkan.h"
#include "stdio.h"
#include "stdlib.h"

#if __cplusplus
extern "C" {



#endif

spudgpu_command_queue spudgpu_get_graphics_queue(spudgpu_device device) {
    if (!device) return NULL;
    spudgpu_device_vulkan *dev = (spudgpu_device_vulkan *) device;

    // Find graphics queue family
    uint32_t family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(dev->_physical_device_vk, &family_count, NULL);
    VkQueueFamilyProperties families[family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(dev->_physical_device_vk, &family_count, families);

    uint32_t graphics_family = UINT32_MAX;
    for (uint32_t i = 0; i < family_count; i++) {
        if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphics_family = i;
            break;
        }
    }
    if (graphics_family == UINT32_MAX) {
        printf("spudgpu: no graphics queue family found\n");
        return NULL;
    }

    spudgpu_command_queue_vulkan *q = calloc(1, sizeof(spudgpu_command_queue_vulkan));
    vkGetDeviceQueue(dev->_logical_device_vk, graphics_family, 0, &q->_queue_vk);
    return (spudgpu_command_queue) q;
}

void spudgpu_submit_command_lists(
    spudgpu_command_queue queue,
    spudgpu_command_list *cmd_lists,
    uint32_t cmd_list_count) {
    if (!queue) return;
    spudgpu_command_queue_vulkan *q = (spudgpu_command_queue_vulkan *) queue;

    VkCommandBuffer buffers[cmd_list_count];
    for (uint32_t i = 0; i < cmd_list_count; i++) {
        spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd_lists[i];
        buffers[i] = cl->_command_buffer_vk;
    }

    VkSubmitInfo submit = {0};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.commandBufferCount = cmd_list_count;
    submit.pCommandBuffers = buffers;

    VkResult r = vkQueueSubmit(q->_queue_vk, 1, &submit, VK_NULL_HANDLE);
    if (r != VK_SUCCESS) {
        printf("spudgpu: vkQueueSubmit failed (%d)\n", r);
    }
}

spudgpu_command_allocator spudgpu_create_command_allocator(spudgpu_device device) {
    if (!device) return NULL;
    spudgpu_device_vulkan *dev = (spudgpu_device_vulkan *) device;

    // Re-query graphics family (you'll want to cache this on the device later)
    uint32_t family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(dev->_physical_device_vk, &family_count, NULL);
    VkQueueFamilyProperties families[family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(dev->_physical_device_vk, &family_count, families);

    uint32_t graphics_family = UINT32_MAX;
    for (uint32_t i = 0; i < family_count; i++) {
        if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphics_family = i;
            break;
        }
    }

    VkCommandPoolCreateInfo pool_info = {0};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = graphics_family;
    // RESET_COMMAND_BUFFER_BIT lets you reset individual command buffers
    // instead of the whole pool — useful for multi-threaded recording.
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    spudgpu_command_allocator_vulkan *alloc = calloc(1, sizeof(spudgpu_command_allocator_vulkan));
    alloc->_device = *dev;
    alloc->_queue_family_index = graphics_family;

    VkResult r = vkCreateCommandPool(
        dev->_logical_device_vk, &pool_info, NULL, &alloc->_command_pool_vk);
    if (r != VK_SUCCESS) {
        printf("spudgpu: vkCreateCommandPool failed (%d)\n", r);
        free(alloc);
        return NULL;
    }
    return (spudgpu_command_allocator) alloc;
}

void spudgpu_reset_command_allocator(spudgpu_command_allocator allocator) {
    if (!allocator) return;
    spudgpu_command_allocator_vulkan *alloc = (spudgpu_command_allocator_vulkan *) allocator;
    // Resets all command buffers allocated from this pool back to initial state.
    // Equivalent to calling Reset() on a D3D12CommandAllocator.
    vkResetCommandPool(alloc->_device._logical_device_vk, alloc->_command_pool_vk, 0);
}

void spudgpu_destroy_command_allocator(spudgpu_command_allocator allocator) {
    if (!allocator) return;
    spudgpu_command_allocator_vulkan *alloc = (spudgpu_command_allocator_vulkan *) allocator;
    vkDestroyCommandPool(alloc->_device._logical_device_vk, alloc->_command_pool_vk, NULL);
    free(alloc);
}

spudgpu_command_list spudgpu_create_command_list(spudgpu_command_allocator allocator) {
    if (!allocator) return NULL;
    spudgpu_command_allocator_vulkan *alloc = (spudgpu_command_allocator_vulkan *) allocator;

    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = alloc->_command_pool_vk;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    spudgpu_command_list_vulkan *cl = calloc(1, sizeof(spudgpu_command_list_vulkan));
    cl->_allocator = *alloc;

    VkResult r = vkAllocateCommandBuffers(
        alloc->_device._logical_device_vk, &alloc_info, &cl->_command_buffer_vk);
    if (r != VK_SUCCESS) {
        printf("spudgpu: vkAllocateCommandBuffers failed (%d)\n", r);
        free(cl);
        return NULL;
    }
    return (spudgpu_command_list) cl;
}

void spudgpu_destroy_command_list(spudgpu_command_list cmd) {
    if (!cmd) return;
    spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd;
    vkFreeCommandBuffers(
        cl->_allocator._device._logical_device_vk,
        cl->_allocator._command_pool_vk,
        1, &cl->_command_buffer_vk);
    free(cl);
}

void spudgpu_begin_command_list(spudgpu_command_list cmd) {
    if (!cmd) return;
    spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd;
    VkCommandBufferBeginInfo begin = {0};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // ONE_TIME_SUBMIT_BIT is a hint that this buffer is re-recorded each frame.
    // Remove it if you plan to record once and submit many times.
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cl->_command_buffer_vk, &begin);
}

void spudgpu_end_command_list(spudgpu_command_list cmd) {
    if (!cmd) return;
    spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd;
    vkEndCommandBuffer(cl->_command_buffer_vk);
}

void spudgpu_set_viewports(
    spudgpu_command_list cmd,
    uint32_t first_viewport,
    uint32_t viewport_count,
    const SPUDGPU_VIEWPORT *viewports) {
    if (!(cmd && viewports) || viewport_count == 0) return;
    spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd;

    VkViewport vk_viewports[viewport_count];
    for (uint32_t i = 0; i < viewport_count; i++) {
        vk_viewports[i].x = viewports[i].x;
        vk_viewports[i].y = viewports[i].y;
        vk_viewports[i].width = viewports[i].width;
        vk_viewports[i].height = viewports[i].height;
        vk_viewports[i].minDepth = viewports[i].minDepth;
        vk_viewports[i].maxDepth = viewports[i].maxDepth;
    }

    vkCmdSetViewport(cl->_command_buffer_vk, first_viewport, viewport_count, vk_viewports);
}

void spudgpu_set_scissor_rects(
    spudgpu_command_list cmd,
    uint32_t first_scissor_rect,
    uint32_t scissor_rect_count,
    const SPUDGPU_SCISSOR_RECT *scissor_rects) {
    if (!(cmd && scissor_rects) || scissor_rect_count == 0) return;
    spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd;

    VkRect2D vk_scissors[scissor_rect_count];
    for (uint32_t i = 0; i < scissor_rect_count; i++) {
        vk_scissors[i].offset.x = (int32_t) scissor_rects[i].x;
        vk_scissors[i].offset.y = (int32_t) scissor_rects[i].y;
        vk_scissors[i].extent.width = (uint32_t) scissor_rects[i].width;
        vk_scissors[i].extent.height = (uint32_t) scissor_rects[i].height;
    }

    vkCmdSetScissor(cl->_command_buffer_vk, first_scissor_rect, scissor_rect_count, vk_scissors);
}

void spudgpu_set_vertex_buffers(
    spudgpu_command_list cmd,
    uint32_t start_slot,
    uint32_t view_count,
    spudgpu_buffer_view *buffer_views) {
    if (!(cmd && buffer_views) || view_count == 0) return;
    spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd;

    VkBuffer vk_buffers[view_count];
    VkDeviceSize vk_offsets[view_count];

    for (uint32_t i = 0; i < view_count; i++) {
        spudgpu_buffer_view_vulkan *bv = (spudgpu_buffer_view_vulkan *) buffer_views[i];
        vk_buffers[i] = bv->_parent_buffer._buffer_vk;
        vk_offsets[i] = (VkDeviceSize) bv->_desc.offset_from_parent_buffer;
    }

    vkCmdBindVertexBuffers(
        cl->_command_buffer_vk,
        start_slot,
        view_count,
        vk_buffers,
        vk_offsets);
}

void spudgpu_set_index_buffers(
    spudgpu_command_list cmd,
    uint32_t view_count,
    spudgpu_buffer_view *buffer_views) {
    if (!(cmd && buffer_views) || view_count == 0) return;
    spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd;

    // Vulkan only supports a single index buffer at a time; bind each in sequence.
    // In practice view_count is almost always 1.
    for (uint32_t i = 0; i < view_count; i++) {
        spudgpu_buffer_view_vulkan *bv = (spudgpu_buffer_view_vulkan *) buffer_views[i];

        // Infer index type from the stride stored in the view descriptor.
        VkIndexType index_type = bv->_desc.stride == sizeof(uint16_t)
                                     ? VK_INDEX_TYPE_UINT16
                                     : VK_INDEX_TYPE_UINT32;

        vkCmdBindIndexBuffer(
            cl->_command_buffer_vk,
            bv->_parent_buffer._buffer_vk,
            (VkDeviceSize) bv->_desc.offset_from_parent_buffer,
            index_type);
    }
}

void spudgpu_draw(
    spudgpu_command_list cmd,
    uint32_t vertex_count,
    uint32_t start_vertex_location) {
    if (!cmd) return;
    spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd;
    vkCmdDraw(
        cl->_command_buffer_vk,
        vertex_count,
        1,
        start_vertex_location,
        0);
}

void spudgpu_draw_indexed(
    spudgpu_command_list cmd,
    uint32_t index_count,
    uint32_t start_index_location,
    int32_t base_vertex_location) {
    if (!cmd) return;
    spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd;
    vkCmdDrawIndexed(
        cl->_command_buffer_vk,
        index_count,
        1,
        start_index_location,
        base_vertex_location,
        0);
}

void spudgpu_draw_instanced(
    spudgpu_command_list cmd,
    uint32_t vertex_count_per_instance,
    uint32_t instance_count,
    uint32_t start_vertex_location,
    uint32_t start_instance_location) {
    if (!cmd) return;
    spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd;
    vkCmdDraw(
        cl->_command_buffer_vk,
        vertex_count_per_instance,
        instance_count,
        start_vertex_location,
        start_instance_location);
}

void spudgpu_draw_indexed_instanced(
    spudgpu_command_list cmd,
    uint32_t index_count_per_instance,
    uint32_t instance_count,
    uint32_t start_index_location,
    int32_t base_vertex_location,
    uint32_t start_instance_location) {
    if (!cmd) return;
    spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd;
    vkCmdDrawIndexed(
        cl->_command_buffer_vk,
        index_count_per_instance,
        instance_count,
        start_index_location,
        base_vertex_location,
        start_instance_location);
}

#if __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_VULKAN_API
