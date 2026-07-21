cmake_minimum_required(VERSION 3.30)
project(spudlib)


# Define the allowed choices array
set(VALID_BACKENDS "Vulkan" "D3D12" "Metal")

# Initialize the cache variable with a default value
set(GRAPHICS_BACKEND "Vulkan" CACHE STRING "Choose exactly one graphics backend to compile")

# Create the drop-down menu restriction for interactive GUIs
set_property(CACHE GRAPHICS_BACKEND PROPERTY STRINGS ${VALID_BACKENDS})

# STRICT VALIDATION: Force selection to match exactly one choice from the list
if(NOT GRAPHICS_BACKEND IN_LIST VALID_BACKENDS)
    string(REPLACE ";" ", " PRINTABLE_BACKENDS "${VALID_BACKENDS}")
    message(FATAL_ERROR
        "Invalid backend selection: '${GRAPHICS_BACKEND}'.\n"
        "You must choose EXACTLY ONE option from the following list: [${PRINTABLE_BACKENDS}]"
    )
endif()

# Proceed with your clean compile-time single macro definitions...
message(STATUS "Selected Graphics Backend: ${GRAPHICS_BACKEND}")

# Don't do anything if another library/application is already compiling SpudLib
if(NOT TARGET spudlib)
    # Set initial SpudLib sources
    set(SPUDLIB_SOURCES
        include/spudcore.h
        include/spudfiles.h
        include/spudgpu.h
        include/spudmemory.h
        include/spudnet.h
        include/spudperf.h
        src/spudcore.c
        src/spudmemory.c
        src/spudfiles.c
        src/spudperf.c
        src/spudnet.c
        src/gpu/spudgpuformat.c
        src/gpu/spudgpurendering.c
    )
    # Determine which backend sources to append.
    if(WIN32)
        list(APPEND SPUDLIB_SOURCES
            src/files/backends/windows/spudfileswindows.c
            src/net/backends/windows/spudnetwindows.c
            src/memory/backends/windows/spudmemorywindows.c
        )
    endif()
    if(UNIX AND NOT APPLE)
        list(APPEND SPUDLIB_SOURCES
            src/net/backends/linux/spudnetlinux.c
            src/memory/backends/linux/spudmemorylinux.c
        )
    endif()
    if(WIN32 OR(UNIX AND NOT APPLE))
        # Append Vulkan sources if Vulkan API is selected for compile.
        if(GRAPHICS_BACKEND STREQUAL "Vulkan")
            list(APPEND SPUDLIB_SOURCES
                src/gpu/backends/vulkan/spudgpuvulkan.h
                src/gpu/backends/vulkan/spudgpuvulkancontext.c
                src/gpu/backends/vulkan/spudgpuvulkanbuffer.c
                src/gpu/backends/vulkan/spudgpuvulkanshader.c
                src/gpu/backends/vulkan/spudgpuvulkanswapchain.c
                src/gpu/backends/vulkan/spudgpuvulkanimage.c
                src/gpu/backends/vulkan/spudgpuvulkandescriptors.c
                src/gpu/backends/vulkan/spudgpuvulkancommand.c
                src/gpu/backends/vulkan/spudgpuvulkanrenderpass.c
                src/gpu/backends/vulkan/spudgpuvulkannative.c
                src/gpu/backends/vulkan/spudgpuvulkansync.c
                include/spudgpu_vulkan_natives.h
            )

        # Append D3D12 sources if D3D12 API is selected for compile.
        elseif(GRAPHICS_BACKEND STREQUAL "D3D12")
            list(APPEND SPUDLIB_SOURCES
                src/gpu/backends/d3d12/spudgpud3d12.hpp
                src/gpu/backends/d3d12/spudgpud3d12context.cpp
                src/gpu/backends/d3d12/spudgpud3d12buffer.cpp
                src/gpu/backends/d3d12/spudgpud3d12shader.cpp
                src/gpu/backends/d3d12/spudgpud3d12swapchain.cpp
                src/gpu/backends/d3d12/spudgpud3d12image.cpp
                src/gpu/backends/d3d12/spudgpud3d12descriptors.cpp
                src/gpu/backends/d3d12/spudgpud3d12command.cpp
                src/gpu/backends/d3d12/spudgpud3d12renderpass.cpp
                src/gpu/backends/d3d12/spudgpud3d12native.cpp
                src/gpu/backends/d3d12/spudgpud3d12sync.cpp
                include/spudgpu_d3d12_natives.h
            )
        endif()

        # Append SDL3 support for Windows and Linux systems, but not Apple.
        list(APPEND SPUDLIB_SOURCES
            include/spudgpu_sdl3.h
        )
    elseif(APPLE)
        # Append Metal sources if Metal API is selected for compile.
        if (GRAPHICS_BACKEND STREQUAL "Metal")
            # I don't have a MacBook yet for this stuff.
        endif()
    endif()

    # After determing source backends, add_library to SpudLib target.
    add_library(spudlib STATIC ${SPUDLIB_SOURCES})

    # Now set the compile-time macros and packages and whatnot.
    if(WIN32)
        target_compile_definitions(spudlib PUBLIC SPUDLIB_PLATFORM_WIN32=1)
        target_compile_definitions(spudlib PUBLIC SPUDLIB_PLATFORM_WINDOWS=1)
        target_link_libraries(spudlib PRIVATE shlwapi ws2_32)
    elseif (UNIX AND NOT APPLE)
        target_compile_definitions(spudlib PUBLIC SPUDLIB_PLATFORM_LINUX=1)
    elseif (APPLE)
        target_compile_definitions(spudlib PUBLIC SPUDLIB_PLATFORM_APPLE=1)
    endif()

    if(WIN32 OR(UNIX AND NOT APPLE))
        # Set the Vulkan compile-time packages and macros.
        if(GRAPHICS_BACKEND STREQUAL "Vulkan")
            set_property(TARGET spudlib PROPERTY C_STANDARD 23)
            target_compile_definitions(spudlib PUBLIC SPUDGPU_COMPILE_VULKAN_API=1)

            if(WIN32)
                # Manually hint where Vulkan is located on Windows
                # On Linux, it is automatically /usr/include and /usr/lib64
                set(Vulkan_INCLUDE_DIR "C:/VulkanSDK/1.4.341.1/Include")
                set(Vulkan_LIBRARY "C:/VulkanSDK/1.4.341.1/Lib/vulkan-1.lib")
                set(Vulkan_GLSLC_EXECUTABLE "C:/VulkanSDK/1.4.341.1/Bin/glslc.exe")
            elseif(UNIX AND NOT APPLE)
                # Check for Wayland or default to X11 in Linux
                find_package(PkgConfig REQUIRED)

                # find_package(Wayland QUIET)
                pkg_check_modules(WAYLAND wayland-client)

                if(WAYLAND_FOUND)
                    # target_compile_definitions(spudlib PUBLIC SPUDGPU_PLATFORM_WAYLAND)
                    # target_link_libraries(spudlib PRIVATE wayland-client)
                    target_compile_definitions(spudlib PUBLIC SPUDGPU_PLATFORM_WAYLAND VK_USE_PLATFORM_WAYLAND_KHR)
                    target_link_libraries(spudlib PRIVATE ${WAYLAND_LIBRARIES})
                    target_include_directories(spudlib PRIVATE ${WAYLAND_INCLUDE_DIRS})
                else()
                    target_compile_definitions(spudlib PUBLIC SPUDGPU_PLATFORM_XLIB VK_USE_PLATFORM_XLIB_KHR)
                    target_link_libraries(spudlib PRIVATE X11) # X11 is usually available without find_package on Linux
                endif()
            endif()

            # Add Vulkan packages after setting up everything else for Vulkan.
            find_package(Vulkan REQUIRED)
            target_include_directories(spudlib PRIVATE ${Vulkan_INCLUDE_DIRS})
            target_link_libraries(spudlib PRIVATE Vulkan::Vulkan)
        endif()

        # Set the D3D12 compile-time packages and macros.
        if(GRAPHICS_BACKEND STREQUAL "D3D12")
            set_property(TARGET spudlib PROPERTY CXX_STANDARD 26)
            set_property(TARGET spudlib PROPERTY CXX_SCAN_FOR_MODULES OFF)
            target_compile_definitions(spudlib PUBLIC SPUDGPU_COMPILE_D3D12_API=1)

            # SPIRV-Cross (for SPIR-V → HLSL cross-compilation)
            if(WIN32)
                # Use ENV hint so any installed Vulkan SDK version works.
                set(Vulkan_INCLUDE_DIR "$ENV{VULKAN_SDK}/Include")

                find_package(Vulkan REQUIRED)
                target_include_directories(spudlib PRIVATE ${Vulkan_INCLUDE_DIRS})

                # Built from source (not the Vulkan SDK's prebuilt .lib files) so it
                # picks up CMAKE_MSVC_RUNTIME_LIBRARY and matches our CRT per-config
                # (the SDK's libs are release-only /MD and mismatch /MDd debug builds).
                include(FetchContent)
                set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
                set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
                set(SPIRV_CROSS_ENABLE_CPP OFF CACHE BOOL "" FORCE)
                set(SPIRV_CROSS_ENABLE_MSL OFF CACHE BOOL "" FORCE)
                set(SPIRV_CROSS_ENABLE_REFLECT OFF CACHE BOOL "" FORCE)
                set(SPIRV_CROSS_ENABLE_C_API OFF CACHE BOOL "" FORCE)
                set(SPIRV_CROSS_ENABLE_UTIL OFF CACHE BOOL "" FORCE)
                FetchContent_Declare(
                    spirv_cross
                    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
                    GIT_TAG        vulkan-sdk-1.4.341.0
                )
                set(CMAKE_POLICY_VERSION_MINIMUM 3.10)
                FetchContent_MakeAvailable(spirv_cross)

                target_link_libraries(spudlib PRIVATE
                    d3d12
                    dxgi
                    dxguid
                    "$ENV{VULKAN_SDK}/Lib/dxcompiler.lib"
                    spirv-cross-hlsl
                    spirv-cross-glsl
                    spirv-cross-core
                )
            else()
                return()
            endif()
        endif()

        # Set up SDL3 stuff
        # Path to the folder containing SDL3Config.cmake
        # Usually: [SDL3-folder]/lib/cmake/SDL3
        if(WIN32)
            set(SDL3_DIR "../SDL3-3.4.8/cmake")
        endif()

        find_package(SDL3 REQUIRED)
        target_link_libraries(spudlib PRIVATE SDL3::SDL3)

        # Copy DLL to output folder (So the app actually runs)
        if(WIN32)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:SDL3::SDL3>"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            )
        endif()
    elseif(APPLE)
        # Set the Metal compile-time packages and macros.
        if(GRAPHICS_BACKEND STREQUAL "Metal")
            set(CMAKE_CXX_STANDARD 26)
            target_compile_definitions(spudlib PUBLIC SPUDGPU_COMPILE_METAL_API=1)
            # I don't have a MacBook yet so this is empty
        endif()
    endif()

    target_precompile_headers(spudlib PRIVATE include/spudgpu.h)

    target_include_directories(spudlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()
