//
// Created by nathanmoore on 3/27/26.
//

#ifndef SPUDLIB_SPUDGPUCONTEXT_HPP
#define SPUDLIB_SPUDGPUCONTEXT_HPP

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>


/*
namespace spud::gpu {
    class command_buffer;
    class command_allocator;
    class gpu_resource_pool;

    enum SPUDGPU_COMMAND_QUEUE_TYPE {
        SPUDGPU_COMMAND_QUEUE_TYPE_NONE = 0,
        SPUDGPU_COMMAND_QUEUE_TYPE_GRAPHICS = 1,
        SPUDGPU_COMMAND_QUEUE_TYPE_COMPUTE = 2,
        SPUDGPU_COMMAND_QUEUE_TYPE_TRANSFER = 3,
        SPUDGPU_COMMAND_QUEUE_TYPE_FENCE = 4,
        SPUDGPU_COMMAND_QUEUE_TYPE_EVENT = 5,
        SPUDGPU_COMMAND_QUEUE_TYPE_MAX = 6,
    };

    class command_queue {
    public:
        command_queue() = default;

        virtual ~command_queue() = default;

        [[nodiscard]] virtual SPUDGPU_COMMAND_QUEUE_TYPE get_type() const = 0;

        virtual void execute_commands(
            const uint32_t &cmdBufferCount,
            const std::shared_ptr<command_buffer> *pCmdBuffers) = 0;
    };

    // API used to communicate with the graphics card.
    enum SPUDGPU_API {
        // Placeholder for an invalid graphics API.
        SPUDGPU_API_NONE = 0,

        // Vulkan graphics API (Linux, Windows, Android)
        SPUDGPU_API_VULKAN = 1,

        // Metal graphics API (Apple Silicon, macOS, iPadOS)
        SPUDGPU_API_METAL = 2
    };

    // Return the graphics API currently used.
    SPUDGPU_API get_gpu_api();

    // Initialize the entire SpudGPU system.
    bool init(
        SPUDGPU_API api,
        const std::string &applicationName,
        const uint32_t &applicationVersion,
        const std::string &engineName,
        const uint32_t &engineVersion);

    // Terminate the entire SpudGPU system.
    void terminate();

    // Set flags hinting the usage of the graphics device.
    enum SPUDGPU_DEVICE_USAGE {
        SPUDGPU_DEVICE_USAGE_NONE = 0,
        SPUDGPU_DEVICE_USAGE_RENDER = 1,
        SPUDGPU_DEVICE_USAGE_BLOCKCHAIN = 2
    };

    // GPU device class.
    class gpu_device {
    public:
        gpu_device() = default;

        virtual ~gpu_device() = default;

        // @return The default resource pool object used to create resources in the graphics card.
        [[nodiscard]] virtual std::shared_ptr<gpu_resource_pool> get_default_resource_pool() const = 0;

        // @return A new resource pool linked to this device.
        [[nodiscard]] virtual std::shared_ptr<gpu_resource_pool> create_resource_pool() const = 0;

        [[nodiscard]] virtual std::shared_ptr<command_allocator> create_command_allocator() const = 0;

        // Creates a command queue object based on type.
        [[nodiscard]] virtual std::shared_ptr<command_queue> get_command_queue(SPUDGPU_COMMAND_QUEUE_TYPE type) const = 0;

        virtual void destroy() = 0;

        std::string name;
    };

    // Get a list of all the GPU devices on this computer.
    const std::vector<std::shared_ptr<gpu_device> > get_gpu_devices();
}*/

#endif //SPUDLIB_SPUDGPUCONTEXT_HPP
