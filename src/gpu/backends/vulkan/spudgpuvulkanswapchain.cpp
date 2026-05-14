//
// Created by Nathan on 5/13/2026.
//

#include "gpu/backends/vulkan/spudgpuvulkanswapchain.hpp"
#include <algorithm>

namespace spud::gpu::backends::vulkan {
	swap_chain_vulkan::swap_chain_vulkan(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface,
		const uint32_t &width,
		const uint32_t &height) :
		m_device(device),
		m_physicalDevice(physicalDevice),
		m_surface(surface) {
		this->create_swapchain(width, height);
		this->create_image_views();
	}

	swap_chain_vulkan::~swap_chain_vulkan() {
		this->cleanup_internal();
	}

	void swap_chain_vulkan::cleanup_internal() {
		for (auto imageView : m_swapchain_image_views) {
			vkDestroyImageView(m_device, imageView, nullptr);
		}
		m_swapchain_image_views.clear();
		if (m_swapchain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
		}
	}

	void swap_chain_vulkan::set_vsync(const bool &vsync) {
	}

	bool swap_chain_vulkan::is_vsync() {
		return false;
	}

	void swap_chain_vulkan::recreate(const uint32_t &width, const uint32_t &height) {
		// Wait for the GPU to finish using current swap chain
		vkDeviceWaitIdle(m_device);

		VkSwapchainKHR oldHandle = m_swapchain;

		// Clean up only the views (they can't be reused)
		for (auto imageView : m_swapchain_image_views) {
			vkDestroyImageView(m_device, imageView, nullptr);
		}
		m_swapchain_image_views.clear();

		this->create_swapchain(width, height, oldHandle);

		// Now it is safe to destroy the old handle
		if (oldHandle != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(m_device, oldHandle, nullptr);
		}

		// Create new views for the new images
		this->create_image_views();
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

	std::vector<VkSurfaceFormatKHR> swap_chain_vulkan::get_available_formats() const {
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, nullptr);

		std::vector<VkSurfaceFormatKHR> formats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, formats.data());
		return formats;
	}
	std::vector<VkPresentModeKHR> swap_chain_vulkan::get_available_present_modes() const {
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, nullptr);

		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, presentModes.data());
		return presentModes;
	}
	VkSurfaceFormatKHR swap_chain_vulkan::choose_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			// We prefer SRGB for better color accuracy
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
				}
		}

		// If our preferred isn't found, just return the first one available
		return availableFormats[0];
	}
	VkPresentModeKHR swap_chain_vulkan::choose_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			// "Mailbox" is the gold standard (Triple Buffering)
			// It avoids tearing while maintaining low latency.
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		// FIFO is guaranteed to be available by the Vulkan spec (Standard V-Sync)
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	VkExtent2D swap_chain_vulkan::choose_extent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {
		// If currentExtent is NOT set to the max value of uint32_t,
		// it means the surface size is determined by the window manager.
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			// Otherwise, we manually clamp our window dimensions to the
			// min/max supported by the GPU surface.
			VkExtent2D actualExtent = { width, height };

			actualExtent.width = std::clamp(actualExtent.width,
				capabilities.minImageExtent.width,
				capabilities.maxImageExtent.width);

			actualExtent.height = std::clamp(actualExtent.height,
				capabilities.minImageExtent.height,
				capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	void swap_chain_vulkan::create_image_views() {
		// Resize the list to hold a view for every swapchain image
		m_swapchain_image_views.resize(m_swapchain_images.size());

		for (size_t i = 0; i < m_swapchain_images.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_swapchain_images[i];

			// Treat the image as a standard 2D texture
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_format; // Use the format chosen in chooseSurfaceFormat

			// Components allow you to remap color channels (swizzling)
			// VK_COMPONENT_SWIZZLE_IDENTITY means use the default mapping
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			// SubresourceRange describes what the image's purpose is
			// and which parts of the image should be accessed
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device, &createInfo, nullptr, &m_swapchain_image_views[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image views!");
			}
		}
	}
	void swap_chain_vulkan::create_swapchain(
			const uint32_t &width,
			const uint32_t &height,
			VkSwapchainKHR oldHandle) {
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

		// Pass the old swap chain handle when this function is called inside of recreate()
		// This is for Vulkan optimization
		createInfo.oldSwapchain = oldHandle;

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
