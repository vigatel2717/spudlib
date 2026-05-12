//
// Created by nathanmoore on 3/27/26.
//

#ifndef SPUDLIB_SPUDGPUVULKANRESOURCE_HPP
#define SPUDLIB_SPUDGPUVULKANRESOURCE_HPP

#include "gpu/spudgpuresource.hpp"
#include "gpu/backends/vulkan/spudgpuvulkandef.hpp"

namespace spud::gpu::backends::vulkan {
    class gpu_buffer_vulkan : public gpu_buffer {
    protected:
        std::shared_ptr<gpu_buffer_view> create_view(
            const uint64_t &offset,
            const uint64_t &stride,
            const uint64_t &size) const override;

    public:
        gpu_buffer_vulkan();

        ~gpu_buffer_vulkan() override;

        [[nodiscard]] inline gpu_buffer_desc get_desc() const override { return m_desc; }

    private:
        gpu_buffer_desc m_desc;
    };

    class gpu_buffer_view_vulkan : public gpu_buffer_view {
    public:
        gpu_buffer_view_vulkan();

        ~gpu_buffer_view_vulkan() override;

        [[nodiscard]] std::shared_ptr<gpu_buffer> get_buffer() const override;

        [[nodiscard]] uint64_t get_offset() const override { return m_Offset; }
        [[nodiscard]] uint64_t get_stride() const override { return m_Stride; }
        [[nodiscard]] uint64_t get_size() const override { return m_Size; }

        [[nodiscard]] uint64_t get_native_api_object() const override { return 0; }

    private:
        std::shared_ptr<gpu_buffer_vulkan> m_pBufferVulkan;
        uint64_t m_Offset, m_Stride, m_Size;

    };


    class gpu_image_vulkan : public gpu_image {
    public:
        gpu_image_vulkan();

        ~gpu_image_vulkan() override;

        [[nodiscard]] inline gpu_image_desc get_desc() const override { return m_desc; }

    private:
        gpu_image_desc m_desc;
    };

    class gpu_resource_pool_vulkan : public gpu_resource_pool {
    public:
        gpu_resource_pool_vulkan();

        ~gpu_resource_pool_vulkan() override;

        void reserve(uint64_t bytes) override;

        std::shared_ptr<gpu_buffer> allocate_buffer(const gpu_buffer_desc &desc) override;

        std::shared_ptr<gpu_image> allocate_image(const gpu_image_desc &desc) override;

        void free(const std::shared_ptr<gpu_buffer> &buffer) override;

        void free(const std::shared_ptr<gpu_image> &image) override;
    };
}

#endif //SPUDLIB_SPUDGPUVULKANRESOURCE_HPP
