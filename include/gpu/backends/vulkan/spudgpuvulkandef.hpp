//
// Created by Nathan on 5/10/2026.
//

#ifndef SPUDLIB_SPUDGPUVULKANDEF_HPP
#define SPUDLIB_SPUDGPUVULKANDEF_HPP

#include <vulkan/vulkan.hpp>
#include "gpu/spudgpuformats.hpp"

namespace spud::gpu::backends::vulkan {
    VkFormat convert_spud_to_vulkan_format(SPUDGPU_FORMAT format) {
        // TODO: Convert SpudGPU Format to Vulkan Formats
        return VK_FORMAT_UNDEFINED;
    }

}

#endif //SPUDLIB_SPUDGPUVULKANDEF_HPP
