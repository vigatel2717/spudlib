#include "gpu/backends/vulkan/spudgpuvulkanshader.hpp"

namespace spud::gpu::backends::vulkan {
    gpu_shader_vulkan::gpu_shader_vulkan() : m_desc{} {
    }

    gpu_shader_vulkan::~gpu_shader_vulkan() {
    }

    gpu_shader_pipeline_vulkan::gpu_shader_pipeline_vulkan()
        : m_shaders{} {
    }

    gpu_shader_pipeline_vulkan::~gpu_shader_pipeline_vulkan() {
    }
}
