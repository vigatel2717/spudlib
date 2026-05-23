
#if SPUDGPU_COMPILE_VULKAN_API

#include "spudgpuvulkan.h"
#include "spudgpu.h"
#include <stdlib.h>

VkPrimitiveTopology spudgpuvulkan___primitive_topology(SPUDGPU_PRIMITIVE_TOPOLOGY topology) {
    switch (topology) {
        case SPUDGPU_PRIMITIVE_TOPOLOGY_POINT_LIST: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case SPUDGPU_PRIMITIVE_TOPOLOGY_LINE_STRIP: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case SPUDGPU_PRIMITIVE_TOPOLOGY_LINE_LIST: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case SPUDGPU_PRIMITIVE_TOPOLOGY_PATCH_LIST: return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
        case SPUDGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case SPUDGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        default:
            return SPUDGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

VkShaderStageFlagBits spudgpuvulkan___shader_stage_flag_internal(
    SPUDGPU_SHADER_STAGE stage) {
    switch (stage) {
        case SPUDGPU_SHADER_STAGE_VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
        case SPUDGPU_SHADER_STAGE_FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case SPUDGPU_SHADER_STAGE_COMPUTE: return VK_SHADER_STAGE_COMPUTE_BIT;
        case SPUDGPU_SHADER_STAGE_GEOMETRY: return VK_SHADER_STAGE_GEOMETRY_BIT;
        case SPUDGPU_SHADER_STAGE_TESSELLATION_CONTROL: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case SPUDGPU_SHADER_STAGE_TESSELLATION_EVALUATION: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        default: return (VkShaderStageFlagBits) 0;
    }
}

VkFormat spudgpuvulkan___vertex_attr_format_internal(SPUDGPU_FORMAT fmt) {
    return convert_spud_to_vulkan_format((VkFormat) fmt);
}

VkResult spudgpuvulkan___create_shader_module_internal(
    VkDevice vk_device,
    const uint32_t *spirv_code,
    size_t spirv_size,
    VkShaderModule *out_module) {
    VkShaderModuleCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirv_size;
    createInfo.pCode = spirv_code;
    return vkCreateShaderModule(vk_device, &createInfo, NULL, out_module);
}

VkResult spudgpuvulkan___create_render_pass_internal(
    VkDevice vk_device,
    const spudgpu_shader_pipeline_desc *desc,
    VkRenderPass *out_render_pass) {
    // Colour attachment
    VkAttachmentDescription attachments[2] = {0};
    uint32_t attachment_count = 0;

    attachments[0].format = convert_spud_to_vulkan_format((VkFormat) desc->color_attachment_format);
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachment_count++;

    VkAttachmentReference color_ref = {0};
    color_ref.attachment = 0;
    color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Optional depth attachment
    VkAttachmentReference depth_ref = {0};
    int has_depth = (desc->depth_format != SPUDGPU_FORMAT_UNKNOWN);
    if (has_depth) {
        attachments[1].format = convert_spud_to_vulkan_format((VkFormat) desc->depth_format);
        attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachment_count++;

        depth_ref.attachment = 1;
        depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_ref;
    subpass.pDepthStencilAttachment = has_depth ? &depth_ref : NULL;

    // Single subpass dependency: external → colour output stage
    VkSubpassDependency dependency = {0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = {0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = attachment_count;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    return vkCreateRenderPass(vk_device, &renderPassInfo, NULL, out_render_pass);
}

#if __cplusplus
extern "C" {



#endif

spudgpu_shader_module spudgpu_create_shader_module(
    spudgpu_device device,
    const spudgpu_shader_module_desc *desc) {
    if (!(device && desc)) return NULL;
    if (!(desc->spirv_code && desc->spirv_size)) return NULL;
    if (desc->spirv_size % 4 != 0) return NULL; // SPIR-V must be 4-byte aligned

    spudgpu_shader_module_vulkan result = {0};
    result._device = *((spudgpu_device_vulkan *) device);
    result._stage_vk = spudgpuvulkan___shader_stage_flag_internal(desc->stage);

    if (result._stage_vk == 0) return NULL; // Unknown shader stage

    VkDevice vk_device = result._device._logical_device_vk;

    if (spudgpuvulkan___create_shader_module_internal(
            vk_device,
            (const uint32_t *) desc->spirv_code,
            desc->spirv_size,
            &result._shader_module_vk) != VK_SUCCESS) {
        return NULL;
    }

    spudgpu_shader_module_vulkan *pResult = malloc(sizeof(spudgpu_shader_module_vulkan));
    memcpy(pResult, &result, sizeof(spudgpu_shader_module_vulkan));
    return (spudgpu_shader_module) pResult;
}

void spudgpu_destroy_shader_module(spudgpu_device device, spudgpu_shader_module shader_module) {
    if (!(device && shader_module)) return;
    spudgpu_shader_module_vulkan *vkModule = (spudgpu_shader_module_vulkan *) shader_module;
    vkDestroyShaderModule(vkModule->_device._logical_device_vk, vkModule->_shader_module_vk, NULL);
    free(vkModule);
}

spudgpu_shader_pipeline spudgpu_create_shader_pipeline(
    spudgpu_device device,
    const spudgpu_shader_pipeline_desc *desc) {
    if (!(device && desc)) return NULL;

    // Validate: at minimum a vertex and fragment shader are required for graphics
    if (!(desc->vertex_module && desc->fragment_module)) return NULL;

    spudgpu_shader_pipeline_vulkan result = {0};
    result._device = *((spudgpu_device_vulkan *) device);
    result._desc = *desc;

    VkDevice vk_device = result._device._logical_device_vk;

    // ------------------------------------------------------------------
    // 1. Shader stages
    // ------------------------------------------------------------------
    VkPipelineShaderStageCreateInfo shader_stages[6] = {0};
    uint32_t stage_count = 0;

    // Vertex stage (required)
    {
        spudgpu_shader_module_vulkan *vert = (spudgpu_shader_module_vulkan *) desc->vertex_module;
        shader_stages[stage_count].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[stage_count].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stages[stage_count].module = vert->_shader_module_vk;
        shader_stages[stage_count].pName = desc->vertex_entry_point ? desc->vertex_entry_point : "main";
        stage_count++;
    }

    // Fragment stage (required for graphics pipelines)
    {
        spudgpu_shader_module_vulkan *frag = (spudgpu_shader_module_vulkan *) desc->fragment_module;
        shader_stages[stage_count].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[stage_count].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stages[stage_count].module = frag->_shader_module_vk;
        shader_stages[stage_count].pName = desc->fragment_entry_point ? desc->fragment_entry_point : "main";
        stage_count++;
    }

    // Optional geometry stage
    if (desc->geometry_module) {
        spudgpu_shader_module_vulkan *geom = (spudgpu_shader_module_vulkan *) desc->geometry_module;
        shader_stages[stage_count].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[stage_count].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        shader_stages[stage_count].module = geom->_shader_module_vk;
        shader_stages[stage_count].pName = desc->geometry_entry_point ? desc->geometry_entry_point : "main";
        stage_count++;
    }

    // Optional tessellation control stage
    if (desc->tess_control_module) {
        spudgpu_shader_module_vulkan *tc = (spudgpu_shader_module_vulkan *) desc->tess_control_module;
        shader_stages[stage_count].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[stage_count].stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        shader_stages[stage_count].module = tc->_shader_module_vk;
        shader_stages[stage_count].pName = desc->tess_control_entry_point ? desc->tess_control_entry_point : "main";
        stage_count++;
    }

    // Optional tessellation evaluation stage
    if (desc->tess_eval_module) {
        spudgpu_shader_module_vulkan *te = (spudgpu_shader_module_vulkan *) desc->tess_eval_module;
        shader_stages[stage_count].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[stage_count].stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        shader_stages[stage_count].module = te->_shader_module_vk;
        shader_stages[stage_count].pName = desc->tess_eval_entry_point ? desc->tess_eval_entry_point : "main";
        stage_count++;
    }

    // ------------------------------------------------------------------
    // 2. Vertex input state  (binding 0, per-vertex, all attributes)
    // ------------------------------------------------------------------
    // We support up to SPUDGPU_MAX_VERTEX_ATTRIBUTES attributes described
    // in desc->vertex_attributes / desc->vertex_attribute_count.
    VkVertexInputAttributeDescription attr_descs[SPUDGPU_MAX_VERTEX_ATTRIBUTES] = {0};
    for (uint32_t i = 0; i < desc->vertex_attribute_count; i++) {
        attr_descs[i].location = desc->vertex_attributes[i].location;
        attr_descs[i].binding = desc->vertex_attributes[i].binding;
        attr_descs[i].format = spudgpuvulkan___vertex_attr_format_internal(
            desc->vertex_attributes[i].format);
        attr_descs[i].offset = desc->vertex_attributes[i].offset;
    }

    VkVertexInputBindingDescription binding_descs[SPUDGPU_MAX_VERTEX_BINDINGS] = {0};
    for (uint32_t i = 0; i < desc->vertex_binding_count; i++) {
        binding_descs[i].binding = desc->vertex_bindings[i].binding;
        binding_descs[i].stride = desc->vertex_bindings[i].stride;
        binding_descs[i].inputRate = desc->vertex_bindings[i].per_instance
                                         ? VK_VERTEX_INPUT_RATE_INSTANCE
                                         : VK_VERTEX_INPUT_RATE_VERTEX;
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = desc->vertex_binding_count;
    vertexInputInfo.pVertexBindingDescriptions = binding_descs;
    vertexInputInfo.vertexAttributeDescriptionCount = desc->vertex_attribute_count;
    vertexInputInfo.pVertexAttributeDescriptions = attr_descs;

    // ------------------------------------------------------------------
    // 3. Input assembly
    // ------------------------------------------------------------------
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = spudgpuvulkan___primitive_topology(desc->primitive_topology);
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // ------------------------------------------------------------------
    // 4. Viewport / scissor  (dynamic; set via command list at draw time)
    // ------------------------------------------------------------------
    VkPipelineViewportStateCreateInfo viewportState = {0};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    // pViewports / pScissors intentionally left NULL – declared dynamic below.

    // ------------------------------------------------------------------
    // 5. Rasterizer
    // ------------------------------------------------------------------
    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = desc->wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = (VkCullModeFlags) desc->cull_mode;
    rasterizer.frontFace = desc->front_face_ccw
                               ? VK_FRONT_FACE_COUNTER_CLOCKWISE
                               : VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // ------------------------------------------------------------------
    // 6. Multisampling  (1× — no MSAA for now)
    // ------------------------------------------------------------------
    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // ------------------------------------------------------------------
    // 7. Depth / stencil
    // ------------------------------------------------------------------
    VkPipelineDepthStencilStateCreateInfo depthStencil = {0};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = desc->depth_test_enable ? VK_TRUE : VK_FALSE;
    depthStencil.depthWriteEnable = desc->depth_write_enable ? VK_TRUE : VK_FALSE;
    depthStencil.depthCompareOp = (VkCompareOp) desc->depth_compare_op;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    // ------------------------------------------------------------------
    // 8. Colour blend attachment  (standard alpha blending or opaque)
    // ------------------------------------------------------------------
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
                                          | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    if (desc->blend_attachment.blend_enable) {
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    } else {
        colorBlendAttachment.blendEnable = VK_FALSE;
    }

    VkPipelineColorBlendStateCreateInfo colorBlending = {0};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // ------------------------------------------------------------------
    // 9. Dynamic state – viewport + scissor changed per frame via commands
    // ------------------------------------------------------------------
    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamic_states;

    // ------------------------------------------------------------------
    // 10. Pipeline layout  (push constant ranges + descriptor set layouts)
    // ------------------------------------------------------------------
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    VkPushConstantRange push_constant_ranges[SPUDGPU_MAX_PUSH_CONSTANT_RANGES] = {0};
    for (uint32_t i = 0; i < desc->push_constant_range_count; i++) {
        push_constant_ranges[i].stageFlags =
                (VkShaderStageFlags) spudgpuvulkan___shader_stage_flag_internal(
                    desc->push_constant_ranges[i].stage_flags);
        push_constant_ranges[i].offset = desc->push_constant_ranges[i].offset;
        push_constant_ranges[i].size = desc->push_constant_ranges[i].size;
    }
    pipelineLayoutInfo.pushConstantRangeCount = desc->push_constant_range_count;
    pipelineLayoutInfo.pPushConstantRanges = push_constant_ranges;

    VkDescriptorSetLayout vk_layouts[SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS];
    for (uint32_t i = 0; i < desc->descriptor_set_layout_count; i++) {
        spudgpu_descriptor_set_layout_vulkan *vk_layout =
                (spudgpu_descriptor_set_layout_vulkan *) desc->descriptor_set_layouts[i];
        vk_layouts[i] = vk_layout->_layout_vk;
    }

    pipelineLayoutInfo.setLayoutCount = desc->descriptor_set_layout_count;
    pipelineLayoutInfo.pSetLayouts = vk_layouts;

    if (vkCreatePipelineLayout(vk_device, &pipelineLayoutInfo, NULL,
                               &result._pipeline_layout_vk) != VK_SUCCESS) {
        return NULL;
    }

    // ------------------------------------------------------------------
    // 11. Render pass
    // ------------------------------------------------------------------
    if (spudgpuvulkan___create_render_pass_internal(vk_device, desc,
                                                    &result._render_pass_vk) != VK_SUCCESS) {
        vkDestroyPipelineLayout(vk_device, result._pipeline_layout_vk, NULL);
        return NULL;
    }

    // ------------------------------------------------------------------
    // 12. Tessellation state (only wired up when both tess stages present)
    // ------------------------------------------------------------------
    VkPipelineTessellationStateCreateInfo tessellationState = {0};
    tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellationState.patchControlPoints = desc->patch_control_points ? desc->patch_control_points : 3;

    // ------------------------------------------------------------------
    // 13. Assemble and create the graphics pipeline
    // ------------------------------------------------------------------
    VkGraphicsPipelineCreateInfo pipelineInfo = {0};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = stage_count;
    pipelineInfo.pStages = shader_stages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = result._pipeline_layout_vk;
    pipelineInfo.renderPass = result._render_pass_vk;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    // Tessellation state is only meaningful when both tess stages exist.
    if (desc->tess_control_module && desc->tess_eval_module) {
        pipelineInfo.pTessellationState = &tessellationState;
    }

    if (vkCreateGraphicsPipelines(vk_device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL,
                                  &result._pipeline_vk) != VK_SUCCESS) {
        vkDestroyRenderPass(vk_device, result._render_pass_vk, NULL);
        vkDestroyPipelineLayout(vk_device, result._pipeline_layout_vk, NULL);
        return NULL;
    }

    // If all successful, return a heap-allocated copy.
    spudgpu_shader_pipeline_vulkan *pResult = malloc(sizeof(spudgpu_shader_pipeline_vulkan));
    memcpy(pResult, &result, sizeof(spudgpu_shader_pipeline_vulkan));
    return (spudgpu_shader_pipeline) pResult;
}

spudgpu_shader_pipeline_desc spudgpu_get_shader_pipeline_desc(spudgpu_shader_pipeline pipeline) {
    if (!pipeline) return (spudgpu_shader_pipeline_desc){0};
    return ((spudgpu_shader_pipeline_vulkan *) pipeline)->_desc;
}

void spudgpu_destroy_shader_pipeline(spudgpu_device device, spudgpu_shader_pipeline pipeline) {
    if (!(device && pipeline)) return;
    spudgpu_shader_pipeline_vulkan *vkPipeline = (spudgpu_shader_pipeline_vulkan *) pipeline;
    VkDevice vk_device = vkPipeline->_device._logical_device_vk;
    vkDestroyPipeline(vk_device, vkPipeline->_pipeline_vk, NULL);
    vkDestroyPipelineLayout(vk_device, vkPipeline->_pipeline_layout_vk, NULL);
    vkDestroyRenderPass(vk_device, vkPipeline->_render_pass_vk, NULL);
    free(vkPipeline);
}

spudgpu_compute_pipeline spudgpu_create_compute_pipeline(
    spudgpu_device device,
    const spudgpu_compute_pipeline_desc *desc) {
    if (!(device && desc)) return NULL;
    if (!desc->compute_module) return NULL;

    spudgpu_compute_pipeline_vulkan result = {0};
    result._device = *((spudgpu_device_vulkan *) device);
    result._desc = *desc;

    VkDevice vk_device = result._device._logical_device_vk;
    spudgpu_shader_module_vulkan *comp = (spudgpu_shader_module_vulkan *) desc->compute_module;

    // Pipeline layout
    VkPushConstantRange push_constant_ranges[SPUDGPU_MAX_PUSH_CONSTANT_RANGES] = {0};
    for (uint32_t i = 0; i < desc->push_constant_range_count; i++) {
        push_constant_ranges[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        push_constant_ranges[i].offset = desc->push_constant_ranges[i].offset;
        push_constant_ranges[i].size = desc->push_constant_ranges[i].size;
    }

    VkPipelineLayoutCreateInfo layoutInfo = {0};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = desc->descriptor_set_layout_count;
    layoutInfo.pSetLayouts = (const VkDescriptorSetLayout *) desc->descriptor_set_layouts;
    layoutInfo.pushConstantRangeCount = desc->push_constant_range_count;
    layoutInfo.pPushConstantRanges = push_constant_ranges;

    if (vkCreatePipelineLayout(
            vk_device, &layoutInfo, NULL,
            &result._pipeline_layout_vk) != VK_SUCCESS) {
        return NULL;
    }

    // Compute pipeline
    VkComputePipelineCreateInfo pipelineInfo = {0};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = result._pipeline_layout_vk;
    pipelineInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineInfo.stage.module = comp->_shader_module_vk;
    pipelineInfo.stage.pName = desc->compute_entry_point ? desc->compute_entry_point : "main";
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if (vkCreateComputePipelines(
            vk_device, VK_NULL_HANDLE, 1,
            &pipelineInfo, NULL, &result._pipeline_vk) != VK_SUCCESS) {
        vkDestroyPipelineLayout(vk_device, result._pipeline_layout_vk, NULL);
        return NULL;
    }

    spudgpu_compute_pipeline_vulkan *pResult = malloc(sizeof(spudgpu_compute_pipeline_vulkan));
    memcpy(pResult, &result, sizeof(spudgpu_compute_pipeline_vulkan));
    return (spudgpu_compute_pipeline) pResult;
}

void spudgpu_destroy_compute_pipeline(spudgpu_device device, spudgpu_compute_pipeline pipeline) {
    if (!(device && pipeline)) return;
    spudgpu_compute_pipeline_vulkan *vkPipeline = (spudgpu_compute_pipeline_vulkan *) pipeline;
    VkDevice vk_device = vkPipeline->_device._logical_device_vk;
    vkDestroyPipeline(vk_device, vkPipeline->_pipeline_vk, NULL);
    vkDestroyPipelineLayout(vk_device, vkPipeline->_pipeline_layout_vk, NULL);
    free(vkPipeline);
}

#if __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_VULKAN_API
