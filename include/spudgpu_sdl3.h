//
// Created by nathanmoore on 5/22/26.
//

#ifndef SPUDLIB_SPUDGPU_SDL3_H
#define SPUDLIB_SPUDGPU_SDL3_H

#include <spudgpu.h>

#if SPUDGPU_COMPILE_VULKAN_API
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
#endif // SPUDGPU_COMPILE_VULKAN_API


#if SPUDGPU_COMPILE_D3D12_API
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_video.h>
#include <windef.h>

static inline spudgpu_surface spudgpu_create_surface_from_sdl3(
    spudgpu_instance instance,
    SDL_Window *window) {
    HWND hwnd = (HWND)SDL_GetPointerProperty(
        SDL_GetWindowProperties(window),
        SDL_PROP_WINDOW_WIN32_HWND_POINTER,
        NULL);
    spudgpu_surface surface = NULL;
    spudgpu_create_surface(instance, hwnd, NULL, &surface);
    return surface;
}
#endif // SPUDGPU_COMPILE_D3D12_API

#endif //SPUDLIB_SPUDGPU_SDL3_H
