
#if SPUDGPU_COMPILE_VULKAN_API

#include <stdio.h>
#include <stdlib.h>
#include "spudgpuvulkan.h"

// ---------------------------------------------------------------------------
// Internal helpers: map SpudGPU layout enum → VkImageLayout
// ---------------------------------------------------------------------------

static VkImageLayout spud_layout_to_vk(SPUDGPU_IMAGE_LAYOUT layout) {
    switch (layout) {
        case SPUDGPU_IMAGE_LAYOUT_UNDEFINED:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        case SPUDGPU_IMAGE_LAYOUT_GENERAL:
            return VK_IMAGE_LAYOUT_GENERAL;
        case SPUDGPU_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case SPUDGPU_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case SPUDGPU_IMAGE_LAYOUT_SHADER_READ_ONLY:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case SPUDGPU_IMAGE_LAYOUT_TRANSFER_SRC:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case SPUDGPU_IMAGE_LAYOUT_TRANSFER_DST:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case SPUDGPU_IMAGE_LAYOUT_PRESENT_SRC:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        default:
            return VK_IMAGE_LAYOUT_UNDEFINED;
    }
}

// Infer the right access mask and pipeline stage from a layout,
// so callers don't have to spell it out every time.
static VkAccessFlags layout_to_access(VkImageLayout layout) {
    switch (layout) {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return VK_ACCESS_SHADER_READ_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return VK_ACCESS_TRANSFER_READ_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return VK_ACCESS_TRANSFER_WRITE_BIT;
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            return 0; // presentation engine handles its own sync
        default:
            return 0;
    }
}

static VkPipelineStageFlags layout_to_stage(VkImageLayout layout) {
    switch (layout) {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return VK_PIPELINE_STAGE_TRANSFER_BIT;
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        case VK_IMAGE_LAYOUT_UNDEFINED:
            return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        default:
            return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
}

// ---------------------------------------------------------------------------
// Internal helper: issue a VkImageMemoryBarrier for a given VkImage
// ---------------------------------------------------------------------------

static void spudgpuvulkan___cmd_image_barrier_raw(
    VkCommandBuffer cmd_buf,
    VkImage image,
    VkImageLayout old_layout,
    VkImageLayout new_layout,
    VkImageAspectFlags aspect_mask) {
    VkImageMemoryBarrier barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = aspect_mask;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = layout_to_access(old_layout);
    barrier.dstAccessMask = layout_to_access(new_layout);

    vkCmdPipelineBarrier(
        cmd_buf,
        layout_to_stage(old_layout), // srcStageMask
        layout_to_stage(new_layout), // dstStageMask
        0, // dependencyFlags
        0, NULL, // memory barriers
        0, NULL, // buffer barriers
        1, &barrier); // image barriers
}


#if __cplusplus
extern "C" {

#endif


// ---------------------------------------------------------------------------
// Public API: spudgpu_cmd_image_barrier  (spudgpu_image variant)
// ---------------------------------------------------------------------------

void spudgpu_cmd_image_barrier(
    spudgpu_command_list cmd,
    spudgpu_image image,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout) {

    VkImageLayout vk_old = spud_layout_to_vk(old_layout);
    VkImageLayout vk_new = spud_layout_to_vk(new_layout);

    // Choose aspect: depth formats get DEPTH_BIT, everything else COLOR_BIT.
    VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
        old_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    spudgpuvulkan___cmd_image_barrier_raw(
        cmd->_command_buffer_vk,
        image->_image_vk,
        vk_old, vk_new, aspect);
}

// ---------------------------------------------------------------------------
// Public API: spudgpu_cmd_image_barrier_view (spudgpu_image_view variant)
// For swap chain images that have no spudgpu_image handle.
// ---------------------------------------------------------------------------

void spudgpu_cmd_image_barrier_view(
    spudgpu_command_list cmd,
    spudgpu_image_view image_view,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout) {
    spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *) cmd;
    spudgpu_image_view_vulkan *view = (spudgpu_image_view_vulkan *) image_view;

    VkImageLayout vk_old = spud_layout_to_vk(old_layout);
    VkImageLayout vk_new = spud_layout_to_vk(new_layout);

    VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
        old_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    // Pull the raw VkImage out of the parent image stored in the view struct.
    spudgpuvulkan___cmd_image_barrier_raw(
        cmd->_command_buffer_vk,
        view->_desc.parent_image->_image_vk,
        vk_old, vk_new, aspect);
}

// ---------------------------------------------------------------------------
// Public API: spudgpu_cmd_begin_render_pass
// Creates a per-frame VkFramebuffer on the fly and begins the render pass.
// ---------------------------------------------------------------------------

void spudgpu_cmd_begin_render_pass(
    spudgpu_command_list cmd,
    const spudgpu_render_pass_begin_desc *desc) {
    spudgpu_shader_pipeline_vulkan *pipeline = (spudgpu_shader_pipeline_vulkan *) desc->pipeline;
    spudgpu_image_view_vulkan *color_view = (spudgpu_image_view_vulkan *) desc->color_attachment;

    VkDevice vk_device = pipeline->_device._logical_device_vk;

    // Build the attachment list for the framebuffer.
    VkImageView attachments[2];
    uint32_t attachment_count = 0;

    attachments[attachment_count++] = color_view->_image_view_vk;

    if (desc->depth_attachment) {
        spudgpu_image_view_vulkan *depth_view = (spudgpu_image_view_vulkan *) desc->depth_attachment;
        attachments[attachment_count++] = depth_view->_image_view_vk;
    }

    // Create a transient VkFramebuffer that lives for this one render pass.
    // (In a real engine you'd cache these per swap chain image index.)
    VkFramebufferCreateInfo fb_info = {0};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.renderPass = pipeline->_render_pass_vk;
    fb_info.attachmentCount = attachment_count;
    fb_info.pAttachments = attachments;
    fb_info.width = desc->width;
    fb_info.height = desc->height;
    fb_info.layers = 1;

    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    VkResult r = vkCreateFramebuffer(vk_device, &fb_info, NULL, &framebuffer);
    if (r != VK_SUCCESS) {
        printf("spudgpu: vkCreateFramebuffer failed (%d)\n", r);
        return;
    }

    // Store the framebuffer on the command list so end_render_pass can destroy it.
    cmd->_transient_framebuffer_vk = framebuffer;
    cmd->_transient_framebuffer_device_vk = vk_device;

    // Build clear values (color + optional depth).
    VkClearValue clear_values[2] = {0};
    clear_values[0].color.float32[0] = desc->clear_color[0];
    clear_values[0].color.float32[1] = desc->clear_color[1];
    clear_values[0].color.float32[2] = desc->clear_color[2];
    clear_values[0].color.float32[3] = desc->clear_color[3];
    clear_values[1].depthStencil.depth = desc->clear_depth;
    clear_values[1].depthStencil.stencil = desc->clear_stencil;

    VkRenderPassBeginInfo rp_begin = {0};
    rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin.renderPass = pipeline->_render_pass_vk;
    rp_begin.framebuffer = framebuffer;
    rp_begin.renderArea.offset.x = 0;
    rp_begin.renderArea.offset.y = 0;
    rp_begin.renderArea.extent.width = desc->width;
    rp_begin.renderArea.extent.height = desc->height;
    rp_begin.clearValueCount = attachment_count;
    rp_begin.pClearValues = clear_values;

    vkCmdBeginRenderPass(
        cmd->_command_buffer_vk,
        &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
}

// ---------------------------------------------------------------------------
// Public API: spudgpu_cmd_end_render_pass
// ---------------------------------------------------------------------------

void spudgpu_cmd_end_render_pass(spudgpu_command_list cmd) {
    vkCmdEndRenderPass(cmd->_command_buffer_vk);

    // Destroy the transient framebuffer that begin_render_pass created.
    if (cmd->_transient_framebuffer_vk != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(cmd->_transient_framebuffer_device_vk,
                             cmd->_transient_framebuffer_vk, NULL);
        cmd->_transient_framebuffer_vk = VK_NULL_HANDLE;
        cmd->_transient_framebuffer_device_vk = VK_NULL_HANDLE;
    }
}

#if __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_VULKAN_API
