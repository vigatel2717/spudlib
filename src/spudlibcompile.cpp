//
// Created by Nathan on 5/9/2026.
// This file is intended to be included in an external project.
// If you don't want to link static binaries,
// then you can just include this file,
// and compile SpudLib while you create your application.
//

#if SPUDLIB_EXTERNAL_COMPILE

#if SPUDGPU_EXTERNAL_COMPILE
#define SPUDGPU_COMPILE_VULKAN 1
#endif

#endif
