#include <vulkan/vulkan_core.h>
#include <vector>
#include <stdexcept>

#include "spudgpu.h"

namespace spud::gpu::backends::vulkan {
    struct spudgpu_instance_vulkan {
        VkInstance _instance;
    };

    spudgpu_instance initialize_vulkan_impl(
        const std::string &applicationName,
        const uint32_t &applicationVersion,
        const std::string &engineName,
        const uint32_t &engineVersion) {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        appInfo.apiVersion = VK_API_VERSION_1_3;
        appInfo.pApplicationName = applicationName.c_str();
        appInfo.applicationVersion = applicationVersion;
        appInfo.pEngineName = engineName.c_str();
        appInfo.engineVersion = engineVersion;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = 0;
        createInfo.ppEnabledExtensionNames = nullptr;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

        VkInstance instance = VK_NULL_HANDLE;
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan instance!");
        }

        auto *result = new spudgpu_instance_vulkan();
        result->_instance = instance;
        return reinterpret_cast<spudgpu_instance>(result);
    }

    struct spudgpu_device_vulkan {
        spudgpu_device_vulkan() = default;

        void _initialize_from_vk_physical_device(
            const spudgpu_instance_vulkan &instance,
            VkPhysicalDevice physicalDevice) {
            _instance = instance._instance;
            _physical_device = physicalDevice;

            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

            // Set params to create (1) command queue with type graphics.
            // TODO: Later we will implement more types of command queues.
            uint32_t queueCount = 1;
            VkDeviceQueueCreateInfo queueCreateInfo{};
            int graphicsQueueFamilyIndex = -1;
            {
                for (int i = 0; i < queueFamilies.size(); i++) {
                    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                        graphicsQueueFamilyIndex = i;
                        break;
                    }
                }

                float graphicsQueuePriority = 1.0f;

                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
                queueCreateInfo.queueCount = queueCount;
                queueCreateInfo.pQueuePriorities = &graphicsQueuePriority;
            }

            VkPhysicalDeviceFeatures deviceFeatures{};
            // Example: deviceFeatures.samplerAnisotropy = VK_TRUE;

            const std::vector<const char *> deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };

            VkDeviceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.pQueueCreateInfos = &queueCreateInfo;
            createInfo.queueCreateInfoCount = queueCount;
            createInfo.pEnabledFeatures = &deviceFeatures;
            createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
            createInfo.ppEnabledExtensionNames = deviceExtensions.data();

            if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &_logical_device) != VK_SUCCESS) {
                throw std::runtime_error("failed to create logical device!");
            }

            /*
            // Take that graphics queue and make a SpudGPU command queue from it.
            // TODO: Later we will implement more types of command queues.
            m_command_queues.resize(1);
            {
                VkQueue graphicsQueue = VK_NULL_HANDLE;
                vkGetDeviceQueue(m_device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
                auto spudCommandQueue = std::make_shared<command_queue_vulkan>(graphicsQueue);
                m_command_queues[0] = spudCommandQueue;
            }*/

            // Grab the properties and feature descriptors.
            {
                vkGetPhysicalDeviceProperties(_physical_device, &_properties);
                vkGetPhysicalDeviceFeatures(_physical_device, &_features);
            }

            /*
            // Create the SpudGPU resource pool object
            {
                std::shared_ptr<gpu_resource_pool_vulkan> spudGpuVulkanPool = std::make_shared<
                    gpu_resource_pool_vulkan>();
                m_resource_pool = std::static_pointer_cast<gpu_resource_pool_vulkan>(spudGpuVulkanPool);
            }*/
        };

        ~spudgpu_device_vulkan() {
            vkDestroyDevice(_logical_device, nullptr);
        }

        VkInstance _instance = VK_NULL_HANDLE;
        VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
        VkDevice _logical_device = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties _properties = {};
        VkPhysicalDeviceFeatures _features = {};
    };

    struct spudgpu_command_queue_vulkan {
        spudgpu_command_queue_vulkan(VkQueue queue) {}

        ~spudgpu_command_queue_vulkan() = default;
    };
}

extern "C" {
spudgpu_instance spudgpu_init(
    SPUDGPU_NATIVE_API native_api,
    const char *application_name,
    uint32_t application_version,
    const char *engine_name,
    uint32_t engine_version) {
    spudgpu_instance result = spud::gpu::backends::vulkan::initialize_vulkan_impl(
        application_name,
        application_version,
        engine_name,
        engine_version);
    return result;
}

void spudgpu_terminate(spudgpu_instance instance) {
    if (!instance) return;
    auto *vk_instance = reinterpret_cast<
        spud::gpu::backends::vulkan::spudgpu_instance_vulkan *>(instance);
    vkDestroyInstance(vk_instance->_instance, nullptr);
    delete vk_instance;
}

SPUDGPU_DEVICE_LIST spudgpu_enumerate_devices(spudgpu_instance instance) {
    if (!instance) return static_cast<SPUDGPU_DEVICE_LIST>(0);

    auto spudVulkanDevices = std::vector<spud::gpu::backends::vulkan::spudgpu_device_vulkan>();

    auto vkInstance = reinterpret_cast<spud::gpu::backends::vulkan::spudgpu_instance_vulkan *>(instance);

    uint32_t deviceCount = 0;
    {
        vkEnumeratePhysicalDevices(vkInstance->_instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(vkInstance->_instance, &deviceCount, physicalDevices.data());
        spudVulkanDevices.resize(deviceCount);

        for (size_t i = 0; i < deviceCount; i++)
            spudVulkanDevices[i]._initialize_from_vk_physical_device(*vkInstance, physicalDevices[i]);
    }

    SPUDGPU_DEVICE_LIST result = {};
    result.devices = reinterpret_cast<spudgpu_device *>(spudVulkanDevices.data());
    result.device_count = deviceCount;
    return result;
}

SPUDGPU_NATIVE_API spudgpu_get_native_gpu_api(spudgpu_instance instance) {
    if (!instance) return SPUDGPU_NATIVE_API_NONE;
    return SPUDGPU_NATIVE_API_VULKAN;
}

/*
std::vector<std::shared_ptr<gpu_device> > initialize_devices_vulkan() {
    std::vector<std::shared_ptr<gpu_device_vulkan> > spudVulkanDevices = std::vector<std::shared_ptr<
        gpu_device_vulkan> >();

    uint32_t deviceCount = 0;
    {
        vkEnumeratePhysicalDevices(g_pVulkanInstance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(g_pVulkanInstance, &deviceCount, physicalDevices.data());
        spudVulkanDevices.resize(deviceCount);

        for (size_t i = 0; i < deviceCount; i++) {
            auto spudVulkanDevice = std::make_shared<gpu_device_vulkan>();
            spudVulkanDevice->initialize_from_physical_device_impl(physicalDevices[i]);
            spudVulkanDevices[i] = spudVulkanDevice;
        }
    }

    // Convert all to base classes for return.
    std::vector<std::shared_ptr<gpu_device> > spudGpuDevices(spudVulkanDevices.size());
    for (size_t i = 0; i < spudVulkanDevices.size(); i++)
        spudGpuDevices[i] = spudVulkanDevices[i];

    return spudGpuDevices;
}

void terminate_devices_vulkan() {
}
}*/
}
