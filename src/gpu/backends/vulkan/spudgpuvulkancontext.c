
#if SPUDGPU_COMPILE_VULKAN_API

#include <stdio.h>

#include "spudgpuvulkan.h"
#include "stdlib.h"
#include "stdbool.h"

VkFormat convert_spud_to_vulkan_format(SPUDGPU_FORMAT format) {
    switch (format) {
        // ── 128-bit ──────────────────────────────────────────────────────────
        case SPUDGPU_FORMAT_R32G32B32A32_TYPELESS: return VK_FORMAT_R32G32B32A32_SFLOAT;
        // no typeless in Vulkan; treat as float
        case SPUDGPU_FORMAT_R32G32B32A32_FLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
        case SPUDGPU_FORMAT_R32G32B32A32_UINT: return VK_FORMAT_R32G32B32A32_UINT;
        case SPUDGPU_FORMAT_R32G32B32A32_SINT: return VK_FORMAT_R32G32B32A32_SINT;

        // ── 96-bit ───────────────────────────────────────────────────────────
        case SPUDGPU_FORMAT_R32G32B32_TYPELESS: return VK_FORMAT_R32G32B32_SFLOAT;
        case SPUDGPU_FORMAT_R32G32B32_FLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
        case SPUDGPU_FORMAT_R32G32B32_UINT: return VK_FORMAT_R32G32B32_UINT;
        case SPUDGPU_FORMAT_R32G32B32_SINT: return VK_FORMAT_R32G32B32_SINT;

        // ── 64-bit ───────────────────────────────────────────────────────────
        case SPUDGPU_FORMAT_R16G16B16A16_TYPELESS: return VK_FORMAT_R16G16B16A16_SFLOAT;
        case SPUDGPU_FORMAT_R16G16B16A16_FLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;
        case SPUDGPU_FORMAT_R16G16B16A16_UNORM: return VK_FORMAT_R16G16B16A16_UNORM;
        case SPUDGPU_FORMAT_R16G16B16A16_UINT: return VK_FORMAT_R16G16B16A16_UINT;
        case SPUDGPU_FORMAT_R16G16B16A16_SNORM: return VK_FORMAT_R16G16B16A16_SNORM;
        case SPUDGPU_FORMAT_R16G16B16A16_SINT: return VK_FORMAT_R16G16B16A16_SINT;

        case SPUDGPU_FORMAT_R32G32_TYPELESS: return VK_FORMAT_R32G32_SFLOAT;
        case SPUDGPU_FORMAT_R32G32_FLOAT: return VK_FORMAT_R32G32_SFLOAT;
        case SPUDGPU_FORMAT_R32G32_UINT: return VK_FORMAT_R32G32_UINT;
        case SPUDGPU_FORMAT_R32G32_SINT: return VK_FORMAT_R32G32_SINT;

        // ── Depth/stencil packed (64-bit) ────────────────────────────────────
        case SPUDGPU_FORMAT_R32G8X24_TYPELESS: return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case SPUDGPU_FORMAT_D32_FLOAT_S8X24_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case SPUDGPU_FORMAT_R32_FLOAT_X8X24_TYPELESS: return VK_FORMAT_D32_SFLOAT_S8_UINT;
        // no direct equivalent; sampled depth view
        case SPUDGPU_FORMAT_X32_TYPELESS_G8X24_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT; // sampled stencil view

        // ── 32-bit ───────────────────────────────────────────────────────────
        case SPUDGPU_FORMAT_R10G10B10A2_TYPELESS: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        case SPUDGPU_FORMAT_R10G10B10A2_UNORM: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        case SPUDGPU_FORMAT_R10G10B10A2_UINT: return VK_FORMAT_A2B10G10R10_UINT_PACK32;
        case SPUDGPU_FORMAT_R11G11B10_FLOAT: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;

        case SPUDGPU_FORMAT_R8G8B8A8_TYPELESS: return VK_FORMAT_R8G8B8A8_UNORM;
        case SPUDGPU_FORMAT_R8G8B8A8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
        case SPUDGPU_FORMAT_R8G8B8A8_UNORM_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
        case SPUDGPU_FORMAT_R8G8B8A8_UINT: return VK_FORMAT_R8G8B8A8_UINT;
        case SPUDGPU_FORMAT_R8G8B8A8_SNORM: return VK_FORMAT_R8G8B8A8_SNORM;
        case SPUDGPU_FORMAT_R8G8B8A8_SINT: return VK_FORMAT_R8G8B8A8_SINT;

        case SPUDGPU_FORMAT_R16G16_TYPELESS: return VK_FORMAT_R16G16_SFLOAT;
        case SPUDGPU_FORMAT_R16G16_FLOAT: return VK_FORMAT_R16G16_SFLOAT;
        case SPUDGPU_FORMAT_R16G16_UNORM: return VK_FORMAT_R16G16_UNORM;
        case SPUDGPU_FORMAT_R16G16_UINT: return VK_FORMAT_R16G16_UINT;
        case SPUDGPU_FORMAT_R16G16_SNORM: return VK_FORMAT_R16G16_SNORM;
        case SPUDGPU_FORMAT_R16G16_SINT: return VK_FORMAT_R16G16_SINT;

        case SPUDGPU_FORMAT_R32_TYPELESS: return VK_FORMAT_R32_SFLOAT;
        case SPUDGPU_FORMAT_D32_FLOAT: return VK_FORMAT_D32_SFLOAT;
        case SPUDGPU_FORMAT_R32_FLOAT: return VK_FORMAT_R32_SFLOAT;
        case SPUDGPU_FORMAT_R32_UINT: return VK_FORMAT_R32_UINT;
        case SPUDGPU_FORMAT_R32_SINT: return VK_FORMAT_R32_SINT;

        // ── Depth/stencil packed (32-bit) ────────────────────────────────────
        case SPUDGPU_FORMAT_R24G8_TYPELESS: return VK_FORMAT_D24_UNORM_S8_UINT;
        case SPUDGPU_FORMAT_D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
        case SPUDGPU_FORMAT_R24_UNORM_X8_TYPELESS: return VK_FORMAT_D24_UNORM_S8_UINT;
        case SPUDGPU_FORMAT_X24_TYPELESS_G8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;

        // ── 16-bit ───────────────────────────────────────────────────────────
        case SPUDGPU_FORMAT_R8G8_TYPELESS: return VK_FORMAT_R8G8_UNORM;
        case SPUDGPU_FORMAT_R8G8_UNORM: return VK_FORMAT_R8G8_UNORM;
        case SPUDGPU_FORMAT_R8G8_UINT: return VK_FORMAT_R8G8_UINT;
        case SPUDGPU_FORMAT_R8G8_SNORM: return VK_FORMAT_R8G8_SNORM;
        case SPUDGPU_FORMAT_R8G8_SINT: return VK_FORMAT_R8G8_SINT;

        case SPUDGPU_FORMAT_R16_TYPELESS: return VK_FORMAT_R16_SFLOAT;
        case SPUDGPU_FORMAT_R16_FLOAT: return VK_FORMAT_R16_SFLOAT;
        case SPUDGPU_FORMAT_D16_UNORM: return VK_FORMAT_D16_UNORM;
        case SPUDGPU_FORMAT_R16_UNORM: return VK_FORMAT_R16_UNORM;
        case SPUDGPU_FORMAT_R16_UINT: return VK_FORMAT_R16_UINT;
        case SPUDGPU_FORMAT_R16_SNORM: return VK_FORMAT_R16_SNORM;
        case SPUDGPU_FORMAT_R16_SINT: return VK_FORMAT_R16_SINT;

        // ── 8-bit ────────────────────────────────────────────────────────────
        case SPUDGPU_FORMAT_R8_TYPELESS: return VK_FORMAT_R8_UNORM;
        case SPUDGPU_FORMAT_R8_UNORM: return VK_FORMAT_R8_UNORM;
        case SPUDGPU_FORMAT_R8_UINT: return VK_FORMAT_R8_UINT;
        case SPUDGPU_FORMAT_R8_SNORM: return VK_FORMAT_R8_SNORM;
        case SPUDGPU_FORMAT_R8_SINT: return VK_FORMAT_R8_SINT;
        case SPUDGPU_FORMAT_A8_UNORM: return VK_FORMAT_R8_UNORM;
        // Vulkan has no dedicated A8; use R8 and swizzle in the image view

        // ── Exotic / packed ──────────────────────────────────────────────────
        case SPUDGPU_FORMAT_R1_UNORM: return VK_FORMAT_UNDEFINED; // no Vulkan equivalent
        case SPUDGPU_FORMAT_R9G9B9E5_SHAREDEXP: return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
        case SPUDGPU_FORMAT_R8G8_B8G8_UNORM: return VK_FORMAT_B8G8R8G8_422_UNORM; // DXGI packed 4:2:2
        case SPUDGPU_FORMAT_G8R8_G8B8_UNORM: return VK_FORMAT_G8B8G8R8_422_UNORM;

        // ── BGR / swapchain formats ───────────────────────────────────────────
        case SPUDGPU_FORMAT_B5G6R5_UNORM: return VK_FORMAT_B5G6R5_UNORM_PACK16;
        case SPUDGPU_FORMAT_B5G5R5A1_UNORM: return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
        case SPUDGPU_FORMAT_B8G8R8A8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM;
        case SPUDGPU_FORMAT_B8G8R8X8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM; // X = ignored alpha; same bits
        case SPUDGPU_FORMAT_B8G8R8A8_TYPELESS: return VK_FORMAT_B8G8R8A8_UNORM;
        case SPUDGPU_FORMAT_B8G8R8A8_UNORM_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
        case SPUDGPU_FORMAT_B8G8R8X8_TYPELESS: return VK_FORMAT_B8G8R8A8_UNORM;
        case SPUDGPU_FORMAT_B8G8R8X8_UNORM_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
        case SPUDGPU_FORMAT_B4G4R4A4_UNORM: return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
        case SPUDGPU_FORMAT_R10G10B10_XR_BIAS_A2_UNORM: return VK_FORMAT_UNDEFINED;
        // XR bias is display-only; no Vulkan match

        // ── BC block compression ──────────────────────────────────────────────
        case SPUDGPU_FORMAT_BC1_TYPELESS: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case SPUDGPU_FORMAT_BC1_UNORM: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case SPUDGPU_FORMAT_BC1_UNORM_SRGB: return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
        case SPUDGPU_FORMAT_BC2_TYPELESS: return VK_FORMAT_BC2_UNORM_BLOCK;
        case SPUDGPU_FORMAT_BC2_UNORM: return VK_FORMAT_BC2_UNORM_BLOCK;
        case SPUDGPU_FORMAT_BC2_UNORM_SRGB: return VK_FORMAT_BC2_SRGB_BLOCK;
        case SPUDGPU_FORMAT_BC3_TYPELESS: return VK_FORMAT_BC3_UNORM_BLOCK;
        case SPUDGPU_FORMAT_BC3_UNORM: return VK_FORMAT_BC3_UNORM_BLOCK;
        case SPUDGPU_FORMAT_BC3_UNORM_SRGB: return VK_FORMAT_BC3_SRGB_BLOCK;
        case SPUDGPU_FORMAT_BC4_TYPELESS: return VK_FORMAT_BC4_UNORM_BLOCK;
        case SPUDGPU_FORMAT_BC4_UNORM: return VK_FORMAT_BC4_UNORM_BLOCK;
        case SPUDGPU_FORMAT_BC4_SNORM: return VK_FORMAT_BC4_SNORM_BLOCK;
        case SPUDGPU_FORMAT_BC5_TYPELESS: return VK_FORMAT_BC5_UNORM_BLOCK;
        case SPUDGPU_FORMAT_BC5_UNORM: return VK_FORMAT_BC5_UNORM_BLOCK;
        case SPUDGPU_FORMAT_BC5_SNORM: return VK_FORMAT_BC5_SNORM_BLOCK;
        case SPUDGPU_FORMAT_BC6H_TYPELESS: return VK_FORMAT_BC6H_UFLOAT_BLOCK;
        case SPUDGPU_FORMAT_BC6H_UF16: return VK_FORMAT_BC6H_UFLOAT_BLOCK;
        case SPUDGPU_FORMAT_BC6H_SF16: return VK_FORMAT_BC6H_SFLOAT_BLOCK;
        case SPUDGPU_FORMAT_BC7_TYPELESS: return VK_FORMAT_BC7_UNORM_BLOCK;
        case SPUDGPU_FORMAT_BC7_UNORM: return VK_FORMAT_BC7_UNORM_BLOCK;
        case SPUDGPU_FORMAT_BC7_UNORM_SRGB: return VK_FORMAT_BC7_SRGB_BLOCK;

        // ── YUV / video formats ───────────────────────────────────────────────
        case SPUDGPU_FORMAT_NV12: return VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
        case SPUDGPU_FORMAT_P010: return VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16;
        case SPUDGPU_FORMAT_P016: return VK_FORMAT_G16_B16R16_2PLANE_420_UNORM;
        case SPUDGPU_FORMAT_YUY2: return VK_FORMAT_G8B8G8R8_422_UNORM;
        case SPUDGPU_FORMAT_420_OPAQUE: return VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM;
        // Y210/Y216/Y410/Y416/NV11/AYUV/AI44/IA44/P8/A8P8/P208/V208/V408
        // have no standard Vulkan equivalents; fall through to UNDEFINED

        default: return VK_FORMAT_UNDEFINED;
    }
}


static VkDevice spudgpuvulkan___initialize_vk_logical_device_internal(
    spudgpu_device_vulkan *device) {
    VkDevice result = VK_NULL_HANDLE;

    VkPhysicalDevice physicalDevice = device->_physical_device_vk;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);

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

        device->_graphics_queue_family_index = (uint32_t) graphicsQueueFamilyIndex;

        float graphicsQueuePriority = 1.0f;

        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext = NULL;
        queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        queueCreateInfo.queueCount = queueCount;
        queueCreateInfo.pQueuePriorities = &graphicsQueuePriority;
    }

    VkPhysicalDeviceFeatures deviceFeatures = {0};

    VkPhysicalDeviceVulkan13Features vk13Features = {0};
    vk13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    vk13Features.dynamicRendering = VK_TRUE;
    vk13Features.synchronization2 = VK_TRUE;

    const char *deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    const uint32_t deviceExtensionCount = sizeof(deviceExtensions) / sizeof(deviceExtensions[0]);

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &vk13Features;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = queueCount;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = deviceExtensionCount;
    createInfo.ppEnabledExtensionNames = deviceExtensions;

    VkResult r = vkCreateDevice(physicalDevice, &createInfo, NULL, &result);
    if (r != VK_SUCCESS) {
        //throw std::runtime_error("failed to create logical device!");
        printf("failed to create logical device\n");
        return NULL;
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

static void spudgpuvulkan__determine_vulkan_extensions(VkInstanceCreateInfo *pOutput) {
    if (!pOutput) return;
#if defined(_WIN32)
    static const char *extensions[] = {"VK_KHR_surface", "VK_KHR_win32_surface"};
#elif defined(__linux__)
#if defined(SPUDGPU_PLATFORM_XLIB)
    // Pick one based on your display server target:
    static const char *extensions[] = {"VK_KHR_surface", "VK_KHR_xlib_surface"};
#elif defined(SPUDGPU_PLATFORM_WAYLAND)
    static const char *extensions[] = {"VK_KHR_surface", "VK_KHR_wayland_surface"};
#else
#error "In Linux, you either need XLIB or WAYLAND"
#endif
#endif
    pOutput->enabledExtensionCount = 2;
    pOutput->ppEnabledExtensionNames = extensions;
}

#if __cplusplus
extern "C" {



#endif


SPUDRESULT spudgpu_create_instance(
    SPUDGPU_NATIVE_API native_api,
    const char *application_name,
    uint32_t application_version,
    const char *engine_name,
    uint32_t engine_version,
    spudgpu_instance *out_instance) {
    if (native_api == SPUDGPU_NATIVE_API_NONE) return SPUDRESULT_INVALID_API;
    if (!out_instance) return SPUD_SUCCESS;
    spudgpu_instance_vulkan result = {0};

    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.apiVersion = VK_API_VERSION_1_3;
    appInfo.pApplicationName = application_name;
    appInfo.applicationVersion = application_version;
    appInfo.pEngineName = engine_name;
    appInfo.engineVersion = engine_version;

    VkInstanceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;
    spudgpuvulkan__determine_vulkan_extensions(&createInfo);

    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = NULL;

    if (vkCreateInstance(&createInfo, NULL, &result._instance_vk) != VK_SUCCESS) {
        //throw std::runtime_error("failed to create vulkan instance!");
        return SPUDRESULT_API_SPECIFIC_FAILURE;
    }

    // If successful, return a memcpy'ed heap pointer of the result
    spudgpu_instance_vulkan *pResult = malloc(sizeof(spudgpu_instance_vulkan));
    memcpy(pResult, &result, sizeof(spudgpu_instance_vulkan));
    *out_instance = pResult;
    return SPUD_SUCCESS;
}

SPUDRESULT spudgpu_destroy_instance(spudgpu_instance instance) {
    if (!instance) return SPUD_SUCCESS;
    vkDestroyInstance(instance->_instance_vk, NULL);
    free(instance);
    return SPUD_SUCCESS;
}

SPUDRESULT spudgpu_enumerate_devices(
    spudgpu_instance instance,
    spudgpu_device **ppOutputDevices,
    uint32_t *pOutputDevicesCount) {
    if (!instance) return SPUDRESULT_GPU_INVALID_INSTANCE;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance->_instance_vk, &deviceCount, NULL);
    if (deviceCount == 0) {
        //throw std::runtime_error("failed to find GPUs with Vulkan support!");
        return SPUDRESULT_GPU_DEVICE_ENUMERATION_FAILURE;
    }
    VkPhysicalDevice physicalDevices[deviceCount];
    vkEnumeratePhysicalDevices(instance->_instance_vk, &deviceCount, physicalDevices);
    //
    //spudVulkanDevices = (spudgpu_device_vulkan *) malloc(sizeof(spudgpu_device_vulkan) * deviceCount);
    //spudVulkanDevices.resize(deviceCount);

    // Finally initialize the Vulkan logical devices and gather them.
    *ppOutputDevices = malloc(sizeof(spudgpu_device) * deviceCount);
    *pOutputDevicesCount = deviceCount;

    // Instance needs to keep track for memory management.
    instance->_devices_pointer_array = malloc(sizeof(uint64_t) * deviceCount);
    instance->_devices_count = deviceCount;

    for (size_t i = 0; i < deviceCount; i++) {
        spudgpu_device_vulkan *pDeviceVulkan = malloc(sizeof(spudgpu_device_vulkan));
        pDeviceVulkan->_instance = *instance;
        pDeviceVulkan->_physical_device_vk = physicalDevices[i];
        pDeviceVulkan->_logical_device_vk = spudgpuvulkan___initialize_vk_logical_device_internal(pDeviceVulkan);

        // Grab the properties and feature descriptors.
        vkGetPhysicalDeviceProperties(pDeviceVulkan->_physical_device_vk, &pDeviceVulkan->_properties_vk);
        vkGetPhysicalDeviceFeatures(pDeviceVulkan->_physical_device_vk, &pDeviceVulkan->_features_vk);

        (*ppOutputDevices)[i] = (spudgpu_device) pDeviceVulkan;

        // Instance needs to keep track for memory management.
        instance->_devices_pointer_array[i] = (uint64_t) (*ppOutputDevices)[i];
    }

    return SPUD_SUCCESS;
}

SPUDGPU_NATIVE_API spudgpu_get_native_gpu_api(spudgpu_instance instance) {
    return instance ? SPUDGPU_NATIVE_API_VULKAN : SPUDGPU_NATIVE_API_NONE;
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
        vkEnumeratePhysicalDevices(g_pVulkanInstance, &deviceCount, NULL);
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

#endif //SPUDGPU_COMPILE_VULKAN_API

