//
// Created by Nathan on 5/13/2026.
//

#ifndef SPUDLIB_SPUDGPUVULKANSWAPCHAIN_HPP
#define SPUDLIB_SPUDGPUVULKANSWAPCHAIN_HPP

#include "gpu/spudgpuswapchain.hpp"
#include "spudgpuvulkandef.hpp"

/*
namespace spud::gpu::backends::vulkan {
	class swap_chain_vulkan : public swap_chain {
	public:
		swap_chain_vulkan(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			VkSurfaceKHR surface,
			const uint32_t &width,
			const uint32_t &height);
		~swap_chain_vulkan() override;

		void recreate(const uint32_t &width, const uint32_t &height) override;

		void set_vsync(const bool &vsync) override;
		bool is_vsync() override;

		uint32_t acquire_next_image() override;
		void present() override;

		[[nodiscard]] uint32_t get_width() const override;
		[[nodiscard]] uint32_t get_height() const override;

		[[nodiscard]] uint64_t get_native_swapchain_object() const override { return reinterpret_cast<uint64_t>(m_swapchain); }

	private:
		void create_swapchain(
			const uint32_t &width,
			const uint32_t &height,
			VkSwapchainKHR oldHandle = nullptr);
		void create_image_views();

		void cleanup_internal();
		std::vector<VkSurfaceFormatKHR> get_available_formats() const;
		std::vector<VkPresentModeKHR> get_available_present_modes() const;
		static VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		static VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

		VkDevice m_device;
		VkPhysicalDevice m_physicalDevice;
		VkSurfaceKHR m_surface;

		VkSwapchainKHR m_swapchain;
		VkFormat m_format;
		VkExtent2D m_extent;
		std::vector<VkImage> m_swapchain_images;
		std::vector<VkImageView> m_swapchain_image_views;
	};
}
*/

#endif //SPUDLIB_SPUDGPUVULKANSWAPCHAIN_HPP
