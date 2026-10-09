
#ifndef SPUDCORE_H
#define SPUDCORE_H

#include <stdint.h>

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
	SPUDRESULT_NULL_OBJECT             = 11,

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
	SPUDRESULT_GPU_BINDLESS_OUT_OF_SLOTS                       = 324,

	// One SPUDRESULT_GPU_EXT_<NAME>_NOT_SUPPORTED per SPUDGPU_EXT_<NAME>
	// compile-time capability macro in spudgpu.h — returned by that
	// extension's runtime entry points when the active backend compiles the
	// extension in (the macro is 1) but the specific device/driver still
	// doesn't support it. Never returned when the macro is 0: the extension's
	// functions aren't declared at all in that case, so calling one is a
	// compile/link error, not a runtime result.
	SPUDRESULT_GPU_EXT_BINDLESS_DESCRIPTOR_INDEXING_NOT_SUPPORTED = 325,
	SPUDRESULT_GPU_EXT_MESH_SHADING_NOT_SUPPORTED                 = 326,
	SPUDRESULT_GPU_EXT_DEPTH_BOUNDS_TEST_NOT_SUPPORTED            = 327,

	SPUDRESULT_GPU_INVALID_RESOURCE_STATE                      = 350,
	SPUDRESULT_GPU_INVALID_IMAGE_LAYOUT                        = 351,
	SPUDRESULT_GPU_INVALID_PIPELINE_STAGE                      = 352,
	SPUDRESULT_GPU_INVALID_BUFFER_BARRIER                      = 353,
	SPUDRESULT_GPU_INVALID_IMAGE_BARRIER                       = 354,

	SPUDRESULT_SFS_NULL_PATH = 1001,
	SPUDRESULT_SFS_INVALID_FILE = 1002,
	SPUDRESULT_SFS_INVALID_MAPPING = 1003,
	SPUDRESULT_SFS_NOT_READABLE = 1004,     // mapping a file not opened for reading
	SPUDRESULT_SFS_DIFFERENT_VOLUME = 1005, // replace across volumes (EXDEV / ERROR_NOT_SAME_DEVICE)
	SPUDRESULT_SFS_IN_USE = 1006,           // replace target in use or read-only (Windows)
	SPUDRESULT_SFS_FLUSH_LEVEL_NOT_SUPPORTED = 1007, // the file system can't flush to the level asked for

	SPUDRESULT_SMEM_ZERO_CAPACITY = 2001,
	SPUDRESULT_SMEM_INSUFFICIENT_CAPACITY = 2002,
	SPUDRESULT_SMEM_PLAT_COMMIT_FAIL = 2003,
	SPUDRESULT_SMEM_INVALID_ARENA = 2026,

	SPUDRESULT_SPUDNET_STARTUP_FAILED        = 3001, // spudnet_instance_create: the platform's networking wouldn't start
	SPUDRESULT_SPUDNET_INVALID_TCP_SOCKET    = 3002,
	SPUDRESULT_SPUDNET_RESOLVE_FAILED        = 3003,
	SPUDRESULT_SPUDNET_CONNECT_FAILED        = 3004,
	SPUDRESULT_SPUDNET_LISTEN_FAILED         = 3005,
	SPUDRESULT_SPUDNET_ACCEPT_FAILED         = 3006,
	SPUDRESULT_SPUDNET_SEND_FAILED           = 3007,
	SPUDRESULT_SPUDNET_RECV_FAILED           = 3008,
	// 3009 and 3010 (WOULD_BLOCK, SET_BLOCKING_FAILED) went with SpudNet's
	// non-blocking mode and are not reused.
	SPUDRESULT_SPUDNET_INVALID_URL           = 3011, // not a URL, or a scheme the call doesn't carry
	SPUDRESULT_SPUDNET_INVALID_HTTP_CLIENT   = 3012,
	SPUDRESULT_SPUDNET_INVALID_WEBSOCKET     = 3013,
	SPUDRESULT_SPUDNET_TLS_FAILED            = 3014, // handshake or certificate check failed
	SPUDRESULT_SPUDNET_TIMED_OUT             = 3015, // the call's own time limit ran out, and nothing else
	SPUDRESULT_SPUDNET_HTTP_FAILED           = 3016, // an HTTP transfer's start or its wait for the response failed
	SPUDRESULT_SPUDNET_WS_UPGRADE_REFUSED    = 3017, // the server answered, but not with 101
	SPUDRESULT_SPUDNET_WS_CLOSED             = 3018, // closed by a close frame, either side's
	SPUDRESULT_SPUDNET_WS_MESSAGE_TOO_BIG    = 3019, // over the connect desc's max_message_size
	SPUDRESULT_SPUDNET_UNSUPPORTED           = 3020, // this build's platform stack doesn't have it
	SPUDRESULT_SPUDNET_ABORTED               = 3022, // the object's _abort was called
	SPUDRESULT_SPUDNET_INVALID_TCP_LISTENER  = 3024,
	SPUDRESULT_SPUDNET_INVALID_ADDRESS       = 3025, // not a numeric IPv4 or IPv6 address
	SPUDRESULT_SPUDNET_ADDRESS_IN_USE        = 3026, // another listener already has that address and port
	SPUDRESULT_SPUDNET_INVALID_WAIT_SET      = 3027,
	SPUDRESULT_SPUDNET_ALREADY_IN_WAIT_SET   = 3028, // the object is in this set already, or a WebSocket in another
	SPUDRESULT_SPUDNET_INVALID_RESOLVER      = 3029, // NULL, or one whose lookup has already been run
	SPUDRESULT_SPUDNET_SYSTEM_TIMED_OUT      = 3030, // TCP: the operating system gave up waiting for the other end
	SPUDRESULT_SPUDNET_STACK_TIMED_OUT       = 3031, // HTTP, WebSocket: the client stack gave up waiting, or says something under it did
	SPUDRESULT_SPUDNET_INVALID_HTTP_TRANSFER = 3032, // NULL, or a call out of the transfer's order of use
	SPUDRESULT_SPUDNET_INVALID_INSTANCE      = 3033, // NULL, or not the instance the other object was made with

	SPUDRESULT_SAUD_INVALID_INSTANCE      = 4001,
	SPUDRESULT_SAUD_INVALID_DEVICE        = 4002,
	SPUDRESULT_SAUD_INVALID_STREAM        = 4003,
	SPUDRESULT_SAUD_FORMAT_NOT_SUPPORTED  = 4004,
	SPUDRESULT_SAUD_EXCLUSIVE_UNAVAILABLE = 4005,
	SPUDRESULT_SAUD_DEVICE_LOST           = 4006,
	SPUDRESULT_SAUD_PERIOD_OUT_OF_RANGE   = 4007,
	SPUDRESULT_SAUD_BUFFER_OUT_OF_RANGE   = 4008,
	SPUDRESULT_SAUD_THREAD_PRIORITY_UNSUPPORTED = 4009,
	SPUDRESULT_SAUD_THREAD_PRIORITY_DENIED      = 4010,
	SPUDRESULT_SAUD_SAMPLE_RATE_NOT_SETTABLE    = 4011,
	SPUDRESULT_SAUD_OS_CONVERSION_UNAVAILABLE   = 4012,

} SPUDRESULT;

#define SPUDFAIL(sr) ((sr) != SPUD_SUCCESS)

const char *spudresult_str(SPUDRESULT r);

#ifdef _DEBUG
/**
 * @brief Gives a SpudLib object a debug name, replacing any it already has.
 *
 * The string is copied, so it need not outlive the call. The object's
 * previous name is freed, and the object's own destroy call frees the name it
 * holds at that point. Exists in debug builds only.
 *
 * @param[in] object A handle to an object that carries a debug name. Every
 *                   SpudGPU, SpudAudio and SpudNet handle does, and so does an
 *                   sfs_file.
 * @param[in] name   The new name, or NULL to remove the current one.
 *
 * @retval SPUD_SUCCESS The name was replaced.
 * @retval SPUDRESULT_NULL_OBJECT `object` is NULL.
 * @retval SPUDRESULT_OUT_OF_MEMORY The copy could not be allocated. The
 *         object keeps the name it had.
 */
SPUDRESULT spud_debug_name_set(void *object, const char *name);
#define SPUD_SET_DEBUG_NAME(obj, name)                                         \
	spud_debug_name_set((void *)(obj), (name))

/**
 * @brief Returns an object's debug name, or NULL if it has none or `object`
 * is NULL.
 *
 * The string belongs to the object: it stays valid until the name is next set
 * or the object is destroyed.
 */
const char *spud_debug_name_get(void *object);
#define SPUD_GET_DEBUG_NAME(obj) spud_debug_name_get((void *)(obj))
#endif

#if __cplusplus
}
#endif

#endif // SPUDCORE_H
