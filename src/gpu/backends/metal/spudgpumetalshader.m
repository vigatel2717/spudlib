//
// SpudGPU Metal backend - shader modules and pipeline state.
// SPIR-V is cross-compiled to MSL via SPIRV-Cross's C API (the same
// dependency the D3D12 backend already vendors for its own SPIR-V -> HLSL
// step, built here with the MSL backend instead - see CMakeLists.txt). The
// C API, not the C++ one, since this file is plain Objective-C and can't
// link against SPIRV-Cross's C++ template-heavy interface directly.
//

#if SPUDGPU_COMPILE_METAL_API

#include "spudgpu.h"
#include "spudcore.h"
#include "spudgpumetal.h"
#include <Metal/Metal.h>
#include <spirv_cross_c.h>

// -----------------------------------------------------------------------
// SPIR-V -> MSL cross-compilation
// -----------------------------------------------------------------------

static SPUDRESULT spudgpumetal___internal_cross_compile_spirv_to_msl(
    const void *spirv_code,
    size_t spirv_size,
    NSString **out_msl_source) {
	spvc_context context = NULL;
	if (spvc_context_create(&context) != SPVC_SUCCESS)
		return SPUDRESULT_GPU_SHADER_COMPILATION_FAILED;

	spvc_parsed_ir parsed_ir = NULL;
	spvc_result result = spvc_context_parse_spirv(
	    context, (const SpvId *)spirv_code, spirv_size / sizeof(SpvId), &parsed_ir);
	if (result != SPVC_SUCCESS) {
		printf("spudgpu: SPIR-V parse failed: %s\n", spvc_context_get_last_error_string(context));
		spvc_context_destroy(context);
		return SPUDRESULT_GPU_SHADER_COMPILATION_FAILED;
	}

	spvc_compiler compiler = NULL;
	result = spvc_context_create_compiler(
	    context, SPVC_BACKEND_MSL, parsed_ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler);
	if (result != SPVC_SUCCESS) {
		spvc_context_destroy(context);
		return SPUDRESULT_GPU_SHADER_COMPILATION_FAILED;
	}

	spvc_compiler_options options = NULL;
	spvc_compiler_create_compiler_options(compiler, &options);
	// 2.4.0 encoded as (major * 10000 + minor * 100 + patch), matching
	// CompilerMSL::Options::make_msl_version - a recent-enough MSL version
	// for everything this backend's command-encoding side already uses
	// (base vertex/instance draws, argument buffers) without depending on
	// spvc_msl_make_version() being available in every SPIRV-Cross revision.
	spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_MSL_VERSION, 20400u);
	spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_MSL_PLATFORM, SPVC_MSL_PLATFORM_MACOS);
	spvc_compiler_install_compiler_options(compiler, options);

	const char *msl_source = NULL;
	result = spvc_compiler_compile(compiler, &msl_source);
	if (result != SPVC_SUCCESS || !msl_source) {
		printf("spudgpu: SPIR-V -> MSL cross-compile failed: %s\n", spvc_context_get_last_error_string(context));
		spvc_context_destroy(context);
		return SPUDRESULT_GPU_SHADER_COMPILATION_FAILED;
	}

	// -stringWithUTF8String: copies the bytes into a new NSString, so the
	// context (which owns msl_source's storage) can be torn down right
	// after this - nothing above needs to outlive this function.
	*out_msl_source = [NSString stringWithUTF8String:msl_source];
	spvc_context_destroy(context);
	return SPUD_SUCCESS;
}

// -----------------------------------------------------------------------
// Shader modules
// -----------------------------------------------------------------------

SPUDRESULT spudgpu_create_shader_module(
    spudgpu_device device,
    const spudgpu_shader_module_desc *desc,
    spudgpu_shader_module *out_module) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!desc)
		return SPUDRESULT_NULL_DESC;
	if (!out_module)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	if (!desc->spirv_code)
		return SPUDRESULT_GPU_NULL_SPIRV;
	if (desc->spirv_size == 0 || desc->spirv_size % 4 != 0)
		return SPUDRESULT_GPU_INVALID_SPIRV_ALIGNMENT;

	spudgpu_device_metal *device_metal = (spudgpu_device_metal *)device;
	SPUDRESULT sr                      = SPUD_SUCCESS;

	spudgpu_shader_module_metal *module_metal =
	    (spudgpu_shader_module_metal *)calloc(1, sizeof(spudgpu_shader_module_metal));
	if (!module_metal) {
		sr = SPUDRESULT_GENERAL_FAILURE;
		goto failedattempt;
	}
	module_metal->_parent_device = device_metal;
	module_metal->_desc          = *desc;

	{
		NSString *msl_source = nil;
		sr = spudgpumetal___internal_cross_compile_spirv_to_msl(desc->spirv_code, desc->spirv_size, &msl_source);
		if (sr != SPUD_SUCCESS)
			goto failedattempt;

		NSError *compile_error             = nil;
		MTLCompileOptions *compile_options = [[MTLCompileOptions alloc] init];
		module_metal->_library_mtl         = [device_metal->_device_mtl newLibraryWithSource:msl_source
		                                                                             options:compile_options
		                                                                               error:&compile_error];
		[compile_options release];
		if (!module_metal->_library_mtl) {
			printf(
			    "spudgpu: MSL compile failed: %s\n",
			    compile_error ? compile_error.localizedDescription.UTF8String : "(unknown)");
			sr = SPUDRESULT_GPU_SHADER_COMPILATION_FAILED;
			goto failedattempt;
		}

		// SPIRV-Cross's MSL backend always renames a SPIR-V module's entry
		// point to "main0" when the module has exactly one - the expected
		// shape for this project's per-stage-compiled shaders - since MSL
		// reserves the literal name "main". spudgpu_shader_pipeline_desc's/
		// spudgpu_compute_pipeline_desc's *_entry_point strings are accepted
		// for cross-backend API symmetry but not consulted on this backend.
		module_metal->_function_mtl = [module_metal->_library_mtl newFunctionWithName:@"main0"];
		if (!module_metal->_function_mtl) {
			sr = SPUDRESULT_GPU_SHADER_COMPILATION_FAILED;
			goto failedattempt;
		}
	}

	*out_module = (spudgpu_shader_module)module_metal;

	return sr;

failedattempt:
	if (module_metal) {
		if (module_metal->_function_mtl) {
			[module_metal->_function_mtl release];
		}
		if (module_metal->_library_mtl) {
			[module_metal->_library_mtl release];
		}
		free(module_metal);
	}
	return sr;
}

void spudgpu_destroy_shader_module(spudgpu_shader_module shader_module) {
	spudgpu_shader_module_metal *module_metal = (spudgpu_shader_module_metal *)shader_module;
	if (module_metal) {
		if (module_metal->_function_mtl) {
			[module_metal->_function_mtl release];
		}
		if (module_metal->_library_mtl) {
			[module_metal->_library_mtl release];
		}
		free(module_metal);
	}
}

// -----------------------------------------------------------------------
// Render (shader) pipelines
// -----------------------------------------------------------------------

static MTLVertexFormat spudgpumetal___internal_vertex_format(SPUDGPU_FORMAT format) {
	switch (format) {
	case SPUDGPU_FORMAT_R8_UNORM:            return MTLVertexFormatUCharNormalized;
	case SPUDGPU_FORMAT_R8_SNORM:            return MTLVertexFormatCharNormalized;
	case SPUDGPU_FORMAT_R8_UINT:             return MTLVertexFormatUChar;
	case SPUDGPU_FORMAT_R8_SINT:             return MTLVertexFormatChar;
	case SPUDGPU_FORMAT_R8G8_UNORM:          return MTLVertexFormatUChar2Normalized;
	case SPUDGPU_FORMAT_R8G8_SNORM:          return MTLVertexFormatChar2Normalized;
	case SPUDGPU_FORMAT_R8G8_UINT:           return MTLVertexFormatUChar2;
	case SPUDGPU_FORMAT_R8G8_SINT:           return MTLVertexFormatChar2;
	case SPUDGPU_FORMAT_R8G8B8A8_UNORM:      return MTLVertexFormatUChar4Normalized;
	case SPUDGPU_FORMAT_R8G8B8A8_SNORM:      return MTLVertexFormatChar4Normalized;
	case SPUDGPU_FORMAT_R8G8B8A8_UINT:       return MTLVertexFormatUChar4;
	case SPUDGPU_FORMAT_R8G8B8A8_SINT:       return MTLVertexFormatChar4;
	case SPUDGPU_FORMAT_R16_FLOAT:           return MTLVertexFormatHalf;
	case SPUDGPU_FORMAT_R16_UNORM:           return MTLVertexFormatUShortNormalized;
	case SPUDGPU_FORMAT_R16_SNORM:           return MTLVertexFormatShortNormalized;
	case SPUDGPU_FORMAT_R16_UINT:            return MTLVertexFormatUShort;
	case SPUDGPU_FORMAT_R16_SINT:            return MTLVertexFormatShort;
	case SPUDGPU_FORMAT_R16G16_FLOAT:        return MTLVertexFormatHalf2;
	case SPUDGPU_FORMAT_R16G16_UNORM:        return MTLVertexFormatUShort2Normalized;
	case SPUDGPU_FORMAT_R16G16_SNORM:        return MTLVertexFormatShort2Normalized;
	case SPUDGPU_FORMAT_R16G16_UINT:         return MTLVertexFormatUShort2;
	case SPUDGPU_FORMAT_R16G16_SINT:         return MTLVertexFormatShort2;
	case SPUDGPU_FORMAT_R16G16B16A16_FLOAT:  return MTLVertexFormatHalf4;
	case SPUDGPU_FORMAT_R16G16B16A16_UNORM:  return MTLVertexFormatUShort4Normalized;
	case SPUDGPU_FORMAT_R16G16B16A16_SNORM:  return MTLVertexFormatShort4Normalized;
	case SPUDGPU_FORMAT_R16G16B16A16_UINT:   return MTLVertexFormatUShort4;
	case SPUDGPU_FORMAT_R16G16B16A16_SINT:   return MTLVertexFormatShort4;
	case SPUDGPU_FORMAT_R32_FLOAT:           return MTLVertexFormatFloat;
	case SPUDGPU_FORMAT_R32_UINT:            return MTLVertexFormatUInt;
	case SPUDGPU_FORMAT_R32_SINT:            return MTLVertexFormatInt;
	case SPUDGPU_FORMAT_R32G32_FLOAT:        return MTLVertexFormatFloat2;
	case SPUDGPU_FORMAT_R32G32_UINT:         return MTLVertexFormatUInt2;
	case SPUDGPU_FORMAT_R32G32_SINT:         return MTLVertexFormatInt2;
	// Unlike MTLPixelFormat, MTLVertexFormat does have 3-component 32-bit
	// variants - vertex buffers aren't going through the texture unit, so
	// the same restriction spudgpumetal___internal_image_pixel_format
	// documents doesn't apply here.
	case SPUDGPU_FORMAT_R32G32B32_FLOAT:     return MTLVertexFormatFloat3;
	case SPUDGPU_FORMAT_R32G32B32_UINT:      return MTLVertexFormatUInt3;
	case SPUDGPU_FORMAT_R32G32B32_SINT:      return MTLVertexFormatInt3;
	case SPUDGPU_FORMAT_R32G32B32A32_FLOAT:  return MTLVertexFormatFloat4;
	case SPUDGPU_FORMAT_R32G32B32A32_UINT:   return MTLVertexFormatUInt4;
	case SPUDGPU_FORMAT_R32G32B32A32_SINT:   return MTLVertexFormatInt4;
	case SPUDGPU_FORMAT_R10G10B10A2_UNORM:   return MTLVertexFormatUInt1010102Normalized;
	default:                                 return MTLVertexFormatInvalid;
	}
}

static MTLCullMode spudgpumetal___internal_cull_mode(SPUDGPU_CULL_MODE mode) {
	switch (mode) {
	case SPUDGPU_CULL_MODE_FRONT: return MTLCullModeFront;
	case SPUDGPU_CULL_MODE_BACK:  return MTLCullModeBack;
	default:                      return MTLCullModeNone;
	}
}

static MTLCompareFunction spudgpumetal___internal_compare_function(SPUDGPU_COMPARE_OP op) {
	switch (op) {
	case SPUDGPU_COMPARE_OP_NEVER:            return MTLCompareFunctionNever;
	case SPUDGPU_COMPARE_OP_LESS:             return MTLCompareFunctionLess;
	case SPUDGPU_COMPARE_OP_EQUAL:            return MTLCompareFunctionEqual;
	case SPUDGPU_COMPARE_OP_LESS_OR_EQUAL:    return MTLCompareFunctionLessEqual;
	case SPUDGPU_COMPARE_OP_GREATER:          return MTLCompareFunctionGreater;
	case SPUDGPU_COMPARE_OP_NOT_EQUAL:        return MTLCompareFunctionNotEqual;
	case SPUDGPU_COMPARE_OP_GREATER_OR_EQUAL: return MTLCompareFunctionGreaterEqual;
	default:                                  return MTLCompareFunctionAlways;
	}
}

static MTLBlendFactor spudgpumetal___internal_blend_factor(SPUDGPU_BLEND_FACTOR factor) {
	switch (factor) {
	case SPUDGPU_BLEND_FACTOR_ONE:                 return MTLBlendFactorOne;
	case SPUDGPU_BLEND_FACTOR_SRC_ALPHA:           return MTLBlendFactorSourceAlpha;
	case SPUDGPU_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA: return MTLBlendFactorOneMinusSourceAlpha;
	case SPUDGPU_BLEND_FACTOR_DST_ALPHA:           return MTLBlendFactorDestinationAlpha;
	case SPUDGPU_BLEND_FACTOR_ONE_MINUS_DST_ALPHA: return MTLBlendFactorOneMinusDestinationAlpha;
	case SPUDGPU_BLEND_FACTOR_SRC_COLOR:           return MTLBlendFactorSourceColor;
	case SPUDGPU_BLEND_FACTOR_ONE_MINUS_SRC_COLOR: return MTLBlendFactorOneMinusSourceColor;
	case SPUDGPU_BLEND_FACTOR_DST_COLOR:           return MTLBlendFactorDestinationColor;
	case SPUDGPU_BLEND_FACTOR_ONE_MINUS_DST_COLOR: return MTLBlendFactorOneMinusDestinationColor;
	default:                                       return MTLBlendFactorZero;
	}
}

static MTLBlendOperation spudgpumetal___internal_blend_operation(SPUDGPU_BLEND_OP op) {
	switch (op) {
	case SPUDGPU_BLEND_OP_SUBTRACT:         return MTLBlendOperationSubtract;
	case SPUDGPU_BLEND_OP_REVERSE_SUBTRACT: return MTLBlendOperationReverseSubtract;
	case SPUDGPU_BLEND_OP_MIN:              return MTLBlendOperationMin;
	case SPUDGPU_BLEND_OP_MAX:              return MTLBlendOperationMax;
	default:                                return MTLBlendOperationAdd;
	}
}

SPUDRESULT spudgpu_create_shader_pipeline(
    spudgpu_device device,
    const spudgpu_shader_pipeline_desc *desc,
    spudgpu_shader_pipeline *out_pipeline) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!desc)
		return SPUDRESULT_NULL_DESC;
	if (!out_pipeline)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	if (!desc->vertex_module || !desc->fragment_module)
		return SPUDRESULT_GPU_VERTEX_AND_FRAGMENT_SHADER_REQUIRED;
	// Metal has no geometry-shader stage at all, and tessellation goes
	// through a completely different pipeline shape (post-tessellation
	// vertex functions + -drawPatches:/-drawIndexedPatches:, not a
	// tess-control/tess-eval stage pair) that this backend doesn't
	// implement - reject rather than silently dropping these stages.
	if (desc->geometry_module || desc->tess_control_module || desc->tess_eval_module)
		return SPUDRESULT_GPU_INVALID_SHADER_STAGE;

	spudgpu_shader_module_metal *vertex_module_metal   = (spudgpu_shader_module_metal *)desc->vertex_module;
	spudgpu_shader_module_metal *fragment_module_metal = (spudgpu_shader_module_metal *)desc->fragment_module;
	if (!vertex_module_metal->_function_mtl || !fragment_module_metal->_function_mtl)
		return SPUDRESULT_GPU_INVALID_SHADER_MODULE;

	MTLPixelFormat color_format = spudgpumetal___internal_image_pixel_format(desc->color_attachment_format);
	if (color_format == MTLPixelFormatInvalid)
		return SPUDRESULT_GPU_INVALID_FORMAT;
	MTLPixelFormat depth_format = MTLPixelFormatInvalid;
	if (desc->depth_format != SPUDGPU_FORMAT_UNKNOWN) {
		depth_format = spudgpumetal___internal_image_pixel_format(desc->depth_format);
		if (depth_format == MTLPixelFormatInvalid)
			return SPUDRESULT_GPU_INVALID_FORMAT;
	}

	spudgpu_device_metal *device_metal = (spudgpu_device_metal *)device;
	SPUDRESULT sr                      = SPUD_SUCCESS;

	spudgpu_shader_pipeline_metal *pipeline_metal =
	    (spudgpu_shader_pipeline_metal *)calloc(1, sizeof(spudgpu_shader_pipeline_metal));
	if (!pipeline_metal) {
		sr = SPUDRESULT_GENERAL_FAILURE;
		goto failedattempt;
	}
	pipeline_metal->_parent_device = device_metal;
	pipeline_metal->_desc          = *desc;

	{
		// descriptor_set_layouts is intentionally unconsumed here: unlike
		// Vulkan's VkPipelineLayout or D3D12's ID3D12RootSignature, building
		// an MTLRenderPipelineState needs no resource-layout object at all -
		// the (set, binding) -> flat-argument-index scheme lives entirely in
		// the already-cross-compiled MSL source (SPIRV-Cross's own default
		// assignment, for now - see spudgpumetaldescriptors.m, which will
		// need to agree with whatever scheme is used once it's implemented).
		MTLVertexDescriptor *vertex_desc = [MTLVertexDescriptor vertexDescriptor];
		for (uint32_t i = 0; i < desc->vertex_attribute_count; i++) {
			const spudgpu_vertex_attribute_desc *a = &desc->vertex_attributes[i];
			MTLVertexAttributeDescriptor *attr     = vertex_desc.attributes[a->location];
			attr.format                            = spudgpumetal___internal_vertex_format(a->format);
			attr.offset                            = a->offset;
			attr.bufferIndex                       = a->binding;
		}
		for (uint32_t i = 0; i < desc->vertex_binding_count; i++) {
			const spudgpu_vertex_binding_desc *b   = &desc->vertex_bindings[i];
			MTLVertexBufferLayoutDescriptor *layout = vertex_desc.layouts[b->binding];
			layout.stride                           = b->stride;
			layout.stepFunction = b->per_instance ? MTLVertexStepFunctionPerInstance : MTLVertexStepFunctionPerVertex;
			layout.stepRate     = 1;
		}

		MTLRenderPipelineDescriptor *pipeline_desc = [[MTLRenderPipelineDescriptor alloc] init];
		pipeline_desc.vertexFunction               = vertex_module_metal->_function_mtl;
		pipeline_desc.fragmentFunction              = fragment_module_metal->_function_mtl;
		pipeline_desc.vertexDescriptor              = vertex_desc;
		pipeline_desc.colorAttachments[0].pixelFormat = color_format;

		const spudgpu_blend_attachment_desc *blend = &desc->blend_attachment;
		pipeline_desc.colorAttachments[0].blendingEnabled = blend->blend_enable;
		if (blend->blend_enable) {
			pipeline_desc.colorAttachments[0].sourceRGBBlendFactor   = spudgpumetal___internal_blend_factor(blend->src_color_blend_factor);
			pipeline_desc.colorAttachments[0].destinationRGBBlendFactor = spudgpumetal___internal_blend_factor(blend->dst_color_blend_factor);
			pipeline_desc.colorAttachments[0].rgbBlendOperation      = spudgpumetal___internal_blend_operation(blend->color_blend_op);
			pipeline_desc.colorAttachments[0].sourceAlphaBlendFactor = spudgpumetal___internal_blend_factor(blend->src_alpha_blend_factor);
			pipeline_desc.colorAttachments[0].destinationAlphaBlendFactor = spudgpumetal___internal_blend_factor(blend->dst_alpha_blend_factor);
			pipeline_desc.colorAttachments[0].alphaBlendOperation    = spudgpumetal___internal_blend_operation(blend->alpha_blend_op);
		}

		if (depth_format != MTLPixelFormatInvalid) {
			pipeline_desc.depthAttachmentPixelFormat = depth_format;
			// Depth32Float_Stencil8 is the only depth/stencil combined
			// format this backend maps images to (see
			// spudgpumetalimage.m) - a texture in that format is
			// implicitly usable as a stencil attachment too.
			if (depth_format == MTLPixelFormatDepth32Float_Stencil8) {
				pipeline_desc.stencilAttachmentPixelFormat = depth_format;
			}
		}

		NSError *pipeline_error                     = nil;
		pipeline_metal->_render_pipeline_state_mtl  = [device_metal->_device_mtl
		    newRenderPipelineStateWithDescriptor:pipeline_desc
		                                    error:&pipeline_error];
		[pipeline_desc release];
		if (!pipeline_metal->_render_pipeline_state_mtl) {
			printf(
			    "spudgpu: MTLRenderPipelineState creation failed: %s\n",
			    pipeline_error ? pipeline_error.localizedDescription.UTF8String : "(unknown)");
			sr = SPUDRESULT_GPU_SHADER_COMPILATION_FAILED;
			goto failedattempt;
		}

		// Vulkan/D3D12 bake depth/stencil test config into the same
		// monolithic pipeline object as everything above; Metal keeps it as
		// a wholly separate native object - see spudgpu_cmd_bind_pipeline in
		// spudgpumetalcommand.m, which binds both together.
		MTLDepthStencilDescriptor *depth_stencil_desc = [[MTLDepthStencilDescriptor alloc] init];
		depth_stencil_desc.depthCompareFunction =
		    desc->depth_test_enable ? spudgpumetal___internal_compare_function(desc->depth_compare_op) : MTLCompareFunctionAlways;
		depth_stencil_desc.depthWriteEnabled = desc->depth_test_enable && desc->depth_write_enable;
		pipeline_metal->_depth_stencil_state_mtl =
		    [device_metal->_device_mtl newDepthStencilStateWithDescriptor:depth_stencil_desc];
		[depth_stencil_desc release];
		if (!pipeline_metal->_depth_stencil_state_mtl) {
			sr = SPUDRESULT_GPU_SHADER_COMPILATION_FAILED;
			goto failedattempt;
		}
	}

	*out_pipeline = (spudgpu_shader_pipeline)pipeline_metal;

	return sr;

failedattempt:
	if (pipeline_metal) {
		if (pipeline_metal->_depth_stencil_state_mtl) {
			[pipeline_metal->_depth_stencil_state_mtl release];
		}
		if (pipeline_metal->_render_pipeline_state_mtl) {
			[pipeline_metal->_render_pipeline_state_mtl release];
		}
		free(pipeline_metal);
	}
	return sr;
}

void spudgpu_destroy_shader_pipeline(spudgpu_shader_pipeline pipeline) {
	spudgpu_shader_pipeline_metal *pipeline_metal = (spudgpu_shader_pipeline_metal *)pipeline;
	if (pipeline_metal) {
		if (pipeline_metal->_depth_stencil_state_mtl) {
			[pipeline_metal->_depth_stencil_state_mtl release];
		}
		if (pipeline_metal->_render_pipeline_state_mtl) {
			[pipeline_metal->_render_pipeline_state_mtl release];
		}
		free(pipeline_metal);
	}
}

// -----------------------------------------------------------------------
// Compute pipelines
// -----------------------------------------------------------------------

SPUDRESULT spudgpu_create_compute_pipeline(
    spudgpu_device device,
    const spudgpu_compute_pipeline_desc *desc,
    spudgpu_compute_pipeline *out_pipeline) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!desc)
		return SPUDRESULT_NULL_DESC;
	if (!out_pipeline)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	if (!desc->compute_module)
		return SPUDRESULT_GPU_INVALID_COMPUTE_MODULE;

	spudgpu_shader_module_metal *compute_module_metal = (spudgpu_shader_module_metal *)desc->compute_module;
	if (!compute_module_metal->_function_mtl)
		return SPUDRESULT_GPU_INVALID_SHADER_MODULE;

	spudgpu_device_metal *device_metal = (spudgpu_device_metal *)device;
	SPUDRESULT sr                      = SPUD_SUCCESS;

	spudgpu_compute_pipeline_metal *pipeline_metal =
	    (spudgpu_compute_pipeline_metal *)calloc(1, sizeof(spudgpu_compute_pipeline_metal));
	if (!pipeline_metal) {
		sr = SPUDRESULT_GENERAL_FAILURE;
		goto failedattempt;
	}
	pipeline_metal->_parent_device = device_metal;
	pipeline_metal->_desc          = *desc;

	{
		// Same reasoning as spudgpu_create_shader_pipeline above -
		// descriptor_set_layouts needs no consumption here either; a
		// MTLComputePipelineState needs no resource-layout object.
		NSError *pipeline_error = nil;
		pipeline_metal->_compute_pipeline_state_mtl =
		    [device_metal->_device_mtl newComputePipelineStateWithFunction:compute_module_metal->_function_mtl
		                                                              error:&pipeline_error];
		if (!pipeline_metal->_compute_pipeline_state_mtl) {
			printf(
			    "spudgpu: MTLComputePipelineState creation failed: %s\n",
			    pipeline_error ? pipeline_error.localizedDescription.UTF8String : "(unknown)");
			sr = SPUDRESULT_GPU_SHADER_COMPILATION_FAILED;
			goto failedattempt;
		}
	}

	*out_pipeline = (spudgpu_compute_pipeline)pipeline_metal;

	return sr;

failedattempt:
	if (pipeline_metal) {
		if (pipeline_metal->_compute_pipeline_state_mtl) {
			[pipeline_metal->_compute_pipeline_state_mtl release];
		}
		free(pipeline_metal);
	}
	return sr;
}

void spudgpu_destroy_compute_pipeline(spudgpu_compute_pipeline pipeline) {
	spudgpu_compute_pipeline_metal *pipeline_metal = (spudgpu_compute_pipeline_metal *)pipeline;
	if (pipeline_metal) {
		if (pipeline_metal->_compute_pipeline_state_mtl) {
			[pipeline_metal->_compute_pipeline_state_mtl release];
		}
		free(pipeline_metal);
	}
}

SPUDRESULT spudgpu_get_compute_pipeline_desc(
    spudgpu_compute_pipeline pipeline,
    spudgpu_compute_pipeline_desc *out_desc) {
	spudgpu_compute_pipeline_metal *pipeline_metal = (spudgpu_compute_pipeline_metal *)pipeline;
	if (!pipeline_metal)
		return SPUDRESULT_GPU_INVALID_COMPUTE_PIPELINE;
	if (!out_desc)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	*out_desc = pipeline_metal->_desc;
	return SPUD_SUCCESS;
}

#endif // SPUDGPU_COMPILE_METAL_API
