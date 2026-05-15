//
// Created by nathanmoore on 3/27/26.
//

#ifndef SPUDLIB_SPUDGPUVULKANSHADER_HPP
#define SPUDLIB_SPUDGPUVULKANSHADER_HPP

#include "gpu/spudgpushader.hpp"

/*
namespace spud::gpu::backends::vulkan {
    class gpu_shader_vulkan : public gpu_shader {
    public:
        gpu_shader_vulkan();

        ~gpu_shader_vulkan() override;

        [[nodiscard]] const gpu_shader_desc &get_desc() const override { return m_desc; }

    private:
        gpu_shader_desc m_desc;
    };

    class gpu_shader_pipeline_vulkan : public gpu_shader_pipeline {
    public:
        gpu_shader_pipeline_vulkan();

        ~gpu_shader_pipeline_vulkan() override;

        [[nodiscard]] const std::vector<std::shared_ptr<gpu_shader> > &get_gpu_shaders() const override {
            return m_shaders;
        }

    private:
        std::vector<std::shared_ptr<gpu_shader> > m_shaders;
    };
}*/

#endif //SPUDLIB_SPUDGPUVULKANSHADER_HPP
