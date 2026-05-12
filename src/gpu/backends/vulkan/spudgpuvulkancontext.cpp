#include "gpu/backends/vulkan/spudgpuvulkandef.hpp"
#include "gpu/backends/vulkan/spudgpuvulkancontext.hpp"

namespace spud::gpu::backends::vulkan {
    VkInstance g_pVulkanInstance = VK_NULL_HANDLE;

    command_queue_vulkan::command_queue_vulkan(VkQueue queue) : m_queue(nullptr), m_type() {
    }

    command_queue_vulkan::~command_queue_vulkan() = default;

    void command_queue_vulkan::execute_commands(
        const uint32_t &cmdBufferCount,
        const std::shared_ptr<command_buffer> *pCmdBuffers) {
    }

    gpu_device_vulkan::gpu_device_vulkan() : m_resource_pool(nullptr),
                                             m_physicalDevice(nullptr),
                                             m_device(nullptr) {
    }

    gpu_device_vulkan::~gpu_device_vulkan()
    = default;

    void gpu_device_vulkan::initialize_from_physical_device_impl(VkPhysicalDevice physicalDevice) {
        m_physicalDevice = physicalDevice;

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

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        // Take that graphics queue and make a SpudGPU command queue from it.
        // TODO: Later we will implement more types of command queues.
        m_command_queues.resize(1);
        {
            VkQueue graphicsQueue = VK_NULL_HANDLE;
            vkGetDeviceQueue(m_device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
            auto spudCommandQueue = std::make_shared<command_queue_vulkan>(graphicsQueue);
            m_command_queues[0] = spudCommandQueue;
        }

        // Grab the properties and feature descriptors.
        {
            vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);
            vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_features);
        }
    }

    std::shared_ptr<command_allocator> gpu_device_vulkan::create_command_allocator() const {
        return nullptr;
    }

    std::shared_ptr<command_queue> gpu_device_vulkan::get_command_queue(SPUDGPU_COMMAND_QUEUE_TYPE type) const {
        for (uint32_t i = 0; i < m_command_queues.size(); i++) {
            auto result = m_command_queues[i];
            if (result->get_type() == SPUDGPU_COMMAND_QUEUE_TYPE_GRAPHICS)
                return result;
        }
        return nullptr;
    }

    void gpu_device_vulkan::destroy() {
        vkDestroyDevice(m_device, nullptr);
    }

    void initialize_vulkan_impl(
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

        if (vkCreateInstance(&createInfo, nullptr, &g_pVulkanInstance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan instance!");
        }
    }

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
}
