//
// SpudGPU Metal backend - render pass / render pipeline descriptors.
// spudgpu_cmd_begin_rendering/end_rendering are the one piece of this file
// implemented against real Apple hardware; the copy/blit functions below
// remain unimplemented placeholders (no caller needs them yet).
//

#if SPUDGPU_COMPILE_METAL_API

#include "spudgpu.h"
#include "spudcore.h"
#include "spudgpumetal.h"
#include <Metal/Metal.h>
#include <stdio.h>

// SPUDGPU_IMAGE_LAYOUT is a Vulkan/D3D12-shaped state machine (UNDEFINED,
// COLOR_ATTACHMENT_OPTIMAL, SHADER_READ_ONLY, ...) that Metal has no
// equivalent to at all - a texture is usable as a render target,
// shader-read resource, or blit source/destination interchangeably with no
// transition call, ever. The three barrier functions below are correctly
// no-ops for any recognized layout value; this only rejects (silently, with
// a diagnostic) a value outside the defined enum range, which is a caller
// bug (garbage/uninitialized input) on every backend, not a Metal
// limitation - there is nothing in the recognized range Metal "doesn't
// support," since none of it does anything here regardless.
static bool spudgpumetal___internal_layout_recognized(SPUDGPU_IMAGE_LAYOUT layout) {
	return layout <= SPUDGPU_IMAGE_LAYOUT_PRESENT_SRC;
}

void spudgpu_cmd_image_barrier(
    spudgpu_command_list cmd,
    spudgpu_image image,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout) {
	if (!cmd || !image)
		return;
	if (!spudgpumetal___internal_layout_recognized(old_layout) ||
	    !spudgpumetal___internal_layout_recognized(new_layout)) {
		printf("spudgpu: spudgpu_cmd_image_barrier - unrecognized SPUDGPU_IMAGE_LAYOUT value\n");
		return;
	}

	// No-op - see the note above spudgpumetal___internal_layout_recognized.
}

void spudgpu_cmd_image_barrier_view(
    spudgpu_command_list cmd,
    spudgpu_image_view image_view,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout) {
	if (!cmd || !image_view)
		return;
	if (!spudgpumetal___internal_layout_recognized(old_layout) ||
	    !spudgpumetal___internal_layout_recognized(new_layout)) {
		printf("spudgpu: spudgpu_cmd_image_barrier_view - unrecognized SPUDGPU_IMAGE_LAYOUT value\n");
		return;
	}

	// No-op - see the note above spudgpumetal___internal_layout_recognized.
}

void spudgpu_cmd_image_barrier_subresource(
    spudgpu_command_list cmd,
    spudgpu_image image,
    const spudgpu_image_view_desc_subresource_range *range,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout) {
	if (!cmd || !image || !range)
		return;
	if (!spudgpumetal___internal_layout_recognized(old_layout) ||
	    !spudgpumetal___internal_layout_recognized(new_layout)) {
		printf("spudgpu: spudgpu_cmd_image_barrier_subresource - unrecognized SPUDGPU_IMAGE_LAYOUT value\n");
		return;
	}

	// No-op - see the note above spudgpumetal___internal_layout_recognized.
}

void spudgpu_cmd_copy_image_to_buffer(
    spudgpu_command_list cmd,
    spudgpu_image src_image,
    spudgpu_buffer dst_buffer,
    const spudgpu_image_buffer_copy_desc *desc) {
	if (!cmd || !src_image || !dst_buffer || !desc)
		return;

	// METAL API CODE
}

void spudgpu_cmd_copy_buffer_to_image(
    spudgpu_command_list cmd,
    spudgpu_buffer src_buffer,
    spudgpu_image dst_image,
    const spudgpu_image_buffer_copy_desc *desc) {
	if (!cmd || !src_buffer || !dst_image || !desc)
		return;

	// METAL API CODE
}

void spudgpu_get_image_buffer_copy_size(
    spudgpu_image image,
    uint32_t mip_level,
    uint64_t *out_row_pitch,
    uint64_t *out_total_size) {
	if (!image || !out_row_pitch || !out_total_size)
		return;

	*out_row_pitch  = 0;
	*out_total_size = 0;

	// METAL API CODE
}

void spudgpu_cmd_blit_image(
    spudgpu_command_list cmd,
    spudgpu_image src_image,
    spudgpu_image dst_image,
    const spudgpu_image_blit_desc *desc) {
	if (!cmd || !src_image || !dst_image || !desc)
		return;

	// METAL API CODE
}

static MTLLoadAction spudgpumetal___internal_load_action(SPUDGPU_LOAD_OP op) {
	switch (op) {
	case SPUDGPU_LOAD_OP_LOAD:  return MTLLoadActionLoad;
	case SPUDGPU_LOAD_OP_CLEAR: return MTLLoadActionClear;
	default:                    return MTLLoadActionDontCare;
	}
}

static MTLStoreAction spudgpumetal___internal_store_action(SPUDGPU_STORE_OP op) {
	switch (op) {
	case SPUDGPU_STORE_OP_STORE: return MTLStoreActionStore;
	default:                     return MTLStoreActionDontCare;
	}
}

void spudgpu_cmd_begin_rendering(
    spudgpu_command_list cmd,
    const spudgpu_rendering_begin_desc *desc) {
	if (!cmd || !desc)
		return;

	spudgpu_command_list_metal *cmd_metal = (spudgpu_command_list_metal *)cmd;

	// Metal disallows two live encoders of different kinds on one command
	// buffer at once - end any compute encoder left active by
	// spudgpu_cmd_bind_compute_pipeline/spudgpu_cmd_dispatch before opening
	// the render encoder below (spudgpumetalcommand.m).
	spudgpumetal___internal_end_active_compute_encoder(cmd_metal);

	MTLRenderPassDescriptor *pass_desc = [MTLRenderPassDescriptor renderPassDescriptor];

	uint32_t color_count = desc->color_attachment_count;
	if (color_count > SPUDGPU_MAX_COLOR_ATTACHMENTS)
		color_count = SPUDGPU_MAX_COLOR_ATTACHMENTS;

	for (uint32_t i = 0; i < color_count; i++) {
		const spudgpu_color_attachment_desc *src = &desc->color_attachments[i];
		spudgpu_image_view_metal *view           = (spudgpu_image_view_metal *)src->image_view;
		if (!view || !view->_texture_view_mtl)
			continue;

		MTLRenderPassColorAttachmentDescriptor *attach = pass_desc.colorAttachments[i];
		attach.texture                                 = view->_texture_view_mtl;
		attach.loadAction                              = spudgpumetal___internal_load_action(src->load_op);
		attach.storeAction                             = spudgpumetal___internal_store_action(src->store_op);
		attach.clearColor = MTLClearColorMake(src->clear_color[0], src->clear_color[1], src->clear_color[2], src->clear_color[3]);
	}

	if (desc->depth_attachment.image_view) {
		spudgpu_image_view_metal *view = (spudgpu_image_view_metal *)desc->depth_attachment.image_view;
		if (view->_texture_view_mtl) {
			pass_desc.depthAttachment.texture     = view->_texture_view_mtl;
			pass_desc.depthAttachment.loadAction  = spudgpumetal___internal_load_action(desc->depth_attachment.depth_load_op);
			pass_desc.depthAttachment.storeAction = spudgpumetal___internal_store_action(desc->depth_attachment.depth_store_op);
			pass_desc.depthAttachment.clearDepth  = desc->depth_attachment.clear_depth;

			// Depth32Float_Stencil8 is the only combined depth/stencil format
			// this backend produces (see spudgpumetalimage.m) - the same
			// texture doubles as the stencil attachment when it's in that
			// format.
			if (view->_texture_view_mtl.pixelFormat == MTLPixelFormatDepth32Float_Stencil8) {
				pass_desc.stencilAttachment.texture      = view->_texture_view_mtl;
				pass_desc.stencilAttachment.loadAction   = spudgpumetal___internal_load_action(desc->depth_attachment.stencil_load_op);
				pass_desc.stencilAttachment.storeAction  = spudgpumetal___internal_store_action(desc->depth_attachment.stencil_store_op);
				pass_desc.stencilAttachment.clearStencil = desc->depth_attachment.clear_stencil;
			}
		}
	}

	pass_desc.renderTargetWidth  = desc->width;
	pass_desc.renderTargetHeight = desc->height;

	// -renderCommandEncoderWithDescriptor: returns an autoreleased encoder
	// (same as -nextDrawable in spudgpumetalswapchain.m) - retain it so it
	// survives past this call, to be released in spudgpu_cmd_end_rendering.
	id<MTLRenderCommandEncoder> encoder = [cmd_metal->_command_buffer_mtl renderCommandEncoderWithDescriptor:pass_desc];
	cmd_metal->_active_render_encoder   = [encoder retain];
}

void spudgpu_cmd_end_rendering(spudgpu_command_list cmd) {
	if (!cmd)
		return;

	spudgpu_command_list_metal *cmd_metal = (spudgpu_command_list_metal *)cmd;
	if (!cmd_metal->_active_render_encoder)
		return;

	[cmd_metal->_active_render_encoder endEncoding];
	[cmd_metal->_active_render_encoder release];
	cmd_metal->_active_render_encoder = nil;
}

// spudgpu_cmd_clear_color_attachment / spudgpu_cmd_clear_depth_attachment —
// Metal's native idiom for clearing an attachment already *is*
// MTLRenderPassDescriptor.colorAttachments[i].loadAction = .clear, which is
// exactly what begin/end-with-LOAD_OP_CLEAR expresses; there is no separate
// Metal clear command to call instead the way D3D12/Vulkan have. This is
// pure composition of the two functions above (no new Metal API surface),
// so it's correct today even though begin/end_rendering are still stubs —
// nothing here needs revisiting once those are implemented for real.
void spudgpu_cmd_clear_color_attachment(
    spudgpu_command_list cmd,
    spudgpu_image_view attachment,
    float r,
    float g,
    float b,
    float a,
    uint32_t width,
    uint32_t height) {
	if (!cmd || !attachment)
		return;

	spudgpu_color_attachment_desc color = {0};
	color.image_view     = attachment;
	color.load_op        = SPUDGPU_LOAD_OP_CLEAR;
	color.store_op       = SPUDGPU_STORE_OP_STORE;
	color.clear_color[0] = r;
	color.clear_color[1] = g;
	color.clear_color[2] = b;
	color.clear_color[3] = a;

	spudgpu_rendering_begin_desc desc = {0};
	desc.color_attachments[0]   = color;
	desc.color_attachment_count = 1;
	desc.width                  = width;
	desc.height                 = height;

	spudgpu_cmd_begin_rendering(cmd, &desc);
	spudgpu_cmd_end_rendering(cmd);
}

void spudgpu_cmd_clear_depth_attachment(
    spudgpu_command_list cmd,
    spudgpu_image_view attachment,
    bool clear_depth,
    bool clear_stencil,
    float depth,
    uint32_t stencil,
    uint32_t width,
    uint32_t height) {
	if (!cmd || !attachment)
		return;

	spudgpu_rendering_begin_desc desc = {0};
	desc.depth_attachment.image_view       = attachment;
	desc.depth_attachment.depth_load_op    = clear_depth ? SPUDGPU_LOAD_OP_CLEAR : SPUDGPU_LOAD_OP_LOAD;
	desc.depth_attachment.depth_store_op   = SPUDGPU_STORE_OP_STORE;
	desc.depth_attachment.stencil_load_op  = clear_stencil ? SPUDGPU_LOAD_OP_CLEAR : SPUDGPU_LOAD_OP_LOAD;
	desc.depth_attachment.stencil_store_op = SPUDGPU_STORE_OP_STORE;
	desc.depth_attachment.clear_depth      = depth;
	desc.depth_attachment.clear_stencil    = stencil;
	desc.width  = width;
	desc.height = height;

	spudgpu_cmd_begin_rendering(cmd, &desc);
	spudgpu_cmd_end_rendering(cmd);
}

void spudgpu_cmd_pipeline_barrier(
    spudgpu_command_list cmd,
    const spudgpu_buffer_barrier *buffer_barriers,
    uint32_t buffer_barrier_count,
    const spudgpu_image_barrier *image_barriers,
    uint32_t image_barrier_count) {
	if (!cmd)
		return;
	if (buffer_barrier_count > 0 && !buffer_barriers)
		return;
	if (image_barrier_count > 0 && !image_barriers)
		return;

	// No-op, deliberately - same underlying reason as the image_barrier
	// functions above (spudgpumetal___internal_layout_recognized): resources
	// created against a device (not a heap, which is the only way to opt out
	// via MTLHazardTrackingModeUntracked - see spudgpumetalbuffer.m/
	// spudgpumetalimage.m, neither of which does) are automatically
	// hazard-tracked by Metal, which inserts the required execution
	// dependency between encoders on the same command buffer that
	// read/write the same resource (Write-After-Write, Write-After-Read,
	// Read-After-Write) with no explicit call needed - this is what makes
	// "compute writes a buffer, then a later render pass on the same command
	// buffer reads it as an indirect-draw argument buffer" (see
	// SpudGPUExecuteIndirect) correct with zero code here.
	//
	// This does NOT cover a dependency spanning two different command
	// buffers or queues, where automatic tracking can't reach - that case
	// needs an explicit id<MTLFence> (update on the writer's encoder before
	// it ends, wait on the reader's encoder after it begins - see
	// ../../../CLAUDE.md's note on MTLFence belonging here, not in fence/
	// semaphore). Left unimplemented until a caller actually needs
	// cross-command-buffer buffer/image synchronization; this call's single
	// (cmd, before, after) shape has no active encoder to attach
	// update/waitForFence calls to at the point it's invoked, so it can't be
	// expressed generically for that case without a bigger signature change.
}

#endif // SPUDGPU_COMPILE_METAL_API
