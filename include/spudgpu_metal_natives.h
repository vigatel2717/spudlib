
#ifndef SPUDLIB_SPUDGPU_METAL_NATIVES_H
#define SPUDLIB_SPUDGPU_METAL_NATIVES_H

#if SPUDGPU_COMPILE_METAL_API

#include <Metal/Metal.h>
#include <spudgpu.h>

#ifdef __cplusplus
extern "C" {

#endif

/* Escape hatch: returns raw Metal handles from SpudGPU opaque objects.
 * Use only for interop with third-party Metal libraries (e.g. ImGui, MetalFX).
 * SpudGPU has no visibility into Metal calls made with these handles.
 *
 * Metal's object model doesn't line up 1:1 with the Vulkan/D3D12 natives
 * headers in a few places - see the comments below rather than assuming a
 * missing accessor here is an oversight.
 */

/* No spudgpu_get_mtl_instance(): Metal has no top-level instance/factory
 * object (unlike VkInstance or IDXGIFactory). Devices are enumerated
 * directly via MTLCopyAllDevices(), so spudgpu_instance has no native
 * Metal handle to hand back. */

id<MTLDevice> spudgpu_get_mtl_device(spudgpu_device device);

/* No physical/logical device split: id<MTLDevice> already covers both
 * roles Vulkan splits into VkPhysicalDevice/VkDevice. */

id<MTLCommandQueue> spudgpu_get_mtl_command_queue(spudgpu_command_queue queue);

/* No spudgpu_get_mtl_queue_family_index(): Metal command queues aren't
 * partitioned into families the way Vulkan queues are. */

/* No native accessor for spudgpu_command_allocator: Metal has no
 * persistent allocator/pool object to reset between frames - command
 * buffers are requested fresh from the queue each time. */

id<MTLCommandBuffer> spudgpu_get_mtl_command_buffer(spudgpu_command_list cmd);

id<MTLBuffer> spudgpu_get_mtl_buffer(spudgpu_buffer buffer);

/* Metal has no separate buffer-view object (Vulkan's VkBufferView, D3D12's
 * *_BUFFER_VIEW descs) - buffers are bound directly with an offset/stride
 * at bind time. This struct carries the same information out of SpudGPU's
 * own view abstraction instead of a native handle. */
typedef struct SPUDGPU_MTL_BUFFER_VIEW {
	id<MTLBuffer> buffer;
	NSUInteger offset;
	NSUInteger length;
	NSUInteger stride;
} SPUDGPU_MTL_BUFFER_VIEW;
bool spudgpu_get_mtl_buffer_view(
    spudgpu_buffer_view view,
    SPUDGPU_MTL_BUFFER_VIEW *out);

id<MTLTexture> spudgpu_get_mtl_texture(spudgpu_image image);

/* Metal represents a "view of a texture" (reinterpreted format/mip/array
 * range, via -newTextureViewWithPixelFormat:...) as another
 * id<MTLTexture>, not a distinct lightweight descriptor type the way
 * Vulkan's VkImageView or D3D12's *_VIEW_DESC structs are - so this
 * returns the same native type as spudgpu_get_mtl_texture(), just for the
 * view object rather than the base image. */
id<MTLTexture> spudgpu_get_mtl_texture_view(spudgpu_image_view view);

id<MTLRenderPipelineState>
spudgpu_get_mtl_render_pipeline_state(spudgpu_shader_pipeline pipeline);

/* No spudgpu_get_mtl_root_signature(): Metal has no runtime root-signature
 * / pipeline-layout object - binding indices come from [[buffer(n)]] /
 * [[texture(n)]] attributes baked into the shader source itself. */

/* Fences and semaphores both come back as id<MTLSharedEvent>, not
 * MTLFence: plain MTLFence only tracks hazards between encoders within a
 * single command buffer and can't be waited on from the CPU or across
 * command buffers, so it can't stand in for either Vulkan/D3D12 concept.
 * MTLSharedEvent supports both CPU- and GPU-side signal/wait, which is
 * what spudgpu_fence/spudgpu_semaphore actually need. */
id<MTLSharedEvent> spudgpu_get_mtl_shared_event(spudgpu_fence fence);
id<MTLSharedEvent>
spudgpu_get_mtl_shared_event_from_semaphore(spudgpu_semaphore semaphore);

MTLPixelFormat spudgpu_get_mtl_swap_chain_format(spudgpu_swap_chain swap_chain);

#ifdef __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_METAL_API

#endif //SPUDLIB_SPUDGPU_METAL_NATIVES_H
