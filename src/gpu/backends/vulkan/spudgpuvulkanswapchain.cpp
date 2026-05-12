//
// Created by Nathan on 5/13/2026.
//

#include "gpu/backends/vulkan/spudgpuvulkanswapchain.hpp"

namespace spud::gpu::backends::vulkan {
	swap_chain_vulkan::swap_chain_vulkan(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface,
		const uint32_t &width,
		const uint32_t &height) {
	}

	swap_chain_vulkan::~swap_chain_vulkan() {
	}

	void swap_chain_vulkan::set_vsync(const bool &vsync) {
	}

	bool swap_chain_vulkan::is_vsync() {
		return false;
	}

	void swap_chain_vulkan::recreate(const uint32_t &width, const uint32_t &height) {
	}

	uint32_t swap_chain_vulkan::acquire_next_image() {
		return 0;
	}

	void swap_chain_vulkan::present() {
	}

	uint32_t swap_chain_vulkan::get_width() const {
		return 0;
	}

	uint32_t swap_chain_vulkan::get_height() const {
		return 0;
	}


	void swap_chain_vulkan::create_swapchain(const uint32_t &width, const uint32_t &height) {
		// Query capabilities
		VkSurfaceCapabilitiesKHR capabilities;
		VkSurfaceFormatKHR surfaceFormat;
		{
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &capabilities);

			surfaceFormat = choose_surface_format(get_available_formats());
			VkPresentModeKHR presentMode = choose_present_mode(get_available_present_modes());
			this->m_extent = choose_extent(capabilities, width, height);
			this->m_format = surfaceFormat.format;
		}

		// Determine image count
		uint32_t imageCount = capabilities.minImageCount + 1;
		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
			imageCount = capabilities.maxImageCount;
		}

		// Populate Creation Info
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = m_extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		// Handle graphics vs presentation queue sharing here (Concurrent vs Exclusive)
		// ...

		if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		// Retrieve handles to the images
		vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
		m_swapchain_images.resize(imageCount);
		vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_swapchain_images.data());
	}
}
