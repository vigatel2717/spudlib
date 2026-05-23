//
// Created by nathanmoore on 5/22/26.
//

#ifndef SPUDLIB_SPUDGPU_SDL3_H
#define SPUDLIB_SPUDGPU_SDL3_H

#include <spudgpu.h>
#include <SDL3/SDL_vulkan.h>

static bool spudgpu_impl_sdl3_create_surface_fn(
    void *vk_instance,
    void *user_data,
    void *out_surface) {
    return SDL_Vulkan_CreateSurface(
        (SDL_Window *) user_data,
        (VkInstance) vk_instance,
        NULL,
        (VkSurfaceKHR *) out_surface);
}

static inline spudgpu_surface spudgpu_create_surface_from_sdl3(
    spudgpu_instance instance,
    SDL_Window *window) {
    return spudgpu_create_surface_from_callback(
        instance,
        window,
        spudgpu_impl_sdl3_create_surface_fn);
}

#endif //SPUDLIB_SPUDGPU_SDL3_H
