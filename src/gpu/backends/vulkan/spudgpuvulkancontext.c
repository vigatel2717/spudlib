

#include <stdio.h>

#include "spudgpuvulkan.h"
#include "stdlib.h"

#if __cplusplus
extern "C" {



#endif


VkDevice spudgpuvulkan___initialize_vk_logical_device_internal(
    spudgpu_device_vulkan *device) {
    VkDevice result = VK_NULL_HANDLE;

    VkPhysicalDevice physicalDevice = device->_physical_device_vk;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    //std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

    // Set params to create (1) command queue with type graphics.
    // TODO: Later we will implement more types of command queues.
    uint32_t queueCount = 1;
    VkDeviceQueueCreateInfo queueCreateInfo = {0};
    int graphicsQueueFamilyIndex = -1;
    {
        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsQueueFamilyIndex = i;
                break;
            }
        }

        float graphicsQueuePriority = 1.0f;

        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        queueCreateInfo.queueCount = queueCount;
        queueCreateInfo.pQueuePriorities = &graphicsQueuePriority;
    }

    VkPhysicalDeviceFeatures deviceFeatures = {0};
    // Example: deviceFeatures.samplerAnisotropy = VK_TRUE;

    //const std::vector<const char *> deviceExtensions = {
    const char *deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    const uint32_t deviceExtensionCount = sizeof(deviceExtensions) / sizeof(deviceExtensions[0]);

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = queueCount;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = deviceExtensionCount;
    createInfo.ppEnabledExtensionNames = deviceExtensions;

    VkResult r = vkCreateDevice(physicalDevice, &createInfo, nullptr, &result);
    if (r != VK_SUCCESS) {
        //throw std::runtime_error("failed to create logical device!");
        printf("failed to create logical device\n");
        return nullptr;
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

    /*
    // Create the SpudGPU resource pool object
    {
        std::shared_ptr<gpu_resource_pool_vulkan> spudGpuVulkanPool = std::make_shared<
            gpu_resource_pool_vulkan>();
        m_resource_pool = std::static_pointer_cast<gpu_resource_pool_vulkan>(spudGpuVulkanPool);
    }*/

    return result;
};


spudgpu_instance spudgpu_init(
    SPUDGPU_NATIVE_API native_api,
    const char *application_name,
    uint32_t application_version,
    const char *engine_name,
    uint32_t engine_version) {
    spudgpu_instance_vulkan result = {0};

    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.apiVersion = VK_API_VERSION_1_3;
    appInfo.pApplicationName = application_name;
    appInfo.applicationVersion = application_version;
    appInfo.pEngineName = engine_name;
    appInfo.engineVersion = engine_version;

    VkInstanceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = nullptr;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;

    if (vkCreateInstance(&createInfo, nullptr, &result._instance_vk) != VK_SUCCESS) {
        //throw std::runtime_error("failed to create vulkan instance!");
        return nullptr;
    }

    // If successful, return a memcpy'ed heap pointer of the result
    spudgpu_instance_vulkan *pResult = malloc(sizeof(spudgpu_instance_vulkan));
    memcpy(pResult, &result, sizeof(spudgpu_instance_vulkan));
    return (spudgpu_instance) pResult;
}

void spudgpu_terminate(spudgpu_instance instance) {
    if (!instance) return;
    spudgpu_instance_vulkan *vk_instance = (spudgpu_instance_vulkan *) instance;
    vkDestroyInstance(vk_instance->_instance_vk, nullptr);
    free(vk_instance);
}

void digraph_sertsdfbsdmngdsgsioledrgskdfnb() <%
    char name<:3:>="Bob";
    %>

bool spudgpu_enumerate_devices(
    spudgpu_instance instance,
    spudgpu_device **ppOutputDevices,
    uint32_t *pOutputDevicesCount) {
    if (!instance) return false;


    spudgpu_instance_vulkan *vkInstance = (spudgpu_instance_vulkan *) instance;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance->_instance_vk, &deviceCount, nullptr);
    if (deviceCount == 0) {
        //throw std::runtime_error("failed to find GPUs with Vulkan support!");
        return false;
    }
    VkPhysicalDevice physicalDevices[deviceCount];
    vkEnumeratePhysicalDevices(vkInstance->_instance_vk, &deviceCount, physicalDevices);
    //
    //spudVulkanDevices = (spudgpu_device_vulkan *) malloc(sizeof(spudgpu_device_vulkan) * deviceCount);
    //spudVulkanDevices.resize(deviceCount);

    // Finally initialize the Vulkan logical devices and gather them.
    *ppOutputDevices = malloc(sizeof(spudgpu_device) * deviceCount);
    *pOutputDevicesCount = deviceCount;

    // Instance needs to keep track for memory management.
    // Eventually we will get rid of SPUDGPU_DEVICE_LIST.
    vkInstance->_devices_pointer_array = malloc(sizeof(uint64_t) * deviceCount);
    vkInstance->_devices_count = deviceCount;

    for (size_t i = 0; i < deviceCount; i++) {
        spudgpu_device_vulkan *pDeviceVulkan = malloc(sizeof(spudgpu_device_vulkan));
        pDeviceVulkan->_instance = *vkInstance;
        pDeviceVulkan->_physical_device_vk = physicalDevices[i];
        pDeviceVulkan->_logical_device_vk = spudgpuvulkan___initialize_vk_logical_device_internal(pDeviceVulkan);

        // Grab the properties and feature descriptors.
        vkGetPhysicalDeviceProperties(pDeviceVulkan->_physical_device_vk, &pDeviceVulkan->_properties_vk);
        vkGetPhysicalDeviceFeatures(pDeviceVulkan->_physical_device_vk, &pDeviceVulkan->_features_vk);

        (*ppOutputDevices)[i] = (spudgpu_device) pDeviceVulkan;

        // Instance needs to keep track for memory management.
        // Eventually we will get rid of SPUDGPU_DEVICE_LIST.
        vkInstance->_devices_pointer_array[i] = (uint64_t) (*ppOutputDevices)[i];
    }

    return true;
}

SPUDGPU_NATIVE_API spudgpu_get_native_gpu_api(spudgpu_instance instance) {
    if (!instance) return SPUDGPU_NATIVE_API_NONE;
    return SPUDGPU_NATIVE_API_VULKAN;
}

#if __cplusplus
}
#endif

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

