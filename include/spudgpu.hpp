//
// Created by nathanmoore on 3/27/26.
//

#ifndef SPUDLIB_SPUDGPU_HPP
#define SPUDLIB_SPUDGPU_HPP

#include "gpu/spudgpucontext.hpp"
#include "gpu/spudgpuresource.hpp"
#include "gpu/spudgpushader.hpp"
#include "gpu/spudgpuformats.hpp"
#include "gpu/spudgpucmd.hpp"

/*
 * Define SPUDGPU_COMPILE_METAL_API=1 or SPUDGPU_COMPILE_VULKAN_API=1
 * These are the flags needed to be able to use SpudGPU libraries.
 * If a flag is 0 or undefined then it won't be seen as true in SpudGPU.
*/

#if SPUDGPU_COMPILE_METAL_API
#endif
#if SPUDGPU_COMPILE_VULKAN_API
#include "gpu/backends/vulkan/spudgpuvulkandef.hpp"
#include "gpu/backends/vulkan/spudgpuvulkancontext.hpp"
#include "gpu/backends/vulkan/spudgpuvulkanresource.hpp"
#include "gpu/backends/vulkan/spudgpuvulkanshader.hpp"
#endif

#include "gpu/spudgpucontext.hpp"
#include "gpu/spudgpucmd.hpp"
#include "gpu/spudgpuformats.hpp"
#include "gpu/spudgpuresource.hpp"
#include "gpu/spudgpushader.hpp"

#endif //SPUDLIB_SPUDGPU_HPP
