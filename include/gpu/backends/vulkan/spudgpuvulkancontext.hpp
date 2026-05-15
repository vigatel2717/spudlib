//
// Created by nathanmoore on 3/27/26.
//

#ifndef SPUDLIB_SPUDGPUVULKANCONTEXT_HPP
#define SPUDLIB_SPUDGPUVULKANCONTEXT_HPP

#include "spudgpu.hpp"
#include "gpu/spudgpucontext.hpp"

namespace spud::gpu::backends::vulkan {
	struct gpu_device_vulkan {
		VkInstance instance = VK_NULL_HANDLE;
		VkPhysicalDevice physical_device = VK_NULL_HANDLE;
		VkDevice logical_device = VK_NULL_HANDLE;
	};

	struct gpu_command_queue {
		VkQueue queue = VK_NULL_HANDLE;
	};

	struct gpu_swap_chain_vulkan {
		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	};

	struct gpu_buffer_vulkan {
		gpu_buffer_desc desc;
		gpu_device_vulkan *pDevice = nullptr;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};

	struct gpu_buffer_view_vulkan {
		gpu_buffer_view_desc desc;
		VkBufferView buffer_view = VK_NULL_HANDLE;
	};

	struct gpu_image_vulkan {
		gpu_image_desc desc;
		gpu_device_vulkan *pDevice = nullptr;
		VkImage image = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkFormat format = VK_FORMAT_UNDEFINED;
	};

	struct gpu_image_view_vulkan {
		gpu_image_view_desc desc;
		VkImageView view = VK_NULL_HANDLE;
	};
}

/*
namespace spud::gpu::backends::vulkan {
	class command_queue_vulkan : public command_queue {
	public:
		command_queue_vulkan(VkQueue queue);

		~command_queue_vulkan() override;

		void execute_commands(
			const uint32_t &cmdBufferCount,
			const std::shared_ptr<command_buffer> *pCmdBuffers) override;

		[[nodiscard]] virtual SPUDGPU_COMMAND_QUEUE_TYPE get_type() const override { return m_type; }

	private:
		VkQueue m_queue;
		SPUDGPU_COMMAND_QUEUE_TYPE m_type;
	};

	class gpu_device_vulkan : public gpu_device {
	public:
		gpu_device_vulkan();

		~gpu_device_vulkan() override;

		void initialize_from_physical_device_impl(VkPhysicalDevice physicalDevice);

		[[nodiscard]] std::shared_ptr<gpu_resource_pool> get_default_resource_pool() const override { return m_resource_pool; }
		[[nodiscard]] std::shared_ptr<gpu_resource_pool> create_resource_pool() const override { return nullptr; }

		[[nodiscard]] std::shared_ptr<command_allocator> create_command_allocator() const override;

		[[nodiscard]] std::shared_ptr<command_queue> get_command_queue(SPUDGPU_COMMAND_QUEUE_TYPE type) const override;

		void destroy() override;

		VkDevice get_vkdevice_native() const { return m_device; }
		VkPhysicalDevice get_vkphysicaldevice_native() const { return m_physicalDevice; }

	private:
		std::shared_ptr<gpu_resource_pool> m_resource_pool;
		std::vector<std::shared_ptr<command_queue> > m_command_queues;
		VkPhysicalDevice m_physicalDevice;
		VkDevice m_device;
		VkPhysicalDeviceProperties m_properties;
		VkPhysicalDeviceFeatures m_features;
	};

	void initialize_vulkan_impl(
		const std::string &applicationName,
		const uint32_t &applicationVersion,
		const std::string &engineName,
		const uint32_t &engineVersion);

	std::vector<std::shared_ptr<gpu_device> > initialize_devices_vulkan();

	void terminate_devices_vulkan();
}
*/

#endif //SPUDLIB_SPUDGPUVULKANCONTEXT_HPP
