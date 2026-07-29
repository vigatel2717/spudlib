
#if SPUDGPU_COMPILE_VULKAN_API

#include <vulkan/vulkan.h>
#include "spudgpu.h"
#include "spudgpuvulkan.h"
#include <stdlib.h>

#if __cplusplus
extern "C" {

#endif

static VkDescriptorType spudgpuvulkan___descriptor_type_internal(
    SPUDGPU_DESCRIPTOR_TYPE type) {
    switch (type) {
        case SPUDGPU_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case SPUDGPU_DESCRIPTOR_TYPE_STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case SPUDGPU_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case SPUDGPU_DESCRIPTOR_TYPE_SAMPLED_IMAGE: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case SPUDGPU_DESCRIPTOR_TYPE_SAMPLER: return VK_DESCRIPTOR_TYPE_SAMPLER;
        case SPUDGPU_DESCRIPTOR_TYPE_STORAGE_IMAGE: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        default: return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }
}

SPUDRESULT spudgpu_create_descriptor_set_layout(
    spudgpu_device device,
    const spudgpu_descriptor_set_layout_desc *desc,
	spudgpu_descriptor_set_layout *out_layout) {
    if (!device) return SPUDRESULT_GPU_INVALID_DEVICE;
    if (!desc) return SPUDRESULT_NULL_DESC;
    if (desc->binding_count > SPUDGPU_MAX_DESCRIPTOR_BINDINGS_PER_SET) return SPUDRESULT_GPU_TOO_MANY_DESCRIPTOR_BINDINGS;
    if (!out_layout) return SPUD_SUCCESS;

    spudgpu_descriptor_set_layout_vulkan result = {0};
    result._device = *device;
    result._desc = *desc;

    VkDevice vk_device = result._device._logical_device_vk;

    // Translate each binding entry to VkDescriptorSetLayoutBinding.
    VkDescriptorSetLayoutBinding vk_bindings[SPUDGPU_MAX_DESCRIPTOR_BINDINGS_PER_SET] = {0};
    for (uint32_t i = 0; i < desc->binding_count; i++) {
        const spudgpu_descriptor_binding_desc *b = &desc->bindings[i];

        VkDescriptorType vk_type = spudgpuvulkan___descriptor_type_internal(b->descriptor_type);
        if (vk_type == VK_DESCRIPTOR_TYPE_MAX_ENUM) return SPUDRESULT_GPU_CANNOT_RESOLVE_API_SPECIFIC_DESCRIPTOR_TYPE; // Unknown type — bail early

        vk_bindings[i].binding = b->binding;
        vk_bindings[i].descriptorType = vk_type;
        vk_bindings[i].descriptorCount = b->count;
        vk_bindings[i].stageFlags = (VkShaderStageFlags) b->stage_flags;
        vk_bindings[i].pImmutableSamplers = NULL; // Dynamic samplers only for now
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = desc->binding_count;
    layoutInfo.pBindings = vk_bindings;

    if (vkCreateDescriptorSetLayout(
        vk_device, &layoutInfo, NULL,
        &result._layout_vk) != VK_SUCCESS) {
        return SPUDRESULT_API_SPECIFIC_FAILURE;
    }

#if _DEBUG
    // If VK_EXT_debug_utils is loaded, tag the layout object for RenderDoc / validation layers.
    if (desc->debug_name) {
        VkDebugUtilsObjectNameInfoEXT nameInfo = {0};
        nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        nameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
        nameInfo.objectHandle = (uint64_t) result._layout_vk;
        nameInfo.pObjectName = desc->debug_name;
        // Best-effort: ignore the return value — not all drivers load the extension.
        PFN_vkSetDebugUtilsObjectNameEXT fn =
                (PFN_vkSetDebugUtilsObjectNameEXT) vkGetDeviceProcAddr(
                    vk_device, "vkSetDebugUtilsObjectNameEXT");
        if (fn) fn(vk_device, &nameInfo);
    }
#endif

    spudgpu_descriptor_set_layout_vulkan *pResult =
            malloc(sizeof(spudgpu_descriptor_set_layout_vulkan));
    memcpy(pResult, &result, sizeof(spudgpu_descriptor_set_layout_vulkan));
    *out_layout = pResult;
    return SPUD_SUCCESS;
}

void spudgpu_destroy_descriptor_set_layout(
    spudgpu_descriptor_set_layout layout) {
    if (!layout) return;
    vkDestroyDescriptorSetLayout(
        layout->_device._logical_device_vk,
        layout->_layout_vk,
        NULL);
    free(layout);
}

SPUDRESULT spudgpu_create_descriptor_pool(
    spudgpu_device device,
    const spudgpu_descriptor_pool_desc *desc,
	spudgpu_descriptor_pool *out_pool) {
    if (!device) return SPUDRESULT_GPU_INVALID_DEVICE;
    if (!desc) return SPUDRESULT_NULL_DESC;
    if (desc->pool_size_count > SPUDGPU_MAX_DESCRIPTOR_POOL_SIZES) return SPUDRESULT_GPU_TOO_MANY_DESCRIPTOR_POOLS;
    if (!out_pool) return SPUD_SUCCESS;

    spudgpu_descriptor_pool_vulkan result = {0};
    result._device = *device;
    result._desc = *desc;

    VkDevice vk_device = result._device._logical_device_vk;

    VkDescriptorPoolSize vk_pool_sizes[SPUDGPU_MAX_DESCRIPTOR_POOL_SIZES] = {0};
    for (uint32_t i = 0; i < desc->pool_size_count; i++) {
        VkDescriptorType vk_type =
                spudgpuvulkan___descriptor_type_internal(desc->pool_sizes[i].descriptor_type);
        if (vk_type == VK_DESCRIPTOR_TYPE_MAX_ENUM) return SPUDRESULT_GPU_CANNOT_RESOLVE_API_SPECIFIC_DESCRIPTOR_TYPE;

        vk_pool_sizes[i].type = vk_type;
        vk_pool_sizes[i].descriptorCount = desc->pool_sizes[i].count;
    }

    VkDescriptorPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    // FREE_DESCRIPTOR_SET_BIT is not set: individual free is not supported,
    // which is the intended usage — reset the whole pool each frame instead.
    poolInfo.maxSets = desc->max_sets;
    poolInfo.poolSizeCount = desc->pool_size_count;
    poolInfo.pPoolSizes = vk_pool_sizes;

    if (vkCreateDescriptorPool(
        vk_device, &poolInfo, NULL,
        &result._pool_vk) != VK_SUCCESS) {
        return SPUDRESULT_API_SPECIFIC_FAILURE;
    }

#if _DEBUG
    if (desc->debug_name) {
        VkDebugUtilsObjectNameInfoEXT nameInfo = {0};
        nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        nameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_POOL;
        nameInfo.objectHandle = (uint64_t) result._pool_vk;
        nameInfo.pObjectName = desc->debug_name;
        SPUDRESULT sr = SPUD_SUCCESS;
        PFN_vkSetDebugUtilsObjectNameEXT fn =
                (PFN_vkSetDebugUtilsObjectNameEXT) vkGetDeviceProcAddr(
                    vk_device, "vkSetDebugUtilsObjectNameEXT");
		if (fn)
			if (fn(vk_device, &nameInfo))
				sr = SPUDRESULT_API_SPECIFIC_FAILURE;
		if (sr != SPUD_SUCCESS)
			return sr;
	}
#endif

    spudgpu_descriptor_pool_vulkan *pResult = malloc(sizeof(spudgpu_descriptor_pool_vulkan));
    memcpy(pResult, &result, sizeof(spudgpu_descriptor_pool_vulkan));
    *out_pool = pResult;
    return SPUD_SUCCESS;
}

void spudgpu_reset_descriptor_pool(
    spudgpu_descriptor_pool pool) {
    if (!pool) return;
    // Bulk-free all sets allocated from this pool. The 'flags' parameter is reserved and must be 0.
	vkResetDescriptorPool(pool->_device._logical_device_vk, pool->_pool_vk, 0);
}

void spudgpu_destroy_descriptor_pool(
    spudgpu_descriptor_pool pool) {
    if (!pool) return;
    // vkDestroyDescriptorPool implicitly frees all descriptor sets allocated from it.
    vkDestroyDescriptorPool(
        pool->_device._logical_device_vk,
        pool->_pool_vk,
        NULL);
    free(pool);
}

SPUDRESULT spudgpu_create_descriptor_sets(
    spudgpu_device device,
    const spudgpu_descriptor_set_desc *desc,
    spudgpu_descriptor_set *out_sets) {
    if (!device) return SPUDRESULT_GPU_INVALID_DEVICE;
    if (!desc) return SPUDRESULT_NULL_DESC;
    if (!desc->pool) return SPUDRESULT_GPU_INVALID_DESCRIPTOR_POOL;
	if (!desc->set_count)
		return SPUDRESULT_GPU_ZERO_DESCRIPTOR_SET_LAYOUTS;
	if (desc->set_count > SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS)
		return SPUDRESULT_GPU_TOO_MANY_DESCRIPTOR_SET_LAYOUTS;

    VkDevice vk_device = desc->pool->_device._logical_device_vk;

    // Gather the raw VkDescriptorSetLayout handles from the typed wrappers.
    VkDescriptorSetLayout vk_layouts[SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS] = {0};
    for (uint32_t i = 0; i < desc->set_count; i++) {
        if (!desc->set_layouts[i]) return SPUDRESULT_GPU_INVALID_DESCRIPTOR_SET_LAYOUT;
        spudgpu_descriptor_set_layout_vulkan *vkLayout =
                (spudgpu_descriptor_set_layout_vulkan *) desc->set_layouts[i];
        vk_layouts[i] = vkLayout->_layout_vk;
    }

    // Allocate all sets in a single driver call.
    VkDescriptorSet vk_sets[SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS] = {0};

    VkDescriptorSetAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = desc->pool->_pool_vk;
    allocInfo.descriptorSetCount = desc->set_count;
    allocInfo.pSetLayouts = vk_layouts;

    if (vkAllocateDescriptorSets(vk_device, &allocInfo, vk_sets) != VK_SUCCESS) {
        return SPUDRESULT_API_SPECIFIC_FAILURE;
    }

    // Wrap each raw VkDescriptorSet in a heap-allocated SpudGPU handle.
    for (uint32_t i = 0; i < desc->set_count; i++) {
        spudgpu_descriptor_set_vulkan *pSet =
                malloc(sizeof(spudgpu_descriptor_set_vulkan));
        if (!pSet) {
            // Partial allocation — caller's out_sets is partially populated.
            // The pool reset at the next frame will reclaim the Vulkan-side sets.
            return SPUDRESULT_GPU_INTERNAL_DESCRIPTOR_SET_ALLOCATION_FAIL;
        }
        pSet->_pool = *desc->pool;
        pSet->_set_vk = vk_sets[i];
        out_sets[i] = (spudgpu_descriptor_set) pSet;
    }

    return SPUD_SUCCESS;
}


void spudgpu_update_descriptor_sets(
    spudgpu_device device,
    const spudgpu_write_descriptor_set *writes,
    uint32_t write_count) {
    if (!(device && writes && write_count)) return;

    // We translate each SpudGPU write into a VkWriteDescriptorSet on the stack.
    // For larger write counts a heap allocation would be safer, but the per-frame
    // update pattern rarely exceeds a few dozen writes.
    VkWriteDescriptorSet vk_writes[64] = {0};
    VkDescriptorBufferInfo vk_buf_infos[64] = {0};
    VkDescriptorImageInfo vk_img_infos[64] = {0};

    uint32_t clamped = write_count < 64 ? write_count : 64;

    for (uint32_t i = 0; i < clamped; i++) {
        const spudgpu_write_descriptor_set *w = &writes[i];
        if (!w->dst_set) continue;

        spudgpu_descriptor_set_vulkan *vkSet =
                (spudgpu_descriptor_set_vulkan *) w->dst_set;

        VkDescriptorType vk_type =
                spudgpuvulkan___descriptor_type_internal(w->descriptor_type);
        if (vk_type == VK_DESCRIPTOR_TYPE_MAX_ENUM) continue;

        vk_writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        vk_writes[i].dstSet = vkSet->_set_vk;
        vk_writes[i].dstBinding = w->dst_binding;
        vk_writes[i].dstArrayElement = w->dst_array_element;
        vk_writes[i].descriptorCount = w->descriptor_count;
        vk_writes[i].descriptorType = vk_type;

        if (w->buffer_info) {
            spudgpu_buffer_vulkan *vkBuf =
                    (spudgpu_buffer_vulkan *) w->buffer_info->buffer;
            vk_buf_infos[i].buffer = vkBuf->_buffer_vk;
            vk_buf_infos[i].offset = w->buffer_info->offset;
            // A range of 0 from the caller means "bind the whole buffer".
            vk_buf_infos[i].range = w->buffer_info->range
                                        ? w->buffer_info->range
                                        : VK_WHOLE_SIZE;
            vk_writes[i].pBufferInfo = &vk_buf_infos[i];
        }

        if (w->image_info) {
            spudgpu_image_view_vulkan *vkView =
                    (spudgpu_image_view_vulkan *) w->image_info->image_view;
            vk_img_infos[i].imageView = vkView->_image_view_vk;
            vk_img_infos[i].imageLayout = (VkImageLayout) w->image_info->image_layout;
            // Sampler is NULL — set it if COMBINED_IMAGE_SAMPLER support is added later.
            vk_img_infos[i].sampler = VK_NULL_HANDLE;
            vk_writes[i].pImageInfo = &vk_img_infos[i];
        }
    }

    vkUpdateDescriptorSets(device->_logical_device_vk, clamped, vk_writes, 0, NULL);
}

void spudgpu_cmd_bind_descriptor_sets(
    spudgpu_command_list cmd,
    spudgpu_shader_pipeline pipeline,
    uint32_t first_set,
    const spudgpu_descriptor_set *sets,
    uint32_t set_count) {
    if (!(cmd && pipeline && sets && set_count)) return;
    if (set_count > SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS) return;

    // The command list's internal VkCommandBuffer lives at the front of the struct,
    // matching the pattern used in spudgpuvulkanbuffer.c / the command list implementation.
    VkCommandBuffer vk_cmd = cmd->_command_buffer_vk;

    // Unwrap each SpudGPU handle to its raw VkDescriptorSet.
    VkDescriptorSet vk_sets[SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS] = {0};
    for (uint32_t i = 0; i < set_count; i++) {
        if (!sets[i]) return;
        spudgpu_descriptor_set_vulkan *vkSet =
                (spudgpu_descriptor_set_vulkan *) sets[i];
        vk_sets[i] = vkSet->_set_vk;
    }

    vkCmdBindDescriptorSets(
        vk_cmd,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline->_pipeline_layout_vk,
        first_set,
        set_count,
        vk_sets,
        0, // No dynamic offsets
        NULL);
}

void spudgpu_cmd_bind_descriptor_sets_compute(
    spudgpu_command_list cmd,
    spudgpu_compute_pipeline pipeline,
    uint32_t first_set,
    const spudgpu_descriptor_set *sets,
    uint32_t set_count) {
    if (!(cmd && pipeline && sets && set_count)) return;
    if (set_count > SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS) return;

    VkDescriptorSet vk_sets[SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS] = {0};
    for (uint32_t i = 0; i < set_count; i++) {
        if (!sets[i]) return;
        spudgpu_descriptor_set_vulkan *vkSet =
                (spudgpu_descriptor_set_vulkan *) sets[i];
        vk_sets[i] = vkSet->_set_vk;
    }

    vkCmdBindDescriptorSets(
        cmd->_command_buffer_vk,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        pipeline->_pipeline_layout_vk,
        first_set,
        set_count,
        vk_sets,
        0,
        NULL);
}


#if __cplusplus
}
#endif

#endif // SPUDGPU_COMPILE_VULKAN_API
