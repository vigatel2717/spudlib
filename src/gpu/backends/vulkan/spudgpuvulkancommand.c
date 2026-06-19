
#if SPUDGPU_COMPILE_VULKAN_API

#include "spudgpuvulkan.h"
#include "stdio.h"
#include "stdlib.h"

#if __cplusplus
extern "C" {



#endif

spudgpu_command_queue spudgpu_get_graphics_queue(spudgpu_device device) {
    if (!device) return NULL;

    // Find graphics queue family
    uint32_t family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device->_physical_device_vk, &family_count, NULL);
    VkQueueFamilyProperties families[family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device->_physical_device_vk, &family_count, families);

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
    vkGetDeviceQueue(device->_logical_device_vk, graphics_family, 0, &q->_queue_vk);
    q->_queue_family_index = graphics_family;
    return q;
}

SPUDRESULT spudgpu_submit_command_lists(
    spudgpu_command_queue queue,
    spudgpu_command_list *cmd_lists,
    uint32_t cmd_list_count) {
    if (!queue) return SPUD_SUCCESS;
    
    VkCommandBuffer buffers[cmd_list_count];
    for (uint32_t i = 0; i < cmd_list_count; i++) {
        spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd_lists[i];
        buffers[i] = cl->_command_buffer_vk;
    }

    VkSubmitInfo submit = {0};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.commandBufferCount = cmd_list_count;
    submit.pCommandBuffers = buffers;

    VkResult r = vkQueueSubmit(queue->_queue_vk, 1, &submit, VK_NULL_HANDLE);
    if (r != VK_SUCCESS) {
        printf("spudgpu: vkQueueSubmit failed (%d)\n", r);
        return SPUDRESULT_API_SPECIFIC_FAILURE;
    }
    return SPUD_SUCCESS;
}

// Submit command lists with full swap chain synchronization.
// Waits on the swap chain's image_available semaphore,
// signals its render_finished semaphore, and signals the in-flight fence.
// Call this instead of spudgpu_submit_command_lists when rendering to a swap chain.
SPUDRESULT spudgpu_submit_command_lists_synced(
    spudgpu_command_queue queue,
    spudgpu_command_list *cmd_lists,
    uint32_t cmd_list_count,
    spudgpu_swap_chain swap_chain) {
    if (!(queue && swap_chain)) return SPUD_SUCCESS;

    uint32_t frame = swap_chain->_current_frame;

    VkCommandBuffer buffers[cmd_list_count];
    for (uint32_t i = 0; i < cmd_list_count; i++) {
        spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd_lists[i];
        buffers[i] = cl->_command_buffer_vk;
    }

    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit = {0};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &swap_chain->_image_available_semaphores[frame]._semaphore_vk;
    submit.pWaitDstStageMask = &wait_stage;
    submit.commandBufferCount = cmd_list_count;
    submit.pCommandBuffers = buffers;
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &swap_chain->_render_finished_semaphores[frame]._semaphore_vk;

    VkResult r = vkQueueSubmit(
        queue->_queue_vk, 1, &submit,
        swap_chain->_in_flight_fences[frame]._fence_vk); // <-- fence gets signaled here
    if (r != VK_SUCCESS) {
        printf("spudgpu: vkQueueSubmit (synced) failed (%d)\n", r);
        return SPUDRESULT_API_SPECIFIC_FAILURE;
    }
    return SPUD_SUCCESS;
}

SPUDRESULT spudgpu_create_command_allocator(
    spudgpu_device device,
	const spudgpu_command_allocator_desc *desc,
	spudgpu_command_allocator *out_allocator) {
    if (!device) return SPUDRESULT_GPU_INVALID_DEVICE;
    if (!desc) return SPUDRESULT_NULL_DESC;
    if (!out_allocator) return SPUD_SUCCESS;

    // Re-query graphics family (you'll want to cache this on the device later)
    uint32_t family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device->_physical_device_vk, &family_count, NULL);
    VkQueueFamilyProperties families[family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device->_physical_device_vk, &family_count, families);

    uint32_t graphics_family = SPUD_UINT32_MAX;
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
    alloc->_device = *device;
    alloc->_queue_family_index = graphics_family;

    VkResult r = vkCreateCommandPool(
        device->_logical_device_vk, &pool_info, NULL, &alloc->_command_pool_vk);
    if (r != VK_SUCCESS) {
        printf("spudgpu: vkCreateCommandPool failed (%d)\n", r);
        free(alloc);
        return SPUDRESULT_API_SPECIFIC_FAILURE;
    }

    *out_allocator = alloc;
    return SPUD_SUCCESS;
}

SPUDRESULT spudgpu_reset_command_allocator(spudgpu_command_allocator allocator) {
    if (!allocator) return SPUDRESULT_GPU_INVALID_COMMAND_ALLOCATOR;
    VkResult r = vkResetCommandPool(
        allocator->_device._logical_device_vk, allocator->_command_pool_vk, 0);
    return (r == VK_SUCCESS) ? SPUD_SUCCESS : SPUDRESULT_API_SPECIFIC_FAILURE;
}

void spudgpu_destroy_command_allocator(spudgpu_command_allocator allocator) {
    if (!allocator) return;
    vkDestroyCommandPool(allocator->_device._logical_device_vk, allocator->_command_pool_vk, NULL);
    free(allocator);
}

SPUDRESULT spudgpu_create_command_list(
    spudgpu_command_allocator allocator, spudgpu_command_list *out_cmd_list) {
    if (!allocator) return SPUDRESULT_GPU_INVALID_COMMAND_ALLOCATOR;
    if (!out_cmd_list) return SPUD_SUCCESS;
    
    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = allocator->_command_pool_vk;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    spudgpu_command_list_vulkan *cl = calloc(1, sizeof(spudgpu_command_list_vulkan));
    cl->_allocator = *allocator;

    VkResult r = vkAllocateCommandBuffers(
        allocator->_device._logical_device_vk, &alloc_info, &cl->_command_buffer_vk);
    if (r != VK_SUCCESS) {
        printf("spudgpu: vkAllocateCommandBuffers failed (%d)\n", r);
        free(cl);
        return SPUDRESULT_API_SPECIFIC_FAILURE;
    }

	*out_cmd_list = cl;
    return SPUD_SUCCESS;
}

void spudgpu_destroy_command_list(spudgpu_command_list cmd) {
    if (!cmd) return;
    vkFreeCommandBuffers(
        cmd->_allocator._device._logical_device_vk,
        cmd->_allocator._command_pool_vk,
        1, &cmd->_command_buffer_vk);
    free(cmd);
}

void spudgpu_begin_command_list(spudgpu_command_list cmd) {
    if (!cmd) return;

    // Reset so this buffer can be re-recorded this frame.
    vkResetCommandBuffer(cmd->_command_buffer_vk, 0);

    // ONE_TIME_SUBMIT_BIT is a hint that this buffer is re-recorded each frame.
    // Remove it if you plan to record once and submit many times.
    VkCommandBufferBeginInfo begin = {0};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd->_command_buffer_vk, &begin);
}

void spudgpu_end_command_list(spudgpu_command_list cmd) {
    if (!cmd) return;
    vkEndCommandBuffer(cmd->_command_buffer_vk);
}

void spudgpu_set_viewports(
    spudgpu_command_list cmd,
    uint32_t first_viewport,
    uint32_t viewport_count,
    const SPUDGPU_VIEWPORT *viewports) {
    if (!(cmd && viewports) || viewport_count == 0) return;

    VkViewport vk_viewports[viewport_count];
    for (uint32_t i = 0; i < viewport_count; i++) {
        vk_viewports[i].x = viewports[i].x;
        vk_viewports[i].y = viewports[i].y;
        vk_viewports[i].width = viewports[i].width;
        vk_viewports[i].height = viewports[i].height;
        vk_viewports[i].minDepth = viewports[i].minDepth;
        vk_viewports[i].maxDepth = viewports[i].maxDepth;
    }

    vkCmdSetViewport(cmd->_command_buffer_vk, first_viewport, viewport_count, vk_viewports);
}

void spudgpu_set_scissor_rects(
    spudgpu_command_list cmd,
    uint32_t first_scissor_rect,
    uint32_t scissor_rect_count,
    const SPUDGPU_SCISSOR_RECT *scissor_rects) {
    if (!(cmd && scissor_rects) || scissor_rect_count == 0) return;

    VkRect2D vk_scissors[scissor_rect_count];
    for (uint32_t i = 0; i < scissor_rect_count; i++) {
        vk_scissors[i].offset.x = (int32_t) scissor_rects[i].x;
        vk_scissors[i].offset.y = (int32_t) scissor_rects[i].y;
        vk_scissors[i].extent.width = (uint32_t) scissor_rects[i].width;
        vk_scissors[i].extent.height = (uint32_t) scissor_rects[i].height;
    }

    vkCmdSetScissor(cmd->_command_buffer_vk, first_scissor_rect, scissor_rect_count, vk_scissors);
}

void spudgpu_set_vertex_buffers(
    spudgpu_command_list cmd,
    uint32_t start_slot,
    uint32_t view_count,
    spudgpu_buffer_view *buffer_views) {
    if (!(cmd && buffer_views) || view_count == 0) return;

    VkBuffer vk_buffers[view_count];
    VkDeviceSize vk_offsets[view_count];

    for (uint32_t i = 0; i < view_count; i++) {
        spudgpu_buffer_view_vulkan *bv = (spudgpu_buffer_view_vulkan *) buffer_views[i];
        vk_buffers[i] = bv->_desc.parent_buffer->_buffer_vk;
        vk_offsets[i] = (VkDeviceSize) bv->_desc.offset_from_parent_buffer;
    }

    vkCmdBindVertexBuffers(
        cmd->_command_buffer_vk,
        start_slot,
        view_count,
        vk_buffers,
        vk_offsets);
}

void spudgpu_set_index_buffer(
    spudgpu_command_list cmd,
    spudgpu_buffer_view buffer_view) {
    if (!(cmd && buffer_view)) return;
    spudgpu_buffer_view_vulkan *bv = (spudgpu_buffer_view_vulkan *) buffer_view;
    VkIndexType index_type = bv->_desc.stride == sizeof(uint16_t)
                                 ? VK_INDEX_TYPE_UINT16
                                 : VK_INDEX_TYPE_UINT32;
    vkCmdBindIndexBuffer(
        cmd->_command_buffer_vk,
        bv->_desc.parent_buffer->_buffer_vk,
        (VkDeviceSize) bv->_desc.offset_from_parent_buffer,
        index_type);
}

void spudgpu_draw(
    spudgpu_command_list cmd,
    uint32_t vertex_count,
    uint32_t start_vertex_location) {
    if (!cmd) return;
    vkCmdDraw(
        cmd->_command_buffer_vk,
        vertex_count,
        1,
        start_vertex_location,
        0);
}

void spudgpu_cmd_bind_pipeline(
    spudgpu_command_list cmd,
    spudgpu_shader_pipeline pipeline) {
    if (!(cmd && pipeline)) return;

    vkCmdBindPipeline(
        cmd->_command_buffer_vk,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline->_pipeline_vk);
}

void spudgpu_draw_indexed(
    spudgpu_command_list cmd,
    uint32_t index_count,
    uint32_t start_index_location,
    int32_t base_vertex_location) {
    if (!cmd) return;
    vkCmdDrawIndexed(
        cmd->_command_buffer_vk,
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
    vkCmdDraw(
        cmd->_command_buffer_vk,
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
    vkCmdDrawIndexed(
        cmd->_command_buffer_vk,
        index_count_per_instance,
        instance_count,
        start_index_location,
        base_vertex_location,
        start_instance_location);
}

void spudgpu_queue_submit(spudgpu_command_queue queue, const spudgpu_submit_desc *desc) {
    if (!(queue && desc && desc->cmd_list_count > 0)) return;

    VkCommandBuffer cmd_bufs[desc->cmd_list_count];
    for (uint32_t i = 0; i < desc->cmd_list_count; i++) {
        spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) desc->cmd_lists[i];
        cmd_bufs[i] = cl->_command_buffer_vk;
    }

    uint32_t wait_count   = desc->wait_semaphore_count;
    uint32_t signal_count = desc->signal_semaphore_count;

    // Use heap for semaphore arrays since counts may legitimately be 0 (VLA of size 0 is UB).
    VkSemaphore          *wait_sems   = wait_count   ? malloc(wait_count   * sizeof(VkSemaphore))          : NULL;
    VkPipelineStageFlags *wait_stages = wait_count   ? malloc(wait_count   * sizeof(VkPipelineStageFlags)) : NULL;
    VkSemaphore          *signal_sems = signal_count ? malloc(signal_count * sizeof(VkSemaphore))           : NULL;

    for (uint32_t i = 0; i < wait_count; i++) {
        spudgpu_semaphore_vulkan *sem = (spudgpu_semaphore_vulkan *) desc->wait_semaphores[i];
        wait_sems[i]   = sem->_semaphore_vk;
        wait_stages[i] = (VkPipelineStageFlags) desc->wait_stage_masks[i];
    }
    for (uint32_t i = 0; i < signal_count; i++) {
        spudgpu_semaphore_vulkan *sem = (spudgpu_semaphore_vulkan *) desc->signal_semaphores[i];
        signal_sems[i] = sem->_semaphore_vk;
    }

    VkFence signal_fence = VK_NULL_HANDLE;
    if (desc->signal_fence) {
        spudgpu_fence_vulkan *f = (spudgpu_fence_vulkan *) desc->signal_fence;
        signal_fence = f->_fence_vk;
    }

    VkSubmitInfo submit = {0};
    submit.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.waitSemaphoreCount   = wait_count;
    submit.pWaitSemaphores      = wait_sems;
    submit.pWaitDstStageMask    = wait_stages;
    submit.commandBufferCount   = desc->cmd_list_count;
    submit.pCommandBuffers      = cmd_bufs;
    submit.signalSemaphoreCount = signal_count;
    submit.pSignalSemaphores    = signal_sems;

    VkResult r = vkQueueSubmit(queue->_queue_vk, 1, &submit, signal_fence);
    if (r != VK_SUCCESS) {
        printf("spudgpu: vkQueueSubmit failed (%d)\n", r);
    }

    free(wait_sems);
    free(wait_stages);
    free(signal_sems);
}

void spudgpu_queue_wait_idle(spudgpu_command_queue queue) {
    if (!queue) return;
    vkQueueWaitIdle(queue->_queue_vk);
}

void spudgpu_cmd_push_constants(
    spudgpu_command_list    cmd,
    spudgpu_shader_pipeline pipeline,
    uint32_t                offset,
    uint32_t                size,
    const void             *data) {
    if (!cmd || !pipeline || !data || size == 0) return;

    /* Collect stage flags from all push constant ranges that overlap [offset, offset+size). */
    VkShaderStageFlags stages = 0;
    uint32_t range_count = pipeline->_desc.push_constant_range_count;
    for (uint32_t i = 0; i < range_count; i++) {
        const spudgpu_push_constant_range_desc *r = &pipeline->_desc.push_constant_ranges[i];
        if (r->offset < offset + size && r->offset + r->size > offset) {
            SPUDGPU_SHADER_STAGE sf = (SPUDGPU_SHADER_STAGE)r->stage_flags;
            if (sf & SPUDGPU_SHADER_STAGE_VERTEX)   stages |= VK_SHADER_STAGE_VERTEX_BIT;
            if (sf & SPUDGPU_SHADER_STAGE_FRAGMENT) stages |= VK_SHADER_STAGE_FRAGMENT_BIT;
            if (sf & SPUDGPU_SHADER_STAGE_COMPUTE)  stages |= VK_SHADER_STAGE_COMPUTE_BIT;
        }
    }
    if (!stages) return;

    vkCmdPushConstants(
        cmd->_command_buffer_vk,
        pipeline->_pipeline_layout_vk,
        stages, offset, size, data);
}

#if __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_VULKAN_API
