//
// Created by Nathan on 5/13/2026.
//

#if SPUDGPU_COMPILE_VULKAN_API

#include "spudgpuvulkan.h"
#include "spudgpu.h"

#if defined(SPUDGPU_PLATFORM_WIN32)
#include <windows.h>
#include <vulkan/vulkan_win32.h>

#elif defined(SPUDGPU_PLATFORM_WAYLAND)
#include <wayland-client.h>
#include <vulkan/vulkan_wayland.h>

#elif defined(SPUDGPU_PLATFORM_XLIB)
//#include <vulkan/vulkan_xlib.h>
//#include <X11/Xlib.h>
#endif

#include <stdlib.h>


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
	*ppOutput = malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
	*pOutputCount = formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(pSwapChain->_device._physical_device_vk, pSwapChain->_surface_vk, &formatCount,
	                                     *ppOutput);
}

void spudgpuvulkan___get_available_present_modes_internal(
	spudgpu_swap_chain_vulkan *pSwapChain,
	VkPresentModeKHR **ppOutput,
	uint32_t *pOutputCount) {
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(pSwapChain->_device._physical_device_vk, pSwapChain->_surface_vk,
	                                          &presentModeCount, nullptr);
	*ppOutput = malloc(sizeof(VkPresentModeKHR) * presentModeCount);
	*pOutputCount = presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(pSwapChain->_device._physical_device_vk, pSwapChain->_surface_vk,
	                                          &presentModeCount, *ppOutput);
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
	VkExtent2D actualExtent = {width, height};
	if (actualExtent.width < capabilities.minImageExtent.width)
		actualExtent.width = capabilities.minImageExtent.width;
	if (actualExtent.width > capabilities.maxImageExtent.width)
		actualExtent.width = capabilities.maxImageExtent.width;
	if (actualExtent.height < capabilities.minImageExtent.height)
		actualExtent.height = capabilities.minImageExtent.height;
	if (actualExtent.height > capabilities.maxImageExtent.height)
		actualExtent.height = capabilities.maxImageExtent.height;
	return actualExtent;
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
	VkPresentModeKHR presentMode;
	{
		result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, pSwapChain->_surface_vk, &capabilities);
		if (result != VK_SUCCESS) return result;

		VkSurfaceFormatKHR *pSurfaceFormats = nullptr;
		uint32_t surfaceFormatCount = 0;
		spudgpuvulkan___get_available_formats_internal(pSwapChain, &pSurfaceFormats, &surfaceFormatCount);
		surfaceFormat = spudgpuvulkan___choose_surface_format_internal(pSurfaceFormats, surfaceFormatCount);
		free(pSurfaceFormats);

		VkPresentModeKHR *pPresentModes = nullptr;
		uint32_t presentModeCount = 0;
		spudgpuvulkan___get_available_present_modes_internal(pSwapChain, &pPresentModes, &presentModeCount);
		presentMode = spudgpuvulkan___choose_present_mode_internal(pPresentModes, presentModeCount);
		free(pPresentModes);
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
	createInfo.presentMode = presentMode;

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

	pSwapChain->_swapchain_images_count = imageCount; // Set the image count before malloc()
	pSwapChain->_swapchain_images_vk = malloc(pSwapChain->_swapchain_images_count * sizeof(VkImage));
	result = vkGetSwapchainImagesKHR(
		logicalDevice, pSwapChain->_swapchain_vk, &imageCount,
		pSwapChain->_swapchain_images_vk);
	if (result != VK_SUCCESS) return result;

	return result;
}

void spudgpuvulkan___fences_semaphores_swapchain_creation_internal(
	spudgpu_swap_chain_vulkan *pSwapChain) {
	// In spudgpu_create_swap_chain, after spudgpuvulkan_create_image_views_internal():

	uint32_t max_frames = pSwapChain->_desc.buffer_count > 0 ? pSwapChain->_desc.buffer_count : 2;
	pSwapChain->_max_frames_in_flight = max_frames;
	pSwapChain->_current_frame = 0;

	pSwapChain->_image_available_semaphores_vk = malloc(max_frames * sizeof(VkSemaphore));
	pSwapChain->_render_finished_semaphores_vk = malloc(max_frames * sizeof(VkSemaphore));
	pSwapChain->_in_flight_fences_vk = malloc(max_frames * sizeof(VkFence));

	VkDevice vk_device = pSwapChain->_device._logical_device_vk;

	VkSemaphoreCreateInfo semInfo = {0};
	semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {0};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // <-- CRITICAL

	for (uint32_t i = 0; i < max_frames; i++) {
		vkCreateSemaphore(vk_device, &semInfo, NULL, &pSwapChain->_image_available_semaphores_vk[i]);
		vkCreateSemaphore(vk_device, &semInfo, NULL, &pSwapChain->_render_finished_semaphores_vk[i]);
		vkCreateFence(vk_device, &fenceInfo, NULL, &pSwapChain->_in_flight_fences_vk[i]);
	}
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
	pSwapChain->_swapchain_image_views_count = swap_chain_image_view_count; // Set image view count before malloc()
	pSwapChain->_swapchain_image_views_vk = calloc(pSwapChain->_swapchain_image_views_count,
	                                               sizeof(spudgpu_image_view_vulkan));

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

		spudgpu_image_view_vulkan *view = &pSwapChain->_swapchain_image_views_vk[i];
		view->_parent_image._image_vk = pSwapChain->_swapchain_images_vk[i]; // Store the VkImage into the parent image object
		result = vkCreateImageView(pSwapChain->_device._logical_device_vk, &createInfo, nullptr,
		                           &pSwapChain->_swapchain_image_views_vk[i]._image_view_vk);
		if (result != VK_SUCCESS) {
			//throw std::runtime_error("failed to create image views!");
			return result;
		}


	}
	return result;
}


spudgpu_surface spudgpu_create_surface(
	spudgpu_instance instance,
	void *window_handle,
	void *display_handle) {
	if (!(instance && window_handle)) return NULL;

	spudgpu_instance_vulkan *vkInstance = (spudgpu_instance_vulkan *) instance;
	VkInstance vk = vkInstance->_instance_vk;

	spudgpu_surface_vulkan result = {0};
	result._instance = *vkInstance;

	VkResult r = VK_SUCCESS;

#if defined(SPUDGPU_PLATFORM_WIN32)
	VkWin32SurfaceCreateInfoKHR ci = {0};
	ci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	ci.hwnd = (HWND) window_handle;
	ci.hinstance = GetModuleHandle(NULL);
	r = vkCreateWin32SurfaceKHR(vk, &ci, NULL, &result._surface_vk);

#elif defined(SPUDGPU_PLATFORM_WAYLAND)
	VkWaylandSurfaceCreateInfoKHR ci = {0};
	ci.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
	ci.display = (struct wl_display *) display_handle;
	ci.surface = (struct wl_surface *) window_handle;
	r = vkCreateWaylandSurfaceKHR(vk, &ci, NULL, &result._surface_vk);

#elif defined(SPUDGPU_PLATFORM_XLIB)
	VkXlibSurfaceCreateInfoKHR ci = {0};
	ci.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	ci.dpy = (Display *) display_handle;
	ci.window = (Window)(uintptr_t) window_handle;
	r = vkCreateXlibSurfaceKHR(vk, &ci, NULL, &result._surface_vk);

#else
#error "No platform defined."
#endif

	if (r != VK_SUCCESS) return NULL;

	spudgpu_surface_vulkan *pResult = malloc(sizeof(spudgpu_surface_vulkan));
	memcpy(pResult, &result, sizeof(spudgpu_surface_vulkan));
	return (spudgpu_surface) pResult;
}

void spudgpu_destroy_surface(spudgpu_surface surface) {
	if (!surface) return;
	spudgpu_surface_vulkan *vk = (spudgpu_surface_vulkan *) surface;
	vkDestroySurfaceKHR(vk->_instance._instance_vk, vk->_surface_vk, NULL);
	free(vk);
}

spudgpu_surface spudgpu_create_surface_from_callback(
	spudgpu_instance instance,
	void *user_data,
	spudgpu_surface_create_fn create_fn) {
	if (!(instance && create_fn)) return NULL;

	spudgpu_instance_vulkan *vkInst = (spudgpu_instance_vulkan *) instance;

	spudgpu_surface_vulkan *result = calloc(1, sizeof(spudgpu_surface_vulkan));
	if (!result) return NULL;

	result->_instance = *vkInst;

	bool ok = create_fn(
		(void *) vkInst->_instance_vk,
		user_data,
		(void *) &result->_surface_vk);

	if (!ok) {
		free(result);
		return NULL;
	}

	return (spudgpu_surface) result;
}


spudgpu_swap_chain spudgpu_create_swap_chain(
	spudgpu_device device,
	const spudgpu_swap_chain_desc *desc) {
	if (!(device && desc)) return nullptr;

	spudgpu_swap_chain_vulkan result = {0};
	result._device = *((spudgpu_device_vulkan *) device);
	memcpy(&result._desc, desc, sizeof(spudgpu_swap_chain_desc));

	if (desc->surface) {
		spudgpu_surface_vulkan *vk_surface = (spudgpu_surface_vulkan *) desc->surface;
		result._surface_vk = vk_surface->_surface_vk;
	}

	VkResult r = VK_SUCCESS;
	if (r != VK_SUCCESS) return nullptr;

	//result._desc = desc; Can't do this because of 'const'
	r = spudgpuvulkan___create_swapchain_internal(&result, desc->width, desc->height, nullptr);
	if (r != VK_SUCCESS) return nullptr;

	r = spudgpuvulkan_create_image_views_internal(&result, result._swapchain_images_count);
	if (r != VK_SUCCESS) return nullptr;

	spudgpuvulkan___fences_semaphores_swapchain_creation_internal(&result);

	spudgpu_swap_chain_vulkan *pResult = malloc(sizeof(spudgpu_swap_chain_vulkan));
	memcpy(pResult, &result, sizeof(spudgpu_swap_chain_vulkan));
	return (spudgpu_swap_chain) pResult;
}

void spudgpu_destroy_swap_chain(spudgpu_swap_chain swap_chain) {
	if (!swap_chain) return;
	spudgpu_swap_chain_vulkan *vk_SwapChain = (spudgpu_swap_chain_vulkan *) swap_chain;
	for (uint32_t i = 0; i < vk_SwapChain->_swapchain_image_views_count; i++) {
		//vkDestroyImageView(vk_SwapChain->_device._logical_device_vk, vk_SwapChain->_swapchain_image_views_vk[i]._image_view_vk, nullptr);
		spudgpu_destroy_image_view((spudgpu_device) &vk_SwapChain->_device,
		                           (spudgpu_image_view) &vk_SwapChain->_swapchain_image_views_vk[i]);
	}
	if (vk_SwapChain->_surface_vk != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(vk_SwapChain->_device._instance._instance_vk, vk_SwapChain->_surface_vk, nullptr);
	}
	//free(vk_SwapChain->_swapchain_image_views_vk); Don't need this since spudgpu_destroy_image_view() already calls it for each image view destroyed.
	if (vk_SwapChain->_swapchain_vk != VK_NULL_HANDLE)
		vkDestroySwapchainKHR(vk_SwapChain->_device._logical_device_vk, vk_SwapChain->_swapchain_vk, nullptr);
	free(swap_chain);
}


spudgpu_swap_chain_desc spudgpu_get_swap_chain_desc(spudgpu_swap_chain swap_chain) {
	if (!swap_chain) return (spudgpu_swap_chain_desc){0};
	return ((spudgpu_swap_chain_vulkan *) swap_chain)->_desc;
}

uint32_t spudgpu_swap_chain_acquire_next_image(spudgpu_swap_chain swap_chain) {
	if (!swap_chain) return UINT32_MAX;
	spudgpu_swap_chain_vulkan *vk = (spudgpu_swap_chain_vulkan *) swap_chain;
	VkDevice device = vk->_device._logical_device_vk;

	// Wait for the fence of the current frame slot, so we don't overwrite
	// GPU resources that are still in flight.
	vkWaitForFences(device, 1, &vk->_in_flight_fences_vk[vk->_current_frame],
	                VK_TRUE, UINT64_MAX);
	vkResetFences(device, 1, &vk->_in_flight_fences_vk[vk->_current_frame]);

	// Ask the driver for the next available swapchain image.
	// Signals _image_available_semaphores_vk[_current_frame] when the image
	// is ready for rendering.
	VkResult result = vkAcquireNextImageKHR(
		device,
		vk->_swapchain_vk,
		UINT64_MAX, // No timeout — block until an image is available
		vk->_image_available_semaphores_vk[vk->_current_frame],
		VK_NULL_HANDLE,
		&vk->_current_image_index);

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		// Caller should handle swap chain recreation on VK_ERROR_OUT_OF_DATE_KHR
		return UINT32_MAX;
	}

	return vk->_current_image_index;
}

void spudgpu_swap_chain_present(spudgpu_swap_chain swap_chain) {
	if (!swap_chain) return;
	spudgpu_swap_chain_vulkan *vk = (spudgpu_swap_chain_vulkan *) swap_chain;

	// Tell the presentation engine to display _current_image_index.
	// Wait on _render_finished_semaphores_vk[_current_frame], which your
	// command submission must signal when rendering is done.
	VkSemaphore waitSemaphores[] = {
		vk->_render_finished_semaphores_vk[vk->_current_frame]
	};

	// Retrieve the graphics queue at present-time.
	// The graphics queue family index was selected during logical device creation
	// (queueFamilyIndex = first family with VK_QUEUE_GRAPHICS_BIT).
	// On the vast majority of hardware this family also supports presentation.
	VkQueue presentQueue = VK_NULL_HANDLE;
	vkGetDeviceQueue(vk->_device._logical_device_vk, vk->_device._graphics_queue_family_index, 0, &presentQueue);

	VkPresentInfoKHR presentInfo = {0};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = waitSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &vk->_swapchain_vk;
	presentInfo.pImageIndices = &vk->_current_image_index;
	presentInfo.pResults = nullptr; // Optional per-swapchain result

	// Get the presentation queue — you'll need to store this on the struct
	// (VkQueue _present_queue_vk) similar to how _device stores the logical device.
	vkQueuePresentKHR(presentQueue, &presentInfo);

	// Advance the frame-in-flight cursor
	vk->_current_frame = (vk->_current_frame + 1) % vk->_max_frames_in_flight;
}

spudgpu_image_view spudgpu_get_swap_chain_image_view(
	spudgpu_swap_chain swap_chain,
	uint32_t image_index) {
	spudgpu_swap_chain_vulkan *sc = (spudgpu_swap_chain_vulkan *) swap_chain;
	if (image_index >= sc->_swapchain_image_views_count) return NULL;
	return (spudgpu_image_view) &sc->_swapchain_image_views_vk[image_index];
}

#if __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_VULKAN_API
