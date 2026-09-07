//
// Created by nathanmoore on 5/16/26.
//

#ifndef SPUDLIB_SPUDGPU_H
#define SPUDLIB_SPUDGPU_H

#include "spudcore.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {

#endif // __cplusplus

#if SPUDGPU_COMPILE_VULKAN_API
#endif

#if SPUDGPU_COMPILE_D3D12_API
#endif

#if SPUDGPU_COMPILE_METAL_API
#endif

typedef uint32_t SPUDGPU_FORMAT;
enum {
	SPUDGPU_FORMAT_UNKNOWN                    = 0,
	SPUDGPU_FORMAT_R32G32B32A32_TYPELESS      = 1,
	SPUDGPU_FORMAT_R32G32B32A32_FLOAT         = 2,
	SPUDGPU_FORMAT_R32G32B32A32_UINT          = 3,
	SPUDGPU_FORMAT_R32G32B32A32_SINT          = 4,
	SPUDGPU_FORMAT_R32G32B32_TYPELESS         = 5,
	SPUDGPU_FORMAT_R32G32B32_FLOAT            = 6,
	SPUDGPU_FORMAT_R32G32B32_UINT             = 7,
	SPUDGPU_FORMAT_R32G32B32_SINT             = 8,
	SPUDGPU_FORMAT_R16G16B16A16_TYPELESS      = 9,
	SPUDGPU_FORMAT_R16G16B16A16_FLOAT         = 10,
	SPUDGPU_FORMAT_R16G16B16A16_UNORM         = 11,
	SPUDGPU_FORMAT_R16G16B16A16_UINT          = 12,
	SPUDGPU_FORMAT_R16G16B16A16_SNORM         = 13,
	SPUDGPU_FORMAT_R16G16B16A16_SINT          = 14,
	SPUDGPU_FORMAT_R32G32_TYPELESS            = 15,
	SPUDGPU_FORMAT_R32G32_FLOAT               = 16,
	SPUDGPU_FORMAT_R32G32_UINT                = 17,
	SPUDGPU_FORMAT_R32G32_SINT                = 18,
	SPUDGPU_FORMAT_R32G8X24_TYPELESS          = 19,
	SPUDGPU_FORMAT_D32_FLOAT_S8X24_UINT       = 20,
	SPUDGPU_FORMAT_R32_FLOAT_X8X24_TYPELESS   = 21,
	SPUDGPU_FORMAT_X32_TYPELESS_G8X24_UINT    = 22,
	SPUDGPU_FORMAT_R10G10B10A2_TYPELESS       = 23,
	SPUDGPU_FORMAT_R10G10B10A2_UNORM          = 24,
	SPUDGPU_FORMAT_R10G10B10A2_UINT           = 25,
	SPUDGPU_FORMAT_R11G11B10_FLOAT            = 26,
	SPUDGPU_FORMAT_R8G8B8A8_TYPELESS          = 27,
	SPUDGPU_FORMAT_R8G8B8A8_UNORM             = 28,
	SPUDGPU_FORMAT_R8G8B8A8_UNORM_SRGB        = 29,
	SPUDGPU_FORMAT_R8G8B8A8_UINT              = 30,
	SPUDGPU_FORMAT_R8G8B8A8_SNORM             = 31,
	SPUDGPU_FORMAT_R8G8B8A8_SINT              = 32,
	SPUDGPU_FORMAT_R16G16_TYPELESS            = 33,
	SPUDGPU_FORMAT_R16G16_FLOAT               = 34,
	SPUDGPU_FORMAT_R16G16_UNORM               = 35,
	SPUDGPU_FORMAT_R16G16_UINT                = 36,
	SPUDGPU_FORMAT_R16G16_SNORM               = 37,
	SPUDGPU_FORMAT_R16G16_SINT                = 38,
	SPUDGPU_FORMAT_R32_TYPELESS               = 39,
	SPUDGPU_FORMAT_D32_FLOAT                  = 40,
	SPUDGPU_FORMAT_R32_FLOAT                  = 41,
	SPUDGPU_FORMAT_R32_UINT                   = 42,
	SPUDGPU_FORMAT_R32_SINT                   = 43,
	SPUDGPU_FORMAT_R24G8_TYPELESS             = 44,
	SPUDGPU_FORMAT_D24_UNORM_S8_UINT          = 45,
	SPUDGPU_FORMAT_R24_UNORM_X8_TYPELESS      = 46,
	SPUDGPU_FORMAT_X24_TYPELESS_G8_UINT       = 47,
	SPUDGPU_FORMAT_R8G8_TYPELESS              = 48,
	SPUDGPU_FORMAT_R8G8_UNORM                 = 49,
	SPUDGPU_FORMAT_R8G8_UINT                  = 50,
	SPUDGPU_FORMAT_R8G8_SNORM                 = 51,
	SPUDGPU_FORMAT_R8G8_SINT                  = 52,
	SPUDGPU_FORMAT_R16_TYPELESS               = 53,
	SPUDGPU_FORMAT_R16_FLOAT                  = 54,
	SPUDGPU_FORMAT_D16_UNORM                  = 55,
	SPUDGPU_FORMAT_R16_UNORM                  = 56,
	SPUDGPU_FORMAT_R16_UINT                   = 57,
	SPUDGPU_FORMAT_R16_SNORM                  = 58,
	SPUDGPU_FORMAT_R16_SINT                   = 59,
	SPUDGPU_FORMAT_R8_TYPELESS                = 60,
	SPUDGPU_FORMAT_R8_UNORM                   = 61,
	SPUDGPU_FORMAT_R8_UINT                    = 62,
	SPUDGPU_FORMAT_R8_SNORM                   = 63,
	SPUDGPU_FORMAT_R8_SINT                    = 64,
	SPUDGPU_FORMAT_A8_UNORM                   = 65,
	SPUDGPU_FORMAT_R1_UNORM                   = 66,
	SPUDGPU_FORMAT_R9G9B9E5_SHAREDEXP         = 67,
	SPUDGPU_FORMAT_R8G8_B8G8_UNORM            = 68,
	SPUDGPU_FORMAT_G8R8_G8B8_UNORM            = 69,
	SPUDGPU_FORMAT_BC1_TYPELESS               = 70,
	SPUDGPU_FORMAT_BC1_UNORM                  = 71,
	SPUDGPU_FORMAT_BC1_UNORM_SRGB             = 72,
	SPUDGPU_FORMAT_BC2_TYPELESS               = 73,
	SPUDGPU_FORMAT_BC2_UNORM                  = 74,
	SPUDGPU_FORMAT_BC2_UNORM_SRGB             = 75,
	SPUDGPU_FORMAT_BC3_TYPELESS               = 76,
	SPUDGPU_FORMAT_BC3_UNORM                  = 77,
	SPUDGPU_FORMAT_BC3_UNORM_SRGB             = 78,
	SPUDGPU_FORMAT_BC4_TYPELESS               = 79,
	SPUDGPU_FORMAT_BC4_UNORM                  = 80,
	SPUDGPU_FORMAT_BC4_SNORM                  = 81,
	SPUDGPU_FORMAT_BC5_TYPELESS               = 82,
	SPUDGPU_FORMAT_BC5_UNORM                  = 83,
	SPUDGPU_FORMAT_BC5_SNORM                  = 84,
	SPUDGPU_FORMAT_B5G6R5_UNORM               = 85,
	SPUDGPU_FORMAT_B5G5R5A1_UNORM             = 86,
	SPUDGPU_FORMAT_B8G8R8A8_UNORM             = 87,
	SPUDGPU_FORMAT_B8G8R8X8_UNORM             = 88,
	SPUDGPU_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	SPUDGPU_FORMAT_B8G8R8A8_TYPELESS          = 90,
	SPUDGPU_FORMAT_B8G8R8A8_UNORM_SRGB        = 91,
	SPUDGPU_FORMAT_B8G8R8X8_TYPELESS          = 92,
	SPUDGPU_FORMAT_B8G8R8X8_UNORM_SRGB        = 93,
	SPUDGPU_FORMAT_BC6H_TYPELESS              = 94,
	SPUDGPU_FORMAT_BC6H_UF16                  = 95,
	SPUDGPU_FORMAT_BC6H_SF16                  = 96,
	SPUDGPU_FORMAT_BC7_TYPELESS               = 97,
	SPUDGPU_FORMAT_BC7_UNORM                  = 98,
	SPUDGPU_FORMAT_BC7_UNORM_SRGB             = 99,
	SPUDGPU_FORMAT_AYUV                       = 100,
	SPUDGPU_FORMAT_Y410                       = 101,
	SPUDGPU_FORMAT_Y416                       = 102,
	SPUDGPU_FORMAT_NV12                       = 103,
	SPUDGPU_FORMAT_P010                       = 104,
	SPUDGPU_FORMAT_P016                       = 105,
	SPUDGPU_FORMAT_420_OPAQUE                 = 106,
	SPUDGPU_FORMAT_YUY2                       = 107,
	SPUDGPU_FORMAT_Y210                       = 108,
	SPUDGPU_FORMAT_Y216                       = 109,
	SPUDGPU_FORMAT_NV11                       = 110,
	SPUDGPU_FORMAT_AI44                       = 111,
	SPUDGPU_FORMAT_IA44                       = 112,
	SPUDGPU_FORMAT_P8                         = 113,
	SPUDGPU_FORMAT_A8P8                       = 114,
	SPUDGPU_FORMAT_B4G4R4A4_UNORM             = 115,

	SPUDGPU_FORMAT_P208 = 130,
	SPUDGPU_FORMAT_V208 = 131,
	SPUDGPU_FORMAT_V408 = 132,

	SPUDGPU_FORMAT_FORCE_UINT = -1 // 0xffffffff
};

// Use the return value / 8 for byte size.
// @return Pixel bit size according to the format.
uint32_t spudgpu_format_bit_count(SPUDGPU_FORMAT fmt);

// @return SPUDGPU_FORMAT according to string, or SPUDGPU_FORMAT_UNKNOWN if
// fmt_str is NULL or doesn't match a known format name.
SPUDGPU_FORMAT spudgpu_format_from_string(const char *fmt_str);

typedef struct spudgpu_instance_t *spudgpu_instance;
typedef struct spudgpu_device_t *spudgpu_device;
typedef struct spudgpu_buffer_t *spudgpu_buffer;
typedef struct spudgpu_buffer_view_t *spudgpu_buffer_view;
typedef struct spudgpu_image_t *spudgpu_image;
typedef struct spudgpu_image_view_t *spudgpu_image_view;
typedef struct spudgpu_shader_pipeline_t *spudgpu_shader_pipeline;
typedef struct spudgpu_command_list_t *spudgpu_command_list;
typedef struct spudgpu_command_allocator_t *spudgpu_command_allocator;
typedef struct spudgpu_command_queue_t *spudgpu_command_queue;
typedef struct spudgpu_swap_chain_t *spudgpu_swap_chain;
typedef struct spudgpu_shader_module_t *spudgpu_shader_module;
typedef struct spudgpu_compute_pipeline_t *spudgpu_compute_pipeline;
typedef struct spudgpu_surface_t *spudgpu_surface;
typedef struct spudgpu_semaphore_t *spudgpu_semaphore;
typedef struct spudgpu_fence_t *spudgpu_fence;
typedef struct spudgpu_descriptor_set_layout_t *spudgpu_descriptor_set_layout;
typedef struct spudgpu_descriptor_pool_t *spudgpu_descriptor_pool;
typedef struct spudgpu_descriptor_set_t *spudgpu_descriptor_set;
typedef struct spudgpu_pipeline_layout_t *spudgpu_pipeline_layout;

/**
 * @brief Handle representing the active graphics API backend.
 * * This binds SpudGPU to a specific graphics API.
 * Vulkan and Metal are currently supported.
 */
typedef uint32_t SPUDGPU_NATIVE_API;

/**
 * @name Supported Graphics APIs
 * @anchor SPUDGPU_API_Constants
 * * Defined constants representing the target graphics hardware interface.
 * Metal for Apple Silicon (macOS, iPadOS, iOS), Vulkan for everything else
 * (Windows, Linux).
 */
///@{
enum {
	/// Placeholder representing an uninitialized, unsupported, or invalid
	/// graphics API.
	SPUDGPU_NATIVE_API_NONE = 0,

	/// Vulkan cross-platform graphics API. Used primarily on Linux, Windows,
	/// and Android.
	SPUDGPU_NATIVE_API_VULKAN = 1,

	/// Direct3D 12 graphics API used exclusively for Windows and Xbox.
	SPUDGPU_NATIVE_API_D3D12 = 2,

	/// Metal proprietary graphics API. Used exclusively for Apple Silicon
	/// ecosystems (macOS, iOS, iPadOS, watchOS).
	SPUDGPU_NATIVE_API_METAL = 3
};

///@}

/**
 * @brief Initializes the global SpudGPU context.
 * * Configures the underlying graphics API backend, sets up instances, and
 * internally gathers a list of GPU devices. Use spudgpu_get_devices() after
 * calling this function.
 * @param[in] native_api          The targeted graphics backend (Vulkan or
 * Metal).
 * @param[in] application_name    Null-terminated string containing the client
 * application's name.
 * @param[in] application_version Packed 32-bit integer representing the
 * application version.
 * @param[in] engine_name         Null-terminated string containing the custom
 * game/rendering engine name.
 * @param[in] engine_version      Packed 32-bit integer representing the engine
 * version.
 * @param[out] out_instance       The returned spudgpu_instance.
 * @return SPUD_SUCCESS if successful, otherwise another SPUDRESULT.
 * @note Must be called before invoking any other SpudGPU API function.
 */
SPUDRESULT spudgpu_create_instance(
    SPUDGPU_NATIVE_API native_api,
    const char *application_name,
    uint32_t application_version,
    const char *engine_name,
    uint32_t engine_version,
    spudgpu_instance *out_instance);

/**
 * @brief Shuts down the SpudGPU instance.
 * Disposes of all internal runtime states, destroys active backend contexts,
 * and releases hooks into the graphics hardware.
 * @param[in] instance The SpudGPU instance of which to be terminated.
 * @warning Calling this while resources (like buffers, textures, or pipelines)
 * are still active will result in undefined behavior or memory leaks.
 */
SPUDRESULT spudgpu_destroy_instance(spudgpu_instance instance);

/**
 * @brief Retreive an enumeration of the physical graphics devices available on
 * the host machine.
 * @param[in] instance The SpudGPU instance of which to enumerate devices
 * through.
 * @param[out] out_devices Array of enumerated device handles.
 * @param[out] out_devices_count Number of devices written to out_devices.
 * @return SPUD_SUCCESS or SPUDRESULT_GPU_DEVICE_ENUMERATION_FAILURE
 */
SPUDRESULT spudgpu_enumerate_devices(
    spudgpu_instance instance,
    spudgpu_device **out_devices,
    uint32_t *out_devices_count);

#define SPUDGPU_VENDOR_INTEL 0x8086
#define SPUDGPU_VENDOR_AMD_ATI 0x1002
#define SPUDGPU_VENDOR_NVIDIA 0X10DE
#define SPUDGPU_VENDOR_ARM 0x13B5
#define SPUDGPU_VENDOR_QUALCOMM 0x5143
#define SPUDGPU_VENDOR_IMGTEC 0x1010
#define SPUDGPU_VENDOR_APPLE 0x106B
#define SPUDGPU_VENDOR_VMWARE 0x15AD

typedef struct SPUDGPU_DEVICE_PROPERTIES {
	char description[128];
	uint32_t vendor_id;
	uint32_t device_id;
	uint32_t subSys_id;
	uint32_t revision;
	uint64_t dedicated_video_memory;
	uint64_t dedicated_system_memory;
	uint64_t shared_system_memory;
} SPUDGPU_DEVICE_PROPERTIES;
SPUDRESULT spudgpu_get_device_properties(
    spudgpu_device device,
    SPUDGPU_DEVICE_PROPERTIES *out_properties);

/**
 * @brief Retrieves the active graphics API backend for the SpudGPU instance.
 * @param[in] instance The SpudGPU instance of which to get GPU API.
 * @return SPUDGPU_API The enum value representing the active backend.
 * Returns `SPUDGPU_API_NONE` if instance is NULL.
 */
SPUDGPU_NATIVE_API spudgpu_get_native_gpu_api(spudgpu_instance instance);

/**
 * @brief Get the graphics command queue linked to the GPU device.
 * @param[in] device The GPU device to get the command queue.
 * @return Graphics Command Queue.
 * @return NULL if device is NULL.
 */
spudgpu_command_queue spudgpu_get_graphics_queue(spudgpu_device device);

typedef uint32_t SPUDGPU_COMMAND_LIST_TYPE;
enum { SPUDGPU_COMMAND_LIST_TYPE_DIRECT = 0, SPUDGPU_COMMAND_LIST_TYPE_COPY = 1, SPUDGPU_COMMAND_LIST_TYPE_BUNDLE = 2, SPUDGPU_COMMAND_LIST_TYPE_COMPUTE = 3 };

uint32_t spudgpu_get_max_queue_count(
    spudgpu_device device,
    SPUDGPU_COMMAND_LIST_TYPE type);

SPUDRESULT spudgpu_get_command_queue(
    spudgpu_device device,
    SPUDGPU_COMMAND_LIST_TYPE type,
    uint32_t index,
    spudgpu_command_queue *out_queue);

SPUDRESULT spudgpu_submit_command_lists(
    spudgpu_command_queue queue,
    spudgpu_command_list *cmd_lists,
    uint32_t cmd_list_count);

// Submit command lists with full swap chain synchronization.
// Waits on the swap chain's image_available semaphore,
// signals its render_finished semaphore, and signals the in-flight fence.
// Call this instead of spudgpu_submit_command_lists when rendering to a swap
// chain.
SPUDRESULT spudgpu_submit_command_lists_synced(
    spudgpu_command_queue queue,
    spudgpu_command_list *cmd_lists,
    uint32_t cmd_list_count,
    spudgpu_swap_chain swap_chain);
typedef struct spudgpu_submit_desc {
	spudgpu_command_list *cmd_lists;
	uint32_t cmd_list_count;

	// GPU-GPU Sync
	spudgpu_semaphore *wait_semaphores;
	uint32_t wait_semaphore_count;
	// One SPUDGPU_PIPELINE_STAGE mask per wait semaphore — required when
	// wait_semaphore_count > 0.
	uint32_t *wait_stage_masks;
	spudgpu_semaphore *signal_semaphores;
	uint32_t signal_semaphore_count;

	// CPU-GPU Sync (Optional, can be NULL)
	spudgpu_fence signal_fence;
} spudgpu_submit_desc;

void spudgpu_queue_submit(
    spudgpu_command_queue queue,
    const spudgpu_submit_desc *desc);

/**
 * @brief Blocks the calling CPU thread until all pending work on the queue has
 * completed. Equivalent to vkQueueWaitIdle. Prefer fence-based synchronization
 * in hot paths.
 */
void spudgpu_queue_wait_idle(spudgpu_command_queue queue);

typedef struct spudgpu_command_allocator_desc {
	SPUDGPU_COMMAND_LIST_TYPE type;
	uint32_t flags;
	// uint32_t queue_family_index;
} spudgpu_command_allocator_desc;

/**
 * @brief Create a command allocator on the GPU device.
 * @param[in] device The GPU device to create a command allocator on.
 * @param[out] out_allocator The GPU command allocator to be created.
 * @return A new command allocater.
 * @return SPUD_SUCCESS or another SPUDRESULT.
 */
SPUDRESULT spudgpu_create_command_allocator(
    spudgpu_device device,
    const spudgpu_command_allocator_desc *desc,
    spudgpu_command_allocator *out_allocator);

/**
 * @brief Destroy the command allocator.
 * param[in] allocator The command allocator to destroy.
 */
void spudgpu_destroy_command_allocator(spudgpu_command_allocator allocator);

/**
 * @brief Reset the command allocator.
 * @param[in] allocator The command allocator to reset.
 */
SPUDRESULT spudgpu_reset_command_allocator(spudgpu_command_allocator allocator);

/**
 * @brief Create a new command list used on the command allocator.
 * @param[in] allocator The command allocator used for the command list.
 * @param[out] out_cmd_list The new command list.
 * @return SPUD_SUCCESS or another SPUDRESULT_
 */
SPUDRESULT spudgpu_create_command_list(
    spudgpu_command_allocator allocator,
    spudgpu_command_list *out_cmd_list);

/**
 * @brief Destroy a command list.
 * @param[in] cmd The command list to destroy.
 */
void spudgpu_destroy_command_list(spudgpu_command_list cmd);
/**
 * @brief Begins recording graphics or compute infrastructure commands.
 * * Resets the command buffer's internal state tracking and sets it to a
 * recording mode.
 * @param[in] cmd Target command list instance to open.
 * * @warning A command list must not be simultaneously recorded across multiple
 * CPU threads.
 */
void spudgpu_begin_command_list(spudgpu_command_list cmd);

/**
 * @brief Finalizes recording for the specified command list.
 * * Closes the command stream, compiling it into an immutable hardware
 * execution package ready for queue submission.
 * @param[in] cmd Target command list instance to close.
 */
void spudgpu_end_command_list(spudgpu_command_list cmd);

typedef uint32_t SPUDGPU_FENCE_FLAGS;
enum { SPUDGPU_FENCE_FLAG_NONE = 0, SPUDGPU_FENCE_FLAG_SHARED = 1 << 0 };

/**
 * @brief Creates a fence primitive used to synchronize the CPU with GPU
 * progress.
 * @param[in] device The GPU Device to own the fence.
 * @param[in] signaled_on_creation If true, the fence starts in a signaled state.
 * @param[out] out_fence The new GPU Fence to be created.
 * @return SPUD_SUCCESS; or an error code.
 */
SPUDRESULT spudgpu_create_fence(
    spudgpu_device device,
    SPUDGPU_FENCE_FLAGS flags,
    bool signaled_on_creation,
    spudgpu_fence *out_fence);

/**
 * @brief Destroys a fence primitive.
 */
void spudgpu_destroy_fence(spudgpu_fence fence);

uint64_t spudgpu_get_fence_value(spudgpu_fence fence);
SPUDRESULT spudgpu_signal_fence(
    spudgpu_device device,
    spudgpu_fence fence,
    uint64_t value);

/**
 * @brief Blocks the calling CPU thread until one or all specified fences are
 * signaled.
 * @param[in] timeout_ns Timeout duration in nanoseconds (use UINT64_MAX for
 * infinite).
 * @param[in] wait_all If true, waits for all fences; if false, waits for at
 * least one.
 */
SPUDRESULT spudgpu_wait_for_fences(
    spudgpu_device device,
    spudgpu_fence *fences,
    uint32_t fence_count,
    bool wait_all,
    uint64_t timeout_ns);

/**
 * @brief Creates a binary semaphore for GPU-GPU synchronization.
 * @param[in] device The GPU Device to own the semaphore.
 * @param[out] out_sem The new GPU semaphore.
 * @return SPUD_SUCCESS; or an error code.
 */
SPUDRESULT spudgpu_create_semaphore(
    spudgpu_device device,
    spudgpu_semaphore *out_semaphore);

/**
 * @brief Destroys a semaphore primitive.
 */
void spudgpu_destroy_semaphore(spudgpu_semaphore semaphore);

/*
 * Static GPU Data = DEVICE_LOCAL
 * Dynamic CPU Data = HOST_VISIBLE | HOST COHERENT
 * Direct-Write GPU Data = DEVICE_LOCAL | HOST_VISIBLE | HOST COHERENT
 * Readback Data = HOST_VISIBLE | HOST_CACHED
 */
/**
 * @brief Bitmask representing physical memory allocation properties.
 * * These flags specify where a memory resource physically resides (VRAM vs.
 * System RAM) and how the CPU and GPU cache or synchronize access to it.
 * * @see SPUDGPU_MEMORY_FLAGS_Constants
 */
typedef uint32_t SPUDGPU_MEMORY_FLAGS;

/**
 * @name Memory Property Flags
 * @anchor SPUDGPU_MEMORY_FLAGS_Constants
 * * Bitmask flags used to configure memory allocations. These can be combined
 * using the bitwise OR (`|`) operator.
 * * ### Common Configurations:
 * - **Static GPU Data:** `SPUDGPU_MEMORY_FLAGS_DEVICE_LOCAL`
 * - **Dynamic CPU Data:** `SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE |
 * SPUDGPU_MEMORY_FLAGS_HOST_COHERENT`
 * - **Direct-Write GPU Data:** `SPUDGPU_MEMORY_FLAGS_DEVICE_LOCAL |
 * SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE | SPUDGPU_MEMORY_FLAGS_HOST_COHERENT`
 * - **Readback Data:** `SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE |
 * SPUDGPU_MEMORY_FLAGS_HOST_CACHED`
 */
///@{
enum {
	/// No memory flags specified. Default or uninitialized state.
	SPUDGPU_MEMORY_FLAGS_NONE = 0,

	/** * @brief Memory is local to the device (VRAM).
	 * * Offers the highest throughput for GPU execution. Usually inaccessible
	 * directly by the CPU unless combined with
	 * `SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE`.
	 */
	SPUDGPU_MEMORY_FLAGS_DEVICE_LOCAL = 1 << 0,

	/** * @brief Memory can be mapped for CPU access (System RAM or Bar-Mapped
	 * VRAM).
	 * * Required if you intend to use `memcpy` or pointers from the CPU host
	 * code.
	 */
	SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE = 1 << 1,

	/** * @brief Memory writes are automatically synchronized between host and
	 * device.
	 * * Ensures that CPU modifications are instantly visible to the GPU without
	 * requiring explicit cache flushing commands (e.g.,
	 * `vkFlushMappedMemoryRanges`).
	 */
	SPUDGPU_MEMORY_FLAGS_HOST_COHERENT = 1 << 2,

	/** * @brief Memory allocation is cached on the host CPU.
	 * * Optimizes performance for CPU read operations (e.g., transferring data
	 * back from the GPU to the CPU). CPU reads from non-cached memory are
	 * notoriously slow.
	 */
	SPUDGPU_MEMORY_FLAGS_HOST_CACHED = 1 << 3
};

///@}

/**
 * @brief Bitmask defining the intended pipeline binding targets for a GPU
 * buffer.
 * * Informs the driver how the memory will be bound within hardware command
 * buffers to allow internal optimization.
 * * @see SPUDGPU_BUFFER_USAGE_Constants
 */
typedef uint32_t SPUDGPU_BUFFER_USAGE;

/**
 * @name Buffer Pipeline Usage Flags
 * @anchor SPUDGPU_BUFFER_USAGE_Constants
 * * Bitmask flags denoting buffer compatibility. A single buffer can fulfill
 * multiple roles by combining flags via bitwise OR (`|`).
 */
///@{
enum {
	/// No hardware binding target specified.
	SPUDGPU_BUFFER_USAGE_NONE = 0,

	/// Buffer is compatible for binding as a Input Assembler Vertex Stream
	/// (Vertex Buffer).
	SPUDGPU_BUFFER_USAGE_VERTEX = 1 << 0,

	/// Buffer contains index arrays (16-bit or 32-bit indices) for indexed draw
	/// calls (Index Buffer).
	SPUDGPU_BUFFER_USAGE_INDEX = 1 << 1,

	/** * @brief Buffer acts as a Uniform/Constant Buffer block (Uniform Buffer
	 * or Constant Buffer).
	 * * Optimized for uniform, read-only shader data accessed globally across
	 * shader stages.
	 */
	SPUDGPU_BUFFER_USAGE_UNIFORM = 1 << 2,

	/** * @brief Buffer acts as a Shader Storage Buffer Object (SSBO) /
	 * Structured Buffer.
	 * * Typically used for large, variable-sized data structures requiring
	 * read/write capabilities within compute or graphics shaders.
	 */
	SPUDGPU_BUFFER_USAGE_STORAGE = 1 << 3,

	/// Buffer can be used as the source of a copy operation (e.g. a staging
	/// buffer read by spudgpu_cmd_copy_buffer_to_image).
	SPUDGPU_BUFFER_USAGE_TRANSFER_SRC = 1 << 4,

	/// Buffer can be used as the destination of a copy operation (e.g. a
	/// readback buffer written by spudgpu_cmd_copy_image_to_buffer).
	SPUDGPU_BUFFER_USAGE_TRANSFER_DST = 1 << 5,

	/// Buffer holds a built raytracing acceleration structure (BLAS/TLAS).
	/// This is a binding-compatibility fact like the other usage bits, not a
	/// resource-creation flag.
	SPUDGPU_BUFFER_USAGE_RAYTRACING_ACCELERATION_STRUCTURE = 1 << 6
};

/**
 * @brief Bitmask of heap-allocation behaviors, orthogonal to how the
 * resource will be bound (@see SPUDGPU_BUFFER_USAGE / SPUDGPU_IMAGE_USAGE).
 */
typedef uint32_t SPUDGPU_HEAP_FLAGS;
enum {
	SPUDGPU_HEAP_FLAG_NONE   = 0,

	/// Allocation can be shared across processes/APIs (cross-adapter/external
	/// memory interop).
	SPUDGPU_HEAP_FLAG_SHARED = 0x1,

	/** * @brief Opts the heap into Shader Model 6.6 atomic operations.
	 * * D3D12-specific: some D3D12 memory pools require this explicit opt-in
	 * for SM6.6 atomics to be valid. Vulkan has no per-heap equivalent —
	 * shader atomics on storage buffers/images are core functionality gated
	 * only by shader capability, not resource creation — so this flag is a
	 * no-op on the Vulkan and Metal backends.
	 */
	SPUDGPU_HEAP_FLAG_ALLOW_SHADER_ATOMICS = 0x400,

	/// Skip zero-initializing the allocation (perf hint; contents are
	/// undefined until written).
	SPUDGPU_HEAP_FLAG_NOT_ZEROED           = 0x1000,

	/// Allocate without making the heap resident; caller manages residency
	/// explicitly.
	SPUDGPU_HEAP_FLAG_CREATE_NOT_RESIDENT  = 0x800
};

/**
 * @brief Bitmask of resource-level access flags orthogonal to both binding
 * usage and heap-allocation behavior. Shared by spudgpu_buffer_desc and
 * spudgpu_image_desc since the concepts it holds apply identically to both
 * resource kinds.
 */
typedef uint32_t SPUDGPU_RESOURCE_FLAGS;
enum {
	SPUDGPU_RESOURCE_FLAG_NONE = 0,

	/// Allows the resource to be accessed concurrently from multiple command
	/// queues without an explicit ownership transfer/barrier.
	SPUDGPU_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS = 1 << 0
};

///@}

/**
 * @brief Descriptor used for a physical GPU buffer.
 */
typedef struct spudgpu_buffer_desc {
	/// Is this spudgpu_buffer used as a Vertex, Index, Uniform/Constant,
	/// Storage Buffer? You can have multiple usages as this is a bitmask
	/// configuration.
	/// @see SPUDGPU_BUFFER_USAGE
	SPUDGPU_BUFFER_USAGE usage;

	/// Bitmask configuration specifying VRAM residency and CPU cache coherency
	/// rules.
	/// @see SPUDGPU_MEMORY_FLAGS
	SPUDGPU_MEMORY_FLAGS memory_flags;

	SPUDGPU_HEAP_FLAGS heap_flags;

	/// @see SPUDGPU_RESOURCE_FLAGS
	SPUDGPU_RESOURCE_FLAGS buffer_flags;

	/** @brief Hardware memory address.
	 * @note This will remain '0' until after the gpu_buffer is created.
	 * After creation, this will be assigned a value when calling
	 * 'spudgpu_get_buffer_desc(gpu_buffer)'.
	 */
	uint64_t gpu_address_location;

	/// Total allocated size of the buffer resource in bytes.
	uint64_t size;
#if _DEBUG

	/// @brief A string identifier used for diagnostic tracking.
	const char *debug_name;
#endif
} spudgpu_buffer_desc;

/**
 * @brief Allocates a physical GPU buffer resource.
 * @param[in] device The GPU device this buffer will be allocated on.
 * @param[in] desc   Pointer to the descriptor settings for the new buffer.
 * @param[out] out_buffer The new GPU buffer resource.
 * @return SPUD_SUCCESS or another SPUDRESULT.
 * @note Variable 'gpu_buffer_desc.gpu_address_location' should be zero,
 * otherwise this function will return NULL. The buffer will have an address
 * location after creation.
 */
SPUDRESULT spudgpu_create_buffer(
    spudgpu_device device,
    const spudgpu_buffer_desc *desc,
    spudgpu_buffer *out_buffer);

/**
 * @brief Safely deallocates a physical GPU buffer resource.
 * * Frees underlying driver resources and system/device memory allocations.
 * @param[in] buffer The buffer to destroy.
 * * @warning Any active `spudgpu_buffer_view` dependencies or pending GPU
 * command lists referencing this buffer must be cleared before destruction to
 * avoid hardware faults.
 */
void spudgpu_destroy_buffer(spudgpu_buffer buffer);

/**
 * @brief Retrieves the immutable configuration descriptor used to create the
 * given buffer.
 * @note Variable 'gpu_buffer_desc.gpu_address_location' is set when this
 * function is called using a valid buffer.
 * @param[in] buffer The target buffer handle to query.
 * @param[out] out_desc The descriptor of the target buffer handle.
 * @return SPUD_SUCCESS or another SPUDRESULT.
 */
SPUDRESULT spudgpu_get_buffer_desc(
    spudgpu_buffer buffer,
    spudgpu_buffer_desc *out_desc);

/**
 * @brief Configuration descriptor defining a sub-allocated window (view) into
 * an existing buffer.
 * * Buffer views are lightweight abstractions allowing you to reinterpret
 * specific segments of a parent buffer without duplicating or allocating
 * additional physical device memory.
 */
typedef struct spudgpu_buffer_view_desc {
	/// Handle to the GPU buffer that this view window reads from or writes to.
	spudgpu_buffer parent_buffer;

	/// The byte offset to where this specific view window begins inside the
	/// parent buffer.
	uint64_t offset_from_parent_buffer;

	/** * @brief The data structure element stride in bytes.
	 * * Crucial for structured storage arrays or vertex inputs (e.g., byte size
	 * of a single instance of your custom Vertex struct). Pass `0` for raw,
	 * unformatted buffers.
	 */
	uint64_t stride;

	/// The span/length of this specific buffer view window in bytes.
	uint64_t size;
} spudgpu_buffer_view_desc;

/**
 * @brief Creates a view window into a section of an existing GPU buffer.
 * @param[in] buffer The target GPU buffer containing the raw memory
 * allocation.
 * @param[in] desc   Pointer to the view configuration descriptor.
 * @param[out] out_buffer_view The new GPU buffer view.
 * @return SPUD_SUCCESS or another SPUDRESULT.
 */
SPUDRESULT spudgpu_create_buffer_view(
    spudgpu_buffer buffer,
    const spudgpu_buffer_view_desc *desc,
    spudgpu_buffer_view *out_buffer_view);

/**
 * @brief Destroys a GPU buffer view.
 * * Frees the virtual view mapping context. Does **not** impact the lifecycle
 * or data retention of the underlying parent GPU buffer.
 * @param[in] buffer The target GPU buffer view to destroy.
 */
void spudgpu_destroy_buffer_view(spudgpu_buffer_view buffer_view);

/**
 * @brief Retrieves the immutable configuration descriptor of an active buffer
 * view.
 * @param[in] view The GPU buffer view to get the descriptor of.
 * @param[out] out_desc The descriptor to be read.
 * @return SPUD_SUCCESS or another SPUDRESULT.
 */
SPUDRESULT spudgpu_get_buffer_view_desc(
    spudgpu_buffer_view view,
    spudgpu_buffer_view_desc *out_desc);

/**
 * @brief Maps a GPU buffer's memory into CPU-accessible address space.
 *
 * Only valid on buffers created with SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE.
 * Equivalent to D3D12's Map() / vkMapMemory().
 *
 * @param buffer  The target buffer to map.
 * @param offset  Byte offset into the buffer to begin the mapped region.
 * @param size    Number of bytes to map. Pass 0 to map the entire buffer.
 * @param ppData  Output pointer — receives the CPU-writable address.
 * @return SPUD_SUCCESS on success, an error code if buffer is NULL or not HOST_VISIBLE.
 */
SPUDRESULT spudgpu_map_buffer(
    spudgpu_buffer buffer,
    uint64_t offset,
    uint64_t size,
    void **ppData);

/**
 * @brief Unmaps a previously mapped buffer, releasing the CPU pointer.
 *
 * If the buffer was NOT created with HOST_COHERENT, you must call
 * spudgpu_flush_buffer() before unmapping to push writes to the GPU.
 *
 * @param buffer The buffer to unmap.
 */
void spudgpu_unmap_buffer(spudgpu_buffer buffer);

/**
 * @brief Flushes CPU writes to a non-coherent mapped buffer range.
 *
 * Only required when HOST_COHERENT is NOT set.
 * Equivalent to vkFlushMappedMemoryRanges().
 *
 * @param buffer  The buffer whose writes need flushing.
 * @param offset  Byte offset of the dirty region.
 * @param size    Byte length of the dirty region. Pass 0 for entire buffer.
 */
void spudgpu_flush_buffer(
    spudgpu_buffer buffer,
    uint64_t offset,
    uint64_t size);

/**
 * @brief Invalidates CPU caches for a mapped buffer range before reading
 * GPU-written data.
 *
 * Only required for HOST_CACHED readback buffers.
 * Equivalent to vkInvalidateMappedMemoryRanges().
 *
 * @param buffer  The buffer to invalidate.
 * @param offset  Byte offset of the region to invalidate.
 * @param size    Byte length of the region. Pass 0 for entire buffer.
 * @return SPUD_SUCCESS or another SPUDRESULT.
 */
SPUDRESULT spudgpu_invalidate_buffer(
    spudgpu_buffer buffer,
    uint64_t offset,
    uint64_t size);

typedef uint32_t SPUDGPU_IMAGE_TYPE;

enum {
	SPUDGPU_IMAGE_TYPE_NONE = 0,
	SPUDGPU_IMAGE_TYPE_1D   = 1 << 0,
	SPUDGPU_IMAGE_TYPE_2D   = 1 << 1,
	SPUDGPU_IMAGE_TYPE_3D   = 1 << 2
	// SPUDGPU_IMAGE_TYPE_CUBE = 1 << 3
};

/**
 * @brief Bitmask determining the type and capabilities of a GPU image object.
 * * @see SPUDGPU_IMAGE_USAGE_Constants
 */
typedef uint32_t SPUDGPU_IMAGE_USAGE;

/**
 * @name Image Usage Flags
 * @anchor SPUDGPU_IMAGE_USAGE_Constants
 * * Bitmask configurations dictating how the GPU image object is structured and
 * sampled.
 */
///@{
enum {
	/// No usage target specified. Uninitialized configuration state.
	SPUDGPU_IMAGE_USAGE_NONE = 0,

	SPUDGPU_IMAGE_USAGE_SAMPLED = 1 << 0,

	SPUDGPU_IMAGE_USAGE_COLOR_ATTACHMENT         = 1 << 1,
	SPUDGPU_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 1 << 2,
	SPUDGPU_IMAGE_USAGE_STORAGE                  = 1 << 3,
	SPUDGPU_IMAGE_USAGE_TRANSFER_SRC             = 1 << 4,
	SPUDGPU_IMAGE_USAGE_TRANSFER_DST             = 1 << 5,

	/// Image is compatible with being presented to a swap chain surface.
	/// Reserved for future callers creating a presentable image outside of
	/// spudgpu_swap_chain's own internally-owned backbuffers — no current
	/// caller in this workspace sets it.
	SPUDGPU_IMAGE_USAGE_PRESENTABLE               = 1 << 6,

	/** * @brief Marks a render-pass attachment as transient: written and
	 * read only within the render pass that produces it, never sampled,
	 * read back, or persisted afterward.
	 *
	 * On tile-based GPUs (all Metal devices, and mobile Vulkan hardware such
	 * as Mali/Adreno) this lets the implementation back the image with
	 * on-chip tile memory instead of a real allocation, entirely skipping
	 * device-memory traffic — the standard technique for a multisampled
	 * render target that only exists to be resolved, or a deferred-shading
	 * G-buffer read back via programmable blending within the same pass.
	 * Maps to Metal's `MTLStorageModeMemoryless` and Vulkan's
	 * `VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT` +
	 * `VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT`; D3D12 has no equivalent and
	 * treats it as a no-op hint.
	 *
	 * Must be combined with `SPUDGPU_IMAGE_USAGE_COLOR_ATTACHMENT` and/or
	 * `SPUDGPU_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT`, and must not be
	 * combined with `SPUDGPU_IMAGE_USAGE_SAMPLED`, `_STORAGE`,
	 * `_TRANSFER_SRC`, `_TRANSFER_DST`, or `_PRESENTABLE`, nor with
	 * `SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE` — all of those require the image
	 * to be readable/writable outside the render pass that produced it,
	 * which a transient attachment structurally cannot be. Enforced at
	 * creation on every backend, not just the ones that act on it, so a
	 * desc that's invalid here doesn't silently behave differently
	 * depending on which backend happens to be compiled in.
	 */
	SPUDGPU_IMAGE_USAGE_TRANSIENT_ATTACHMENT      = 1 << 7
};

typedef struct SPUDGPU_DEPTH_STENCIL_VALUE {
	float depth;
	uint8_t stencil;
} SPUDGPU_DEPTH_STENCIL_VALUE;

typedef struct SPUDGPU_CLEAR_VALUE {
	SPUDGPU_FORMAT format;
	union {
		float color[4];
		SPUDGPU_DEPTH_STENCIL_VALUE depth_stencil;
	};
} SPUDGPU_CLEAR_VALUE;

///@}

/**
 * @brief Configuration descriptor used to allocate a physical GPU image
 * resource (texture/surface).
 */
typedef struct spudgpu_image_desc {
	/// Bitmask configuration specifying texture layout type (2D, 3D, etc.).
	/// @see SPUDGPU_IMAGE_USAGE
	SPUDGPU_IMAGE_USAGE usage;

	SPUDGPU_IMAGE_TYPE type;

	/// Bitmask configuration specifying VRAM residency rules.
	/// Typically set to `SPUDGPU_MEMORY_FLAGS_DEVICE_LOCAL` for performance.
	/// @see SPUDGPU_MEMORY_FLAGS
	SPUDGPU_MEMORY_FLAGS memory_flags;

	SPUDGPU_HEAP_FLAGS heap_flags;

	/// @see SPUDGPU_RESOURCE_FLAGS
	SPUDGPU_RESOURCE_FLAGS image_flags;

	SPUDGPU_CLEAR_VALUE clear_value;

	/** @brief Hardware memory address.
	 * @note This will remain '0' until after the gpu_image is created.
	 * After creation, this will be assigned a value when calling
	 * 'spudgpu_get_image_desc(gpu_image)'.
	 */
	uint64_t gpu_address_location;

	/// The texel data layout and channel bit-depth configuration (e.g.,
	/// RGBA8_UNORM, R32_SFLOAT).
	/// @see SPUDGPU_FORMAT
	SPUDGPU_FORMAT format;

	/// The horizontal width the image base layer in texels/pixels.
	uint32_t width;

	/// The vertical height of the image base layer in texels/pixels.
	uint32_t height;

	/** * @brief The volumetric depth of the image in texels.
	 * * Must be set to `1` for standard 2D textures. Represents the third axis
	 * for `SPUDGPU_IMAGE_USAGE_TEXTURE3D` resources.
	 */
	uint32_t depth;

	/** * @brief Number of layers inside a texture array allocation.
	 * * Allows for texture arrays (e.g., a series of 2D textures grouped into a
	 * single bindable allocation). Must be set to at least `1` for standard
	 * standalone textures.
	 */
	uint32_t array_layers;

	/** * @brief Total number of downsampled mipmap resolution steps allocated
	 * for this texture.
	 * * Pass `1` for no mipmaps. Level [0] represents the original
	 * full-resolution image data.
	 */
	uint32_t mip_levels;
#if _DEBUG
	/// @brief A string identifier used for diagnostic tracking.
	const char *debug_name;
#endif
} spudgpu_image_desc;

/**
 * @brief Allocates a physical GPU image/texture.
 * @param[in] device The GPU device this image will be allocated on.
 * @param[in] desc   Pointer to the descriptor settings for the new image.
 * @param[out] out_image The new GPU image.
 * @return SPUD_SUCCESS or another SPUDRESULT.
 * * @note Variable 'gpu_image_desc.gpu_address_location' should be zero,
 * otherwise this function will return NULL. The image will have an address
 * location after creation.
 */
SPUDRESULT spudgpu_create_image(
    spudgpu_device device,
    const spudgpu_image_desc *desc,
    spudgpu_image *out_image);

/**
 * @brief Safely deallocates a physical GPU image.
 * * Frees underlying driver resources and releases system/device memory
 * allocations.
 * @param[in] image  The target GPU image to destroy.
 * * @warning Any active command streams, image views, descriptors, or render
 * passes currently binding or writing to this texture must finish execution on
 * the GPU timeline before invocation.
 */
void spudgpu_destroy_image(spudgpu_image image);

/**
 * @brief Retrieves the configuration descriptor used to create the GPU image
 * object.
 * @param[in] image The target GPU image.
 * @param[out] out_desc The descriptor to be read.
 * @return SPUD_SUCCESS or another SPUDRESULT.
 */
SPUDRESULT spudgpu_get_image_desc(
    spudgpu_image image,
    spudgpu_image_desc *out_desc);

typedef uint32_t SPUDGPU_IMAGE_VIEW_TYPE;

enum {
	SPUDGPU_IMAGE_VIEW_TYPE_NONE       = 0,
	SPUDGPU_IMAGE_VIEW_TYPE_1D         = 1 << 0,
	SPUDGPU_IMAGE_VIEW_TYPE_2D         = 1 << 1,
	SPUDGPU_IMAGE_VIEW_TYPE_3D         = 1 << 2,
	SPUDGPU_IMAGE_VIEW_TYPE_CUBE       = 1 << 3,
	SPUDGPU_IMAGE_VIEW_TYPE_1D_ARRAY   = 1 << 4,
	SPUDGPU_IMAGE_VIEW_TYPE_2D_ARRAY   = 1 << 5,
	SPUDGPU_IMAGE_VIEW_TYPE_CUBE_ARRAY = 1 << 6
};

/**
 * @brief Defines a specific subsection (slice) of a multi-layered or mipmapped
 * texture.
 * * Allows a pipeline stage to isolate and bind specific layers or mip levels
 * of a parent image rather than the entire allocation.
 */
typedef struct spudgpu_image_view_desc_subresource_range {
	/** * @brief Bitmask determining which structural aspect of the texture is
	 * targeted.
	 * * Typically maps to flags like `COLOR`, `DEPTH`, or `STENCIL`. Tells the
	 * driver whether to look at pixel data or depth/stencil metadata buffers.
	 */
	uint64_t aspect_mask;

	/// The starting mipmap level index for this view window (0 being the
	/// highest resolution).
	uint64_t base_mip_level;

	/// The total number of downsampled mipmap levels to include in this view
	/// range.
	uint64_t mip_level_count;

	/// The starting array layer index for this view window (used for texture
	/// arrays/cubemaps).
	uint64_t base_array_layer;

	/// The total number of layers to include in this view range.
	uint64_t array_layer_count;
} spudgpu_image_view_desc_subresource_range;

/**
 * @brief Configuration descriptor defining a read/write window (view) into an
 * existing GPU image.
 * * Like buffer views, image views do not allocate raw VRAM; they wrap existing
 * physical images to declare how shader stages should interpret their
 * boundaries and contents.
 */
typedef struct spudgpu_image_view_desc {
	/// Handle to the physical GPU image containing the pixel data.
	spudgpu_image parent_image;

	/// The image view type
	/// @see SPUDGPU_IMAGE_VIEW_TYPE
	uint32_t type;

	// TODO: Swizzle indentities? Learn about that

	/// The isolated subresource layer and mip boundary definitions for this
	/// view.
	spudgpu_image_view_desc_subresource_range subresource_range;
} spudgpu_image_view_desc;

/**
 * @brief Creates a GPU image view mapping to a specific texture subresource
 * range.
 * @param[in] image The target parent image resource.
 * @param[in] desc  Pointer to the configuration descriptor.
 * @param[out] out_image_view The new image view.
 * @return SPUD_SUCCESS or another SPUDRESULT.
 */
SPUDRESULT spudgpu_create_image_view(
    spudgpu_image image,
    const spudgpu_image_view_desc *desc,
    spudgpu_image_view *out_image_view);

/**
 * @brief Destroys a GPU image view.
 * * This does not affect the pixel/texel data of the parent GPU image object,
 * as it only destroys this view into that GPU image object.
 * @param[in] image_view  The GPU image view to destroy.
 */
void spudgpu_destroy_image_view(spudgpu_image_view image_view);

/**
 * @brief Retrieves the configuration descriptor of the GPU image view.
 * @param[in] image_view The GPU image view handle to query.
 * @param[out] out_desc The configuration descriptor to be read.
 * @return SPUD_SUCCESS or another SPUDRESULT.
 */
SPUDRESULT spudgpu_get_image_view_desc(
    spudgpu_image_view image_view,
    spudgpu_image_view_desc *out_desc);

// ============================================================================
//  Image Layout
//  Maps to: VkImageLayout (Vulkan)
// ============================================================================

typedef uint32_t SPUDGPU_IMAGE_LAYOUT;

enum {
	SPUDGPU_IMAGE_LAYOUT_UNDEFINED                        = 0,
	SPUDGPU_IMAGE_LAYOUT_GENERAL                          = 1,
	SPUDGPU_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL         = 2,
	SPUDGPU_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
	SPUDGPU_IMAGE_LAYOUT_SHADER_READ_ONLY                 = 4,
	SPUDGPU_IMAGE_LAYOUT_TRANSFER_SRC                     = 5,
	SPUDGPU_IMAGE_LAYOUT_TRANSFER_DST                     = 6,
	SPUDGPU_IMAGE_LAYOUT_PRESENT_SRC                      = 7,
};

// ============================================================================
//  Image Barrier
//  Maps to: vkCmdPipelineBarrier (Vulkan)
// ============================================================================

/**
 * @brief Transitions an image from one layout to another, inserting the
 * necessary pipeline barrier so dependent stages wait correctly.
 *
 * Call before spudgpu_cmd_begin_rendering to move a swap chain image from
 * UNDEFINED/PRESENT_SRC to COLOR_ATTACHMENT_OPTIMAL, and after
 * spudgpu_cmd_end_rendering to move it back to PRESENT_SRC.
 *
 * @param[in] cmd          The active recording command list.
 * @param[in] image        The image whose layout is being changed.
 * @param[in] old_layout   The current layout of the image.
 * @param[in] new_layout   The target layout to transition into.
 */
void spudgpu_cmd_image_barrier(
    spudgpu_command_list cmd,
    spudgpu_image image,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout);

/**
 * @brief Variant that takes a raw image view instead of a spudgpu_image.
 *
 * Useful for swap chain images, which are not created through
 * spudgpu_create_image and so don't have a spudgpu_image handle. The image view
 * must have been created from the target image (e.g. a swap chain image view).
 *
 * @param[in] cmd          The active recording command list.
 * @param[in] image_view   An image view whose parent image will be
 * transitioned.
 * @param[in] old_layout   The current layout of the image.
 * @param[in] new_layout   The target layout to transition into.
 */
void spudgpu_cmd_image_barrier_view(
    spudgpu_command_list cmd,
    spudgpu_image_view image_view,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout);

/**
 * @brief Transitions only a specific mip/layer subresource range of an image,
 * leaving the rest of the image's subresources in whatever layout they were
 * already in.
 *
 * Required whenever different subresources of the same image must be in
 * different layouts at once — most notably mip chain generation via
 * spudgpu_cmd_blit_image, where mip level N must be SPUDGPU_IMAGE_LAYOUT_
 * TRANSFER_SRC while mip level N+1 is simultaneously SPUDGPU_IMAGE_LAYOUT_
 * TRANSFER_DST. spudgpu_cmd_image_barrier cannot express this since it
 * transitions the whole image at once.
 *
 * @param[in] cmd        The active recording command list.
 * @param[in] image      The image whose subresource range is being changed.
 * @param[in] range      The mip levels and array layers to transition.
 * @param[in] old_layout The current layout of the given subresource range.
 * @param[in] new_layout The target layout to transition into.
 */
void spudgpu_cmd_image_barrier_subresource(
    spudgpu_command_list cmd,
    spudgpu_image image,
    const spudgpu_image_view_desc_subresource_range *range,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout);
/**
 * @brief Defines the current state/layout of a resource (useful for pipeline
 * barriers).
 */
typedef uint32_t SPUDGPU_RESOURCE_STATE;
enum {
	SPUDGPU_RESOURCE_STATE_COMMON = 0,
	SPUDGPU_RESOURCE_STATE_VERTEX_BUFFER,
	SPUDGPU_RESOURCE_STATE_INDEX_BUFFER,
	SPUDGPU_RESOURCE_STATE_RENDER_TARGET,
	SPUDGPU_RESOURCE_STATE_DEPTH_WRITE,
	SPUDGPU_RESOURCE_STATE_SHADER_RESOURCE,  // Read-only in shader
	SPUDGPU_RESOURCE_STATE_UNORDERED_ACCESS, // Read/Write (SSBO/UAV)
	SPUDGPU_RESOURCE_STATE_PRESENT
};

/**
 * @brief Pipeline stage flags used in semaphore wait masks on queue submission.
 *
 * Values are bit-compatible with VkPipelineStageFlagBits and may be OR'd
 * together. One mask must be provided per wait semaphore in
 * spudgpu_submit_desc.
 */
typedef uint32_t SPUDGPU_PIPELINE_STAGE;
enum {
	SPUDGPU_PIPELINE_STAGE_TOP_OF_PIPE             = 0x00000001,
	SPUDGPU_PIPELINE_STAGE_DRAW_INDIRECT           = 0x00000002,
	SPUDGPU_PIPELINE_STAGE_VERTEX_INPUT            = 0x00000004,
	SPUDGPU_PIPELINE_STAGE_VERTEX_SHADER           = 0x00000008,
	SPUDGPU_PIPELINE_STAGE_TESSELLATION_CONTROL    = 0x00000010,
	SPUDGPU_PIPELINE_STAGE_TESSELLATION_EVALUATION = 0x00000020,
	SPUDGPU_PIPELINE_STAGE_GEOMETRY_SHADER         = 0x00000040,
	SPUDGPU_PIPELINE_STAGE_FRAGMENT_SHADER         = 0x00000080,
	SPUDGPU_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS    = 0x00000100,
	SPUDGPU_PIPELINE_STAGE_LATE_FRAGMENT_TESTS     = 0x00000200,
	SPUDGPU_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT = 0x00000400,
	SPUDGPU_PIPELINE_STAGE_COMPUTE_SHADER          = 0x00000800,
	SPUDGPU_PIPELINE_STAGE_TRANSFER                = 0x00001000,
	SPUDGPU_PIPELINE_STAGE_BOTTOM_OF_PIPE          = 0x00002000,
	SPUDGPU_PIPELINE_STAGE_ALL_GRAPHICS            = 0x00008000,
	SPUDGPU_PIPELINE_STAGE_ALL_COMMANDS            = 0x00010000,
};

typedef struct spudgpu_buffer_barrier {
	spudgpu_buffer buffer;
	SPUDGPU_RESOURCE_STATE state_before;
	SPUDGPU_RESOURCE_STATE state_after;
} spudgpu_buffer_barrier;

typedef struct spudgpu_image_barrier {
	spudgpu_image image;
	SPUDGPU_RESOURCE_STATE state_before;
	SPUDGPU_RESOURCE_STATE state_after;
	// Optional: subresource ranges (mips/layers) if needed
} spudgpu_image_barrier;

/**
 * @brief Injects an execution and memory barrier into the command list.
 * Translates to vkCmdPipelineBarrier in Vulkan or MTLBarrier/MTLEvent in Metal.
 */
void spudgpu_cmd_pipeline_barrier(
    spudgpu_command_list cmd,
    const spudgpu_buffer_barrier *buffer_barriers,
    uint32_t buffer_barrier_count,
    const spudgpu_image_barrier *image_barriers,
    uint32_t image_barrier_count);

/** window_handle Opaque generic pointer targeting the host operating system's
 * native window object.
 * * Must capture and forward the platform-appropriate descriptor handle:
 * - **Windows (Win32):** Pass raw `HWND` pointer.
 * - **macOS (AppKit):** Pass native `NSWindow*` or target `CAMetalLayer*`.
 * - **Linux (X11 / Wayland):** Pass raw `Window` identity or `wl_surface*`.
 *
 * display_handle Platform display connection handle.
 * - Linux Wayland: wl_display*
 * - Linux X11:     Display*
 * - Windows:       unused (set to NULL)
 */
SPUDRESULT spudgpu_create_surface(
    spudgpu_instance instance,
    void *window_handle,
    void *display_handle,
    spudgpu_surface *out_surface);

void spudgpu_destroy_surface(spudgpu_surface surface);

typedef bool (*spudgpu_surface_create_fn)(
    void *vk_instance, // VkInstance, typed as void* to keep spudgpu.h Vulkan-free
    void *user_data,
    void *out_surface); // VkSurfaceKHR*, typed as void*

spudgpu_surface spudgpu_create_surface_from_callback(
    spudgpu_instance instance,
    void *user_data,
    spudgpu_surface_create_fn create_fn);

/**
 * @brief Dictates the sync relationship between the GPU frame completion and
 * the monitor's refresh cycle.
 * * Governs whether the engine caps frame rates to prevent screen tearing or
 * uses extra back buffers to reduce input latency.
 * * @see SPUDGPU_PRESENT_MODE_Constants
 */
typedef uint32_t SPUDGPU_PRESENT_MODE;

/**
 * @name Presentation Sync Modes
 * @anchor SPUDGPU_PRESENT_MODE_Constants
 * * Configurations mapped to underlying driver swap rules (e.g.,
 * VkPresentModeKHR or NSOpenGLContext).
 */
///@{
enum {
	/** * @brief Immediate Mode (V-Sync Disabled).
	 * * The GPU transfers completed frames to the screen instantly. Offers the
	 * lowest possible input latency but results in visible screen tearing as
	 * the monitor splits images mid-refresh.
	 */
	SPUDGPU_PRESENT_MODE_IMMEDIATE = 0,

	/** * @brief FIFO Mode (V-Sync Enabled - First In, First Out).
	 * * Frames are queued up and synchronized strictly with the display's
	 * vertical refresh rate (e.g., 60Hz/144Hz). Completely eliminates screen
	 * tearing, but will throttle the CPU/GPU thread if the queue fills up.
	 */
	SPUDGPU_PRESENT_MODE_FIFO = 1,

	/** * @brief Mailbox Mode (Triple Buffering / Ultra-Low Latency V-Sync).
	 * * Synchronizes with the vertical refresh rate to eliminate tearing, but
	 * does not block the application when the queue is full. Instead, the
	 * newest completed frame continuously replaces unrendered frames in the
	 * queue, ensuring the monitor always pulls the absolute freshest data.
	 */
	SPUDGPU_PRESENT_MODE_MAILBOX = 2
};

typedef uint8_t SPUDGPU_FULLSCREEN_MODE;
enum {
	SPUDGPU_FULLSCREEN_MODE_WINDOWED   = 0,
	SPUDGPU_FULLSCREEN_MODE_FULLSCREEN = 1,
	SPUDGPU_FULLSCREEN_MODE_BORDERLESS = 2,
};

///@}

/**
 * @brief Configuration descriptor used to initialize a rendering surface target
 * linked to the OS windowing system.
 * * Handled by the driver to allocate a ring buffer of textures (back buffers)
 * that flip onto the screen.
 */
typedef struct spudgpu_swap_chain_desc {
	spudgpu_surface surface;

	/** * @brief The queue this swap chain's presentation is ordered against.
	 * * Required on every backend, not just a hint: D3D12 must bind the
	 * DXGI swap chain to a specific ID3D12CommandQueue at creation time and
	 * cannot rebind it later, so this is where that queue comes from instead
	 * of a backend silently assuming "the graphics queue." Render all work
	 * destined for this swap chain's images on this same queue before
	 * calling spudgpu_swap_chain_present - presentation is ordered relative
	 * to it, not to whichever queue you happen to submit on.
	 */
	spudgpu_command_queue queue;

	/// Requested back-buffer width in pixels. Usually matches the window client
	/// area width.
	uint32_t width;

	/// Requested back-buffer height in pixels. Usually matches the window
	/// client area height.
	uint32_t height;

	/** * @brief Total number of images in the swap chain ring buffer.
	 * * Typically configured as `2` for simple Double Buffering or `3` for
	 * Mailbox Triple Buffering layouts.
	 */
	uint32_t buffer_count;

	/// The color and pixel format schema requested for the surface (e.g.,
	/// BGRA8_UNORM or RGBA16_SFLOAT).
	SPUDGPU_FORMAT format;

	/// The sync and presentation timing rule to apply when presenting completed
	/// frames.
	/// /// @see SPUDGPU_PRESENT_MODE
	SPUDGPU_PRESENT_MODE present_mode;

	/** * @brief Toggles whether the graphics device initializes in exclusive
	 * fullscreen monitor mode or stays bounded inside a standard desktop window
	 * framework.
	 */
	SPUDGPU_FULLSCREEN_MODE fullscreen_mode;
} spudgpu_swap_chain_desc;

/**
 * Creates a new swap chain for rendering to a window.
 * @param[in] device The active spudgpu device.
 * @param[in] desc Pointer to the configuration descriptor.
 * @param[out] out_swap_chain The new swap chain.
 * @return SPUD_SUCCESS or another SPUDRESULT.
 */
SPUDRESULT spudgpu_create_swap_chain(
    spudgpu_device device,
    const spudgpu_swap_chain_desc *desc,
    spudgpu_swap_chain *out_swap_chain);

/**
 * Destroys a swap chain and frees its resources.
 */
void spudgpu_destroy_swap_chain(spudgpu_swap_chain swap_chain);

SPUDRESULT spudgpu_get_swap_chain_desc(
    spudgpu_swap_chain swap_chain,
    spudgpu_swap_chain_desc *out_desc);

/**
 * Acquires the index of the next available backbuffer image.
 * This blocks if the GPU is falling too far behind.
 * @param swap_chain The swap chain to acquire from.
 * @return The index of the next backbuffer (e.g., 0, 1, or 2).
 */
uint32_t spudgpu_swap_chain_acquire_next_image(spudgpu_swap_chain swap_chain);

/**
 * Presents the current back buffer to the screen.
 * Call this after you have finished recording and submitting commands for the
 * frame to the queue given as spudgpu_swap_chain_desc::queue - presentation
 * is ordered against that queue's work, not whichever queue you happened to
 * submit on.
 * * @param swap_chain The swap chain holding the image to display.
 */
void spudgpu_swap_chain_present(spudgpu_swap_chain swap_chain);

/**
 * @brief Returns the image view for the given swap chain image index.
 * Use the index returned by vkAcquireNextImageKHR (or your equivalent).
 */
spudgpu_image_view spudgpu_get_swap_chain_image_view(
    spudgpu_swap_chain swap_chain,
    uint32_t image_index);

/**
 * @brief Returns the image-available semaphore for the swap chain's current
 * frame slot. This semaphore is signaled by vkAcquireNextImageKHR when the
 * image is ready to render into. Pass it as a wait semaphore in
 * spudgpu_submit_desc. Do NOT destroy it — the swap chain owns it.
 */
spudgpu_semaphore spudgpu_swap_chain_get_image_available_semaphore(spudgpu_swap_chain swap_chain);

/**
 * @brief Returns the render-finished semaphore for the swap chain's current
 * frame slot. Signal this semaphore on queue submission so the presentation
 * engine waits for rendering. Pass it as a signal semaphore in
 * spudgpu_submit_desc. Do NOT destroy it — the swap chain owns it.
 */
spudgpu_semaphore spudgpu_swap_chain_get_render_finished_semaphore(spudgpu_swap_chain swap_chain);

/**
 * @brief Returns the in-flight fence for the swap chain's current frame slot.
 * Signal it on queue submission; spudgpu_swap_chain_acquire_next_image waits
 * and resets it. Pass it as signal_fence in spudgpu_submit_desc. Do NOT destroy
 * it — the swap chain owns it.
 */
spudgpu_fence spudgpu_swap_chain_get_in_flight_fence(spudgpu_swap_chain swap_chain);

// ============================================================================
//  Descriptor Set Layout
//  Maps to: VkDescriptorSetLayout (Vulkan) / MTLArgumentEncoder schema (Metal)
// ============================================================================


/**
 * @brief Enumerates the kinds of resources a shader binding slot can hold.
 *
 * Maps directly to VkDescriptorType on Vulkan and the corresponding
 * MTLArgumentEncoder argument kinds on Metal.
 */
typedef uint32_t SPUDGPU_DESCRIPTOR_TYPE;

enum {
	/// Read-only structured constant block (UBO / constant buffer).
	SPUDGPU_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 0,

	/// Read/write large data array (SSBO / structured buffer).
	SPUDGPU_DESCRIPTOR_TYPE_STORAGE_BUFFER = 1,

	/// Combined image + sampler in a single binding (texture2D + sampler).
	SPUDGPU_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 2,

	/// Sampled image without a sampler (use alongside SAMPLER binding).
	SPUDGPU_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 3,

	/// Standalone sampler object.
	SPUDGPU_DESCRIPTOR_TYPE_SAMPLER = 4,

	/// Image the shader can both read and write (compute UAV / storage image).
	SPUDGPU_DESCRIPTOR_TYPE_STORAGE_IMAGE = 5,
};

typedef uint32_t SPUDGPU_SHADER_STAGE;

enum {
	SPUDGPU_SHADER_STAGE_NONE                    = 0,
	SPUDGPU_SHADER_STAGE_VERTEX                  = 1 << 0,
	SPUDGPU_SHADER_STAGE_FRAGMENT                = 1 << 1,
	SPUDGPU_SHADER_STAGE_COMPUTE                 = 1 << 2,
	SPUDGPU_SHADER_STAGE_GEOMETRY                = 1 << 3,
	SPUDGPU_SHADER_STAGE_TESSELLATION_CONTROL    = 1 << 4,
	SPUDGPU_SHADER_STAGE_TESSELLATION_EVALUATION = 1 << 5
};

/**
 * @brief Describes a single binding slot within a descriptor set layout.
 *
 * Each entry corresponds to one `layout(set=N, binding=M)` declaration in GLSL.
 */
typedef struct spudgpu_descriptor_binding_desc {
	/// Slot index matching `layout(binding = N)` in GLSL. Must be unique within
	/// a layout.
	uint32_t binding;

	/// Resource type expected at this binding slot.
	/// @see SPUDGPU_DESCRIPTOR_TYPE
	SPUDGPU_DESCRIPTOR_TYPE descriptor_type;

	/// Number of resources in this binding. Use 1 for a single resource,
	/// or N for a fixed-size array (e.g. `uniform sampler2D textures[8]`).
	uint32_t count;

	/// Bitmask of shader stages that can access this binding.
	/// @see SPUDGPU_SHADER_STAGE
	SPUDGPU_SHADER_STAGE stage_flags;
} spudgpu_descriptor_binding_desc;

#define SPUDGPU_MAX_DESCRIPTOR_BINDINGS_PER_SET 16

/**
 * @brief Configuration descriptor for a descriptor set layout.
 *
 * Describes the binding slots that make up one set. Pass this to
 * spudgpu_create_descriptor_set_layout(), then hand the resulting
 * handle into spudgpu_shader_pipeline_desc::descriptor_set_layouts[].
 */
typedef struct spudgpu_descriptor_set_layout_desc {
#if _DEBUG
	const char *debug_name;
#endif

	spudgpu_descriptor_binding_desc bindings[SPUDGPU_MAX_DESCRIPTOR_BINDINGS_PER_SET];
	uint32_t binding_count;
} spudgpu_descriptor_set_layout_desc;
/**
 * @brief Creates an immutable descriptor set layout (schema).
 *
 * On Vulkan this allocates a VkDescriptorSetLayout. On Metal it creates an
 * MTLArgumentEncoder schema. The resulting handle is passed into the pipeline
 * desc to declare the expected binding shape.
 *
 * @param[in] device The GPU device to create this layout on.
 * @param[in] desc   Pointer to the binding slot configuration.
 * @param[out] out_layout The new Descriptor Set Layout.
 * @return SPUD_SUCCESS or another SPUDRESULT.
 */
SPUDRESULT spudgpu_create_descriptor_set_layout(
    spudgpu_device device,
    const spudgpu_descriptor_set_layout_desc *desc,
    spudgpu_descriptor_set_layout *out_layout);

/**
 * @brief Destroys a descriptor set layout.
 *
 * @warning All descriptor sets allocated from this layout, and all pipelines
 * referencing it, must be destroyed before calling this.
 */
void spudgpu_destroy_descriptor_set_layout(spudgpu_descriptor_set_layout layout);
// ============================================================================
//  Descriptor Pool
//  Maps to: VkDescriptorPool (Vulkan) / heap of argument buffers (Metal)
// ============================================================================

/**
 * @brief Declares how many descriptors of each type a pool should pre-allocate.
 *
 * Vulkan requires knowing the total capacity up front. Size your pool to cover
 * the worst-case count across all frames-in-flight.
 */
typedef struct spudgpu_descriptor_pool_size {
	SPUDGPU_DESCRIPTOR_TYPE descriptor_type;
	uint32_t count;
} spudgpu_descriptor_pool_size;

#define SPUDGPU_MAX_DESCRIPTOR_POOL_SIZES 8

/**
 * @brief Configuration descriptor for a descriptor pool.
 */
typedef struct spudgpu_descriptor_pool_desc {
#if _DEBUG
	const char *debug_name;
#endif

	/// Maximum number of descriptor sets that can be allocated from this pool.
	uint32_t max_sets;

	spudgpu_descriptor_pool_size pool_sizes[SPUDGPU_MAX_DESCRIPTOR_POOL_SIZES];
	uint32_t pool_size_count;
} spudgpu_descriptor_pool_desc;

/**
 * @brief Allocates a descriptor pool — the backing memory for descriptor sets.
 *
 * Create one pool per frame-in-flight (or one large shared pool) and reset it
 * each frame rather than allocating/freeing individual sets every frame.
 *
 * @param[in] device The GPU device to create this pool on.
 * @param[in] desc   Pointer to the capacity configuration.
 * @param[out] out_pool The new Descriptor Pool.
 * @return SPUD_SUCCESS or another SPUDRESULT.
 */
SPUDRESULT spudgpu_create_descriptor_pool(
    spudgpu_device device,
    const spudgpu_descriptor_pool_desc *desc,
    spudgpu_descriptor_pool *out_pool);

/**
 * @brief Resets the pool, freeing all sets allocated from it in bulk.
 *
 * Cheaper than freeing sets individually. Call once per frame before
 * re-recording new descriptor writes.
 *
 * @param[in] pool   The pool to reset.
 */
void spudgpu_reset_descriptor_pool(spudgpu_descriptor_pool pool);

/**
 * @brief Destroys a descriptor pool and all sets allocated from it.
 *
 * @warning All command lists currently using sets from this pool must have
 * finished GPU execution before calling this.
 */
void spudgpu_destroy_descriptor_pool(spudgpu_descriptor_pool pool);
// ============================================================================
//  Descriptor Set
//  Maps to: VkDescriptorSet (Vulkan) / MTLBuffer argument buffer (Metal)
// ============================================================================

#define SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS 4

/**
 * @brief Allocation descriptor for one or more descriptor sets.
 *
 * All sets in a single call are allocated from the same pool in one
 * driver round-trip (matches vkAllocateDescriptorSets semantics).
 */
typedef struct spudgpu_descriptor_set_desc {
	spudgpu_descriptor_pool pool;

	/// Each element describes the layout for one set being allocated.
	spudgpu_descriptor_set_layout set_layouts[SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS];
	uint32_t set_count;
} spudgpu_descriptor_set_desc;

/**
 * @brief Allocates descriptor sets from a pool.
 *
 * @param[in]  device    The GPU device that owns the pool.
 * @param[in]  desc      Allocation configuration (pool + layouts).
 * @param[out] out_sets  Caller-supplied array that receives the allocated
 * handles. Must be at least desc->set_count elements wide.
 * @return SPUD_SUCCESS; an error code if the pool is out of capacity.
 */
SPUDRESULT spudgpu_create_descriptor_sets(
    spudgpu_device device,
    const spudgpu_descriptor_set_desc *desc,
    spudgpu_descriptor_set *out_sets);
// ============================================================================
//  Descriptor Writes
//  Wires actual GPU resources into the allocated binding slots.
// ============================================================================

/**
 * @brief Describes a buffer range to write into a binding slot.
 */
typedef struct spudgpu_descriptor_buffer_info {
	spudgpu_buffer buffer;

	/// Byte offset from the start of the buffer to begin the binding window.
	uint64_t offset;

	/// Byte size of the binding window. Pass 0 to bind the entire buffer.
	uint64_t range;
} spudgpu_descriptor_buffer_info;

/**
 * @brief Describes an image view + sampler to write into a binding slot.
 */
typedef struct spudgpu_descriptor_image_info {
	spudgpu_image_view image_view;

	/**
	 * @brief The layout the image is expected to be in when shaders access it.
	 *
	 * On Vulkan this maps to VkImageLayout. Common values:
	 *   - SPUDGPU_IMAGE_LAYOUT_SHADER_READ_ONLY for sampled textures.
	 *   - SPUDGPU_IMAGE_LAYOUT_GENERAL for storage images (read/write).
	 *
	 * @see SPUDGPU_IMAGE_LAYOUT
	 */
	uint32_t image_layout;
} spudgpu_descriptor_image_info;

/**
 * @brief A single write operation targeting one binding slot in a descriptor
 * set.
 *
 * Fill either buffer_info or image_info depending on the descriptor_type.
 * The unused field is ignored by the backend.
 */
typedef struct spudgpu_write_descriptor_set {
	/// The descriptor set to write into.
	spudgpu_descriptor_set dst_set;

	/// The binding slot index to update (matches
	/// spudgpu_descriptor_binding_desc::binding).
	uint32_t dst_binding;

	/// First array element to update. Use 0 for non-array bindings.
	uint32_t dst_array_element;

	/// Number of descriptors to update starting at dst_array_element.
	uint32_t descriptor_count;

	/// The type of descriptor being written. Must match the layout's declared
	/// type.
	SPUDGPU_DESCRIPTOR_TYPE descriptor_type;

	/// Set when writing UNIFORM_BUFFER or STORAGE_BUFFER descriptors.
	const spudgpu_descriptor_buffer_info *buffer_info;

	/// Set when writing SAMPLED_IMAGE, STORAGE_IMAGE, or COMBINED_IMAGE_SAMPLER
	/// descriptors.
	const spudgpu_descriptor_image_info *image_info;
} spudgpu_write_descriptor_set;

/**
 * @brief Writes resource handles into one or more descriptor sets.
 *
 * This is the equivalent of vkUpdateDescriptorSets. Call this after allocating
 * sets and before binding them to a command list.
 *
 * @param[in] device       The GPU device that owns the sets.
 * @param[in] writes       Array of write operations.
 * @param[in] write_count  Number of elements in the writes array.
 */
void spudgpu_update_descriptor_sets(
    spudgpu_device device,
    const spudgpu_write_descriptor_set *writes,
    uint32_t write_count);

// ============================================================================
//  Command list binding
// ============================================================================

/**
 * @brief Binds descriptor sets to the pipeline for subsequent draw or dispatch
 * calls.
 *
 * Maps to vkCmdBindDescriptorSets. Call after binding the pipeline and before
 * the draw/dispatch.
 *
 * @param[in] cmd          The active command list.
 * @param[in] pipeline     The graphics pipeline whose layout defines the set
 * slots.
 * @param[in] first_set    The set index of the first element in sets[] (usually
 * 0).
 * @param[in] sets         Array of descriptor set handles to bind.
 * @param[in] set_count    Number of sets to bind.
 */
void spudgpu_cmd_bind_descriptor_sets(
    spudgpu_command_list cmd,
    spudgpu_shader_pipeline pipeline,
    uint32_t first_set,
    const spudgpu_descriptor_set *sets,
    uint32_t set_count);

/**
 * @brief Compute-pipeline variant of spudgpu_cmd_bind_descriptor_sets.
 */
void spudgpu_cmd_bind_descriptor_sets_compute(
    spudgpu_command_list cmd,
    spudgpu_compute_pipeline pipeline,
    uint32_t first_set,
    const spudgpu_descriptor_set *sets,
    uint32_t set_count);

// ============================================================================
//  Bindless / Descriptor Indexing
//  Maps to: VK_EXT_descriptor_indexing update-after-bind arrays, promoted
//  core in Vulkan 1.2 (Vulkan) / large update-after-bind-style descriptor
//  tables under Resource Binding Tier 2+ (D3D12 — see
//  spudgpu_bindless_capabilities for why this targets tables rather than
//  Shader Model 6.6 ResourceDescriptorHeap[]) / Argument Buffers Tier 2 +
//  MTLResourceID (Metal).
//
//  Unlike the classic spudgpu_descriptor_set_layout/pool/set model above
//  (one schema per shader, re-bound per draw), bindless exposes ONE global,
//  per-device table per resource class that every pipeline can read from by
//  a plain integer index — the model every modern renderer uses to avoid
//  per-draw descriptor set churn.
//
//  Resource classes are kept separate (sampled image / storage image /
//  storage buffer) rather than one shared flat index space: Vulkan's
//  descriptor model is strongly typed per-binding and cannot mix types in
//  one array, so a shared index space could not be implemented honestly on
//  all three backends. Samplers are intentionally not part of this — SpudGPU
//  has no sampler object yet (@see SPUDGPU_DESCRIPTOR_TYPE_SAMPLER), and real
//  bindless renderers typically pair bindless textures with a small fixed
//  set of samplers bound the ordinary way rather than indexing samplers too.
//
//  GRAPHICS_BACKEND is a single-choice CMake cache variable — a given build
//  of spudlib only ever compiles one backend in (see ../CLAUDE.md), so
//  whether this extension exists at all is a compile-time fact, not a
//  runtime one. SPUDGPU_EXT_BINDLESS_DESCRIPTOR_INDEXING gates the whole
//  section accordingly: 0 and undeclared entirely on a backend that hasn't
//  implemented it yet (currently Metal — it needs a MTLHeap-backed resource
//  allocator to mark bindless resources resident in one useHeap: call
//  instead of one useResource: call per registered resource, which SpudGPU
//  doesn't have yet), so calling one of these functions against such a
//  build is a compile/link error rather than a silent no-op or a runtime
//  NULL surprise discovered on the wrong platform. This is independent of
//  spudgpu_bindless_capabilities::supported below, which reports whether the
//  actual GPU/driver supports it at runtime, given a backend that compiles
//  this section in at all — that runtime case returns
//  SPUDRESULT_GPU_EXT_BINDLESS_DESCRIPTOR_INDEXING_NOT_SUPPORTED (see
//  spudcore.h; every SPUDGPU_EXT_<NAME> macro has a matching
//  SPUDRESULT_GPU_EXT_<NAME>_NOT_SUPPORTED for exactly this case).
// ============================================================================

#if SPUDGPU_COMPILE_VULKAN_API || SPUDGPU_COMPILE_D3D12_API
#define SPUDGPU_EXT_BINDLESS_DESCRIPTOR_INDEXING 1
#else
#define SPUDGPU_EXT_BINDLESS_DESCRIPTOR_INDEXING 0
#endif

#if SPUDGPU_EXT_BINDLESS_DESCRIPTOR_INDEXING

#define SPUDGPU_BINDLESS_INVALID_INDEX 0xFFFFFFFFu

/**
 * @brief Reports whether this device supports bindless resource indexing,
 * and the addressable capacity of each resource class.
 *
 * Vulkan: reflects the shaderSampledImageArrayNonUniformIndexing /
 * descriptorBindingPartiallyBound / descriptorBindingUpdateAfterBind* /
 * runtimeDescriptorArray feature bits (core in 1.2) and the corresponding
 * maxDescriptorSetUpdateAfterBind* device limits.
 * D3D12: reflects Resource Binding Tier 2+ unbounded/update-after-bind-style
 * descriptor tables — SpudGPU's D3D12 backend cross-compiles the same SPIR-V
 * shaders used by Vulkan via SPIRV-Cross rather than hand-authoring HLSL, so
 * bindless goes through a large descriptor table per resource class (what
 * SPIRV-Cross emits for a GLSL runtime-sized binding array) rather than
 * Shader Model 6.6's ResourceDescriptorHeap[] dynamic resources, which would
 * require hand-written HLSL outside that pipeline.
 * Metal: always supported — Argument Buffers Tier 2 is required on every
 * Metal 3 device SpudGPU targets.
 *
 * Every class uses a plain 0-based index on all three backends — each
 * resource class gets its own dedicated table/array/argument-buffer slot
 * range, so unlike a single flat descriptor heap, no base offset is ever
 * needed before indexing it from the shader.
 */
typedef struct spudgpu_bindless_capabilities {
	bool supported;

	uint32_t max_sampled_images;
	uint32_t max_storage_images;
	uint32_t max_storage_buffers;
} spudgpu_bindless_capabilities;

/**
 * @return SPUD_SUCCESS with out_caps populated (out_caps->supported may
 * still be false — that is not an error, it's the answer). Never returns
 * SPUDRESULT_GPU_EXT_BINDLESS_DESCRIPTOR_INDEXING_NOT_SUPPORTED itself —
 * that code is for the register/layout functions below, which are real
 * operations that can fail for that reason; a query reporting "false" is
 * not a failure.
 */
SPUDRESULT spudgpu_get_bindless_capabilities(
    spudgpu_device device,
    spudgpu_bindless_capabilities *out_caps);

/**
 * @brief Returns the device's single global bindless descriptor set layout.
 *
 * Include this handle in spudgpu_shader_pipeline_desc::descriptor_set_layouts[]
 * (or spudgpu_compute_pipeline_desc's) at whichever set index your shaders
 * declare the unbounded arrays at (binding 0 = sampled images, binding 1 =
 * storage images, binding 2 = storage buffers), then pass that same index as
 * set_index to spudgpu_cmd_bind_bindless_resources.
 *
 * D3D12: returns a handle describing three large descriptor-table ranges
 * (one per resource class) rather than SM6.6 ResourceDescriptorHeap[]
 * dynamic resources — see spudgpu_bindless_capabilities for why.
 *
 * @warning Do not pass the returned handle to
 * spudgpu_destroy_descriptor_set_layout — the device owns it for its
 * lifetime.
 * @return NULL if the device/driver doesn't actually support this (see
 * SPUDRESULT_GPU_EXT_BINDLESS_DESCRIPTOR_INDEXING_NOT_SUPPORTED) — check
 * spudgpu_bindless_capabilities::supported first to distinguish that from a
 * NULL device argument.
 */
spudgpu_descriptor_set_layout spudgpu_get_bindless_descriptor_set_layout(spudgpu_device device);

/**
 * @brief Registers a sampled-image view in the device's global bindless
 * table and returns a stable index for it.
 *
 * The index — and the underlying image/view — must remain valid until
 * spudgpu_bindless_unregister_sampled_image is called. Registration uses
 * update-after-bind writes, so it is safe to call between frames, or even
 * while command lists referencing *other* indices are in flight; do not
 * rewrite an index a submitted-but-not-yet-completed command list is
 * currently reading.
 *
 * @return SPUDRESULT_GPU_EXT_BINDLESS_DESCRIPTOR_INDEXING_NOT_SUPPORTED if
 * this device/driver doesn't actually support bindless (check
 * spudgpu_bindless_capabilities::supported to avoid hitting this), or
 * SPUDRESULT_GPU_BINDLESS_OUT_OF_SLOTS if every slot for this resource class
 * is currently registered.
 */
SPUDRESULT spudgpu_bindless_register_sampled_image(
    spudgpu_device device,
    spudgpu_image_view view,
    uint32_t *out_index);

/// Frees a previously registered sampled-image slot for reuse.
void spudgpu_bindless_unregister_sampled_image(
    spudgpu_device device,
    uint32_t index);

/// @see spudgpu_bindless_register_sampled_image
SPUDRESULT spudgpu_bindless_register_storage_image(
    spudgpu_device device,
    spudgpu_image_view view,
    uint32_t *out_index);

void spudgpu_bindless_unregister_storage_image(
    spudgpu_device device,
    uint32_t index);

/// @see spudgpu_bindless_register_sampled_image
SPUDRESULT spudgpu_bindless_register_storage_buffer(
    spudgpu_device device,
    spudgpu_buffer_view view,
    uint32_t *out_index);

void spudgpu_bindless_unregister_storage_buffer(
    spudgpu_device device,
    uint32_t index);

/**
 * @brief Makes the device's global bindless resource tables visible to
 * subsequent draws on this command list.
 *
 * Vulkan: vkCmdBindDescriptorSets against the global set at set_index.
 * D3D12: ID3D12GraphicsCommandList::SetDescriptorHeaps with the device's
 * global CBV/SRV/UAV heap — set_index is ignored.
 * Metal: marks every currently-registered resource resident on this command
 * buffer (useResource:usage:) — set_index is ignored.
 *
 * Call once per command list before the first bindless-indexed draw.
 *
 * @param set_index The slot in the bound pipeline's descriptor_set_layouts[]
 * that holds spudgpu_get_bindless_descriptor_set_layout()'s handle.
 */
void spudgpu_cmd_bind_bindless_resources(
    spudgpu_command_list cmd,
    spudgpu_shader_pipeline pipeline,
    uint32_t set_index);

/// Compute-pipeline variant of spudgpu_cmd_bind_bindless_resources.
void spudgpu_cmd_bind_bindless_resources_compute(
    spudgpu_command_list cmd,
    spudgpu_compute_pipeline pipeline,
    uint32_t set_index);

#endif // SPUDGPU_EXT_BINDLESS_DESCRIPTOR_INDEXING

typedef uint32_t SPUDGPU_PRIMITIVE_TOPOLOGY;

enum {
	SPUDGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST  = 0,
	SPUDGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 1,
	SPUDGPU_PRIMITIVE_TOPOLOGY_LINE_LIST      = 2,
	SPUDGPU_PRIMITIVE_TOPOLOGY_LINE_STRIP     = 3,
	SPUDGPU_PRIMITIVE_TOPOLOGY_POINT_LIST     = 4,
	SPUDGPU_PRIMITIVE_TOPOLOGY_PATCH_LIST     = 5 // Tessellation
};

typedef uint32_t SPUDGPU_CULL_MODE;

enum { SPUDGPU_CULL_MODE_NONE = 0, SPUDGPU_CULL_MODE_FRONT = 1, SPUDGPU_CULL_MODE_BACK = 2 };

// Depth Compare Op
typedef uint32_t SPUDGPU_COMPARE_OP;

enum {
	SPUDGPU_COMPARE_OP_NEVER            = 0,
	SPUDGPU_COMPARE_OP_LESS             = 1,
	SPUDGPU_COMPARE_OP_EQUAL            = 2,
	SPUDGPU_COMPARE_OP_LESS_OR_EQUAL    = 3,
	SPUDGPU_COMPARE_OP_GREATER          = 4,
	SPUDGPU_COMPARE_OP_NOT_EQUAL        = 5,
	SPUDGPU_COMPARE_OP_GREATER_OR_EQUAL = 6,
	SPUDGPU_COMPARE_OP_ALWAYS           = 7
};
// Capacity Limits
#define SPUDGPU_MAX_VERTEX_ATTRIBUTES 16
#define SPUDGPU_MAX_VERTEX_BINDINGS 8
#define SPUDGPU_MAX_PUSH_CONSTANT_RANGES 4
typedef struct spudgpu_vertex_attribute_desc {
	/// Which shader location slot this attribute binds to (layout(location =
	/// N)).
	uint32_t location;

	/// Which vertex buffer binding slot this attribute is sourced from.
	uint32_t binding;

	/// The data layout and channel bit-depth of this attribute (e.g.,
	/// SPUDGPU_FORMAT_R32G32B32_FLOAT).
	SPUDGPU_FORMAT format;

	/// Byte offset of this attribute from the start of a single vertex element.
	uint32_t offset;
} spudgpu_vertex_attribute_desc;

typedef struct spudgpu_vertex_binding_desc {
	/// The binding slot index this entry targets.
	uint32_t binding;

	/// Byte distance between consecutive elements in the buffer (sizeof your
	/// vertex struct).
	uint32_t stride;

	/// When true, advances per-instance rather than per-vertex (instanced
	/// rendering).
	bool per_instance;
} spudgpu_vertex_binding_desc;

typedef struct spudgpu_push_constant_range_desc {
	/// Bitmask of shader stages that can read this push constant range.
	/// @see SPUDGPU_SHADER_STAGE
	SPUDGPU_SHADER_STAGE stage_flags;

	/// Byte offset within the push constant block.
	uint32_t offset;

	/// Byte size of this push constant range.
	uint32_t size;
} spudgpu_push_constant_range_desc;

typedef uint32_t SPUDGPU_BLEND_FACTOR;

enum {
	SPUDGPU_BLEND_FACTOR_ZERO                = 0,
	SPUDGPU_BLEND_FACTOR_ONE                 = 1,
	SPUDGPU_BLEND_FACTOR_SRC_ALPHA           = 2,
	SPUDGPU_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA = 3,
	SPUDGPU_BLEND_FACTOR_DST_ALPHA           = 4,
	SPUDGPU_BLEND_FACTOR_ONE_MINUS_DST_ALPHA = 5,
	SPUDGPU_BLEND_FACTOR_SRC_COLOR           = 6,
	SPUDGPU_BLEND_FACTOR_ONE_MINUS_SRC_COLOR = 7,
	SPUDGPU_BLEND_FACTOR_DST_COLOR           = 8,
	SPUDGPU_BLEND_FACTOR_ONE_MINUS_DST_COLOR = 9
};

typedef uint32_t SPUDGPU_BLEND_OP;

enum { SPUDGPU_BLEND_OP_ADD = 0, SPUDGPU_BLEND_OP_SUBTRACT = 1, SPUDGPU_BLEND_OP_REVERSE_SUBTRACT = 2, SPUDGPU_BLEND_OP_MIN = 3, SPUDGPU_BLEND_OP_MAX = 4 };

/**
 * @brief Per-attachment blend configuration.
 * Ignored entirely when blend_enable is false.
 */
typedef struct spudgpu_blend_attachment_desc {
	bool blend_enable;

	SPUDGPU_BLEND_FACTOR src_color_blend_factor;
	SPUDGPU_BLEND_FACTOR dst_color_blend_factor;
	SPUDGPU_BLEND_OP color_blend_op;

	SPUDGPU_BLEND_FACTOR src_alpha_blend_factor;
	SPUDGPU_BLEND_FACTOR dst_alpha_blend_factor;
	SPUDGPU_BLEND_OP alpha_blend_op;
} spudgpu_blend_attachment_desc;

/**
 * @brief Configuration descriptor for loading a SPIR-V shader binary.
 */
typedef struct spudgpu_shader_module_desc {
	/// Which pipeline stage this module targets.
	/// @see SPUDGPU_SHADER_STAGE
	SPUDGPU_SHADER_STAGE stage;

	/// Pointer to the raw SPIR-V bytecode. Must be 4-byte aligned.
	const void *spirv_code;

	/// Byte size of the SPIR-V blob. Must be a multiple of 4.
	size_t spirv_size;

#if _DEBUG
	/// @brief A string identifier used for diagnostic tracking.
	const char *debug_name;
#endif
} spudgpu_shader_module_desc;

SPUDRESULT spudgpu_create_shader_module(
    spudgpu_device device,
    const spudgpu_shader_module_desc *desc,
    spudgpu_shader_module *out_module);

void spudgpu_destroy_shader_module(spudgpu_shader_module shader_module);
/**
 * @brief Complete configuration descriptor for creating a graphics shader
 * pipeline.
 *
 * Fill in the stage modules you need, leave optional ones NULL.
 * All arrays are inline and fixed-capacity; use the corresponding _count
 * field to indicate how many entries are valid.
 */
typedef struct spudgpu_shader_pipeline_desc {

#if _DEBUG
	/// @brief A string identifier used for diagnostic tracking.
	const char *debug_name;
#endif

	// -----------------------------------------------------------------------
	// Shader stages
	// -----------------------------------------------------------------------

	/// Compiled vertex shader module. Required.
	spudgpu_shader_module vertex_module;
	/// Null-terminated entry point name. Pass NULL to default to "main".
	const char *vertex_entry_point;

	/// Compiled fragment shader module. Required.
	spudgpu_shader_module fragment_module;
	/// Null-terminated entry point name. Pass NULL to default to "main".
	const char *fragment_entry_point;

	/// Compiled geometry shader module. Optional — leave NULL to skip.
	spudgpu_shader_module geometry_module;
	const char *geometry_entry_point;

	/// Compiled tessellation control shader module. Optional — leave NULL to
	/// skip.
	spudgpu_shader_module tess_control_module;
	const char *tess_control_entry_point;

	/// Compiled tessellation evaluation shader module. Optional — leave NULL to
	/// skip.
	spudgpu_shader_module tess_eval_module;
	const char *tess_eval_entry_point;

	// -----------------------------------------------------------------------
	// Vertex input layout
	// -----------------------------------------------------------------------

	spudgpu_vertex_attribute_desc vertex_attributes[SPUDGPU_MAX_VERTEX_ATTRIBUTES];
	uint32_t vertex_attribute_count;

	spudgpu_vertex_binding_desc vertex_bindings[SPUDGPU_MAX_VERTEX_BINDINGS];
	uint32_t vertex_binding_count;

	// -----------------------------------------------------------------------
	// Input assembly
	// -----------------------------------------------------------------------

	/// How raw vertices are assembled into primitives before rasterization.
	/// @see SPUDGPU_PRIMITIVE_TOPOLOGY
	SPUDGPU_PRIMITIVE_TOPOLOGY primitive_topology;

	// -----------------------------------------------------------------------
	// Rasterizer
	// -----------------------------------------------------------------------

	/// Which triangle faces to discard before fragment shading.
	/// @see SPUDGPU_CULL_MODE
	SPUDGPU_CULL_MODE cull_mode;

	/// When true, triangles with counter-clockwise winding are treated as
	/// front-facing.
	bool front_face_ccw;

	/// When true, geometry is rasterized as wireframe lines instead of filled
	/// triangles.
	bool wireframe;

	// -----------------------------------------------------------------------
	// Depth / stencil
	// -----------------------------------------------------------------------

	/// Enable depth testing against the depth attachment.
	bool depth_test_enable;

	/// Allow the depth test to write new values into the depth attachment.
	bool depth_write_enable;

	/// The comparison function used when depth testing a fragment.
	/// @see SPUDGPU_COMPARE_OP
	SPUDGPU_COMPARE_OP depth_compare_op;

	// -----------------------------------------------------------------------
	// Blend state
	// -----------------------------------------------------------------------

	/// Per-attachment blend configuration.
	/// @see spudgpu_blend_attachment_desc
	spudgpu_blend_attachment_desc blend_attachment;

	// -----------------------------------------------------------------------
	// Attachment formats
	// -----------------------------------------------------------------------

	/// Pixel format of the color render target this pipeline will write to.
	/// @see SPUDGPU_FORMAT
	SPUDGPU_FORMAT color_attachment_format;

	/// Pixel format of the depth attachment. Set to SPUDGPU_FORMAT_UNKNOWN for
	/// no depth.
	/// @see SPUDGPU_FORMAT
	SPUDGPU_FORMAT depth_format;

	// -----------------------------------------------------------------------
	// Pipeline layout
	// -----------------------------------------------------------------------

	/// Opaque spudgpu_descriptor_set_layout handles, from
	/// spudgpu_create_descriptor_set_layout() - declared as void* here
	/// rather than the proper typedef, but each backend casts entries back
	/// to its own internal descriptor-set-layout type, the same as every
	/// other opaque handle in this header. Not a raw native handle of any
	/// specific backend's API.
	void *descriptor_set_layouts[SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS];
	uint32_t descriptor_set_layout_count;

	spudgpu_push_constant_range_desc push_constant_ranges[SPUDGPU_MAX_PUSH_CONSTANT_RANGES];
	uint32_t push_constant_range_count;

	// -----------------------------------------------------------------------
	// Tessellation
	// -----------------------------------------------------------------------

	/// Number of control points per patch. Only used when both tess stages are
	/// present. Defaults to 3 when set to 0.
	uint32_t patch_control_points;
} spudgpu_shader_pipeline_desc;

SPUDRESULT spudgpu_create_shader_pipeline(
    spudgpu_device device,
    const spudgpu_shader_pipeline_desc *desc,
    spudgpu_shader_pipeline *out_pipeline);

void spudgpu_destroy_shader_pipeline(spudgpu_shader_pipeline pipeline);

void spudgpu_cmd_bind_pipeline(
    spudgpu_command_list cmd,
    spudgpu_shader_pipeline pipeline);

/**
 * @brief Push constant data to the shader stages declared in the pipeline
 * layout.
 *
 * Stage flags are derived automatically from the pipeline's push constant range
 * declarations — any range that overlaps [offset, offset+size) contributes its
 * stages.
 *
 * @param cmd      The active recording command list.
 * @param pipeline The currently-bound pipeline (used to look up the layout).
 * @param offset   Byte offset within the push constant block.
 * @param size     Byte count to update. Must be a non-zero multiple of 4.
 * @param data     Pointer to the data to push.
 */
void spudgpu_cmd_push_constants(
    spudgpu_command_list cmd,
    spudgpu_shader_pipeline pipeline,
    uint32_t offset,
    uint32_t size,
    const void *data);
/**
 * @brief Defines the normalized window transformation dimensions for rendering
 * output coordinates.
 * * Maps normalized device coordinates (NDC) ranging [-1, 1] horizontally and
 * vertically directly into a target render target screen pixel domain.
 */
typedef struct SPUDGPU_VIEWPORT {
	/// X-coordinate of the upper-left corner of the viewport region in pixels.
	float x;

	/// Y-coordinate of the upper-left corner of the viewport region in pixels.
	float y;

	/// Total width of the targeted viewport frame in pixels.
	float width;

	/// Total height of the targeted viewport frame in pixels.
	float height;

	/// Minimum depth boundary slice. Usually maps to `0.0f` (near clipping
	/// plane).
	float minDepth;

	/// Maximum depth boundary slice. Usually maps to `1.0f` (far clipping
	/// plane).
	float maxDepth;
} SPUDGPU_VIEWPORT;

/**
 * @brief Binds an array of dynamic viewport transforms to the pipeline state.
 * * @param[in] cmd             The active recording command context.
 * @param[in] first_viewport  The zero-based index of the first viewport slot to
 * configure.
 * @param[in] viewport_count  Total number of structural viewports to bind from
 * the array.
 * @param[in] viewports       Pointer to a contiguous array of viewport layout
 * definitions.
 */
void spudgpu_cmd_set_viewports(
    spudgpu_command_list cmd,
    uint32_t first_viewport,
    uint32_t viewport_count,
    const SPUDGPU_VIEWPORT *viewports);

/**
 * @brief Defines a dynamic screen space bounding box for rasterization
 * discarding.
 * * Pixels falling outside the scissor rectangle bounds are completely dropped
 * by the rasterizer, optimizing rendering execution for partial UI elements or
 * UI windows.
 */
typedef struct SPUDGPU_SCISSOR_RECT {
	/// Leftmost X coordinate of the scissor bounding box in pixels.
	float x;

	/// Topmost Y coordinate of the scissor bounding box in pixels.
	float y;

	/// Total horizontal layout width of the box in pixels.
	float width;

	/// Total vertical layout height of the box in pixels.
	float height;
} SPUDGPU_SCISSOR_RECT;

/**
 * @brief Binds an array of dynamic scissor clip boundaries to the graphics
 * pipeline.
 * * @param[in] cmd                 The active recording command context.
 * @param[in] first_scissor_rect  The zero-based index of the first scissor slot
 * to configure.
 * @param[in] scissor_rect_count  Total number of scissor regions to update.
 * @param[in] scissor_rects       Pointer to an array of scissor box dimensions.
 */
void spudgpu_cmd_set_scissor_rects(
    spudgpu_command_list cmd,
    uint32_t first_scissor_rect,
    uint32_t scissor_rect_count,
    const SPUDGPU_SCISSOR_RECT *scissor_rects);

/**
 * @brief Binds a contiguous array of vertex buffers to target input assembler
 * slots.
 * * @param[in] cmd          The active recording command context.
 * @param[in] start_slot   The starting hardware vertex buffer input slot index
 * to update.
 * @param[in] view_count   Total number of vertex buffer views to map.
 * @param[in] buffer_views Pointer to (0, 1, or an array of) active GPU buffer
 * views containing vertex data.
 */
void spudgpu_cmd_set_vertex_buffers(
    spudgpu_command_list cmd,
    uint32_t start_slot,
    uint32_t view_count,
    spudgpu_buffer_view *buffer_views);

/**
 * @brief Binds the index buffer used to drive lookups for indexed draw calls.
 * * @param[in] cmd         The active recording command context.
 * @param[in] buffer_view The active GPU buffer view containing index data.
 */
void spudgpu_cmd_set_index_buffer(
    spudgpu_command_list cmd,
    spudgpu_buffer_view buffer_view);

/**
 * @brief Non-indexed non-instanced drawing invocation.
 * * Processes sequential vertex data streams bound to input channels directly.
 * * @param[in] cmd                   The active recording command context.
 * @param[in] vertex_count          Total vertices to process for the shape
 * primitive array.
 * @param[in] start_vertex_location Index offset representing the first vertex
 * entry to pull.
 */
void spudgpu_cmd_draw(
    spudgpu_command_list cmd,
    uint32_t vertex_count,
    uint32_t start_vertex_location);

/**
 * @brief Indexed non-instanced drawing invocation.
 * * Utilizes an index buffer map to fetch vertices out-of-order, saving vertex
 * storage.
 * * @param[in] cmd                   The active recording command context.
 * @param[in] index_count           The number of indices to read from the
 * active index buffer.
 * @param[in] start_index_location  The element offset inside the index array to
 * begin fetching from.
 * @param[in] base_vertex_location   A value added to every individual index
 * value prior to pulling vertex payload parameters. Effectively handles
 * multi-mesh packaging offsets.
 */
void spudgpu_cmd_draw_indexed(
    spudgpu_command_list cmd,
    uint32_t index_count,
    uint32_t start_index_location,
    int32_t base_vertex_location);

/**
 * @brief Non-indexed instanced drawing invocation.
 * * Draws identical structural vertex topologies multiple times using hardware
 * instancing.
 * * @param[in] cmd                         The active recording command
 * context.
 * @param[in] vertex_count_per_instance   The number of sequential vertices to
 * draw for an instance item.
 * @param[in] instance_count              Total instances of the mesh layout to
 * render.
 * @param[in] start_vertex_location       The base index offset to look up
 * vertex attributes.
 * @param[in] start_instance_location     An index added to instance identifiers
 * prior to loading per-instance shader attributes.
 */
void spudgpu_cmd_draw_instanced(
    spudgpu_command_list cmd,
    uint32_t vertex_count_per_instance,
    uint32_t instance_count,
    uint32_t start_vertex_location,
    uint32_t start_instance_location);

/**
 * @brief Full indexed instanced drawing invocation.
 * * The most optimal, high-throughput path for repeating geometric instances
 * (e.g., foliage, particles).
 * @param[in] cmd                         The active recording command
 * context.
 * @param[in] index_count_per_instance    The count of indices to process for
 * each item instance.
 * @param[in] instance_count              Total unique geometry clones to
 * invoke.
 * @param[in] start_index_location        Element offset index pointer inside
 * the active index buffer block.
 * @param[in] base_vertex_location        Signed value applied directly to index
 * stream integers before fetching.
 * @param[in] start_instance_location     The tracking offset applied directly
 * to your shader instance counts.
 */
void spudgpu_cmd_draw_indexed_instanced(
    spudgpu_command_list cmd,
    uint32_t index_count_per_instance,
    uint32_t instance_count,
    uint32_t start_index_location,
    int32_t base_vertex_location,
    uint32_t start_instance_location);
/**
 * @brief Complete configuration descriptor for creating a compute shader
 * pipeline.
 *
 * Simpler than the graphics pipeline desc — no vertex input, rasterizer,
 * blend state, or render pass. Just a single compute stage and its layout.
 */
typedef struct spudgpu_compute_pipeline_desc {
	/// Compiled compute shader module. Required.
	spudgpu_shader_module compute_module;

	/// Null-terminated entry point name. Pass NULL to default to "main".
	const char *compute_entry_point;

	// -----------------------------------------------------------------------
	// Pipeline layout
	// -----------------------------------------------------------------------

	/// Opaque spudgpu_descriptor_set_layout handles, from
	/// spudgpu_create_descriptor_set_layout() - declared as void* here
	/// rather than the proper typedef, but each backend casts entries back
	/// to its own internal descriptor-set-layout type, the same as every
	/// other opaque handle in this header. Not a raw native handle of any
	/// specific backend's API.
	void *descriptor_set_layouts[SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS];
	uint32_t descriptor_set_layout_count;

	spudgpu_push_constant_range_desc push_constant_ranges[SPUDGPU_MAX_PUSH_CONSTANT_RANGES];
	uint32_t push_constant_range_count;

#if _DEBUG
	/// @brief A string identifier used for diagnostic tracking.
	const char *debug_name;
#endif
} spudgpu_compute_pipeline_desc;

/**
 * @brief Allocates and compiles a compute shader pipeline.
 *
 * @param[in] device The GPU device to create this pipeline on.
 * @param[in] desc   Pointer to the configuration descriptor.
 * @param[out] out_pipeline The new GPU compute pipeline.
 * @return SPUD_SUCCESS, or an error code if
 * the module is missing, the layout is invalid, or SPIR-V compilation fails.
 */
SPUDRESULT spudgpu_create_compute_pipeline(
    spudgpu_device device,
    const spudgpu_compute_pipeline_desc *desc,
    spudgpu_compute_pipeline *out_pipeline);

/**
 * @brief Destroys a compute pipeline and frees all associated driver resources.
 * @param[in] pipeline The compute pipeline to destroy.
 * @warning Any command lists currently recording dispatches against this
 * pipeline must have finished execution on the GPU timeline before calling
 * this.
 */
void spudgpu_destroy_compute_pipeline(spudgpu_compute_pipeline pipeline);

SPUDRESULT spudgpu_get_compute_pipeline_desc(
    spudgpu_compute_pipeline pipeline,
    spudgpu_compute_pipeline_desc *out_desc);

// ============================================================================
//  Image <-> Buffer Copies
//  Maps to: vkCmdCopyImageToBuffer / vkCmdCopyBufferToImage (Vulkan)
// ============================================================================

/**
 * @brief Describes a rectangular texel region and target mip/layer used for a
 * copy between an image and a buffer.
 *
 * Maps to Vulkan's VkBufferImageCopy / D3D12's PlacedFootprint region.
 */
typedef struct spudgpu_image_buffer_copy_desc {
	/// Byte offset into the buffer where this region starts (or is read from).
	uint64_t buffer_offset;

	/// Row length in texels, used to interpret the buffer's layout.
	/// Pass 0 for tightly packed rows (== region width).
	uint32_t buffer_row_length;

	/// Image height in texels, used to interpret the buffer's 2D/3D layout.
	/// Pass 0 for tightly packed rows (== region height).
	uint32_t buffer_image_height;

	/// Mip level of the image being copied.
	uint32_t mip_level;

	/// Starting array layer of the image being copied.
	uint32_t base_array_layer;

	/// Number of array layers to copy.
	uint32_t array_layer_count;

	/// Texel-space offset into the image where the region begins.
	uint32_t image_x, image_y, image_z;

	/// Texel-space size of the copied region.
	uint32_t width, height, depth;
} spudgpu_image_buffer_copy_desc;

/**
 * @brief Records a copy from an image subresource region into a buffer.
 *
 * The image must currently be in SPUDGPU_IMAGE_LAYOUT_TRANSFER_SRC. Use
 * spudgpu_get_image_buffer_copy_size to size the destination buffer before
 * calling this.
 *
 * Maps to: vkCmdCopyImageToBuffer (Vulkan), CopyTextureRegion (D3D12),
 * copyFromTexture:toBuffer: (Metal).
 *
 * @param[in] cmd        The active recording command list.
 * @param[in] src_image  The source image to read texel data from.
 * @param[in] dst_buffer The destination buffer to receive the copied bytes.
 * @param[in] desc       Region, offset, and subresource to copy.
 */
void spudgpu_cmd_copy_image_to_buffer(
    spudgpu_command_list cmd,
    spudgpu_image src_image,
    spudgpu_buffer dst_buffer,
    const spudgpu_image_buffer_copy_desc *desc);

/**
 * @brief Records the reverse copy: buffer bytes into an image subresource
 * region. Used for texture uploads.
 *
 * The image must currently be in SPUDGPU_IMAGE_LAYOUT_TRANSFER_DST. Use
 * spudgpu_get_image_buffer_copy_size to determine the expected source row
 * pitch and total size.
 *
 * Maps to: vkCmdCopyBufferToImage (Vulkan), CopyTextureRegion (D3D12),
 * copyFromBuffer:toTexture: (Metal).
 *
 * @param[in] cmd        The active recording command list.
 * @param[in] src_buffer The source buffer to read bytes from.
 * @param[in] dst_image  The destination image to write texel data into.
 * @param[in] desc       Region, offset, and subresource to copy.
 */
void spudgpu_cmd_copy_buffer_to_image(
    spudgpu_command_list cmd,
    spudgpu_buffer src_buffer,
    spudgpu_image dst_image,
    const spudgpu_image_buffer_copy_desc *desc);

/**
 * @brief Computes the buffer row pitch and total byte size required to
 * receive (or supply) a copy of the given image subresource region.
 *
 * Some backends (D3D12) require a specific row-pitch alignment for buffer
 * footprints; this returns the value already aligned so the caller can
 * allocate a correctly-sized staging buffer without hardcoding backend rules.
 *
 * @param[in]  image          The image the copy targets.
 * @param[in]  mip_level      The mip level being copied.
 * @param[out] out_row_pitch  Required/aligned row pitch, in bytes.
 * @param[out] out_total_size Required buffer size, in bytes.
 */
void spudgpu_get_image_buffer_copy_size(
    spudgpu_image image,
    uint32_t mip_level,
    uint64_t *out_row_pitch,
    uint64_t *out_total_size);

// ============================================================================
//  Image Blit
//  Maps to: vkCmdBlitImage (Vulkan)
// ============================================================================

/**
 * @brief Sampling filter used when a blit's source and destination regions
 * differ in size.
 */
typedef uint32_t SPUDGPU_FILTER;
enum { SPUDGPU_FILTER_NEAREST = 0, SPUDGPU_FILTER_LINEAR = 1 };

/**
 * @brief Describes a source and destination subresource/region pair for an
 * image blit.
 *
 * Each region is given as two opposite corners (x0,y0,z0)-(x1,y1,z1) rather
 * than an offset+extent, matching Vulkan's VkImageBlit. This is what allows
 * the source and destination regions to be different sizes (the whole point
 * of a blit vs. a plain copy) — most commonly src = mip N at full size, dst =
 * mip N+1 at half size, when generating a mip chain.
 */
typedef struct spudgpu_image_blit_desc {
	uint32_t src_mip_level;
	uint32_t src_base_array_layer;
	uint32_t src_array_layer_count;
	uint32_t src_x0, src_y0, src_z0;
	uint32_t src_x1, src_y1, src_z1;

	uint32_t dst_mip_level;
	uint32_t dst_base_array_layer;
	uint32_t dst_array_layer_count;
	uint32_t dst_x0, dst_y0, dst_z0;
	uint32_t dst_x1, dst_y1, dst_z1;

	/// Filter applied when src and dst region sizes differ.
	SPUDGPU_FILTER filter;
} spudgpu_image_blit_desc;

/**
 * @brief Records a (possibly scaling) copy between two image regions.
 *
 * Unlike spudgpu_cmd_copy_image_to_buffer, source and destination regions
 * may differ in size — the driver resamples using desc->filter. The primary
 * use is mip chain generation: blit mip level N (full res) into mip level
 * N+1 (half res) of the same image, one level pair at a time.
 *
 * src_image must currently be in SPUDGPU_IMAGE_LAYOUT_TRANSFER_SRC and
 * dst_image in SPUDGPU_IMAGE_LAYOUT_TRANSFER_DST. src_image and dst_image
 * may be the same image (e.g. mip chain generation).
 *
 * Maps to: vkCmdBlitImage (Vulkan). D3D12/Metal backends implement this via
 * a shader-based resample pass, since neither exposes a direct blit command
 * with filtering equivalent to Vulkan's.
 *
 * @param[in] cmd       The active recording command list.
 * @param[in] src_image The source image to read from.
 * @param[in] dst_image The destination image to write into.
 * @param[in] desc      Source/destination subresources, regions, and filter.
 */
void spudgpu_cmd_blit_image(
    spudgpu_command_list cmd,
    spudgpu_image src_image,
    spudgpu_image dst_image,
    const spudgpu_image_blit_desc *desc);

// ============================================================================
//  Rendering — dynamic rendering: no VkRenderPass/VkFramebuffer object and no
//  pipeline-compatibility coupling. Attachments and their load/store
//  operations are declared per-call instead of baked into a precompiled
//  object.
//  Maps to: vkCmdBeginRendering / vkCmdEndRendering (Vulkan 1.3 core /
//           VK_KHR_dynamic_rendering)
//           ID3D12GraphicsCommandList4::BeginRenderPass / EndRenderPass (D3D12)
//           A render command encoder made from an MTLRenderPassDescriptor
//           (Metal — already this shape natively, no object/compatibility
//           coupling to begin with)
//
//  TODO(vulkan-fallback): some Vulkan devices/drivers — older or low-end
//  Android/Wear OS hardware in particular — may not support Vulkan 1.3 /
//  VK_KHR_dynamic_rendering. If SpudLib ever needs to run there, the Vulkan
//  backend should query VkPhysicalDeviceVulkan13Features.dynamicRendering (or
//  the VK_KHR_dynamic_rendering extension) at device creation and, if
//  unsupported, fall back internally to a classic VkRenderPass/VkFramebuffer
//  implementation of spudgpu_cmd_begin_rendering/end_rendering. That fallback
//  belongs entirely inside the Vulkan backend — this API and every other
//  backend stay unaware of which path is active. Not implemented yet: every
//  target this library currently builds for (desktop Vulkan, D3D12) has
//  dynamic rendering, so there's no hardware yet to validate a fallback
//  against.
// ============================================================================

typedef uint32_t SPUDGPU_LOAD_OP;
enum {
	/// Keep the attachment's existing contents.
	SPUDGPU_LOAD_OP_LOAD = 0,
	/// Clear to the attachment's clear value before drawing.
	SPUDGPU_LOAD_OP_CLEAR,
	/// Contents are undefined; the driver may skip the load entirely.
	SPUDGPU_LOAD_OP_DONT_CARE,
};

typedef uint32_t SPUDGPU_STORE_OP;
enum {
	/// Write the render result back to the attachment.
	SPUDGPU_STORE_OP_STORE = 0,
	/// Discard the result; the driver may skip the writeback.
	SPUDGPU_STORE_OP_DONT_CARE,
};

/// Upper bound on simultaneous color attachments in one rendering pass.
#define SPUDGPU_MAX_COLOR_ATTACHMENTS 8

/**
 * @brief One color attachment bound for a rendering pass.
 */
typedef struct spudgpu_color_attachment_desc {
	/// The view to render into.
	spudgpu_image_view image_view;

	/// @see SPUDGPU_LOAD_OP
	SPUDGPU_LOAD_OP load_op;
	/// @see SPUDGPU_STORE_OP
	SPUDGPU_STORE_OP store_op;

	/// RGBA clear color; only read when load_op == SPUDGPU_LOAD_OP_CLEAR.
	float clear_color[4];
} spudgpu_color_attachment_desc;

/**
 * @brief The depth/stencil attachment bound for a rendering pass.
 */
typedef struct spudgpu_depth_attachment_desc {
	/// NULL for no depth/stencil attachment.
	spudgpu_image_view image_view;

	SPUDGPU_LOAD_OP depth_load_op;
	SPUDGPU_STORE_OP depth_store_op;
	SPUDGPU_LOAD_OP stencil_load_op;
	SPUDGPU_STORE_OP stencil_store_op;

	/// Only read when depth_load_op == SPUDGPU_LOAD_OP_CLEAR.
	float clear_depth;
	/// Only read when stencil_load_op == SPUDGPU_LOAD_OP_CLEAR.
	uint32_t clear_stencil;
} spudgpu_depth_attachment_desc;

/**
 * @brief Describes the attachments and render area for one rendering pass.
 *
 * Carries no pipeline reference — pipelines are bound mid-pass with
 * spudgpu_cmd_bind_pipeline like any other draw-time state, not tied to a
 * specific render pass object.
 */
typedef struct spudgpu_rendering_begin_desc {
	spudgpu_color_attachment_desc color_attachments[SPUDGPU_MAX_COLOR_ATTACHMENTS];
	uint32_t color_attachment_count;

	/// depth_attachment.image_view == NULL means no depth/stencil attachment.
	spudgpu_depth_attachment_desc depth_attachment;

	int32_t x;
	int32_t y;

	/// Render area width in pixels. Usually the attachments' width.
	uint32_t width;

	/// Render area height in pixels. Usually the attachments' height.
	uint32_t height;
} spudgpu_rendering_begin_desc;

/**
 * @brief Begins a rendering pass, binding the color and optional depth/
 * stencil attachments and issuing their configured load operations.
 *
 * Must be matched with spudgpu_cmd_end_rendering before submitting. Bind a
 * pipeline with spudgpu_cmd_bind_pipeline after this call, not before.
 *
 * @param[in] cmd  The active recording command list.
 * @param[in] desc Attachment and render-area configuration.
 */
void spudgpu_cmd_begin_rendering(
    spudgpu_command_list cmd,
    const spudgpu_rendering_begin_desc *desc);

/**
 * @brief Ends the current rendering pass, issuing the configured store
 * operations.
 *
 * @param[in] cmd The active recording command list.
 */
void spudgpu_cmd_end_rendering(spudgpu_command_list cmd);

/**
 * @brief Clears a single color attachment to a solid color.
 *
 * Convenience wrapper around spudgpu_cmd_begin_rendering /
 * spudgpu_cmd_end_rendering with load_op == CLEAR and zero draw calls — no
 * pipeline required. The attachment must already be in
 * SPUDGPU_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL (see spudgpu_cmd_image_barrier).
 *
 * @param[in] cmd        The active recording command list.
 * @param[in] attachment The view to clear.
 * @param[in] r,g,b,a    Clear color.
 * @param[in] width      Attachment width in pixels.
 * @param[in] height     Attachment height in pixels.
 */
void spudgpu_cmd_clear_color_attachment(
    spudgpu_command_list cmd,
    spudgpu_image_view attachment,
    float r,
    float g,
    float b,
    float a,
    uint32_t width,
    uint32_t height);

/**
 * @brief Clears a depth/stencil attachment.
 *
 * Convenience wrapper around spudgpu_cmd_begin_rendering /
 * spudgpu_cmd_end_rendering with zero draw calls — no pipeline required. The
 * attachment must already be in
 * SPUDGPU_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL (see
 * spudgpu_cmd_image_barrier).
 *
 * @param[in] cmd           The active recording command list.
 * @param[in] attachment    The depth/stencil view to clear.
 * @param[in] clear_depth   Whether to clear the depth aspect.
 * @param[in] clear_stencil Whether to clear the stencil aspect.
 * @param[in] depth         Depth clear value (typically 1.0f).
 * @param[in] stencil       Stencil clear value (typically 0).
 * @param[in] width         Attachment width in pixels.
 * @param[in] height        Attachment height in pixels.
 */
void spudgpu_cmd_clear_depth_attachment(
    spudgpu_command_list cmd,
    spudgpu_image_view attachment,
    bool clear_depth,
    bool clear_stencil,
    float depth,
    uint32_t stencil,
    uint32_t width,
    uint32_t height);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SPUDLIB_SPUDGPU_H
