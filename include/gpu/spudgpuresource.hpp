//
// Created by nathanmoore on 3/27/26.
//

#ifndef SPUDLIB_SPUDGPURESOURCE_HPP
#define SPUDLIB_SPUDGPURESOURCE_HPP

#include <cstdint>
#include <memory>
#include "spudgpuformats.hpp"

namespace spud::gpu {
    enum SPUDGPU_BUFFER_USAGE {
        SPUDGPU_BUFFER_USAGE_NONE = 0,
        SPUDGPU_BUFFER_USAGE_VERTEX = 1,
        SPUDGPU_BUFFER_USAGE_INDEX = 2,
        SPUDGPU_BUFFER_USAGE_UNIFORM = 3,
        SPUDGPU_BUFFER_USAGE_STORAGE = 4
    };

    struct gpu_buffer_desc {
        SPUDGPU_BUFFER_USAGE usage;
        uint64_t gpuAddressLocation;
        uint64_t size;
    };

    class gpu_buffer_view;

    class gpu_buffer {
    protected:
        [[nodiscard]] virtual std::shared_ptr<gpu_buffer_view> create_view(
            const uint64_t &offset,
            const uint64_t &stride,
            const uint64_t &size) const = 0;

    public:
        gpu_buffer() = default;

        virtual ~gpu_buffer() = default;

        [[nodiscard]] virtual gpu_buffer_desc get_desc() const = 0;
    };

    // An abstraction of a buffer view in Vulkan, Metal, and D3D12
    class gpu_buffer_view {
    public:
        gpu_buffer_view() = default;

        virtual ~gpu_buffer_view() = default;

        [[nodiscard]] virtual std::shared_ptr<gpu_buffer> get_buffer() const = 0;

        [[nodiscard]] virtual uint64_t get_offset() const = 0;

        [[nodiscard]] virtual uint64_t get_stride() const = 0;

        [[nodiscard]] virtual uint64_t get_size() const = 0;

        // D3D12_VERTEX_BUFFER_VIEW in D3D12
        [[nodiscard]] virtual uint64_t get_native_api_object() const = 0;
    };

    enum SPUDGPU_IMAGE_USAGE {
        SPUDGPU_IMAGE_USAGE_NONE = 0,
        SPUDGPU_IMAGE_USAGE_TEXTURE2D = 1,
        SPUDGPU_IMAGE_USAGE_TEXTURE3D = 2
    };

    struct gpu_image_desc {
        SPUDGPU_IMAGE_USAGE usage;
        uint64_t gpuAddressLocation;
        uint64_t width, height, depth;
        SPUDGPU_FORMAT format;
    };

    class gpu_image {
    public:
        gpu_image() = default;

        virtual ~gpu_image() = default;

        [[nodiscard]] virtual gpu_image_desc get_desc() const = 0;
    };

    class gpu_resource_pool {
    protected:
        gpu_resource_pool() = default;

        virtual ~gpu_resource_pool() = default;

    public:
        virtual void reserve(uint64_t bytes) = 0;

        virtual std::shared_ptr<gpu_buffer> allocate_buffer(const gpu_buffer_desc &desc) = 0;

        virtual std::shared_ptr<gpu_image> allocate_image(const gpu_image_desc &desc) = 0;

        virtual void free(const std::shared_ptr<gpu_buffer> &buffer) = 0;

        virtual void free(const std::shared_ptr<gpu_image> &image) = 0;
    };
}

#endif //SPUDLIB_SPUDGPURESOURCE_HPP
