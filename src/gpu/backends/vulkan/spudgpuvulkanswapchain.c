//
// Created by Nathan on 5/13/2026.
//

#include <stdlib.h>
#include <c++/15/cmath>

#include "spudgpuvulkan.h"
#include "spudgpu.h"

#if __cplusplus
extern "C" {



#endif

void spudgpuvulkan___get_available_formats_internal(
	spudgpu_swap_chain_vulkan *pSwapChain,
	VkSurfaceFormatKHR **ppOutput,
	uint32_t *pOutputCount) {
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(pSwapChain->_device._physical_device_vk, pSwapChain->_surface_vk, &formatCount,
	                                     nullptr);
	vkGetPhysicalDeviceSurfaceFormatsKHR(pSwapChain->_device._physical_device_vk, pSwapChain->_surface_vk, &formatCount,
	                                     *ppOutput);
	*pOutputCount = formatCount;
}

void spudgpuvulkan___get_available_present_modes_internal(
	spudgpu_swap_chain_vulkan *pSwapChain,
	VkPresentModeKHR **ppOutput,
	uint32_t *pOutputCount) {
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(pSwapChain->_device._physical_device_vk, pSwapChain->_surface_vk,
	                                          &presentModeCount, nullptr);
	vkGetPhysicalDeviceSurfacePresentModesKHR(pSwapChain->_device._physical_device_vk, pSwapChain->_surface_vk,
	                                          &presentModeCount, *ppOutput);
	*pOutputCount = presentModeCount;
}

VkSurfaceFormatKHR spudgpuvulkan___choose_surface_format_internal(
	VkSurfaceFormatKHR *pAvailableFormats,
	uint32_t availableFormatCount) {
	for (uint32_t i = 0; i < availableFormatCount; i++)
		if (pAvailableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
		    pAvailableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return pAvailableFormats[i];
	return (VkSurfaceFormatKHR){0};
}

VkPresentModeKHR spudgpuvulkan___choose_present_mode_internal(
	VkPresentModeKHR *pAvailablePresentModes,
	uint32_t availablePresentModeCount) {
	// "Mailbox" is the gold standard (Triple Buffering)
	// It avoids tearing while maintaining low latency.
	for (uint32_t i = 0; i < availablePresentModeCount; i++)
		if (pAvailablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return pAvailablePresentModes[i];
	return VK_PRESENT_MODE_FIFO_KHR; // FIFO is guaranteed to be available by the Vulkan spec (Standard V-Sync)
}

VkExtent2D spudgpuvulkan___choose_extent_internal(
	VkSurfaceCapabilitiesKHR capabilities,
	uint32_t width,
	uint32_t height) {
	// If currentExtent is NOT set to the max value of uint32_t,
	// it means the surface size is determined by the window manager.
	if (capabilities.currentExtent.width != UINT32_MAX) return capabilities.currentExtent;
	else return capabilities.minImageExtent;
}


VkResult spudgpuvulkan___create_swapchain_internal(
	spudgpu_swap_chain_vulkan *pSwapChain,
	uint32_t width,
	uint32_t height,
	VkSwapchainKHR oldHandle) {
	VkResult result = VK_SUCCESS;
	VkPhysicalDevice physicalDevice = pSwapChain->_device._physical_device_vk;
	VkDevice logicalDevice = pSwapChain->_device._logical_device_vk;
	// Query capabilities
	VkSurfaceCapabilitiesKHR capabilities;
	VkSurfaceFormatKHR surfaceFormat;
	{
		result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, pSwapChain->_surface_vk, &capabilities);
		if (result != VK_SUCCESS) return result;

		surfaceFormat =
				spudgpuvulkan___choose_surface_format_internal(spudgpuvulkan___get_available_formats_internal());
		VkPresentModeKHR presentMode = spudgpuvulkan___choose_present_mode_internal(
			spudgpuvulkan___get_available_present_modes_internal());
		pSwapChain->_extent_vk = spudgpuvulkan___choose_extent_internal(capabilities, width, height);
		pSwapChain->_format_vk = surfaceFormat.format;
	}

	// Determine image count
	uint32_t imageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
		imageCount = capabilities.maxImageCount;

	// Populate Creation Info
	VkSwapchainCreateInfoKHR createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.surface = pSwapChain->_surface_vk;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = pSwapChain->_extent_vk;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Pass the old swap chain handle when this function is called inside of recreate()
	// This is for Vulkan optimization
	createInfo.oldSwapchain = oldHandle;

	// Handle graphics vs presentation queue sharing here (Concurrent vs Exclusive)
	// ...

	result = vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &pSwapChain->_swapchain_vk);
	if (result != VK_SUCCESS) {
		//throw std::runtime_error("failed to create swap chain!");
		return result;
	}

	// Retrieve handles to the images
	result = vkGetSwapchainImagesKHR(logicalDevice, pSwapChain->_swapchain_vk, &imageCount, nullptr);
	if (result != VK_SUCCESS) return result;

	pSwapChain->_swapchain_images_vk = malloc(pSwapChain->_swapchain_images_count * sizeof(VkImage));
	result = vkGetSwapchainImagesKHR(logicalDevice, pSwapChain->_swapchain_vk, &imageCount,
	                                 pSwapChain->_swapchain_images_vk);
	if (result != VK_SUCCESS) return result;
	pSwapChain->_swapchain_images_count=imageCount;

	return result;
}

/*
VkExtent2D swap_chain_vulkan::choose_extent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width,
                                            uint32_t height) {
	// If currentExtent is NOT set to the max value of uint32_t,
	// it means the surface size is determined by the window manager.
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		// Otherwise, we manually clamp our window dimensions to the
		// min/max supported by the GPU surface.
		VkExtent2D actualExtent = {width, height};

		actualExtent.width = std::clamp(actualExtent.width,
		                                capabilities.minImageExtent.width,
		                                capabilities.maxImageExtent.width);

		actualExtent.height = std::clamp(actualExtent.height,
		                                 capabilities.minImageExtent.height,
		                                 capabilities.maxImageExtent.height);

		return actualExtent;
	}
}*/

VkResult spudgpuvulkan_create_image_views_internal(
	spudgpu_swap_chain_vulkan *pSwapChain,
	uint32_t swap_chain_image_view_count) {
	pSwapChain->_swapchain_image_views_count = swap_chain_image_view_count;
	pSwapChain->_swapchain_image_views_vk = malloc(pSwapChain->_swapchain_image_views_count * sizeof(VkImageView));

	VkResult result = VK_SUCCESS;
	for (size_t i = 0; i < pSwapChain->_swapchain_images_count; i++) {
		VkImageViewCreateInfo createInfo = {0};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = pSwapChain->_swapchain_images_vk[i];

		// Treat the image as a standard 2D texture
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = pSwapChain->_format_vk; // Use the format chosen in chooseSurfaceFormat

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

		result = vkCreateImageView(pSwapChain->_device._logical_device_vk, &createInfo, nullptr,
		                           &pSwapChain->_swapchain_image_views_vk[i]);
		if (result != VK_SUCCESS) {
			//throw std::runtime_error("failed to create image views!");
			return result;
		}
	}
	return result;
}


spudgpu_swap_chain spudgpu_create_swap_chain(
	spudgpu_device device,
	const spudgpu_swap_chain_desc *desc) {
	if (!(device && desc)) return nullptr;

	spudgpu_swap_chain_vulkan result = {0};
	result._desc = desc;
	VkResult r = spudgpuvulkan___create_swapchain_internal(&result, desc->width, desc->height, nullptr);
	if (r != VK_SUCCESS) return nullptr;

	r = spudgpuvulkan_create_image_views_internal(&result, result._swapchain_images_count);
	if (r != VK_SUCCESS) return nullptr;

	spudgpu_swap_chain_vulkan *pResult = malloc(sizeof(spudgpu_swap_chain_vulkan));
	memcpy(pResult, &result, sizeof(spudgpu_swap_chain_vulkan));
	return (spudgpu_swap_chain) pResult;
}

void spudgpu_destroy_swap_chain(spudgpu_swap_chain swap_chain) {
	if (!swap_chain) return;
	spudgpu_swap_chain_vulkan *vk_SwapChain = (spudgpu_swap_chain_vulkan *) swap_chain;
	for (uint32_t i = 0; i < vk_SwapChain->_swapchain_image_views_count; i++)
		vkDestroyImageView(vk_SwapChain->_device._logical_device_vk, vk_SwapChain->_swapchain_image_views_vk[i], nullptr);
	if (vk_SwapChain->_swapchain_vk != VK_NULL_HANDLE)
		vkDestroySwapchainKHR(vk_SwapChain->_device._logical_device_vk, vk_SwapChain->_swapchain_vk, nullptr);
	//for (auto imageView: m_swapchain_image_views) {
	//	vkDestroyImageView(m_device, imageView, nullptr);
	//}
	//m_swapchain_image_views.clear();
	//if (m_swapchain != VK_NULL_HANDLE) {
	//	vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
	//}
}


/*
spudgpu_swap_chain_desc spudgpu_get_swap_chain_desc(spudgpu_swap_chain swap_chain) {
	if (!swap_chain) return (spudgpu_swap_chain_desc){0};
	return ((spudgpu_swap_chain_vulkan *) swap_chain)->_desc;
}

swap_chain_vulkan::swap_chain_vulkan(
	VkDevice device,
	VkPhysicalDevice physicalDevice,
	VkSurfaceKHR surface,
	const uint32_t &width,
	const uint32_t &height) : m_device(device),
	                          m_physicalDevice(physicalDevice),
	                          m_surface(surface) {
	this->create_swapchain(width, height);
	this->create_image_views();
}

swap_chain_vulkan::~swap_chain_vulkan() {
	this->cleanup_internal();
}

void swap_chain_vulkan::cleanup_internal() {
	for (auto imageView: m_swapchain_image_views) {
		vkDestroyImageView(m_device, imageView, nullptr);
	}
	m_swapchain_image_views.clear();
	if (m_swapchain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
	}
}

void swap_chain_vulkan::recreate(const uint32_t &width, const uint32_t &height) {
	// Wait for the GPU to finish using current swap chain
	vkDeviceWaitIdle(m_device);

	VkSwapchainKHR oldHandle = m_swapchain;

	// Clean up only the views (they can't be reused)
	for (auto imageView: m_swapchain_image_views) {
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
}*/


}

#if __cplusplus
}
#endif
