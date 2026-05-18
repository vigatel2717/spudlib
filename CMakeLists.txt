cmake_minimum_required(VERSION 3.30)
project(spudlib)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif ()

# Target SpudGPU to be Vulkan if on Windows or Linux
# Set the SpudLib compile macro flags based on platform
if (WIN32 OR LINUX)
    set(CMAKE_C_STANDARD 23)
    add_library(spudlib STATIC
            include/spudfiles.h
            include/spudgpu.h
            src/gpu/backends/vulkan/spudgpuvulkan.h
            src/gpu/backends/vulkan/spudgpuvulkancontext.c
            src/gpu/backends/vulkan/spudgpuvulkanbuffer.c
            src/gpu/backends/vulkan/spudgpuvulkanshader.c
            src/gpu/backends/vulkan/spudgpuvulkanswapchain.c
            src/gpu/backends/vulkan/spudgpuvulkanimage.c
    )
    target_compile_definitions(spudlib PUBLIC SPUDGPU_COMPILE_VULKAN_API=1)
    # Target SpudGPU to be Metal if on Apple Silicon
elseif (APPLE)
    set(CMAKE_CXX_STANDARD 26)
    target_compile_definitions(spudlib PUBLIC SPUDGPU_COMPILE_METAL_API=1)
endif ()


target_include_directories(spudlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


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
