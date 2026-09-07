
#if SPUDGPU_COMPILE_D3D12_API

#include "d3dx12.h"
#include "spudgpu.h"
#include "spudgpu_d3d12_natives.h"
#include <array>
#include <dxgi1_6.h>
#include <vector>
#include <wrl/client.h>

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN ((D3D12_GPU_VIRTUAL_ADDRESS) - 1)
#define SPUD_D3D12_COMMAND_QUEUE_COUNT_PER_FAMILY 4


inline D3D12_COMMAND_LIST_TYPE
spudgpu_d3d12_get_command_list_type(SPUDGPU_COMMAND_LIST_TYPE t) {
	switch (t) {
	case SPUDGPU_COMMAND_LIST_TYPE_DIRECT:
		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	case SPUDGPU_COMMAND_LIST_TYPE_COPY:
		return D3D12_COMMAND_LIST_TYPE_COPY;
	case SPUDGPU_COMMAND_LIST_TYPE_COMPUTE:
		return D3D12_COMMAND_LIST_TYPE_COMPUTE;
	case SPUDGPU_COMMAND_LIST_TYPE_BUNDLE:
		return D3D12_COMMAND_LIST_TYPE_BUNDLE;
	default:
		return D3D12_COMMAND_LIST_TYPE_NONE;
	}
}

inline D3D12_HEAP_FLAGS spudgpu_d3d12_get_heap_flags(SPUDGPU_HEAP_FLAGS flags) {
	D3D12_HEAP_FLAGS result = D3D12_HEAP_FLAG_NONE;
	if (flags & SPUDGPU_HEAP_FLAG_SHARED)
		result |= D3D12_HEAP_FLAG_SHARED;
	if (flags & SPUDGPU_HEAP_FLAG_ALLOW_SHADER_ATOMICS)
		result |= D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS;
	if (flags & SPUDGPU_HEAP_FLAG_NOT_ZEROED)
		result |= D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;
	if (flags & SPUDGPU_HEAP_FLAG_CREATE_NOT_RESIDENT)
		result |= D3D12_HEAP_FLAG_CREATE_NOT_RESIDENT;
	return result;
}
inline D3D12_RESOURCE_FLAGS spudgpu_d3d12_get_buffer_resource_flags(
    SPUDGPU_BUFFER_USAGE usage, SPUDGPU_RESOURCE_FLAGS flags) {
	D3D12_RESOURCE_FLAGS result = D3D12_RESOURCE_FLAG_NONE;
	if (usage & SPUDGPU_BUFFER_USAGE_STORAGE)
		result |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (usage & SPUDGPU_BUFFER_USAGE_RAYTRACING_ACCELERATION_STRUCTURE)
		result |= D3D12_RESOURCE_FLAG_RAYTRACING_ACCELERATION_STRUCTURE;
	if (flags & SPUDGPU_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS)
		result |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
	return result;
}
inline D3D12_RESOURCE_FLAGS spudgpu_d3d12_get_image_resource_flags(
    SPUDGPU_IMAGE_USAGE usage, SPUDGPU_RESOURCE_FLAGS flags) {
	D3D12_RESOURCE_FLAGS result = D3D12_RESOURCE_FLAG_NONE;
	if (usage & SPUDGPU_IMAGE_USAGE_COLOR_ATTACHMENT)
		result |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (usage & SPUDGPU_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT)
		result |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	if (usage & SPUDGPU_IMAGE_USAGE_STORAGE)
		result |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (flags & SPUDGPU_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS)
		result |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
	return result;
}
inline DXGI_FORMAT spudgpu_d3d12_get_dxgi_format(SPUDGPU_FORMAT fmt) {
	return static_cast<DXGI_FORMAT>(fmt);
}

inline D3D12_HEAP_PROPERTIES
spudgpu_d3d12_get_heap_properties_from_memory_flags(SPUDGPU_MEMORY_FLAGS flags) {
	D3D12_HEAP_PROPERTIES props = {};
	if (flags & SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE) {
		props.Type = (flags & SPUDGPU_MEMORY_FLAGS_HOST_CACHED)
		                 ? D3D12_HEAP_TYPE_READBACK
		                 : D3D12_HEAP_TYPE_UPLOAD;
	} else {
		props.Type = D3D12_HEAP_TYPE_DEFAULT;
	}
	return props;
}
inline D3D12_RESOURCE_STATES
spudgpu_d3d12_get_initial_buffer_state(SPUDGPU_MEMORY_FLAGS flags) {
	if (flags & SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE) {
		return (flags & SPUDGPU_MEMORY_FLAGS_HOST_CACHED)
		           ? D3D12_RESOURCE_STATE_COPY_DEST
		           : D3D12_RESOURCE_STATE_GENERIC_READ;
	}
	return D3D12_RESOURCE_STATE_COMMON;
}
inline D3D12_RESOURCE_STATES
spudgpu_d3d12_get_initial_image_state(SPUDGPU_IMAGE_USAGE usage) {
	if (usage & SPUDGPU_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT)
		return D3D12_RESOURCE_STATE_DEPTH_WRITE;
	if (usage & SPUDGPU_IMAGE_USAGE_COLOR_ATTACHMENT)
		return D3D12_RESOURCE_STATE_RENDER_TARGET;
	if (usage & SPUDGPU_IMAGE_USAGE_TRANSFER_DST)
		return D3D12_RESOURCE_STATE_COPY_DEST;
	if (usage & SPUDGPU_IMAGE_USAGE_STORAGE)
		return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	return D3D12_RESOURCE_STATE_COMMON;
}

typedef struct spudgpu_instance_t {
#if _DEBUG
	const char *_debug_name;
#endif
	const char *application_name;
	uint32_t application_version;
	const char *engine_name;
	uint32_t engine_version;
	Microsoft::WRL::ComPtr<IDXGIFactory7> _dxgi_factory;
	spudgpu_device *_gpu_devices;
	uint32_t _gpu_device_count;
	bool _gpu_devices_enumerated;
} spudgpu_instance_d3d12;

#define SPUDGPU_BINDLESS_MAX_SLOTS_PER_CLASS 4096

// Lazily created the first time bindless capabilities/registration is used on
// a device. Safe to lazy-init here (unlike the Vulkan backend): every D3D12
// struct above stores spudgpu_device_d3d12* by pointer, never by value, so
// there is no stale-snapshot risk from initializing this after other objects
// already exist.
typedef struct spudgpu_bindless_state_d3d12 {
	// One shader-visible CBV_SRV_UAV heap, 3 fixed-size ranges back-to-back:
	// [0, MAX) sampled images, [MAX, 2*MAX) storage images,
	// [2*MAX, 3*MAX) storage buffers. spudgpu_d3d12_build_root_signature
	// (spudgpud3d12shader.cpp) already builds exactly this shape from
	// layout->_desc.bindings[] with no changes needed — see `layout` below.
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
	uint32_t increment = 0;

	// Sentinel spudgpu_descriptor_set_layout_d3d12 carrying only the 3
	// bindings' shape (binding_count/bindings[]) needed to generate the
	// matching root-signature ranges when a caller includes this handle in
	// their own descriptor_set_layouts[]. Its _slots[]/_cbv_srv_uav_count
	// bookkeeping (used by the classic pool/set allocator) is left zeroed —
	// bindless never goes through spudgpu_create_descriptor_sets.
	spudgpu_descriptor_set_layout_d3d12 *layout = nullptr;

	uint32_t sampled_image_next_unused  = 0;
	uint32_t storage_image_next_unused  = 0;
	uint32_t storage_buffer_next_unused = 0;

	// Stacks of freed indices, reused before drawing from *_next_unused.
	std::vector<uint32_t> sampled_image_free_stack;
	std::vector<uint32_t> storage_image_free_stack;
	std::vector<uint32_t> storage_buffer_free_stack;
} spudgpu_bindless_state_d3d12;

typedef struct spudgpu_device_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_instance_d3d12 *_instance;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> _dxgi_adapter;
	Microsoft::WRL::ComPtr<ID3D12Device14> _d3d_device;
	SPUDGPU_DEVICE_PROPERTIES _properties;
	std::array<spudgpu_command_queue, SPUD_D3D12_COMMAND_QUEUE_COUNT_PER_FAMILY>
	    _cmd_queues_direct;
	std::array<spudgpu_command_queue, SPUD_D3D12_COMMAND_QUEUE_COUNT_PER_FAMILY>
	    _cmd_queues_copy;
	std::array<spudgpu_command_queue, SPUD_D3D12_COMMAND_QUEUE_COUNT_PER_FAMILY>
	    _cmd_queues_compute;

	// Lazily allocated by the first spudgpu_get_bindless_capabilities /
	// spudgpu_bindless_register_* / spudgpu_get_bindless_descriptor_set_layout
	// call. nullptr until then; this device struct is calloc'd (see
	// spudgpud3d12context.cpp), so it starts nullptr for free.
	spudgpu_bindless_state_d3d12 *_bindless;
} spudgpu_device_d3d12;

typedef struct spudgpu_command_queue_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_device_d3d12 *_device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> _d3d_cmd_queue;
} spudgpu_command_queue_d3d12;

typedef struct spudgpu_command_allocator_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_device_d3d12 *_device;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _d3d_cmd_allocator;
	D3D12_COMMAND_LIST_TYPE _d3d_cmd_list_type;
} spudgpu_command_allocator_d3d12;

typedef struct spudgpu_command_list_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_command_allocator_d3d12 *_allocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> _d3d_cmd_list;
} spudgpu_command_list_d3d12;

typedef struct spudgpu_buffer_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_device_d3d12 *_device;
	spudgpu_buffer_desc _desc;
	Microsoft::WRL::ComPtr<ID3D12Resource2> _d3d_resource;
	D3D12_RESOURCE_DESC _d3d_resource_desc;
	D3D12_GPU_VIRTUAL_ADDRESS _d3d_gpu_address;
} spudgpu_buffer_d3d12;

typedef struct spudgpu_buffer_view_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_buffer_d3d12 *_buffer;
	spudgpu_buffer_view_desc _desc;
	SPUDGPU_D3D12_BUFFER_VIEW _d3d_view;
} spudgpu_buffer_view_d3d12;

typedef struct spudgpu_image_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_device_d3d12 *_device;
	spudgpu_image_desc _desc;
	Microsoft::WRL::ComPtr<ID3D12Resource2> _d3d_resource;
	D3D12_RESOURCE_DESC _d3d_resource_desc;
	D3D12_GPU_VIRTUAL_ADDRESS _d3d_gpu_address;
} spudgpu_image_d3d12;

typedef struct spudgpu_image_view_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_image_d3d12 *_image;
	spudgpu_image_view_desc _desc;
	SPUDGPU_D3D12_IMAGE_VIEW _d3d_view_desc;
} spudgpu_image_view_d3d12;

typedef struct spudgpu_shader_module_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_device_d3d12 *_device;
	spudgpu_shader_module_desc _desc;
	Microsoft::WRL::ComPtr<ID3DBlob> _d3d_blob;
} spudgpu_shader_module_d3d12;

typedef struct spudgpu_shader_pipeline_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_device_d3d12 *_device;
	spudgpu_shader_pipeline_desc _desc;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _d3d_root_signature;
	D3D_PRIMITIVE_TOPOLOGY _d3d_primitive_topology;
} spudgpu_shader_pipeline_d3d12;

typedef struct spudgpu_compute_pipeline_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_device_d3d12 *_device;
	spudgpu_compute_pipeline_desc _desc;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _d3d_root_signature;
} spudgpu_compute_pipeline_d3d12;

typedef struct spudgpu_fence_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_device_d3d12 *_device;
	SPUDGPU_FENCE_FLAGS flags;
	bool signaled_on_creation;
	uint64_t _signal_value;
	Microsoft::WRL::ComPtr<ID3D12Fence1> _d3d_fence;
} spudgpu_fence_d3d12;

typedef struct spudgpu_semaphore_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_device_d3d12 *_device;
	uint64_t _signal_value;
	Microsoft::WRL::ComPtr<ID3D12Fence1> _d3d_fence;
} spudgpu_semaphore_d3d12;

// Per-binding heap offsets pre-computed at layout creation time.
typedef struct spudgpu_d3d12_binding_slot {
	uint32_t cbv_srv_uav_offset; // from set base in the CBV/SRV/UAV heap
	uint32_t sampler_offset;     // from set base in the sampler heap
	bool     is_sampler;         // true if this binding lives in the sampler heap
} spudgpu_d3d12_binding_slot;

typedef struct spudgpu_descriptor_set_layout_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_device_d3d12 *_device;
	spudgpu_descriptor_set_layout_desc _desc;
	// Offsets into a set's allocated heap range, indexed by binding desc order.
	spudgpu_d3d12_binding_slot _slots[SPUDGPU_MAX_DESCRIPTOR_BINDINGS_PER_SET];
	uint32_t _cbv_srv_uav_count; // CBV/SRV/UAV descriptors needed per set
	uint32_t _sampler_count;     // sampler descriptors needed per set
} spudgpu_descriptor_set_layout_d3d12;

typedef struct spudgpu_descriptor_pool_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_device_d3d12 *_device;
	spudgpu_descriptor_pool_desc _desc;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _cbv_srv_uav_heap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _sampler_heap;
	uint32_t _cbv_srv_uav_capacity;
	uint32_t _sampler_capacity;
	uint32_t _cbv_srv_uav_cursor; // next free slot index
	uint32_t _sampler_cursor;
	uint32_t _cbv_srv_uav_increment;
	uint32_t _sampler_increment;
} spudgpu_descriptor_pool_d3d12;

typedef struct spudgpu_descriptor_set_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_descriptor_pool_d3d12 *_pool;
	spudgpu_descriptor_set_layout_d3d12 *_layout;
	uint32_t _cbv_srv_uav_base; // base slot index in pool's CBV/SRV/UAV heap
	uint32_t _sampler_base;     // base slot index in pool's sampler heap
} spudgpu_descriptor_set_d3d12;

typedef struct spudgpu_surface_t {
#if _DEBUG
	const char *_debug_name;
#endif
	HWND _hwnd;
	spudgpu_instance_d3d12 *_instance;
} spudgpu_surface_d3d12;

typedef struct spudgpu_swap_chain_t {
#if _DEBUG
	const char *_debug_name;
#endif
	spudgpu_device_d3d12 *_device;
	spudgpu_swap_chain_desc _desc;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> _dxgi_swap_chain;
	spudgpu_image_d3d12 *_back_buffer_images;
	spudgpu_image_view_d3d12 *_back_buffer_image_views;
} spudgpu_swap_chain_d3d12;

#endif // SPUDGPU_COMPILE_D3D12_API
