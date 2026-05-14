#include "gpu/backends/vulkan/spudgpuvulkanresource.hpp"
#include "gpu/backends/vulkan/spudgpuvulkancontext.hpp"

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
                                      m_vk_format(VK_FORMAT_UNDEFINED) {
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

    gpu_resource_pool_vulkan::gpu_resource_pool_vulkan()
    = default;

    gpu_resource_pool_vulkan::~gpu_resource_pool_vulkan()
    = default;

    void gpu_resource_pool_vulkan::reserve(uint64_t bytes) {
    }

    std::shared_ptr<gpu_buffer> gpu_resource_pool_vulkan::allocate_buffer(const gpu_buffer_desc &desc) {
        return nullptr;
    }

    std::shared_ptr<gpu_image> gpu_resource_pool_vulkan::allocate_image(const gpu_image_desc &desc) {
        return nullptr;
    }

    void gpu_resource_pool_vulkan::free(const std::shared_ptr<gpu_buffer> &buffer) {
    }

    void gpu_resource_pool_vulkan::free(const std::shared_ptr<gpu_image> &image) {
    }
}
