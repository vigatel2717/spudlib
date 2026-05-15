#include "gpu/backends/vulkan/spudgpuvulkanresource.hpp"
#include "gpu/backends/vulkan/spudgpuvulkancontext.hpp"

#if SPUDGPU_COMPILE_VULKAN_API

#include "spudgpu.hpp"
#include "gpu/backends/vulkan/spudgpuvulkancontext.hpp"
#include "gpu/backends/vulkan/spudgpuvulkandef.hpp"

namespace spud::gpu {
    uint32_t ___find_memory_type_internal(
        VkPhysicalDevice physicalDevice,
        uint32_t typeFilter,
        VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        throw std::runtime_error("SpudGPU Vulkan: ailed to find suitable memory type!");
    }

    void ___buffer_usage_flags_internal(
        const uint32_t &__spud_gpu_buffer_usage,
        VkBufferUsageFlags &output) {
        if (__spud_gpu_buffer_usage & static_cast<uint32_t>(GPU_BUFFER_USAGE::GPU_BUFFER_USAGE_VERTEX))
            output |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if (__spud_gpu_buffer_usage & static_cast<uint32_t>(GPU_BUFFER_USAGE::GPU_BUFFER_USAGE_INDEX))
            output |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (__spud_gpu_buffer_usage & static_cast<uint32_t>(GPU_BUFFER_USAGE::GPU_BUFFER_USAGE_UNIFORM))
            output |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if (__spud_gpu_buffer_usage & static_cast<uint32_t>(GPU_BUFFER_USAGE::GPU_BUFFER_USAGE_STORAGE))
            output |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }

    void ___image_usage_flags_internal(
        const uint32_t &__spud_gpu_image_usage,
        VkImageUsageFlags &output) {
    }

    void ___memory_property_flags_internal(
        const uint32_t &__spud_gpu_memory_flags,
        VkMemoryPropertyFlags &output) {
        if (__spud_gpu_memory_flags & static_cast<uint32_t>(GPU_MEMORY_FLAGS::GPU_MEMORY_FLAGS_DEVICE_LOCAL))
            output |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        if (__spud_gpu_memory_flags & static_cast<uint32_t>(GPU_MEMORY_FLAGS::GPU_MEMORY_FLAGS_HOST_VISIBLE))
            output |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        if (__spud_gpu_memory_flags & static_cast<uint32_t>(GPU_MEMORY_FLAGS::GPU_MEMORY_FLAGS_HOST_COHERENT))
            output |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        if (__spud_gpu_memory_flags & static_cast<uint32_t>(GPU_MEMORY_FLAGS::GPU_MEMORY_FLAGS_HOST_CACHED))
            output |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    }

    gpu_buffer create_buffer(gpu_device device, const gpu_buffer_desc &desc) {
        backends::vulkan::gpu_buffer_vulkan *result = new backends::vulkan::gpu_buffer_vulkan();
        result->desc = desc;
        result->pDevice = reinterpret_cast<backends::vulkan::gpu_device_vulkan *>(device);
        result->buffer = VK_NULL_HANDLE;
        result->memory = VK_NULL_HANDLE;

        // Validate desc
        {
            /*if (desc.gpu_address_location == 0)
                throw std::runtime_error(
                    "gpu_buffer_vulkan::gpu_buffer_vulkan: invalid GPU address location: " + std::to_string(
                        desc.gpu_address_location));*/

            if (desc.size == 0)
                throw std::runtime_error(
                    "SpudGPU create_buffer: invalid size: " + std::to_string(desc.size));

            if (desc.usage == 0)
                throw std::runtime_error(
                    "SpudGPU create_buffer: invalid usage: GPU_BUFFER_USAGE_NONE");
        }

        // Get native Vulkan device handles
        auto vk_device = result->pDevice->logical_device;
        auto vk_physical_device = result->pDevice->physical_device;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = desc.size;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Covert SPUDGPU_BUFFER_USAGE to native Vulkan usage
        ___buffer_usage_flags_internal(desc.usage, bufferInfo.usage);

        // Create buffer
        if (vkCreateBuffer(vk_device, &bufferInfo, nullptr, &result->buffer) != VK_SUCCESS) {
            throw std::runtime_error("SpudGPU Vulkan: failed to create buffer!");
        }

        // Allocate memory
        {
            // TODO: Vulkan Memory Property Flags
            VkMemoryPropertyFlags properties = 0;
            ___memory_property_flags_internal(desc.memory_flags, properties);

            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(vk_device, result->buffer, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = ___find_memory_type_internal(vk_physical_device, memRequirements.memoryTypeBits,
                                                                     properties);

            if (vkAllocateMemory(vk_device, &allocInfo, nullptr, &result->memory) != VK_SUCCESS) {
                throw std::runtime_error("SpudGPU Vulkan: failed to allocate buffer memory!");
            }

            vkBindBufferMemory(vk_device, result->buffer, result->memory, 0);
        }

        // Retrieve the GPU Address point for gpu_buffer_desc
        {
            VkBufferDeviceAddressInfoKHR info{};
            info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
            info.buffer = result->buffer;
            result->desc.gpu_address_location = vkGetBufferDeviceAddressKHR(vk_device, &info);
        }

        return reinterpret_cast<gpu_buffer>(result);
    }

    gpu_buffer_desc get_buffer_desc(gpu_buffer buffer) {
        return reinterpret_cast<backends::vulkan::gpu_buffer_vulkan *>(buffer)->desc;
    }

    void destroy_buffer(gpu_buffer buffer) {
        backends::vulkan::gpu_buffer_vulkan *vkBuffer = reinterpret_cast<backends::vulkan::gpu_buffer_vulkan *>(buffer);
        vkDestroyBuffer(vkBuffer->pDevice->logical_device, vkBuffer->buffer, nullptr);
        vkFreeMemory(vkBuffer->pDevice->logical_device, vkBuffer->memory, nullptr);
        delete vkBuffer;
    }

    gpu_buffer_view create_buffer_view(gpu_buffer buffer, const gpu_buffer_view_desc &desc) {
        backends::vulkan::gpu_buffer_view_vulkan *result = new backends::vulkan::gpu_buffer_view_vulkan();

        return reinterpret_cast<gpu_buffer_view>(result);
    }

    void destroy_buffer_view(gpu_buffer_view buffer_view) {
        backends::vulkan::gpu_buffer_view_vulkan *vkBufferView = reinterpret_cast<
            backends::vulkan::gpu_buffer_view_vulkan *>(buffer_view);
        delete vkBufferView;
    }

    gpu_buffer_view_desc get_buffer_view_desc(gpu_buffer_view buffer_view) {
        return reinterpret_cast<backends::vulkan::gpu_buffer_view_vulkan *>(buffer_view)->desc;
    }


    gpu_image create_image(gpu_device device, const gpu_image_desc &desc) {
        backends::vulkan::gpu_image_vulkan *result = new backends::vulkan::gpu_image_vulkan();
        result->desc = desc;
        result->pDevice = reinterpret_cast<backends::vulkan::gpu_device_vulkan *>(device);
        result->image = VK_NULL_HANDLE;
        result->memory = VK_NULL_HANDLE;
        result->format = backends::vulkan::convert_spud_to_vulkan_format(desc.format);

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.mipLevels = 1; // Or add to your desc
        imageInfo.arrayLayers = 1;
        imageInfo.format = result->format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Determine the Image Type and Width Height Depth
        imageInfo.extent.width = static_cast<uint32_t>(desc.width);
        imageInfo.extent.height = static_cast<uint32_t>(desc.height);
        // TODO: GPU Image Usage Flags
        /*
        switch (desc.usage) {
            case SPUDGPU_IMAGE_USAGE_TEXTURE3D:
                imageInfo.imageType = VK_IMAGE_TYPE_3D;
                imageInfo.extent.depth = static_cast<uint32_t>(desc.depth);
                break;
            case SPUDGPU_IMAGE_USAGE_TEXTURE2D:
                imageInfo.imageType = VK_IMAGE_TYPE_2D;
                imageInfo.extent.depth = 1;
                break;
            default:
                throw std::runtime_error("Unsupported image usage: SPUDGPU_IMAGE_USAGE_NONE");
        }

        // Tranfer for uploading data, Sampled for shaders
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        */

        if (vkCreateImage(result->pDevice->logical_device, &imageInfo, nullptr, &result->image) != VK_SUCCESS) {
            throw std::runtime_error("SpudGPU Vulkan: Failed to create vulkan image!");
        }

        // Create the Vulkan Memory
        {
            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(result->pDevice->logical_device, result->image, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = ___find_memory_type_internal(
                result->pDevice->physical_device,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                memRequirements.memoryTypeBits);
            if (vkAllocateMemory(result->pDevice->logical_device, &allocInfo, nullptr, &result->memory) != VK_SUCCESS) {
                throw std::runtime_error("SpudGPU Vulkan: Failed to allocate image memory!");
            }

            vkBindImageMemory(result->pDevice->logical_device, result->image, result->memory, 0);
        }

        // Retrieve the GPU Address point for gpu_buffer_desc
        {
            
            VkBufferDeviceAddressInfoKHR info{};
            info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
            info.buffer = result->buffer;
            m_desc.gpuAddressLocation = vkGetBufferDeviceAddressKHR(vk_device, &info);
        }
    }
}

#endif

/*
namespace spud::gpu::backends::vulkan {
    std::shared_ptr<gpu_buffer_view> gpu_buffer_vulkan::create_view(
        const uint64_t &offset,
        const uint64_t &stride,
        const uint64_t &size) const {
        return nullptr;
    }

    gpu_buffer_vulkan::gpu_buffer_vulkan(
        const std::shared_ptr<gpu_device_vulkan> &device,
        const gpu_buffer_desc &desc,
        VkMemoryPropertyFlags properties) : m_buffer(nullptr),
                                            m_memory(nullptr),
                                            m_device(device),
                                            m_properties(properties),
                                            m_desc(desc) {
        // Validate desc
        {
            if (m_desc.gpuAddressLocation == 0)
                throw std::runtime_error(
                    "gpu_buffer_vulkan::gpu_buffer_vulkan: invalid GPU address location: " + std::to_string(
                        m_desc.gpuAddressLocation));

            if (m_desc.size == 0)
                throw std::runtime_error(
                    "gpu_buffer_vulkan::gpu_buffer_vulkan: invalid size: " + std::to_string(m_desc.size));

            if (m_desc.usage == 0)
                throw std::runtime_error(
                    "gpu_buffer_vulkan::gpu_buffer_vulkan: invalid usage: SPUDGPU_BUFFER_USAGE_NONE");
        }

        // Get native Vulkan device handles
        VkDevice vk_device = device->get_vkdevice_native();
        VkPhysicalDevice vk_physicalDevice = device->get_vkphysicaldevice_native();

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = desc.size;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Covert SPUDGPU_BUFFER_USAGE to native Vulkan usage
        switch (desc.usage) {
            case SPUDGPU_BUFFER_USAGE_VERTEX:
                bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                break;
            case SPUDGPU_BUFFER_USAGE_INDEX:
                bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
                break;
            case SPUDGPU_BUFFER_USAGE_UNIFORM:
                bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                break;
            case SPUDGPU_BUFFER_USAGE_STORAGE:
                bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
                break;
            default:
                break;
        }

        // Create buffer
        if (vkCreateBuffer(vk_device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        // Allocate memory
        {
            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(vk_device, m_buffer, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = findMemoryType(vk_physicalDevice, memRequirements.memoryTypeBits, properties);

            if (vkAllocateMemory(vk_device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate buffer memory!");
            }

            vkBindBufferMemory(vk_device, m_buffer, m_memory, 0);
        }

        // Retrieve the GPU Address point for spud::gpu::gpu_buffer_desc
        {
            VkBufferDeviceAddressInfoKHR info{};
            info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
            info.buffer = m_buffer;
            m_desc.gpuAddressLocation = vkGetBufferDeviceAddressKHR(vk_device, &info);
        }
    }

    gpu_buffer_vulkan::~gpu_buffer_vulkan() {
        VkDevice vk_device = m_device->get_vkdevice_native();
        vkDestroyBuffer(vk_device, m_buffer, nullptr);
        vkFreeMemory(vk_device, m_memory, nullptr);
    }

    uint32_t gpu_buffer_vulkan::findMemoryType(
        VkPhysicalDevice physicalDevice,
        uint32_t typeFilter,
        VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        throw std::runtime_error("failed to find suitable memory type!");
    }

    gpu_buffer_view_vulkan::gpu_buffer_view_vulkan(
        const std::shared_ptr<gpu_buffer_vulkan> &parentBuffer,
        const gpu_buffer_view_desc &desc) : m_pParentBuffer(nullptr),
                                            m_desc(desc) {
        // Validate that this new buffer view is within the buffer's memory range.
        gpu_buffer_desc parentBufferDesc = parentBuffer->get_desc();
        if (desc.offsetFromParentBuffer + desc.size > parentBufferDesc.size) {
            throw std::runtime_error("Buffer view exceeds parent buffer size!");
        }
    }

    gpu_buffer_view_vulkan::~gpu_buffer_view_vulkan() = default;

    gpu_image_vulkan::gpu_image_vulkan(
        const std::shared_ptr<gpu_device_vulkan> &device,
        const gpu_image_desc &desc) : m_desc(desc),
                                      m_device(device),
                                      m_image(nullptr),
                                      m_vk_format(VK_FORMAT_UNDEFINED),
                                      m_memory(VK_NULL_HANDLE) {
        auto vk_device = device->get_vkdevice_native();
        auto vk_physical_device = device->get_vkphysicaldevice_native();

        m_vk_format = convert_spud_to_vulkan_format(desc.format);

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.mipLevels = 1; // Or add to your desc
        imageInfo.arrayLayers = 1;
        imageInfo.format = m_vk_format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Determine the Image Type and Width Height Depth
        imageInfo.extent.width = static_cast<uint32_t>(desc.width);
        imageInfo.extent.height = static_cast<uint32_t>(desc.height);
        switch (desc.usage) {
            case SPUDGPU_IMAGE_USAGE_TEXTURE3D:
                imageInfo.imageType = VK_IMAGE_TYPE_3D;
                imageInfo.extent.depth = static_cast<uint32_t>(desc.depth);
                break;
            case SPUDGPU_IMAGE_USAGE_TEXTURE2D:
                imageInfo.imageType = VK_IMAGE_TYPE_2D;
                imageInfo.extent.depth = 1;
                break;
            default:
                throw std::runtime_error("Unsupported image usage: SPUDGPU_IMAGE_USAGE_NONE");
        }

        // Tranfer for uploading data, Sampled for shaders
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        if (vkCreateImage(vk_device, &imageInfo, nullptr, &m_image) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create vulkan image!");
        }

        // Create the Vulkan Memory
        {
            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(vk_device, m_image, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits,
                                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                         vk_physical_device);
            if (vkAllocateMemory(vk_device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
                throw std::runtime_error("Failed to allocate image memory!");
            }

            vkBindImageMemory(vk_device, m_image, m_memory, 0);
        }
    }

    gpu_image_vulkan::~gpu_image_vulkan() {
        auto vk_device = m_device->get_vkdevice_native();
        if (m_image) vkDestroyImage(vk_device, m_image, nullptr);
        if (m_memory) vkFreeMemory(vk_device, m_memory, nullptr);
    }

    uint32_t gpu_image_vulkan::find_memory_type(uint32_t typeFilter,
                                                VkMemoryPropertyFlags properties,
                                                VkPhysicalDevice physicalDevice) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        throw std::runtime_error("failed to find suitable memory type!");
    }

    gpu_resource_pool_vulkan::gpu_resource_pool_vulkan(const std::shared_ptr<gpu_device_vulkan> &device) : m_device(
        device) {
        m_page_size = 64 * 1024 * 1024;

        // We need a dummy check to find the memory type index for Device Local memory
        // In a production raw allocator, you'd do this dynamically.
        m_memory_type_index = find_memory_type(m_device->get_vkphysicaldevice_native(), 0xFFFFFFFF,
                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VkMemoryAllocateInfo alloc_info{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
        alloc_info.allocationSize = m_page_size;
        alloc_info.memoryTypeIndex = m_memory_type_index;

        if (vkAllocateMemory(m_device->get_vkdevice_native(), &alloc_info, nullptr, &m_page_memory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate raw GPU memory page!");
        }
    }

    gpu_resource_pool_vulkan::~gpu_resource_pool_vulkan() {
    }

    void gpu_resource_pool_vulkan::reserve(uint64_t bytes) {
        //TODO: Custom sizing for GPU Resource Pool
    }

    std::shared_ptr<gpu_buffer> gpu_resource_pool_vulkan::allocate_buffer(const gpu_buffer_desc &desc) {
        VkMemoryPropertyFlags flags = 0;
        std::shared_ptr<gpu_buffer_vulkan> buffer = std::make_shared<gpu_buffer_vulkan>(this->m_device, desc, flags);
        return nullptr;
    }

    std::shared_ptr<gpu_image> gpu_resource_pool_vulkan::allocate_image(const gpu_image_desc &desc) {
        return nullptr;
    }

    void gpu_resource_pool_vulkan::free(const std::shared_ptr<gpu_buffer> &buffer) {
    }

    void gpu_resource_pool_vulkan::free(const std::shared_ptr<gpu_image> &image) {
    }

    uint32_t gpu_resource_pool_vulkan::find_memory_type(
        VkPhysicalDevice phys_device,
        uint32_t type_filter,
        VkMemoryPropertyFlags properties) {
    }
}
*/
