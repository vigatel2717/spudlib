
#if SPUDGPU_COMPILE_VULKAN_API

#include "spudgpuvulkan.h"
#include <stdio.h>
#include <stdlib.h>

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
// Internal helper: map a SpudGPU/DXGI-numbered format to the aspect(s) it has.
// Used for buffer<->image copies, blits, and subresource barriers, where we
// have an actual image handle (and thus its real format) rather than just a
// layout to guess from.
// ---------------------------------------------------------------------------

static VkImageAspectFlags spud_format_aspect(VkFormat fmt) {
	switch (fmt) {
	case VK_FORMAT_D16_UNORM:
	case VK_FORMAT_D32_SFLOAT:
	case VK_FORMAT_X8_D24_UNORM_PACK32:
		return VK_IMAGE_ASPECT_DEPTH_BIT;
	case VK_FORMAT_D16_UNORM_S8_UINT:
	case VK_FORMAT_D24_UNORM_S8_UINT:
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	default:
		return VK_IMAGE_ASPECT_COLOR_BIT;
	}
}

static VkFilter spud_filter_to_vk(SPUDGPU_FILTER filter) {
	switch (filter) {
	case SPUDGPU_FILTER_LINEAR:
		return VK_FILTER_LINEAR;
	case SPUDGPU_FILTER_NEAREST:
	default:
		return VK_FILTER_NEAREST;
	}
}

// ---------------------------------------------------------------------------
// Internal helper: issue a VkImageMemoryBarrier for a given VkImage/range
// ---------------------------------------------------------------------------

static void spudgpuvulkan___cmd_image_barrier_raw(
    VkCommandBuffer cmd_buf,
    VkImage image,
    VkImageLayout old_layout,
    VkImageLayout new_layout,
    VkImageSubresourceRange range) {
	VkImageMemoryBarrier barrier = {0};
	barrier.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout            = old_layout;
	barrier.newLayout            = new_layout;
	barrier.srcQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
	barrier.image                = image;
	barrier.subresourceRange     = range;
	barrier.srcAccessMask        = layout_to_access(old_layout);
	barrier.dstAccessMask        = layout_to_access(new_layout);

	vkCmdPipelineBarrier(
	    cmd_buf,
	    layout_to_stage(old_layout), // srcStageMask
	    layout_to_stage(new_layout), // dstStageMask
	    0,                           // dependencyFlags
	    0, NULL,                     // memory barriers
	    0, NULL,                     // buffer barriers
	    1, &barrier);                // image barriers
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

	VkImageSubresourceRange range = {0};
	range.aspectMask              = spud_format_aspect(image->_format_vk);
	range.baseMipLevel            = 0;
	range.levelCount              = 1;
	range.baseArrayLayer          = 0;
	range.layerCount              = 1;

	spudgpuvulkan___cmd_image_barrier_raw(cmd->_command_buffer_vk, image->_image_vk, vk_old, vk_new, range);
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
	spudgpu_command_list_vulkan *cl = (spudgpu_command_list_vulkan *)cmd;
	spudgpu_image_view_vulkan *view = (spudgpu_image_view_vulkan *)image_view;

	VkImageLayout vk_old = spud_layout_to_vk(old_layout);
	VkImageLayout vk_new = spud_layout_to_vk(new_layout);

	VkImageSubresourceRange range = {0};
	range.aspectMask              = spud_format_aspect(view->_desc.parent_image->_format_vk);
	range.baseMipLevel            = 0;
	range.levelCount              = 1;
	range.baseArrayLayer          = 0;
	range.layerCount              = 1;

	// Pull the raw VkImage out of the parent image stored in the view struct.
	spudgpuvulkan___cmd_image_barrier_raw(cmd->_command_buffer_vk, view->_desc.parent_image->_image_vk, vk_old, vk_new, range);
}

// ---------------------------------------------------------------------------
// Public API: spudgpu_cmd_image_barrier_subresource
// ---------------------------------------------------------------------------

void spudgpu_cmd_image_barrier_subresource(
    spudgpu_command_list cmd,
    spudgpu_image image,
    const spudgpu_image_view_desc_subresource_range *range,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout) {
	if (!cmd || !image || !range)
		return;

	VkImageLayout vk_old = spud_layout_to_vk(old_layout);
	VkImageLayout vk_new = spud_layout_to_vk(new_layout);

	VkImageSubresourceRange vk_range = {0};
	vk_range.aspectMask              = spud_format_aspect(image->_format_vk);
	vk_range.baseMipLevel            = (uint32_t)range->base_mip_level;
	vk_range.levelCount              = (uint32_t)range->mip_level_count;
	vk_range.baseArrayLayer          = (uint32_t)range->base_array_layer;
	vk_range.layerCount              = (uint32_t)range->array_layer_count;

	spudgpuvulkan___cmd_image_barrier_raw(cmd->_command_buffer_vk, image->_image_vk, vk_old, vk_new, vk_range);
}

// ---------------------------------------------------------------------------
// Public API: spudgpu_cmd_copy_buffer_to_image / spudgpu_cmd_copy_image_to_buffer
// ---------------------------------------------------------------------------

static VkBufferImageCopy spudgpuvulkan___make_buffer_image_copy(
    VkImageAspectFlags aspect,
    const spudgpu_image_buffer_copy_desc *desc) {
	VkBufferImageCopy region               = {0};
	region.bufferOffset                    = (VkDeviceSize)desc->buffer_offset;
	region.bufferRowLength                 = desc->buffer_row_length;
	region.bufferImageHeight               = desc->buffer_image_height;
	region.imageSubresource.aspectMask     = aspect;
	region.imageSubresource.mipLevel       = desc->mip_level;
	region.imageSubresource.baseArrayLayer = desc->base_array_layer;
	region.imageSubresource.layerCount     = desc->array_layer_count ? desc->array_layer_count : 1;
	region.imageOffset.x                   = (int32_t)desc->image_x;
	region.imageOffset.y                   = (int32_t)desc->image_y;
	region.imageOffset.z                   = (int32_t)desc->image_z;
	region.imageExtent.width               = desc->width;
	region.imageExtent.height              = desc->height;
	region.imageExtent.depth               = desc->depth ? desc->depth : 1;
	return region;
}

void spudgpu_cmd_copy_image_to_buffer(
    spudgpu_command_list cmd,
    spudgpu_image src_image,
    spudgpu_buffer dst_buffer,
    const spudgpu_image_buffer_copy_desc *desc) {
	if (!cmd || !src_image || !dst_buffer || !desc)
		return;

	VkBufferImageCopy region = spudgpuvulkan___make_buffer_image_copy(spud_format_aspect(src_image->_format_vk), desc);

	vkCmdCopyImageToBuffer(cmd->_command_buffer_vk, src_image->_image_vk, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_buffer->_buffer_vk, 1, &region);
}

void spudgpu_cmd_copy_buffer_to_image(
    spudgpu_command_list cmd,
    spudgpu_buffer src_buffer,
    spudgpu_image dst_image,
    const spudgpu_image_buffer_copy_desc *desc) {
	if (!cmd || !src_buffer || !dst_image || !desc)
		return;

	VkBufferImageCopy region = spudgpuvulkan___make_buffer_image_copy(spud_format_aspect(dst_image->_format_vk), desc);

	vkCmdCopyBufferToImage(cmd->_command_buffer_vk, src_buffer->_buffer_vk, dst_image->_image_vk, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

// ---------------------------------------------------------------------------
// Public API: spudgpu_get_image_buffer_copy_size
// Vulkan has no row-pitch alignment requirement for buffer<->image copies
// (bufferRowLength/bufferImageHeight of 0 means tightly packed), so this
// always reports the tightly-packed size for the given mip level.
// ---------------------------------------------------------------------------

void spudgpu_get_image_buffer_copy_size(
    spudgpu_image image,
    uint32_t mip_level,
    uint64_t *out_row_pitch,
    uint64_t *out_total_size) {
	if (!image)
		return;

	uint32_t mip_width  = image->_desc.width >> mip_level;
	uint32_t mip_height = image->_desc.height >> mip_level;
	uint32_t mip_depth  = image->_desc.depth >> mip_level;
	if (!mip_width)
		mip_width = 1;
	if (!mip_height)
		mip_height = 1;
	if (!mip_depth)
		mip_depth = 1;

	uint64_t row_pitch = (uint64_t)mip_width * (spudgpu_format_bit_count(image->_desc.format) / 8);

	if (out_row_pitch)
		*out_row_pitch = row_pitch;
	if (out_total_size)
		*out_total_size = row_pitch * mip_height * mip_depth;
}

// ---------------------------------------------------------------------------
// Public API: spudgpu_cmd_blit_image
// ---------------------------------------------------------------------------

void spudgpu_cmd_blit_image(
    spudgpu_command_list cmd,
    spudgpu_image src_image,
    spudgpu_image dst_image,
    const spudgpu_image_blit_desc *desc) {
	if (!cmd || !src_image || !dst_image || !desc)
		return;

	VkImageBlit region                   = {0};
	region.srcSubresource.aspectMask     = spud_format_aspect(src_image->_format_vk);
	region.srcSubresource.mipLevel       = desc->src_mip_level;
	region.srcSubresource.baseArrayLayer = desc->src_base_array_layer;
	region.srcSubresource.layerCount     = desc->src_array_layer_count ? desc->src_array_layer_count : 1;
	region.srcOffsets[0].x               = (int32_t)desc->src_x0;
	region.srcOffsets[0].y               = (int32_t)desc->src_y0;
	region.srcOffsets[0].z               = (int32_t)desc->src_z0;
	region.srcOffsets[1].x               = (int32_t)desc->src_x1;
	region.srcOffsets[1].y               = (int32_t)desc->src_y1;
	region.srcOffsets[1].z               = (int32_t)desc->src_z1;

	region.dstSubresource.aspectMask     = spud_format_aspect(dst_image->_format_vk);
	region.dstSubresource.mipLevel       = desc->dst_mip_level;
	region.dstSubresource.baseArrayLayer = desc->dst_base_array_layer;
	region.dstSubresource.layerCount     = desc->dst_array_layer_count ? desc->dst_array_layer_count : 1;
	region.dstOffsets[0].x               = (int32_t)desc->dst_x0;
	region.dstOffsets[0].y               = (int32_t)desc->dst_y0;
	region.dstOffsets[0].z               = (int32_t)desc->dst_z0;
	region.dstOffsets[1].x               = (int32_t)desc->dst_x1;
	region.dstOffsets[1].y               = (int32_t)desc->dst_y1;
	region.dstOffsets[1].z               = (int32_t)desc->dst_z1;

	vkCmdBlitImage(
	    cmd->_command_buffer_vk, src_image->_image_vk, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_image->_image_vk, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
	    &region, spud_filter_to_vk(desc->filter));
}

// ---------------------------------------------------------------------------
// Internal helpers: map SpudGPU load/store op → Vulkan
// ---------------------------------------------------------------------------

static VkAttachmentLoadOp spud_load_op_to_vk(SPUDGPU_LOAD_OP op) {
	switch (op) {
	case SPUDGPU_LOAD_OP_CLEAR:
		return VK_ATTACHMENT_LOAD_OP_CLEAR;
	case SPUDGPU_LOAD_OP_DONT_CARE:
		return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	case SPUDGPU_LOAD_OP_LOAD:
	default:
		return VK_ATTACHMENT_LOAD_OP_LOAD;
	}
}

static VkAttachmentStoreOp spud_store_op_to_vk(SPUDGPU_STORE_OP op) {
	switch (op) {
	case SPUDGPU_STORE_OP_DONT_CARE:
		return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	case SPUDGPU_STORE_OP_STORE:
	default:
		return VK_ATTACHMENT_STORE_OP_STORE;
	}
}

// ---------------------------------------------------------------------------
// Public API: spudgpu_cmd_begin_rendering / spudgpu_cmd_end_rendering
// Vulkan 1.3 core dynamic rendering (vkCmdBeginRendering/vkCmdEndRendering) —
// no VkRenderPass/VkFramebuffer object, no pipeline-compatibility coupling.
// See the TODO in spudgpu.h re: a classic VkRenderPass fallback for devices
// without Vulkan 1.3 / VK_KHR_dynamic_rendering support.
// ---------------------------------------------------------------------------

void spudgpu_cmd_begin_rendering(
    spudgpu_command_list cmd,
    const spudgpu_rendering_begin_desc *desc) {
	if (!cmd || !desc)
		return;

	uint32_t color_count = desc->color_attachment_count;
	if (color_count > SPUDGPU_MAX_COLOR_ATTACHMENTS)
		color_count = SPUDGPU_MAX_COLOR_ATTACHMENTS;

	VkRenderingAttachmentInfo color_infos[SPUDGPU_MAX_COLOR_ATTACHMENTS] = {0};
	for (uint32_t i = 0; i < color_count; ++i) {
		const spudgpu_color_attachment_desc *src = &desc->color_attachments[i];
		spudgpu_image_view_vulkan *view          = (spudgpu_image_view_vulkan *)src->image_view;

		VkRenderingAttachmentInfo *info   = &color_infos[i];
		info->sType                       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		info->imageView                   = view->_image_view_vk;
		info->imageLayout                 = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		info->loadOp                      = spud_load_op_to_vk(src->load_op);
		info->storeOp                     = spud_store_op_to_vk(src->store_op);
		info->clearValue.color.float32[0] = src->clear_color[0];
		info->clearValue.color.float32[1] = src->clear_color[1];
		info->clearValue.color.float32[2] = src->clear_color[2];
		info->clearValue.color.float32[3] = src->clear_color[3];
	}

	VkRenderingAttachmentInfo depth_info   = {0};
	VkRenderingAttachmentInfo stencil_info = {0};
	int has_depth                          = (desc->depth_attachment.image_view != NULL);
	int has_stencil                        = 0;
	if (has_depth) {
		spudgpu_image_view_vulkan *view = (spudgpu_image_view_vulkan *)desc->depth_attachment.image_view;
		has_stencil                     = (spud_format_aspect(view->_desc.parent_image->_format_vk) & VK_IMAGE_ASPECT_STENCIL_BIT) != 0;

		depth_info.sType                         = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depth_info.imageView                     = view->_image_view_vk;
		depth_info.imageLayout                   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depth_info.loadOp                        = spud_load_op_to_vk(desc->depth_attachment.depth_load_op);
		depth_info.storeOp                       = spud_store_op_to_vk(desc->depth_attachment.depth_store_op);
		depth_info.clearValue.depthStencil.depth = desc->depth_attachment.clear_depth;

		if (has_stencil) {
			stencil_info                                 = depth_info;
			stencil_info.loadOp                          = spud_load_op_to_vk(desc->depth_attachment.stencil_load_op);
			stencil_info.storeOp                         = spud_store_op_to_vk(desc->depth_attachment.stencil_store_op);
			stencil_info.clearValue.depthStencil.stencil = desc->depth_attachment.clear_stencil;
		}
	}

	VkRenderingInfo rendering_info          = {0};
	rendering_info.sType                    = VK_STRUCTURE_TYPE_RENDERING_INFO;
	rendering_info.renderArea.offset.x      = 0;
	rendering_info.renderArea.offset.y      = 0;
	rendering_info.renderArea.extent.width  = desc->width;
	rendering_info.renderArea.extent.height = desc->height;
	rendering_info.layerCount               = 1;
	rendering_info.colorAttachmentCount     = color_count;
	rendering_info.pColorAttachments        = color_infos;
	rendering_info.pDepthAttachment         = has_depth ? &depth_info : NULL;
	rendering_info.pStencilAttachment       = has_stencil ? &stencil_info : NULL;

	vkCmdBeginRendering(cmd->_command_buffer_vk, &rendering_info);
}

void spudgpu_cmd_end_rendering(spudgpu_command_list cmd) {
	if (!cmd)
		return;
	vkCmdEndRendering(cmd->_command_buffer_vk);
}

#if __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_VULKAN_API
