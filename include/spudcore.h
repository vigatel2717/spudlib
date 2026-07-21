
#ifndef SPUDCORE_H
#define SPUDCORE_H

#if __cplusplus
extern "C" {
#endif

// Common types, macros, and utilities for SPUDGPU and its backends.
// Defined because of a niche issue of VSCode IntelliSense
// not properly recognizing the ends of these definitions in stdint.h on
// Windows. These ends are the numbering 'ui32' 'ui64' etc.
#define SPUD_UINT8_MAX 0xFFU
#define SPUD_UINT16_MAX 0xFFFFU
#define SPUD_UINT32_MAX 0xFFFFFFFFU
#define SPUD_UINT64_MAX 0xFFFFFFFFFFFFFFFFULL

#ifdef _DEBUG
void spud_debug_name_set(void *object, const char *name);
#define SPUD_SET_DEBUG_NAME(obj, name)                                         \
	spud_debug_name_set((void *)(obj), (name))
const char *spud_debug_name_get(void *object);
#define SPUD_GET_DEBUG_NAME(obj) spud_debug_name_get((void *)(obj))
#endif

typedef enum SPUDRESULT {
	SPUD_SUCCESS                       = 0,
	SPUDRESULT_GENERAL_FAILURE         = 1,
	SPUDRESULT_API_SPECIFIC_FAILURE    = 2,
	SPUDRESULT_INVALID_API             = 3,
	SPUDRESULT_NULL_DESC               = 4,
	SPUDRESULT_NULL_OUTPUT_PARAMETER   = 5,
	SPUDRESULT_DESC_INVALID_PARAMETERS = 6,
	SPUDRESULT_INDEX_OUT_OF_RANGE      = 7,
	SPUDRESULT_ZERO_SIZE               = 8,
	SPUDRESULT_OUT_OF_MEMORY           = 9,
	SPUDRESULT_NOT_IMPLEMENTED_YET     = 10,

	SPUDRESULT_GPU_DEVICE_ENUMERATION_FAILURE                  = 199,
	SPUDRESULT_GPU_INVALID_INSTANCE                            = 200,
	SPUDRESULT_GPU_INVALID_DEVICE                              = 201,
	SPUDRESULT_GPU_INVALID_COMMAND_QUEUE                       = 202,
	SPUDRESULT_GPU_INVALID_COMMAND_ALLOCATOR                   = 203,
	SPUDRESULT_GPU_INVALID_COMMAND_LIST                        = 204,
	SPUDRESULT_GPU_INVALID_SWAP_CHAIN                          = 205,
	SPUDRESULT_GPU_INVALID_SURFACE                             = 206,
	SPUDRESULT_GPU_INVALID_FORMAT                              = 207,
	SPUDRESULT_GPU_INVALID_WINDOW_HANDLE                       = 208,
	SPUDRESULT_GPU_INVALID_DISPLAY_HANDLE                      = 209,
	SPUDRESULT_GPU_INVALID_COMMAND_LIST_TYPE                   = 210,
	SPUDRESULT_GPU_INVALID_MEMORY_FLAGS                        = 220,
	SPUDRESULT_GPU_INVALID_BUFFER_USAGE                        = 221,
	SPUDRESULT_GPU_INVALID_IMAGE_USAGE                         = 222,
	SPUDRESULT_GPU_INVALID_BUFFER_STRIDE                       = 223,
	SPUDRESULT_GPU_INVALID_INDEX_STRIDE                        = 224,
	SPUDRESULT_GPU_INVALID_BUFFER                              = 230,
	SPUDRESULT_GPU_INVALID_BUFFER_VIEW                         = 231,
	SPUDRESULT_GPU_INVALID_IMAGE                               = 232,
	SPUDRESULT_GPU_INVALID_IMAGE_VIEW                          = 233,
	SPUDRESULT_GPU_INVALID_IMAGE_VIEW_TYPE                     = 234,
	SPUDRESULT_GPU_INVALID_IMAGE_TYPE                          = 235,
	SPUDRESULT_GPU_ZERO_BUFFER_SIZE                            = 236,
	SPUDRESULT_GPU_INVALID_BUFFER_SIZE                         = 237,
	SPUDRESULT_GPU_BUFFER_OR_IMAGE_VIEW_RANGE_OUT_OF_SCOPE     = 238,
	SPUDRESULT_GPU_MAP_OUT_OF_RANGE                            = 239,
	SPUDRESULT_GPU_INVALID_SEMAPHORE                           = 250,
	SPUDRESULT_GPU_INVALID_FENCE                               = 251,
	SPUDRESULT_GPU_INVALID_SHADER_STAGE                        = 280,
	SPUDRESULT_GPU_INVALID_PRIMITIVE_TOPOLOGY                  = 281,
	SPUDRESULT_GPU_INVALID_CULL_MODE                           = 282,
	SPUDRESULT_GPU_INVALID_COMPARE_OP                          = 283,
	SPUDRESULT_GPU_INVALID_BLEND_FACTOR                        = 290,
	SPUDRESULT_GPU_INVALID_BLEND_OP                            = 291,
	SPUDRESULT_GPU_INVALID_SHADER_MODULE                       = 300,
	SPUDRESULT_GPU_INVALID_SHADER_PIPELINE                     = 301,
	SPUDRESULT_GPU_INVALID_COMPUTE_MODULE                      = 302,
	SPUDRESULT_GPU_INVALID_COMPUTE_PIPELINE                    = 303,
	SPUDRESULT_GPU_NULL_SPIRV                                  = 304,
	SPUDRESULT_GPU_INVALID_SPIRV_ALIGNMENT                     = 305,
	SPUDRESULT_GPU_VERTEX_AND_FRAGMENT_SHADER_REQUIRED         = 306,
	SPUDRESULT_GPU_SHADER_COMPILATION_FAILED                   = 307,
	SPUDRESULT_GPU_INVALID_DESCRIPTOR_TYPE                     = 310,
	SPUDRESULT_GPU_INVALID_DESCRIPTOR_POOL_SIZE                = 311,
	SPUDRESULT_GPU_INVALID_DESCRIPTOR_POOL                     = 312,
	SPUDRESULT_GPU_INVALID_DESCRIPTOR_SET_LAYOUT               = 313,
	SPUDRESULT_GPU_INVALID_DESCRIPTOR_SET                      = 314,
	SPUDRESULT_GPU_INVALID_DESCRIPTOR_BUFFER_INFO              = 315,
	SPUDRESULT_GPU_INVALID_DESCRIPTOR_IMAGE_INFO               = 316,
	SPUDRESULT_GPU_INVALID_WRITE_DESCRIPTOR_SET                = 317,
	SPUDRESULT_GPU_TOO_MANY_DESCRIPTOR_BINDINGS                = 318,
	SPUDRESULT_GPU_TOO_MANY_DESCRIPTOR_POOLS                   = 319,
	SPUDRESULT_GPU_ZERO_DESCRIPTOR_SET_LAYOUTS                 = 320,
	SPUDRESULT_GPU_TOO_MANY_DESCRIPTOR_SET_LAYOUTS             = 321,
	SPUDRESULT_GPU_INTERNAL_DESCRIPTOR_SET_ALLOCATION_FAIL     = 322,
	SPUDRESULT_GPU_CANNOT_RESOLVE_API_SPECIFIC_DESCRIPTOR_TYPE = 323,
	SPUDRESULT_GPU_INVALID_RESOURCE_STATE                      = 350,
	SPUDRESULT_GPU_INVALID_IMAGE_LAYOUT                        = 351,
	SPUDRESULT_GPU_INVALID_PIPELINE_STAGE                      = 352,
	SPUDRESULT_GPU_INVALID_BUFFER_BARRIER                      = 353,
	SPUDRESULT_GPU_INVALID_IMAGE_BARRIER                       = 354,

	SPUDRESULT_SFS_NULL_PATH = 1001,
	SPUDRESULT_SFS_INVALID_FILE = 1002,

	SPUDRESULT_SMEM_ZERO_CAPACITY = 2001,
	SPUDRESULT_SMEM_INSUFFICIENT_CAPACITY = 2002,
	SPUDRESULT_SMEM_PLAT_COMMIT_FAIL = 2003,
	SPUDRESULT_SMEM_INVALID_ARENA = 2026,

} SPUDRESULT;

const char *spudresult_str(SPUDRESULT r);

#if __cplusplus
}
#endif

#endif // SPUDCORE_H
