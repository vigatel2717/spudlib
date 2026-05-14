//
// Created by nathanmoore on 3/27/26.
//

#ifndef SPUDLIB_SPUDGPUVULKANRESOURCE_HPP
#define SPUDLIB_SPUDGPUVULKANRESOURCE_HPP

#include "gpu/spudgpuresource.hpp"
#include "gpu/backends/vulkan/spudgpuvulkandef.hpp"

namespace spud::gpu::backends::vulkan {
    class gpu_device_vulkan;

    class gpu_buffer_vulkan : public gpu_buffer {
    protected:
        std::shared_ptr<gpu_buffer_view> create_view(
            const uint64_t &offset,
            const uint64_t &stride,
            const uint64_t &size) const override;

    public:
        gpu_buffer_vulkan(
            const std::shared_ptr<gpu_device_vulkan> &device,
            const gpu_buffer_desc &desc,
            VkMemoryPropertyFlags properties);

        ~gpu_buffer_vulkan() override;

        [[nodiscard]] inline gpu_buffer_desc get_desc() const override { return m_desc; }

        [[nodiscard]] std::shared_ptr<gpu_device> get_gpu_device() const override {
            return std::reinterpret_pointer_cast<gpu_device>(m_device);
        }

        [[nodiscard]] uint64_t get_native_api_object() const override { return reinterpret_cast<uint64_t>(m_buffer); }

    private:
        static uint32_t findMemoryType(
            VkPhysicalDevice physicalDevice,
            uint32_t typeFilter,
            VkMemoryPropertyFlags properties);

        VkBuffer m_buffer;
        VkDeviceMemory m_memory;
        VkMemoryPropertyFlags m_properties;

        gpu_buffer_desc m_desc;
        std::shared_ptr<gpu_device_vulkan> m_device;
    };

    class gpu_buffer_view_vulkan : public gpu_buffer_view {
    public:
        gpu_buffer_view_vulkan(
            const std::shared_ptr<gpu_buffer_vulkan> &parentBuffer,
            const gpu_buffer_view_desc &desc);

        ~gpu_buffer_view_vulkan() override;

        [[nodiscard]] gpu_buffer_view_desc get_desc() const override { return m_desc; }

        [[nodiscard]] uint64_t get_native_api_object() const override { return 0; }

    private:
        std::shared_ptr<gpu_buffer_vulkan> m_pParentBuffer;
        gpu_buffer_view_desc m_desc;
    };


    class gpu_image_vulkan : public gpu_image {
    public:
        gpu_image_vulkan(
            const std::shared_ptr<gpu_device_vulkan> &device,
            const gpu_image_desc &desc);

        ~gpu_image_vulkan() override;

        [[nodiscard]] inline gpu_image_desc get_desc() const override { return m_desc; }

        [[nodiscard]] std::shared_ptr<gpu_device> get_gpu_device() const override {
            return std::reinterpret_pointer_cast<gpu_device>(m_device);
        }

        [[nodiscard]] uint64_t get_native_api_object() const override { return reinterpret_cast<uint64_t>(m_image); }

    private:
        static uint32_t find_memory_type(
            uint32_t typeFilter,
            VkMemoryPropertyFlags properties,
            VkPhysicalDevice physicalDevice);

        gpu_image_desc m_desc;
        VkImage m_image;
        VkFormat m_vk_format;
        VkDeviceMemory m_memory;
        std::shared_ptr<gpu_device_vulkan> m_device;
    };

    class gpu_image_view_vulkan : public gpu_image_view {
        public:
        gpu_image_view_vulkan();
        ~gpu_image_view_vulkan() override;

        [[nodiscard]] gpu_image_view_desc get_desc() const override { return m_desc; }

        [[nodiscard]] uint64_t get_native_api_object() const override { return 0; }

    private:
        gpu_image_view_desc m_desc;
        VkImageView m_view;
        std::shared_ptr<gpu_image_vulkan> m_device;
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
