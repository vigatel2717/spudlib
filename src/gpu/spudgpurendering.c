
#include "spudgpu.h"

// Backend-agnostic clear helpers, implemented purely in terms of
// spudgpu_cmd_begin_rendering/end_rendering with zero draw calls in between.
// No backend-specific code lives here — see spudgpu.h's rendering section
// for why a standalone clear is expressed this way instead of a dedicated
// per-backend clear-image command.

#if __cplusplus
extern "C" {
#endif

void spudgpu_cmd_clear_color_attachment(
    spudgpu_command_list cmd,
    spudgpu_image_view attachment,
    float r, float g, float b, float a,
    uint32_t width, uint32_t height) {
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
	desc.color_attachments      = &color;
	desc.color_attachment_count = 1;
	desc.width                  = width;
	desc.height                 = height;

	spudgpu_cmd_begin_rendering(cmd, &desc);
	spudgpu_cmd_end_rendering(cmd);
}

void spudgpu_cmd_clear_depth_attachment(
    spudgpu_command_list cmd,
    spudgpu_image_view attachment,
    bool clear_depth, bool clear_stencil,
    float depth, uint32_t stencil,
    uint32_t width, uint32_t height) {
	if (!cmd || !attachment)
		return;

	spudgpu_rendering_begin_desc desc = {0};
	desc.depth_attachment.image_view        = attachment;
	desc.depth_attachment.depth_load_op     = clear_depth ? SPUDGPU_LOAD_OP_CLEAR : SPUDGPU_LOAD_OP_LOAD;
	desc.depth_attachment.depth_store_op    = SPUDGPU_STORE_OP_STORE;
	desc.depth_attachment.stencil_load_op   = clear_stencil ? SPUDGPU_LOAD_OP_CLEAR : SPUDGPU_LOAD_OP_LOAD;
	desc.depth_attachment.stencil_store_op  = SPUDGPU_STORE_OP_STORE;
	desc.depth_attachment.clear_depth       = depth;
	desc.depth_attachment.clear_stencil     = stencil;
	desc.width  = width;
	desc.height = height;

	spudgpu_cmd_begin_rendering(cmd, &desc);
	spudgpu_cmd_end_rendering(cmd);
}

#if __cplusplus
}
#endif
