#include "gpu/backends/vulkan/spudgpuvulkanresource.hpp"

namespace spud::gpu::backends::vulkan {
    std::shared_ptr<gpu_buffer_view> gpu_buffer_vulkan::create_view(
        const uint64_t &offset,
        const uint64_t &stride,
        const uint64_t &size) const {
        return nullptr;
    }

    gpu_buffer_vulkan::gpu_buffer_vulkan()
    = default;

    gpu_buffer_vulkan::~gpu_buffer_vulkan()
    = default;

    gpu_buffer_view_vulkan::gpu_buffer_view_vulkan() : m_pBufferVulkan(nullptr),
                                                       m_Offset(0),
                                                       m_Stride(0),
                                                       m_Size(0) {
    }

    gpu_buffer_view_vulkan::~gpu_buffer_view_vulkan() = default;

    std::shared_ptr<gpu_buffer> gpu_buffer_view_vulkan::get_buffer() const {
        return std::static_pointer_cast<gpu_buffer>(m_pBufferVulkan);
    }

    gpu_image_vulkan::gpu_image_vulkan()
    = default;

    gpu_image_vulkan::~gpu_image_vulkan()
    = default;

    gpu_resource_pool_vulkan::gpu_resource_pool_vulkan()
    = default;

    gpu_resource_pool_vulkan::~gpu_resource_pool_vulkan()
    = default;

    void gpu_resource_pool_vulkan::reserve(uint64_t bytes) {
    }

    std::shared_ptr<gpu_buffer> gpu_resource_pool_vulkan::allocate_buffer(const gpu_buffer_desc &desc) {
        return nullptr;
    }

    std::shared_ptr<gpu_image> gpu_resource_pool_vulkan::allocate_image(const gpu_image_desc &desc) {
        return nullptr;
    }

    void gpu_resource_pool_vulkan::free(const std::shared_ptr<gpu_buffer> &buffer) {
    }

    void gpu_resource_pool_vulkan::free(const std::shared_ptr<gpu_image> &image) {
    }
}
