#include "gpu/spudgpucontext.hpp"
#include "gpu/backends/vulkan/spudgpuvulkandef.hpp"
#include "gpu/backends/vulkan/spudgpuvulkancontext.hpp"

#include <iostream>

namespace spud::gpu {
    SPUDGPU_API g_gpu_api = static_cast<SPUDGPU_API>(0);
    std::vector<std::shared_ptr<gpu_device> > g_gpu_devices = std::vector<std::shared_ptr<gpu_device> >();

    SPUDGPU_API get_gpu_api() { return g_gpu_api; }

    bool init(
        SPUDGPU_API api,
        const std::string &applicationName,
        const uint32_t &applicationVersion,
        const std::string &engineName,
        const uint32_t &engineVersion) {
        if (api == SPUDGPU_API_VULKAN) {
            g_gpu_api = api;
            backends::vulkan::initialize_vulkan_impl(applicationName, applicationVersion, engineName, engineVersion);
            g_gpu_devices = backends::vulkan::initialize_devices_vulkan();
            return true;
        }
        if (api == SPUDGPU_API_METAL) {
            g_gpu_api = api;
            std::cout << "SPUDLIB-SPUDGPU : does not support Metal API yet!" << std::endl;
            return false;
        }
        return false;
    }

    void terminate() {
        if (g_gpu_api == SPUDGPU_API_VULKAN) {
            backends::vulkan::terminate_devices_vulkan();
            return;
        }
        if (g_gpu_api == SPUDGPU_API_METAL) {
            std::cout << "SPUDLIB-SPUDGPU : does not support Metal API yet!" << std::endl;
            return;
        }
    }

    const std::vector<std::shared_ptr<gpu_device> > get_gpu_devices() {
        return g_gpu_devices;
    }
}
