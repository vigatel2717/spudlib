//
// Created by nathanmoore on 5/16/26.
//

#ifndef SPUDLIB_SPUDGPU_H
#define SPUDLIB_SPUDGPU_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if SPUDGPU_COMPILE_METAL_API
#endif

#if SPUDGPU_COMPILE_VULKAN_API
#endif

typedef uint32_t SPUDGPU_FORMAT;

enum {
    SPUDGPU_FORMAT_UNKNOWN = 0,
    SPUDGPU_FORMAT_R32G32B32A32_TYPELESS = 1,
    SPUDGPU_FORMAT_R32G32B32A32_FLOAT = 2,
    SPUDGPU_FORMAT_R32G32B32A32_UINT = 3,
    SPUDGPU_FORMAT_R32G32B32A32_SINT = 4,
    SPUDGPU_FORMAT_R32G32B32_TYPELESS = 5,
    SPUDGPU_FORMAT_R32G32B32_FLOAT = 6,
    SPUDGPU_FORMAT_R32G32B32_UINT = 7,
    SPUDGPU_FORMAT_R32G32B32_SINT = 8,
    SPUDGPU_FORMAT_R16G16B16A16_TYPELESS = 9,
    SPUDGPU_FORMAT_R16G16B16A16_FLOAT = 10,
    SPUDGPU_FORMAT_R16G16B16A16_UNORM = 11,
    SPUDGPU_FORMAT_R16G16B16A16_UINT = 12,
    SPUDGPU_FORMAT_R16G16B16A16_SNORM = 13,
    SPUDGPU_FORMAT_R16G16B16A16_SINT = 14,
    SPUDGPU_FORMAT_R32G32_TYPELESS = 15,
    SPUDGPU_FORMAT_R32G32_FLOAT = 16,
    SPUDGPU_FORMAT_R32G32_UINT = 17,
    SPUDGPU_FORMAT_R32G32_SINT = 18,
    SPUDGPU_FORMAT_R32G8X24_TYPELESS = 19,
    SPUDGPU_FORMAT_D32_FLOAT_S8X24_UINT = 20,
    SPUDGPU_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
    SPUDGPU_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
    SPUDGPU_FORMAT_R10G10B10A2_TYPELESS = 23,
    SPUDGPU_FORMAT_R10G10B10A2_UNORM = 24,
    SPUDGPU_FORMAT_R10G10B10A2_UINT = 25,
    SPUDGPU_FORMAT_R11G11B10_FLOAT = 26,
    SPUDGPU_FORMAT_R8G8B8A8_TYPELESS = 27,
    SPUDGPU_FORMAT_R8G8B8A8_UNORM = 28,
    SPUDGPU_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
    SPUDGPU_FORMAT_R8G8B8A8_UINT = 30,
    SPUDGPU_FORMAT_R8G8B8A8_SNORM = 31,
    SPUDGPU_FORMAT_R8G8B8A8_SINT = 32,
    SPUDGPU_FORMAT_R16G16_TYPELESS = 33,
    SPUDGPU_FORMAT_R16G16_FLOAT = 34,
    SPUDGPU_FORMAT_R16G16_UNORM = 35,
    SPUDGPU_FORMAT_R16G16_UINT = 36,
    SPUDGPU_FORMAT_R16G16_SNORM = 37,
    SPUDGPU_FORMAT_R16G16_SINT = 38,
    SPUDGPU_FORMAT_R32_TYPELESS = 39,
    SPUDGPU_FORMAT_D32_FLOAT = 40,
    SPUDGPU_FORMAT_R32_FLOAT = 41,
    SPUDGPU_FORMAT_R32_UINT = 42,
    SPUDGPU_FORMAT_R32_SINT = 43,
    SPUDGPU_FORMAT_R24G8_TYPELESS = 44,
    SPUDGPU_FORMAT_D24_UNORM_S8_UINT = 45,
    SPUDGPU_FORMAT_R24_UNORM_X8_TYPELESS = 46,
    SPUDGPU_FORMAT_X24_TYPELESS_G8_UINT = 47,
    SPUDGPU_FORMAT_R8G8_TYPELESS = 48,
    SPUDGPU_FORMAT_R8G8_UNORM = 49,
    SPUDGPU_FORMAT_R8G8_UINT = 50,
    SPUDGPU_FORMAT_R8G8_SNORM = 51,
    SPUDGPU_FORMAT_R8G8_SINT = 52,
    SPUDGPU_FORMAT_R16_TYPELESS = 53,
    SPUDGPU_FORMAT_R16_FLOAT = 54,
    SPUDGPU_FORMAT_D16_UNORM = 55,
    SPUDGPU_FORMAT_R16_UNORM = 56,
    SPUDGPU_FORMAT_R16_UINT = 57,
    SPUDGPU_FORMAT_R16_SNORM = 58,
    SPUDGPU_FORMAT_R16_SINT = 59,
    SPUDGPU_FORMAT_R8_TYPELESS = 60,
    SPUDGPU_FORMAT_R8_UNORM = 61,
    SPUDGPU_FORMAT_R8_UINT = 62,
    SPUDGPU_FORMAT_R8_SNORM = 63,
    SPUDGPU_FORMAT_R8_SINT = 64,
    SPUDGPU_FORMAT_A8_UNORM = 65,
    SPUDGPU_FORMAT_R1_UNORM = 66,
    SPUDGPU_FORMAT_R9G9B9E5_SHAREDEXP = 67,
    SPUDGPU_FORMAT_R8G8_B8G8_UNORM = 68,
    SPUDGPU_FORMAT_G8R8_G8B8_UNORM = 69,
    SPUDGPU_FORMAT_BC1_TYPELESS = 70,
    SPUDGPU_FORMAT_BC1_UNORM = 71,
    SPUDGPU_FORMAT_BC1_UNORM_SRGB = 72,
    SPUDGPU_FORMAT_BC2_TYPELESS = 73,
    SPUDGPU_FORMAT_BC2_UNORM = 74,
    SPUDGPU_FORMAT_BC2_UNORM_SRGB = 75,
    SPUDGPU_FORMAT_BC3_TYPELESS = 76,
    SPUDGPU_FORMAT_BC3_UNORM = 77,
    SPUDGPU_FORMAT_BC3_UNORM_SRGB = 78,
    SPUDGPU_FORMAT_BC4_TYPELESS = 79,
    SPUDGPU_FORMAT_BC4_UNORM = 80,
    SPUDGPU_FORMAT_BC4_SNORM = 81,
    SPUDGPU_FORMAT_BC5_TYPELESS = 82,
    SPUDGPU_FORMAT_BC5_UNORM = 83,
    SPUDGPU_FORMAT_BC5_SNORM = 84,
    SPUDGPU_FORMAT_B5G6R5_UNORM = 85,
    SPUDGPU_FORMAT_B5G5R5A1_UNORM = 86,
    SPUDGPU_FORMAT_B8G8R8A8_UNORM = 87,
    SPUDGPU_FORMAT_B8G8R8X8_UNORM = 88,
    SPUDGPU_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
    SPUDGPU_FORMAT_B8G8R8A8_TYPELESS = 90,
    SPUDGPU_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
    SPUDGPU_FORMAT_B8G8R8X8_TYPELESS = 92,
    SPUDGPU_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
    SPUDGPU_FORMAT_BC6H_TYPELESS = 94,
    SPUDGPU_FORMAT_BC6H_UF16 = 95,
    SPUDGPU_FORMAT_BC6H_SF16 = 96,
    SPUDGPU_FORMAT_BC7_TYPELESS = 97,
    SPUDGPU_FORMAT_BC7_UNORM = 98,
    SPUDGPU_FORMAT_BC7_UNORM_SRGB = 99,
    SPUDGPU_FORMAT_AYUV = 100,
    SPUDGPU_FORMAT_Y410 = 101,
    SPUDGPU_FORMAT_Y416 = 102,
    SPUDGPU_FORMAT_NV12 = 103,
    SPUDGPU_FORMAT_P010 = 104,
    SPUDGPU_FORMAT_P016 = 105,
    SPUDGPU_FORMAT_420_OPAQUE = 106,
    SPUDGPU_FORMAT_YUY2 = 107,
    SPUDGPU_FORMAT_Y210 = 108,
    SPUDGPU_FORMAT_Y216 = 109,
    SPUDGPU_FORMAT_NV11 = 110,
    SPUDGPU_FORMAT_AI44 = 111,
    SPUDGPU_FORMAT_IA44 = 112,
    SPUDGPU_FORMAT_P8 = 113,
    SPUDGPU_FORMAT_A8P8 = 114,
    SPUDGPU_FORMAT_B4G4R4A4_UNORM = 115,

    SPUDGPU_FORMAT_P208 = 130,
    SPUDGPU_FORMAT_V208 = 131,
    SPUDGPU_FORMAT_V408 = 132,

    SPUDGPU_FORMAT_FORCE_UINT = -1 //0xffffffff
};

struct spudgpu_format_map {
    const char *name;
    SPUDGPU_FORMAT format;
};

static constexpr struct spudgpu_format_map format_map[] = {
    {"R32G32B32A32_FLOAT", SPUDGPU_FORMAT_R32G32B32A32_FLOAT},
};

// Use the return value / 8 for byte size.
// @return Pixel bit size according to the format.
uint32_t spudgpu_format_bit_count(const SPUDGPU_FORMAT &fmt);

// @return SPUDGPU_FORMAT according to string.
// TODO: Optimize this function. Perhaps make a FormatMap and have this 'static const struct FormatMap FORMAT_TABLE[] = {'
SPUDGPU_FORMAT spudgpu_format_from_string(const char *fmt_str) {
    if (fmt_str == NULL) return SPUDGPU_FORMAT_UNKNOWN;

    if (strcmp(fmt_str, "R32G32B32A32_TYPELESS") == 0) return SPUDGPU_FORMAT_R32G32B32A32_TYPELESS;
    if (strcmp(fmt_str, "R32G32B32A32_FLOAT") == 0) return SPUDGPU_FORMAT_R32G32B32A32_FLOAT;
    if (strcmp(fmt_str, "R32G32B32A32_UINT") == 0) return SPUDGPU_FORMAT_R32G32B32A32_UINT;
    if (strcmp(fmt_str, "R32G32B32A32_SINT") == 0) return SPUDGPU_FORMAT_R32G32B32A32_SINT;

    if (strcmp(fmt_str, "R32G32B32_TYPELESS") == 0) return SPUDGPU_FORMAT_R32G32B32_TYPELESS;
    if (strcmp(fmt_str, "R32G32B32_FLOAT") == 0) return SPUDGPU_FORMAT_R32G32B32_FLOAT;
    if (strcmp(fmt_str, "R32G32B32_UINT") == 0) return SPUDGPU_FORMAT_R32G32B32_UINT;
    if (strcmp(fmt_str, "R32G32B32_SINT") == 0) return SPUDGPU_FORMAT_R32G32B32_SINT;

    if (strcmp(fmt_str, "R16G16B16A16_TYPELESS") == 0) return SPUDGPU_FORMAT_R16G16B16A16_TYPELESS;
    if (strcmp(fmt_str, "R16G16B16A16_FLOAT") == 0) return SPUDGPU_FORMAT_R16G16B16A16_FLOAT;
    if (strcmp(fmt_str, "R16G16B16A16_UNORM") == 0) return SPUDGPU_FORMAT_R16G16B16A16_UNORM;
    if (strcmp(fmt_str, "R16G16B16A16_UINT") == 0) return SPUDGPU_FORMAT_R16G16B16A16_UINT;
    if (strcmp(fmt_str, "R16G16B16A16_SNORM") == 0) return SPUDGPU_FORMAT_R16G16B16A16_SNORM;
    if (strcmp(fmt_str, "R16G16B16A16_SINT") == 0) return SPUDGPU_FORMAT_R16G16B16A16_SINT;

    if (strcmp(fmt_str, "R32G32_TYPELESS") == 0) return SPUDGPU_FORMAT_R32G32_TYPELESS;
    if (strcmp(fmt_str, "R32G32_FLOAT") == 0) return SPUDGPU_FORMAT_R32G32_FLOAT;
    if (strcmp(fmt_str, "R32G32_UINT") == 0) return SPUDGPU_FORMAT_R32G32_UINT;
    if (strcmp(fmt_str, "R32G32_SINT") == 0) return SPUDGPU_FORMAT_R32G32_SINT;

    if (strcmp(fmt_str, "R32G8X24_TYPELESS") == 0) return SPUDGPU_FORMAT_R32G8X24_TYPELESS;
    if (strcmp(fmt_str, "D32_FLOAT_S8X24_UINT") == 0) return SPUDGPU_FORMAT_D32_FLOAT_S8X24_UINT;
    if (strcmp(fmt_str, "R32_FLOAT_X8X24_TYPELESS") == 0) return SPUDGPU_FORMAT_R32_FLOAT_X8X24_TYPELESS;
    if (strcmp(fmt_str, "X32_TYPELESS_G8X24_UINT") == 0) return SPUDGPU_FORMAT_X32_TYPELESS_G8X24_UINT;

    if (strcmp(fmt_str, "Y416") == 0) return SPUDGPU_FORMAT_Y416;
    if (strcmp(fmt_str, "Y210") == 0) return SPUDGPU_FORMAT_Y210;
    if (strcmp(fmt_str, "Y216") == 0) return SPUDGPU_FORMAT_Y216;

    if (strcmp(fmt_str, "R10G10B10A2_TYPELESS") == 0) return SPUDGPU_FORMAT_R10G10B10A2_TYPELESS;
    if (strcmp(fmt_str, "R10G10B10A2_UNORM") == 0) return SPUDGPU_FORMAT_R10G10B10A2_UNORM;
    if (strcmp(fmt_str, "R10G10B10A2_UINT") == 0) return SPUDGPU_FORMAT_R10G10B10A2_UINT;

    if (strcmp(fmt_str, "R11G11B10_FLOAT") == 0) return SPUDGPU_FORMAT_R11G11B10_FLOAT;

    if (strcmp(fmt_str, "R8G8B8A8_TYPELESS") == 0) return SPUDGPU_FORMAT_R8G8B8A8_TYPELESS;
    if (strcmp(fmt_str, "R8G8B8A8_UNORM") == 0) return SPUDGPU_FORMAT_R8G8B8A8_UNORM;
    if (strcmp(fmt_str, "R8G8B8A8_UNORM_SRGB") == 0) return SPUDGPU_FORMAT_R8G8B8A8_UNORM_SRGB;
    if (strcmp(fmt_str, "R8G8B8A8_UINT") == 0) return SPUDGPU_FORMAT_R8G8B8A8_UINT;
    if (strcmp(fmt_str, "R8G8B8A8_SNORM") == 0) return SPUDGPU_FORMAT_R8G8B8A8_SNORM;
    if (strcmp(fmt_str, "R8G8B8A8_SINT") == 0) return SPUDGPU_FORMAT_R8G8B8A8_SINT;

    if (strcmp(fmt_str, "R16G16_TYPELESS") == 0) return SPUDGPU_FORMAT_R16G16_TYPELESS;
    if (strcmp(fmt_str, "R16G16_FLOAT") == 0) return SPUDGPU_FORMAT_R16G16_FLOAT;
    if (strcmp(fmt_str, "R16G16_UNORM") == 0) return SPUDGPU_FORMAT_R16G16_UNORM;
    if (strcmp(fmt_str, "R16G16_UINT") == 0) return SPUDGPU_FORMAT_R16G16_UINT;
    if (strcmp(fmt_str, "R16G16_SNORM") == 0) return SPUDGPU_FORMAT_R16G16_SNORM;
    if (strcmp(fmt_str, "R16G16_SINT") == 0) return SPUDGPU_FORMAT_R16G16_SINT;

    if (strcmp(fmt_str, "R32_TYPELESS") == 0) return SPUDGPU_FORMAT_R32_TYPELESS;
    if (strcmp(fmt_str, "D32_FLOAT") == 0) return SPUDGPU_FORMAT_D32_FLOAT;
    if (strcmp(fmt_str, "R32_FLOAT") == 0) return SPUDGPU_FORMAT_R32_FLOAT;
    if (strcmp(fmt_str, "R32_UINT") == 0) return SPUDGPU_FORMAT_R32_UINT;
    if (strcmp(fmt_str, "R32_SINT") == 0) return SPUDGPU_FORMAT_R32_SINT;

    if (strcmp(fmt_str, "R24G8_TYPELESS") == 0) return SPUDGPU_FORMAT_R24G8_TYPELESS;
    if (strcmp(fmt_str, "D24_UNORM_S8_UINT") == 0) return SPUDGPU_FORMAT_D24_UNORM_S8_UINT;
    if (strcmp(fmt_str, "R24_UNORM_X8_TYPELESS") == 0) return SPUDGPU_FORMAT_R24_UNORM_X8_TYPELESS;
    if (strcmp(fmt_str, "X24_TYPELESS_G8_UINT") == 0) return SPUDGPU_FORMAT_X24_TYPELESS_G8_UINT;

    if (strcmp(fmt_str, "R9G9B9E5_SHAREDEXP") == 0) return SPUDGPU_FORMAT_R9G9B9E5_SHAREDEXP;

    if (strcmp(fmt_str, "R8G8_B8G8_UNORM") == 0) return SPUDGPU_FORMAT_R8G8_B8G8_UNORM;
    if (strcmp(fmt_str, "G8R8_G8B8_UNORM") == 0) return SPUDGPU_FORMAT_G8R8_G8B8_UNORM;

    if (strcmp(fmt_str, "B8G8R8A8_UNORM") == 0) return SPUDGPU_FORMAT_B8G8R8A8_UNORM;
    if (strcmp(fmt_str, "B8G8R8X8_UNORM") == 0) return SPUDGPU_FORMAT_B8G8R8X8_UNORM;

    if (strcmp(fmt_str, "R10G10B10_XR_BIAS_A2_UNORM") == 0) return SPUDGPU_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

    if (strcmp(fmt_str, "B8G8R8A8_TYPELESS") == 0) return SPUDGPU_FORMAT_B8G8R8A8_TYPELESS;
    if (strcmp(fmt_str, "B8G8R8A8_UNORM_SRGB") == 0) return SPUDGPU_FORMAT_B8G8R8A8_UNORM_SRGB;
    if (strcmp(fmt_str, "B8G8R8X8_TYPELESS") == 0) return SPUDGPU_FORMAT_B8G8R8X8_TYPELESS;
    if (strcmp(fmt_str, "B8G8R8X8_UNORM_SRGB") == 0) return SPUDGPU_FORMAT_B8G8R8X8_UNORM_SRGB;

    if (strcmp(fmt_str, "AYUV") == 0) return SPUDGPU_FORMAT_AYUV;
    if (strcmp(fmt_str, "Y410") == 0) return SPUDGPU_FORMAT_Y410;
    if (strcmp(fmt_str, "YUY2") == 0) return SPUDGPU_FORMAT_YUY2;

    if (strcmp(fmt_str, "P010") == 0) return SPUDGPU_FORMAT_P010;
    if (strcmp(fmt_str, "P016") == 0) return SPUDGPU_FORMAT_P016;

    if (strcmp(fmt_str, "R8G8_TYPELESS") == 0) return SPUDGPU_FORMAT_R8G8_TYPELESS;
    if (strcmp(fmt_str, "R8G8_UNORM") == 0) return SPUDGPU_FORMAT_R8G8_UNORM;
    if (strcmp(fmt_str, "R8G8_UINT") == 0) return SPUDGPU_FORMAT_R8G8_UINT;
    if (strcmp(fmt_str, "R8G8_SNORM") == 0) return SPUDGPU_FORMAT_R8G8_SNORM;
    if (strcmp(fmt_str, "R8G8_SINT") == 0) return SPUDGPU_FORMAT_R8G8_SINT;

    if (strcmp(fmt_str, "R16_TYPELESS") == 0) return SPUDGPU_FORMAT_R16_TYPELESS;
    if (strcmp(fmt_str, "R16_FLOAT") == 0) return SPUDGPU_FORMAT_R16_FLOAT;
    if (strcmp(fmt_str, "D16_UNORM") == 0) return SPUDGPU_FORMAT_D16_UNORM;
    if (strcmp(fmt_str, "R16_UNORM") == 0) return SPUDGPU_FORMAT_R16_UNORM;
    if (strcmp(fmt_str, "R16_UINT") == 0) return SPUDGPU_FORMAT_R16_UINT;
    if (strcmp(fmt_str, "R16_SNORM") == 0) return SPUDGPU_FORMAT_R16_SNORM;
    if (strcmp(fmt_str, "R16_SINT") == 0) return SPUDGPU_FORMAT_R16_SINT;

    if (strcmp(fmt_str, "B5G6R5_UNORM") == 0) return SPUDGPU_FORMAT_B5G6R5_UNORM;
    if (strcmp(fmt_str, "B5G5R5A1_UNORM") == 0) return SPUDGPU_FORMAT_B5G5R5A1_UNORM;

    if (strcmp(fmt_str, "A8P8") == 0) return SPUDGPU_FORMAT_A8P8;

    if (strcmp(fmt_str, "B4G4R4A4_UNORM") == 0) return SPUDGPU_FORMAT_B4G4R4A4_UNORM;

    if (strcmp(fmt_str, "NV12") == 0) return SPUDGPU_FORMAT_NV12;
    if (strcmp(fmt_str, "420_OPAQUE") == 0) return SPUDGPU_FORMAT_420_OPAQUE;
    if (strcmp(fmt_str, "NV11") == 0) return SPUDGPU_FORMAT_NV11;

    if (strcmp(fmt_str, "R8_TYPELESS") == 0) return SPUDGPU_FORMAT_R8_TYPELESS;
    if (strcmp(fmt_str, "R8_UNORM") == 0) return SPUDGPU_FORMAT_R8_UNORM;
    if (strcmp(fmt_str, "R8_UINT") == 0) return SPUDGPU_FORMAT_R8_UINT;
    if (strcmp(fmt_str, "R8_SNORM") == 0) return SPUDGPU_FORMAT_R8_SNORM;
    if (strcmp(fmt_str, "R8_SINT") == 0) return SPUDGPU_FORMAT_R8_SINT;
    if (strcmp(fmt_str, "A8_UNORM") == 0) return SPUDGPU_FORMAT_A8_UNORM;

    if (strcmp(fmt_str, "AI44") == 0) return SPUDGPU_FORMAT_AI44;
    if (strcmp(fmt_str, "IA44") == 0) return SPUDGPU_FORMAT_IA44;
    if (strcmp(fmt_str, "P8") == 0) return SPUDGPU_FORMAT_P8;

    if (strcmp(fmt_str, "R1_UNORM") == 0) return SPUDGPU_FORMAT_R1_UNORM;

    if (strcmp(fmt_str, "BC1_TYPELESS") == 0) return SPUDGPU_FORMAT_BC1_TYPELESS;
    if (strcmp(fmt_str, "BC1_UNORM") == 0) return SPUDGPU_FORMAT_BC1_UNORM;
    if (strcmp(fmt_str, "BC1_UNORM_SRGB") == 0) return SPUDGPU_FORMAT_BC1_UNORM_SRGB;
    if (strcmp(fmt_str, "BC4_TYPELESS") == 0) return SPUDGPU_FORMAT_BC4_TYPELESS;
    if (strcmp(fmt_str, "BC4_UNORM") == 0) return SPUDGPU_FORMAT_BC4_UNORM;
    if (strcmp(fmt_str, "BC4_SNORM") == 0) return SPUDGPU_FORMAT_BC4_SNORM;

    if (strcmp(fmt_str, "BC2_TYPELESS") == 0) return SPUDGPU_FORMAT_BC2_TYPELESS;
    if (strcmp(fmt_str, "BC2_UNORM") == 0) return SPUDGPU_FORMAT_BC2_UNORM;
    if (strcmp(fmt_str, "BC2_UNORM_SRGB") == 0) return SPUDGPU_FORMAT_BC2_UNORM_SRGB;
    if (strcmp(fmt_str, "BC3_TYPELESS") == 0) return SPUDGPU_FORMAT_BC3_TYPELESS;
    if (strcmp(fmt_str, "BC3_UNORM") == 0) return SPUDGPU_FORMAT_BC3_UNORM;
    if (strcmp(fmt_str, "BC3_UNORM_SRGB") == 0) return SPUDGPU_FORMAT_BC3_UNORM_SRGB;
    if (strcmp(fmt_str, "BC5_TYPELESS") == 0) return SPUDGPU_FORMAT_BC5_TYPELESS;
    if (strcmp(fmt_str, "BC5_UNORM") == 0) return SPUDGPU_FORMAT_BC5_UNORM;
    if (strcmp(fmt_str, "BC5_UNORM_SRGB") == 0) return SPUDGPU_FORMAT_BC5_SNORM;

    if (strcmp(fmt_str, "BC6H_TYPELESS") == 0) return SPUDGPU_FORMAT_BC6H_TYPELESS;
    if (strcmp(fmt_str, "BC6H_UF16") == 0) return SPUDGPU_FORMAT_BC6H_UF16;
    if (strcmp(fmt_str, "BC6H_SF16") == 0) return SPUDGPU_FORMAT_BC6H_SF16;

    if (strcmp(fmt_str, "BC7_TYPELESS") == 0) return SPUDGPU_FORMAT_BC7_TYPELESS;
    if (strcmp(fmt_str, "BC7_UNORM") == 0) return SPUDGPU_FORMAT_BC7_UNORM;
    if (strcmp(fmt_str, "BC7_UNORM_SRGB") == 0) return SPUDGPU_FORMAT_BC7_UNORM_SRGB;

    return SPUDGPU_FORMAT_UNKNOWN;
}

typedef struct spudgpu_device_t *spudgpu_device;
typedef struct spudgpu_resource_pool_t *spudgpu_resource_pool;
typedef struct spudgpu_buffer_t *spudgpu_buffer;
typedef struct spudgpu_buffer_view_t *spudgpu_buffer_view;
typedef struct spudgpu_image_t *spudgpu_image;
typedef struct spudgpu_image_view_t *spudgpu_image_view;
typedef struct spudgpu_shader_pipeline_t *spudgpu_shader_pipeline;
typedef struct spudgpu_command_list_t *spudgpu_command_list;
typedef struct spudgpu_command_queue_t *spudgpu_command_queue;
typedef struct spudgpu_swap_chain_t *spudgpu_swap_chain;

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
 */
///@{
enum {
    /// Placeholder representing an uninitialized, unsupported, or invalid graphics API.
    SPUDGPU_NATIVE_API_NONE = 0,

    /// Vulkan cross-platform graphics API. Used primarily on Linux, Windows, and Android.
    SPUDGPU_NATIVE_API_VULKAN = 1,

    /// Metal proprietary graphics API. Used exclusively for Apple Silicon ecosystems (macOS, iOS, iPadOS).
    SPUDGPU_NATIVE_API_METAL = 2
};

///@}


/**
 * @brief Initializes the global SpudGPU context.
 * * Configures the underlying graphics API backend, sets up instances, and internally gathers a list of GPU devices.
 * Use spudgpu_get_devices() after calling this function.
 * * @param[in] native_api                 The targeted graphics backend (Vulkan or Metal).
 * @param[in] application_name    Null-terminated string containing the client application's name.
 * @param[in] application_version Packed 32-bit integer representing the application version.
 * @param[in] engine_name         Null-terminated string containing the custom game/rendering engine name.
 * @param[in] engine_version      Packed 32-bit integer representing the engine version.
 * * @return true if initialization succeeded and the hardware driver was successfully hooked.
 * @return false if the requested API is unsupported, or driver initialization failed.
 * * @note Must be called before invoking any other SpudGPU API function.
 */
bool spudgpu_init(
    SPUDGPU_NATIVE_API native_api,
    const char *application_name,
    uint32_t application_version,
    const char *engine_name,
    uint32_t engine_version);

/**
 * @brief Shuts down the SpudGPU subsystem.
 * * Disposes of all internal runtime states, destroys active backend contexts,
 * and releases hooks into the graphics hardware.
 * * @warning Calling this while resources (like buffers, textures, or pipelines)
 * are still active will result in undefined behavior or memory leaks.
 */
void spudgpu_terminate();

/**
 * @brief A container structure holding a list of discovered physical GPUs.
 */
typedef struct SPUDGPU_DEVICE_LIST {
    /// Pointer to a contiguous array of physical device handles.
    spudgpu_device *devices;
    /// Total number of valid devices populated in the `devices` array.
    uint64_t device_count;
} SPUDGPU_DEVICE_LIST;

/**
 * @brief SpudGPU stores an enumeration of the physical graphics devices available on the host machine.
 * * @return SPUDGPU_DEVICE_LIST A structure containing the array pointer and size
 * of all discovered hardware devices. Returns a count of 0 if no
 * compatible hardware is found.
 */
SPUDGPU_DEVICE_LIST spudgpu_get_devices();

/**
 * @brief Retrieves the active graphics API backend.
 * * @return SPUDGPU_API The enum value representing the currently active backend.
 * Returns `SPUDGPU_API_NONE` if the system has not been initialized.
 */
SPUDGPU_NATIVE_API spudgpu_get_native_gpu_api();


/*
 * Static GPU Data = DEVICE_LOCAL
 * Dynamic CPU Data = HOST_VISIBLE | HOST COHERENT
 * Direct-Write GPU Data = DEVICE_LOCAL | HOST_VISIBLE | HOST COHERENT
 * Readback Data = HOST_VISIBLE | HOST_CACHED
 */
/**
 * @brief Bitmask representing physical memory allocation properties.
 * * These flags specify where a memory resource physically resides (VRAM vs. System RAM)
 * and how the CPU and GPU cache or synchronize access to it.
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
 * - **Dynamic CPU Data:** `SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE | SPUDGPU_MEMORY_FLAGS_HOST_COHERENT`
 * - **Direct-Write GPU Data:** `SPUDGPU_MEMORY_FLAGS_DEVICE_LOCAL | SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE | SPUDGPU_MEMORY_FLAGS_HOST_COHERENT`
 * - **Readback Data:** `SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE | SPUDGPU_MEMORY_FLAGS_HOST_CACHED`
 */
///@{
enum {
    /// No memory flags specified. Default or uninitialized state.
    SPUDGPU_MEMORY_FLAGS_NONE = 0,

    /** * @brief Memory is local to the device (VRAM).
     * * Offers the highest throughput for GPU execution. Usually inaccessible directly by the CPU
     * unless combined with `SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE`.
     */
    SPUDGPU_MEMORY_FLAGS_DEVICE_LOCAL = 1 << 0,

    /** * @brief Memory can be mapped for CPU access (System RAM or Bar-Mapped VRAM).
     * * Required if you intend to use `memcpy` or pointers from the CPU host code.
     */
    SPUDGPU_MEMORY_FLAGS_HOST_VISIBLE = 1 << 1,

    /** * @brief Memory writes are automatically synchronized between host and device.
     * * Ensures that CPU modifications are instantly visible to the GPU without requiring
     * explicit cache flushing commands (e.g., `vkFlushMappedMemoryRanges`).
     */
    SPUDGPU_MEMORY_FLAGS_HOST_COHERENT = 1 << 2,

    /** * @brief Memory allocation is cached on the host CPU.
     * * Optimizes performance for CPU read operations (e.g., transferring data back
     * from the GPU to the CPU). CPU reads from non-cached memory are notoriously slow.
     */
    SPUDGPU_MEMORY_FLAGS_HOST_CACHED = 1 << 3
};

///@}


/**
 * @brief Bitmask defining the intended pipeline binding targets for a GPU buffer.
 * * Informs the driver how the memory will be bound within hardware command buffers
 * to allow internal optimization.
 * * @see SPUDGPU_BUFFER_USAGE_Constants
 */
typedef uint32_t SPUDGPU_BUFFER_USAGE;

/**
 * @name Buffer Pipeline Usage Flags
 * @anchor SPUDGPU_BUFFER_USAGE_Constants
 * * Bitmask flags denoting buffer compatibility. A single buffer can fulfill multiple
 * roles by combining flags via bitwise OR (`|`).
 */
///@{
enum {
    /// No hardware binding target specified.
    SPUDGPU_BUFFER_USAGE_NONE = 0,

    /// Buffer is compatible for binding as a Input Assembler Vertex Stream (Vertex Buffer).
    SPUDGPU_BUFFER_USAGE_VERTEX = 1 << 0,

    /// Buffer contains index arrays (16-bit or 32-bit indices) for indexed draw calls (Index Buffer).
    SPUDGPU_BUFFER_USAGE_INDEX = 1 << 1,

    /** * @brief Buffer acts as a Uniform/Constant Buffer block (Uniform Buffer or Constant Buffer).
     * * Optimized for uniform, read-only shader data accessed globally across shader stages.
     */
    SPUDGPU_BUFFER_USAGE_UNIFORM = 1 << 2,

    /** * @brief Buffer acts as a Shader Storage Buffer Object (SSBO) / Structured Buffer.
     * * Typically used for large, variable-sized data structures requiring read/write
     * capabilities within compute or graphics shaders.
     */
    SPUDGPU_BUFFER_USAGE_STORAGE = 1 << 3
};

///@}


/**
 * @brief Descriptor used for a physical GPU buffer.
 */
typedef struct spudgpu_buffer_desc {
    /// Is this spudgpu_buffer used as a Vertex, Index, Uniform/Constant, Storage Buffer?
    /// You can have multiple usages as this is a bitmask configuration.
    /// @see SPUDGPU_BUFFER_USAGE
    uint32_t usage;

    /// Bitmask configuration specifying VRAM residency and CPU cache coherency rules.
    /// @see SPUDGPU_MEMORY_FLAGS
    uint32_t memory_flags;

    /** @brief Hardware memory address.
     * @note This will remain '0' until after the gpu_buffer is created.
     * After creation, this will be assigned a value when calling 'spudgpu_get_buffer_desc(gpu_buffer)'.
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
 * * @param[in] device The GPU device this buffer will be allocated on.
 * @param[in] desc   Pointer to the descriptor settings for the new buffer.
 * * @return spudgpu_buffer A valid handle to the allocated GPU buffer resource,
 * or NULL if allocation fails.
 * * @note Variable 'gpu_buffer_desc.gpu_address_location' should be zero, otherwise this function will return NULL.
 * The buffer will have an address location after creation.
 */
spudgpu_buffer spudgpu_create_buffer(spudgpu_device device, const spudgpu_buffer_desc *desc);

/**
 * @brief Retrieves the immutable configuration descriptor used to create the given buffer.
 * * @note Variable 'gpu_buffer_desc.gpu_address_location' is set when this function is called using a valid buffer.
 * * @param[in] buffer The target buffer handle to query.
 * @return spudgpu_buffer_desc A copy of the configuration descriptor structure.
 */
spudgpu_buffer_desc spudgpu_get_buffer_desc(spudgpu_buffer buffer);

/**
 * @brief Safely deallocates a physical GPU buffer resource.
 * * Frees underlying driver resources and system/device memory allocations.
 * * @param[in] device The GPU device that originally allocated this resource.
 * @param[in] buffer The buffer to destroy.
 * * @warning Any active `spudgpu_buffer_view` dependencies or pending GPU command lists
 * referencing this buffer must be cleared before destruction to avoid hardware faults.
 */
void spudgpu_destroy_buffer(spudgpu_device device, spudgpu_buffer buffer);

/**
 * @brief Configuration descriptor defining a sub-allocated window (view) into an existing buffer.
 * * Buffer views are lightweight abstractions allowing you to reinterpret specific segments
 * of a parent buffer without duplicating or allocating additional physical device memory.
 */
typedef struct spudgpu_buffer_view_desc {
    /// Handle to the GPU buffer that this view window reads from or writes to.
    spudgpu_buffer parent_buffer;

    /// The byte offset to where this specific view window begins inside the parent buffer.
    uint64_t offset_from_parent_buffer;

    /** * @brief The data structure element stride in bytes.
     * * Crucial for structured storage arrays or vertex inputs (e.g., byte size of a
     * single instance of your custom Vertex struct). Pass `0` for raw, unformatted buffers.
     */
    uint64_t stride;

    /// The span/length of this specific buffer view window in bytes.
    uint64_t size;
} spudgpu_buffer_view_desc;

/**
 * @brief Creates a view window into a section of an existing GPU buffer.
 * * @param[in] buffer The target GPU buffer containing the raw memory allocation.
 * @param[in] desc   Pointer to the view configuration descriptor.
 * * @return spudgpu_buffer_view A handle to the generated view.
 */
spudgpu_buffer_view spudgpu_create_buffer_view(spudgpu_buffer buffer, const spudgpu_buffer_view_desc *desc);

/**
 * @brief Destroys a GPU buffer view.
 * * Frees the virtual view mapping context. Does **not** impact the lifecycle or data
 * retention of the underlying parent GPU buffer.
 * * @param[in] device The GPU device containing the parent GPU buffer.
 * @param[in] buffer The target GPU buffer view to destroy.
 */
void spudgpu_destroy_buffer_view(spudgpu_device device, spudgpu_buffer_view buffer);

/**
 * @brief Retrieves the immutable configuration descriptor of an active buffer view.
 * * @param[in] view The GPU buffer view to get the descriptor of.
 * @return spudgpu_buffer_view_desc A copy of the view descriptor structure.
 */
spudgpu_buffer_view_desc get_buffer_view_desc(spudgpu_buffer_view view);

/**
 * @brief Bitmask determining the type and capabilities of a GPU image object.
 * * @see SPUDGPU_IMAGE_USAGE_Constants
 */
typedef uint32_t SPUDGPU_IMAGE_USAGE;

/**
 * @name Image Usage Flags
 * @anchor SPUDGPU_IMAGE_USAGE_Constants
 * * Bitmask configurations dictating how the GPU image object is structured and sampled.
 */
///@{
enum {
    /// No usage target specified. Uninitialized configuration state.
    SPUDGPU_IMAGE_USAGE_NONE = 0,

    // Standard Texture2D used for Texture2D / Render Target.
    SPUDGPU_IMAGE_USAGE_TEXTURE2D = 1 << 0,

    // Standard Texture3D used for Texture3D / Voxel Grid.
    SPUDGPU_IMAGE_USAGE_TEXTURE3D = 1 << 1
};

///@}

/**
 * @brief Configuration descriptor used to allocate a physical GPU image resource (texture/surface).
 */
typedef struct spudgpu_image_desc {
    /// Bitmask configuration specifying texture layout type (2D, 3D, etc.).
    /// @see SPUDGPU_IMAGE_USAGE
    uint32_t usage;

    /// Bitmask configuration specifying VRAM residency rules.
    /// Typically set to `SPUDGPU_MEMORY_FLAGS_DEVICE_LOCAL` for performance.
    /// @see SPUDGPU_MEMORY_FLAGS
    uint32_t memory_flags;

    /** @brief Hardware memory address.
     * @note This will remain '0' until after the gpu_image is created.
     * After creation, this will be assigned a value when calling 'spudgpu_get_image_desc(gpu_image)'.
     */
    uint64_t gpu_address_location;

    /// The texel data layout and channel bit-depth configuration (e.g., RGBA8_UNORM, R32_SFLOAT).
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
     * * Allows for texture arrays (e.g., a series of 2D textures grouped into a single bindable allocation).
     * Must be set to at least `1` for standard standalone textures.
     */
    uint32_t array_layers;

    /** * @brief Total number of downsampled mipmap resolution steps allocated for this texture.
     * * Pass `1` for no mipmaps. Level [0] represents the original full-resolution image data.
     */
    uint32_t mip_levels;
#if _DEBUG
    /// @brief A string identifier used for diagnostic tracking.
    const char *debug_name = nullptr;
#endif
} spudgpu_image_desc;

/**
 * @brief Allocates a physical GPU image/texture.
 * * @param[in] device The GPU device this image will be allocated on.
 * @param[in] desc   Pointer to the descriptor settings for the new image.
 * * @return spudgpu_image A valid handle to the allocated GPU image resource,
 * or NULL if allocation fails.
 * * @note Variable 'gpu_image_desc.gpu_address_location' should be zero, otherwise this function will return NULL.
 * The image will have an address location after creation.
 */
spudgpu_image spudgpu_create_image(spudgpu_device device, const spudgpu_image_desc *desc);

/**
 * @brief Retrieves the configuration descriptor used to create the GPU image object.
 * * @param[in] image The target GPU image.
 * @return spudgpu_image_desc A copy of the configuration descriptor structure.
 */
spudgpu_image_desc spudgpu_get_image_desc(spudgpu_image image);

/**
 * @brief Safely deallocates a physical GPU image.
 * * Frees underlying driver resources and releases system/device memory allocations.
 * * @param[in] device The GPU device that originally allocated the resource.
 * @param[in] image  The target GPU image to destroy.
 * * @warning Any active command streams, image views, descriptors, or render passes currently
 * binding or writing to this texture must finish execution on the GPU timeline before invocation.
 */
void spudgpu_destroy_image(spudgpu_device device, spudgpu_image image);


/**
 * @brief Defines a specific subsection (slice) of a multi-layered or mipmapped texture.
 * * Allows a pipeline stage to isolate and bind specific layers or mip levels of a parent
 * image rather than the entire allocation.
 */
typedef struct spudgpu_image_view_desc_subresource_range {
    /** * @brief Bitmask determining which structural aspect of the texture is targeted.
     * * Typically maps to flags like `COLOR`, `DEPTH`, or `STENCIL`. Tells the driver
     * whether to look at pixel data or depth/stencil metadata buffers.
     */
    uint64_t aspect_mask;

    /// The starting mipmap level index for this view window (0 being the highest resolution).
    uint64_t base_mip_level;

    /// The total number of downsampled mipmap levels to include in this view range.
    uint64_t mip_level_count;

    /// The starting array layer index for this view window (used for texture arrays/cubemaps).
    uint64_t base_array_layer;

    /// The total number of layers to include in this view range.
    uint64_t array_layer_count;
} spudgpu_image_view_desc_subresource_range;

/**
 * @brief Configuration descriptor defining a read/write window (view) into an existing GPU image.
 * * Like buffer views, image views do not allocate raw VRAM; they wrap existing physical
 * images to declare how shader stages should interpret their boundaries and contents.
 */
typedef struct spudgpu_image_view_desc {
    /// Handle to the physical GPU image containing the pixel data.
    spudgpu_image parent_image;

    // TODO: Swizzle indentities? Learn about that

    /// The isolated subresource layer and mip boundary definitions for this view.
    struct spudgpu_image_view_desc_subresource_range subresource_range;
} spudgpu_image_view_desc;

/**
 * @brief Creates a GPU image view mapping to a specific texture subresource range.
 * * @param[in] image The target parent image resource.
 * @param[in] desc  Pointer to the configuration descriptor.
 * * @return spudgpu_image_view A handle to the generatred GPU image view.
 */
spudgpu_image_view spudgpu_create_image_view(spudgpu_image image, const spudgpu_image_view_desc *desc);

/**
 * @brief Destroys a GPU image view.
 * * This does not affect the pixel/texel data of the parent GPU image object,
 * as it only destroys this view into that GPU image object.
 * * @param[in] device The GPU device containing the GPU image.
 * @param[in] image  The GPU image view to destroy.
 */
void spudgpu_destroy_image_view(spudgpu_device device, spudgpu_image_view image);

/**
 * @brief Retrieves the configuration descriptor of the GPU image view.
 * * @param[in] view The GPU image view handle to query.
 * @return spudgpu_image_view_desc A copy of the GPU image view configuration descriptor structure.
 */
spudgpu_image_view_desc spudgpu_get_image_view_desc(spudgpu_image_view view);

/**
 * @brief Begins recording graphics or compute infrastructure commands.
 * * Resets the command buffer's internal state tracking and sets it to a recording mode.
 * @param[in] cmd Target command list instance to open.
 * * @warning A command list must not be simultaneously recorded across multiple CPU threads.
 */
void spudgpu_begin_command_list(spudgpu_command_list cmd);

/**
 * @brief Finalizes recording for the specified command list.
 * * Closes the command stream, compiling it into an immutable hardware execution package
 * ready for queue submission.
 * @param[in] cmd Target command list instance to close.
 */
void spudgpu_end_command_list(spudgpu_command_list cmd);

/**
 * @brief Defines the normalized window transformation dimensions for rendering output coordinates.
 * * Maps normalized device coordinates (NDC) ranging [-1, 1] horizontally and vertically
 * directly into a target render target screen pixel domain.
 */
typedef struct SPUDGPU_VIEWPORT {
    /// X-coordinate of the upper-left corner of the viewport region in pixels.
    float x = 0.0f;

    /// Y-coordinate of the upper-left corner of the viewport region in pixels.
    float y = 0.0f;

    /// Total width of the targeted viewport frame in pixels.
    float width = 0.0f;

    /// Total height of the targeted viewport frame in pixels.
    float height = 0.0f;

    /// Minimum depth boundary slice. Usually maps to `0.0f` (near clipping plane).
    float minDepth = 0.0f;

    /// Maximum depth boundary slice. Usually maps to `1.0f` (far clipping plane).
    float maxDepth = 0.0f;
} SPUDGPU_VIEWPORT;

/**
 * @brief Binds an array of dynamic viewport transforms to the pipeline state.
 * * @param[in] cmd             The active recording command context.
 * @param[in] first_viewport  The zero-based index of the first viewport slot to configure.
 * @param[in] viewport_count  Total number of structural viewports to bind from the array.
 * @param[in] viewports       Pointer to a contiguous array of viewport layout definitions.
 */
void spudgpu_set_viewports(
    spudgpu_command_list cmd,
    uint32_t first_viewport,
    uint32_t viewport_vount,
    const SPUDGPU_VIEWPORT *viewports);

/**
 * @brief Defines a dynamic screen space bounding box for rasterization discarding.
 * * Pixels falling outside the scissor rectangle bounds are completely dropped by the
 * rasterizer, optimizing rendering execution for partial UI elements or UI windows.
 */
typedef struct SPUDGPU_SCISSOR_RECT {
    /// Leftmost X coordinate of the scissor bounding box in pixels.
    float x = 0.0f;

    /// Topmost Y coordinate of the scissor bounding box in pixels.
    float y = 0.0f;

    /// Total horizontal layout width of the box in pixels.
    float width = 0.0f;

    /// Total vertical layout height of the box in pixels.
    float height = 0.0f;
} SPUDGPU_SCISSOR_RECT;

/**
 * @brief Binds an array of dynamic scissor clip boundaries to the graphics pipeline.
 * * @param[in] cmd                 The active recording command context.
 * @param[in] first_scissor_rect  The zero-based index of the first scissor slot to configure.
 * @param[in] scissor_rect_count  Total number of scissor regions to update.
 * @param[in] scissor_rects       Pointer to an array of scissor box dimensions.
 */
void spudgpu_set_scissor_rects(
    spudgpu_command_list cmd,
    uint32_t first_scissor_rect,
    uint32_t scissor_rect_count,
    const SPUDGPU_SCISSOR_RECT *scissor_rects);


/**
 * @brief Binds a contiguous array of vertex buffers to target input assembler slots.
 * * @param[in] cmd          The active recording command context.
 * @param[in] start_slot   The starting hardware vertex buffer input slot index to update.
 * @param[in] view_count   Total number of vertex buffer views to map.
 * @param[in] buffer_views Pointer to (0, 1, or an array of) active GPU buffer views containing vertex data.
 */
void spudgpu_set_vertex_buffers(
    spudgpu_command_list cmd,
    uint32_t start_slot,
    uint32_t view_count,
    spudgpu_buffer_view *buffer_views);

/**
 * @brief Binds a contiguous array of index buffers to drive lookups for indexed draw calls.
 * * @param[in] cmd          The active recording command context.
 * @param[in] view_count   Total number of index buffers to assign (typically `1`).
 * @param[in] buffer_views Pointer to (0, 1, or an array of) active GPU buffer views containing index data.
 */
void spudgpu_set_index_buffers(
    spudgpu_command_list cmd,
    uint32_t view_count,
    spudgpu_buffer_view *buffer_views);

/**
 * @brief Non-indexed non-instanced drawing invocation.
 * * Processes sequential vertex data streams bound to input channels directly.
 * * @param[in] cmd                   The active recording command context.
 * @param[in] vertex_count          Total vertices to process for the shape primitive array.
 * @param[in] start_vertex_location Index offset representing the first vertex entry to pull.
 */
void spudgpu_draw(
    spudgpu_command_list cmd,
    uint32_t vertex_count,
    uint32_t start_vertex_location);

/**
 * @brief Indexed non-instanced drawing invocation.
 * * Utilizes an index buffer map to fetch vertices out-of-order, saving vertex storage.
 * * @param[in] cmd                   The active recording command context.
 * @param[in] index_count           The number of indices to read from the active index buffer.
 * @param[in] start_index_location  The element offset inside the index array to begin fetching from.
 * @param[in] base_vertex_location   A value added to every individual index value prior to pulling
 * vertex payload parameters. Effectively handles multi-mesh packaging offsets.
 */
void spudgpu_draw_indexed(
    spudgpu_command_list cmd,
    uint32_t indexCount,
    uint32_t start_index_location,
    uint32_t base_vertex_location);

/**
 * @brief Non-indexed instanced drawing invocation.
 * * Draws identical structural vertex topologies multiple times using hardware instancing.
 * * @param[in] cmd                         The active recording command context.
 * @param[in] vertex_count_per_instance   The number of sequential vertices to draw for an instance item.
 * @param[in] instance_count              Total instances of the mesh layout to render.
 * @param[in] start_vertex_location       The base index offset to look up vertex attributes.
 * @param[in] start_instance_location     An index added to instance identifiers prior to loading
 * per-instance shader attributes.
 */
void spudgpu_draw_instanced(
    spudgpu_command_list cmd,
    uint32_t vertex_count_per_instance,
    uint32_t instance_count,
    uint32_t start_vertex_location,
    uint32_t start_instance_location);

/**
 * @brief Full indexed instanced drawing invocation.
 * * The most optimal, high-throughput path for repeating geometric instances (e.g., foliage, particles).
 * * @param[in] cmd                         The active recording command context.
 * @param[in] index_count_per_instance    The count of indices to process for each item instance.
 * @param[in] instance_count              Total unique geometry clones to invoke.
 * @param[in] start_index_location        Element offset index pointer inside the active index buffer block.
 * @param[in] base_vertex_location        Signed value applied directly to index stream integers before fetching.
 * @param[in] start_instance_location     The tracking offset applied directly to your shader instance counts.
 */
void spudgpu_draw_indexed_instanced(
    spudgpu_command_list cmd,
    uint32_t index_count_per_instance,
    uint32_t instance_count,
    uint32_t start_index_location,
    uint32_t base_vertex_location,
    uint32_t start_instance_location);


/**
 * @brief Dictates the sync relationship between the GPU frame completion and the monitor's refresh cycle.
 * * Governs whether the engine caps frame rates to prevent screen tearing or uses extra back buffers
 * to reduce input latency.
 * * @see SPUDGPU_PRESENT_MODE_Constants
 */
typedef uint32_t SPUDGPU_PRESENT_MODE;

/**
 * @name Presentation Sync Modes
 * @anchor SPUDGPU_PRESENT_MODE_Constants
 * * Configurations mapped to underlying driver swap rules (e.g., VkPresentModeKHR or NSOpenGLContext).
 */
///@{
enum {
    /** * @brief Immediate Mode (V-Sync Disabled).
     * * The GPU transfers completed frames to the screen instantly. Offers the lowest possible
     * input latency but results in visible screen tearing as the monitor splits images mid-refresh.
     */
    SPUDGPU_PRESENT_MODE_IMMEDIATE = 0,

    /** * @brief FIFO Mode (V-Sync Enabled - First In, First Out).
     * * Frames are queued up and synchronized strictly with the display's vertical refresh rate (e.g., 60Hz/144Hz).
     * Completely eliminates screen tearing, but will throttle the CPU/GPU thread if the queue fills up.
     */
    SPUDGPU_PRESENT_MODE_FIFO = 1,

    /** * @brief Mailbox Mode (Triple Buffering / Ultra-Low Latency V-Sync).
     * * Synchronizes with the vertical refresh rate to eliminate tearing, but does not block the application
     * when the queue is full. Instead, the newest completed frame continuously replaces unrendered frames
     * in the queue, ensuring the monitor always pulls the absolute freshest data.
     */
    SPUDGPU_PRESENT_MODE_MAILBOX = 2
};
///@}

/**
 * @brief Configuration descriptor used to initialize a rendering surface target linked to the OS windowing system.
 * * Handled by the driver to allocate a ring buffer of textures (back buffers) that flip onto the screen.
 */
typedef struct spudgpu_swap_chain_desc {
    /** * @brief Opaque generic pointer targeting the host operating system's native window object.
     * * Must capture and forward the platform-appropriate descriptor handle:
     * - **Windows (Win32):** Pass raw `HWND` pointer.
     * - **macOS (AppKit):** Pass native `NSWindow*` or target `CAMetalLayer*`.
     * - **Linux (X11 / Wayland):** Pass raw `Window` identity or `wl_surface*`.
     */
    void *window_handle;

    /// Requested back-buffer width in pixels. Usually matches the window client area width.
    uint32_t width;

    /// Requested back-buffer height in pixels. Usually matches the window client area height.
    uint32_t height;

    /** * @brief Total number of images in the swap chain ring buffer.
     * * Typically configured as `2` for simple Double Buffering or `3` for Mailbox Triple Buffering layouts.
     */
    uint32_t buffer_count;

    /// The color and pixel format schema requested for the surface (e.g., BGRA8_UNORM or RGBA16_SFLOAT).
    SPUDGPU_FORMAT format;

    /// The sync and presentation timing rule to apply when presenting completed frames.
    /// /// @see SPUDGPU_PRESENT_MODE
    SPUDGPU_PRESENT_MODE present_mode;

    /** * @brief Toggles whether the graphics device initializes in exclusive fullscreen monitor mode
     * or stays bounded inside a standard desktop window framework.
     */
    bool fullscreen;
} spudgpu_swap_chain_desc;

/**
 * Creates a new swap chain for rendering to a window.
 * @param device The active spudgpu device.
 * @param desc Pointer to the configuration descriptor.
 * @return A handle to the created swap chain, or NULL if creation failed.
 */
spudgpu_swap_chain spudgpu_create_swap_chain(
    spudgpu_device device,
    const spudgpu_swap_chain_desc *desc);

/**
 * Destroys a swap chain and frees its resources.
 */
void spudgpu_destroy_swap_chain(spudgpu_swap_chain swap_chain);

/**
 * Acquires the index of the next available backbuffer image.
 * This blocks if the GPU is falling too far behind.
 * * @param swap_chain The swap chain to acquire from.
 * @return The index of the next backbuffer (e.g., 0, 1, or 2).
 */
uint32_t spudgpu_swap_chain_acquire_next_image(spudgpu_swap_chain swap_chain);

/**
 * Presents the current backbuffer to the screen.
 * Call this after you have finished recording and submitting commands for the frame.
 * * @param swap_chain The swap chain holding the image to display.
 */
void spudgpu_swap_chain_present(spudgpu_swap_chain swap_chain);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //SPUDLIB_SPUDGPU_H
