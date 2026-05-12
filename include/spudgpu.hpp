//
// Created by nathanmoore on 3/27/26.
//

#ifndef SPUDLIB_SPUDGPU_HPP
#define SPUDLIB_SPUDGPU_HPP
/*
 * Define SPUDGPU_COMPILE_METAL_API or SPUDGPU_COMPILE_VULKAN_API
 * These are the flags needed to be able to use SpudGPU libraries.
*/

#ifdef SPUDGPU_COMPILE_METAL_API
#endif
#ifdef SPUDGPU_COMPILE_VULKAN_API
#include "gpu/backends/vk/spudgpuvkcontext.hpp"
#include "gpu/backends/vk/spudgpuvkresource.hpp"
#include "gpu/backends/vk/spudgpuvkshader.hpp"
#endif

#include "gpu/spudgpucontext.hpp"
#include "gpu/spudgpucmd.hpp"
#include "gpu/spudgpuformats.hpp"
#include "gpu/spudgpuresource.hpp"
#include "gpu/spudgpushader.hpp"

#endif //SPUDLIB_SPUDGPU_HPP
