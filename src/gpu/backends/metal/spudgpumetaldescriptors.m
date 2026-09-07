//
// SpudGPU Metal backend - descriptor sets / argument buffers.
// Not yet implemented: no Apple hardware available to develop/verify against.
// Wired into the spudlib CMake target ahead of time - see CMakeLists.txt.
//

#if SPUDGPU_COMPILE_METAL_API

#include "spudgpu.h"
#include "spudcore.h"
#include "spudgpumetal.h"
#include <Metal/Metal.h>

SPUDRESULT spudgpu_create_descriptor_set_layout(
    spudgpu_device device,
    const spudgpu_descriptor_set_layout_desc *desc,
    spudgpu_descriptor_set_layout *out_layout) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!desc)
		return SPUDRESULT_NULL_DESC;
	if (!out_layout)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	SPUDRESULT sr = SPUD_SUCCESS;

	spudgpu_descriptor_set_layout_metal *layout_metal =
	    (spudgpu_descriptor_set_layout_metal *)calloc(1, sizeof(spudgpu_descriptor_set_layout_metal));
	if (!layout_metal) {
		sr = SPUDRESULT_GENERAL_FAILURE;
		goto failedattempt;
	}
	layout_metal->_parent_device = (spudgpu_device_metal *)device;
	layout_metal->_desc          = *desc;

	// METAL API CODE

	*out_layout = (spudgpu_descriptor_set_layout)layout_metal;

	return sr;

failedattempt:
	return sr;
}

void spudgpu_destroy_descriptor_set_layout(spudgpu_descriptor_set_layout layout) {
	spudgpu_descriptor_set_layout_metal *layout_metal = (spudgpu_descriptor_set_layout_metal *)layout;
	if (layout_metal) {
		free(layout_metal);
	}
}

SPUDRESULT spudgpu_create_descriptor_pool(
    spudgpu_device device,
    const spudgpu_descriptor_pool_desc *desc,
    spudgpu_descriptor_pool *out_pool) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!desc)
		return SPUDRESULT_NULL_DESC;
	if (!out_pool)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	SPUDRESULT sr = SPUD_SUCCESS;

	spudgpu_descriptor_pool_metal *pool_metal =
	    (spudgpu_descriptor_pool_metal *)calloc(1, sizeof(spudgpu_descriptor_pool_metal));
	if (!pool_metal) {
		sr = SPUDRESULT_GENERAL_FAILURE;
		goto failedattempt;
	}
	pool_metal->_parent_device = (spudgpu_device_metal *)device;
	pool_metal->_desc          = *desc;

	// METAL API CODE

	*out_pool = (spudgpu_descriptor_pool)pool_metal;

	return sr;

failedattempt:
	return sr;
}

void spudgpu_reset_descriptor_pool(spudgpu_descriptor_pool pool) {
	if (!pool)
		return;

	// METAL API CODE
}

void spudgpu_destroy_descriptor_pool(spudgpu_descriptor_pool pool) {
	spudgpu_descriptor_pool_metal *pool_metal = (spudgpu_descriptor_pool_metal *)pool;
	if (pool_metal) {
		free(pool_metal);
	}
}

SPUDRESULT spudgpu_create_descriptor_sets(
    spudgpu_device device,
    const spudgpu_descriptor_set_desc *desc,
    spudgpu_descriptor_set *out_sets) {
	if (!device)
		return SPUDRESULT_GPU_INVALID_DEVICE;
	if (!desc)
		return SPUDRESULT_NULL_DESC;
	if (!out_sets)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	if (!desc->pool)
		return SPUDRESULT_GPU_INVALID_DESCRIPTOR_POOL;
	if (desc->set_count == 0)
		return SPUDRESULT_GPU_ZERO_DESCRIPTOR_SET_LAYOUTS;

	for (uint32_t i = 0; i < desc->set_count; i++) {
		spudgpu_descriptor_set_metal *set_metal =
		    (spudgpu_descriptor_set_metal *)calloc(1, sizeof(spudgpu_descriptor_set_metal));
		if (!set_metal)
			return SPUDRESULT_GENERAL_FAILURE;

		set_metal->_parent_pool = (spudgpu_descriptor_pool_metal *)desc->pool;
		set_metal->_layout      = (spudgpu_descriptor_set_layout_metal *)desc->set_layouts[i];

		// METAL API CODE

		out_sets[i] = (spudgpu_descriptor_set)set_metal;
	}

	return SPUD_SUCCESS;
}

void spudgpu_update_descriptor_sets(
    spudgpu_device device,
    const spudgpu_write_descriptor_set *writes,
    uint32_t write_count) {
	if (!device || !writes || write_count == 0)
		return;

	// METAL API CODE
}

void spudgpu_cmd_bind_descriptor_sets(
    spudgpu_command_list cmd,
    spudgpu_shader_pipeline pipeline,
    uint32_t first_set,
    const spudgpu_descriptor_set *sets,
    uint32_t set_count) {
	if (!cmd || !pipeline || !sets || set_count == 0)
		return;

	// METAL API CODE
}

void spudgpu_cmd_bind_descriptor_sets_compute(
    spudgpu_command_list cmd,
    spudgpu_compute_pipeline pipeline,
    uint32_t first_set,
    const spudgpu_descriptor_set *sets,
    uint32_t set_count) {
	if (!cmd || !pipeline || !sets || set_count == 0)
		return;

	// METAL API CODE
}

#endif // SPUDGPU_COMPILE_METAL_API
