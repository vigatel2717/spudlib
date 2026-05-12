cmake_minimum_required(VERSION 3.30)
project(spudlib)

set(CMAKE_CXX_STANDARD 26)

add_library(spudlib STATIC
        include/library.h
        src/library.cpp
        include/spudgpu.hpp
        include/spudfiles.hpp
        include/gpu/spudgpucmd.hpp
        include/gpu/spudgpucontext.hpp
        include/gpu/spudgpuresource.hpp
        include/gpu/spudgpushader.hpp
        include/gpu/spudgpuformats.hpp
        src/gpu/spudgpuformats.cpp
        src/gpu/spudgpucontext.cpp
        src/spudlibcompile.cpp
        include/gpu/backends/vulkan/spudgpuvulkancontext.hpp
        src/gpu/backends/vulkan/spudgpuvulkancontext.cpp
        include/gpu/backends/vulkan/spudgpuvulkanresource.hpp
        include/gpu/backends/vulkan/spudgpuvulkanshader.hpp
        src/gpu/backends/vulkan/spudgpuvulkanresource.cpp
        src/gpu/backends/vulkan/spudgpuvulkanshader.cpp
        include/gpu/backends/vulkan/spudgpuvulkandef.hpp
        include/gpu/spudgpuswapchain.hpp
        include/gpu/backends/vulkan/spudgpuvulkanswapchain.hpp
        src/gpu/backends/vulkan/spudgpuvulkanswapchain.cpp
)

target_include_directories(spudlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Set the SpudLib compile macro flags based on platform
if (WIN32 OR LINUX)
    target_compile_definitions(spudlib PUBLIC SPUDGPU_COMPILE_VULKAN_API=1)
elseif (APPLE)
    target_compile_definitions(spudlib PUBLIC SPUDGPU_COMPILE_METAL_API=1)
endif ()


# Vulkan stuff
# Manually hint where Vulkan is located on Windows
# On Linux, it is automatically /usr/include and /usr/lib64
if (WIN32)
    set(Vulkan_INCLUDE_DIR "C:/VulkanSDK/1.4.341.1/Include")
    set(Vulkan_LIBRARY "C:/VulkanSDK/1.4.341.1/Lib/vulkan-1.lib")
    set(Vulkan_GLSLC_EXECUTABLE "C:/VulkanSDK/1.4.341.1/Bin/glslc.exe")
endif ()

find_package(Vulkan REQUIRED)
target_include_directories(spudlib PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(spudlib PRIVATE Vulkan::Vulkan)
