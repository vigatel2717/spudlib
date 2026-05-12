//
// Created by nathanmoore on 3/27/26.
//

#ifndef SPUDLIB_SPUDGPUVULKANCONTEXT_HPP
#define SPUDLIB_SPUDGPUVULKANCONTEXT_HPP

#include "gpu/spudgpucontext.hpp"

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

		[[nodiscard]] std::shared_ptr<gpu_resource_pool> get_resource_pool() const override { return m_resource_pool; }

		[[nodiscard]] std::shared_ptr<command_allocator> create_command_allocator() const override;

		[[nodiscard]] std::shared_ptr<command_queue> get_command_queue(SPUDGPU_COMMAND_QUEUE_TYPE type) const override;

		void destroy() override;

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

#endif //SPUDLIB_SPUDGPUVULKANCONTEXT_HPP
