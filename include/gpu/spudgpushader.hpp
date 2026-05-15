//
// Created by nathanmoore on 3/27/26.
//

#ifndef SPUDLIB_SPUDGPUSHADER_HPP
#define SPUDLIB_SPUDGPUSHADER_HPP

#include <vector>
#include <memory>

/*
namespace spud::gpu {
    // Shader type of a gpu_shader.
    enum SPUDGPU_SHADER_TYPE {
        // Placeholder for an invalid type.
        SPUDGPU_SHADER_TYPE_NONE = 0,

        // Vertex shader type.
        SPUDGPU_SHADER_TYPE_VERTEX = 1,

        // Fragment shader type.
        SPUDGPU_SHADER_TYPE_FRAGMENT = 2,

        // Geometry shader type.
        SPUDGPU_SHADER_TYPE_GEOMETRY = 3,

        // Compute shader type.
        SPUDGPU_SHADER_TYPE_COMPUTE = 4
    };

    // Description of a gpu_shader.
    struct gpu_shader_desc {
        SPUDGPU_SHADER_TYPE type;
    };

    // Single shader instance.
    class gpu_shader {
    public:
        gpu_shader() = default;

        virtual ~gpu_shader() = default;

        // Get the description of this shader.
        [[nodiscard]] virtual const gpu_shader_desc &get_desc() const = 0;
    };

    // Input assembly for the shader pipeline program.
    class shader_pipeline_assembly {
    public:
        shader_pipeline_assembly() = default;

        virtual ~shader_pipeline_assembly() = default;
    };

    // Shader pipeline program containing all individual shaders and input assembly.
    class gpu_shader_pipeline {
    public:
        gpu_shader_pipeline() = default;

        virtual ~gpu_shader_pipeline() = default;

        // @return All the individual shaders placed in this pipeline.
        [[nodiscard]] virtual const std::vector<std::shared_ptr<gpu_shader> > &get_gpu_shaders() const = 0;
    };

    void init(int _cpp_par_);
}
*/

#endif //SPUDLIB_SPUDGPUSHADER_HPP
