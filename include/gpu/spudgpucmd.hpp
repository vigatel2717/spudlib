//
// Created by nathanmoore on 3/27/26.
//

#ifndef SPUDLIB_SPUDGPUCMD_HPP
#define SPUDLIB_SPUDGPUCMD_HPP

#include <cstdint>
#include <memory>

/*
namespace spud::gpu {
    class gpu_buffer_view;

    // Viewport descriptor
    struct VIEWPORT_DESC {
        float x; // Top-left x
        float y; // Top-left y
        float width; // Viewport width
        float height; // Viewport height
        float minDepth; // Depth range [0, 1]
        float maxDepth; // Depth range [0, 1]
    };

    // Scissor Rect descriptor
    struct SCISSOR_RECT_DESC {
        float x;
        float y;
        float width;
        float height;
    };

    // Record commands into a command buffer.
    class command_buffer {
    public:
        command_buffer() = default;

        virtual ~command_buffer() = default;

        // Set the viewport rect.
        // Graphics Pipeline Command
        virtual void set_viewports(
            const uint32_t &firstViewport,
            const uint32_t &viewportCount,
            const VIEWPORT_DESC *pViewports) = 0;

        // Set the scissor rect.
        // Graphics Pipeline Command
        virtual void set_scissor_rects(
            const uint32_t &firstScissorRect,
            const uint32_t &scissorRectCount,
            const SCISSOR_RECT_DESC *pScissorRects) = 0;

        // Bind the shader pipeline.
        // Graphics Pipeline Command
        virtual void bind_pipeline() = 0;

        // Bind the descriptor sets.
        // Graphics Pipeline Command
        virtual void bind_descriptor_sets() = 0;

        // Set the vertex buffers to be drawn.
        // Make sure to validate that 'pViews' is indeed all Vertex Buffer data.
        // Graphics Pipeline Command
        virtual void set_vertex_buffers(
            const uint32_t &startSlot,
            const uint32_t &viewCount,
            std::shared_ptr<gpu_buffer_view> *pViews) = 0;

        // Set the index buffers to be drawn.
        // Make sure to validate that 'pViews' is indeed all Index Buffer data.
        // Graphics Pipeline Command
        virtual void set_index_buffers(
            const uint32_t &viewCount,
            std::shared_ptr<gpu_buffer_view> *pViews) = 0;

        // Draw
        // Draw call
        virtual void draw(
            const uint32_t &vertexCount,
            const uint32_t &startVertexLocation) = 0;

        // Draw indexed.
        // Draw call
        virtual void draw_indexed(
            const uint32_t &indexCount,
            const uint32_t &startIndexLocation,
            const uint32_t &baseVertexLocation) = 0;

        // Draw instanced
        // Draw call
        virtual void draw_instanced(
            const uint32_t &vertexCountPerInstance,
            const uint32_t &instanceCount,
            const uint32_t &startVertexLocation,
            const uint32_t &startInstanceLocation) = 0;

        // Draw indexed instanced.
        // Draw call
        virtual void draw_indexed_instanced(
            const uint32_t &indexCountPerInstance,
            const uint32_t &instanceCount,
            const uint32_t &startIndexLocation,
            const uint32_t &baseVertexLocation,
            const uint32_t &startInstanceLocation) = 0;

        // Draw indirect.
        // Draw call
        //virtual void draw_indirect() = 0;

        // Draw indexed indirect.
        // Draw call
        //virtual void draw_indexed_indirect() = 0;

        // Dispatch.
        // Compute Pipeline Command
        virtual void dispatch() = 0;

        // Dispatch indirect.
        // Compute Pipeline Command
        virtual void dispatch_indirect() = 0;

        // Copy data between two buffers.
        // Transfer Command
        virtual void copy_buffer() = 0;

        // Copy data between two images.
        // Transfer Command
        virtual void copy_image() = 0;

        // Blit an image (potentially with scaling, filtering, etc.)
        // Transfer Command
        virtual void blit_image() = 0;

        // Clear an image to a color.
        // Transfer Command
        virtual void clear_color_image() = 0;

        // Clear a depth stencil attachment.
        // Transfer Command
        virtual void clear_depth_stencil_image() = 0;

        // Ensure memory access are ordered correctly.
        // Control Command
        virtual void pipeline_barrier() = 0;

        // Set event.
        // Control Command
        virtual void set_event() = 0;

        // Reset event.
        // Control Command
        virtual void reset_event() = 0;

        // Wait for events.
        // Control Command
        virtual void wait_events() = 0;

        // Begin render pass.
        // Control Command
        virtual void begin_render_pass() = 0;

        // End render pass.
        // Control Command
        virtual void end_render_pass() = 0;
    };

    // Manager with memory pool for creating command buffer objects.
    // Created from gpu_device->create_command_allocator()
    class command_allocator {
    public:
        command_allocator() = default;

        virtual ~command_allocator() = default;

        virtual void reset() = 0;
    };
}*/

#endif //SPUDLIB_SPUDGPUCMD_HPP
