cmake_minimum_required(VERSION 4.2)
project(spudlib)

set(CMAKE_CXX_STANDARD 26)

add_library(spudlib STATIC
        include/library.h
        src/library.cpp
        include/spudgpu.hpp
        include/spudfield.hpp
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
        include/gpu/backends/vulkan/spudgpuvulkandef.hpp)

target_include_directories(spudlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(Vulkan REQUIRED)
target_link_libraries(spudlib PRIVATE Vulkan::Vulkan)
