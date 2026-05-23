//
// Created by nathanmoore on 5/16/26.
//

#ifndef SPUDLIB_SPUDGPU_H
#define SPUDLIB_SPUDGPU_H

#include <stdbool.h>
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

// Use the return value / 8 for byte size.
// @return Pixel bit size according to the format.
uint32_t spudgpu_format_bit_count(SPUDGPU_FORMAT fmt);

/*
struct spudgpu_format_map {
    const char *name;
    SPUDGPU_FORMAT format;
};

static const struct spudgpu_format_map format_map[] = {
    {(const char *)"R32G32B32A32_FLOAT", SPUDGPU_FORMAT_R32G32B32A32_FLOAT},
};*/
// @return SPUDGPU_FORMAT according to string.
// TODO: Optimize this function. Perhaps make a FormatMap and have this 'static const struct FormatMap FORMAT_TABLE[] = {'
inline SPUDGPU_FORMAT spudgpu_format_from_string(const char *fmt_str) {
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
 * * @return a new instance of SpudGPU if successful.
 * @return NULL if the requested API is unsupported, or driver initialization failed.
 * * @note Must be called before invoking any other SpudGPU API function.
 */
spudgpu_instance spudgpu_init(
    SPUDGPU_NATIVE_API native_api,
    const char *application_name,
    uint32_t application_version,
    const char *engine_name,
    uint32_t engine_version);

/**
 * @brief Shuts down the SpudGPU instance.
 * * Disposes of all internal runtime states, destroys active backend contexts,
 * and releases hooks into the graphics hardware.
 * * @param[in] instance The SpudGPU instance of which to be terminated.
 * * @warning Calling this while resources (like buffers, textures, or pipelines)
 * are still active will result in undefined behavior or memory leaks.
 */
void spudgpu_terminate(spudgpu_instance instance);


/**
 * @brief Retreive an enumeration of the physical graphics devices available on the host machine.
 * * @param[in] instance The SpudGPU instance of which to enumerate devices through.
 * * @param ppOutputDevices[out]
 * * @param pOutputDevicesCount[out]
 * * @return true if all is successful, false if instance is null, or if the native GPU API had an error.
 */
bool spudgpu_enumerate_devices(
    spudgpu_instance instance,
    spudgpu_device **ppOutputDevices,
    uint32_t *pOutputDevicesCount);

/**
 * @brief Retrieves the active graphics API backend for the SpudGPU instance.
 * * @param[in] instance The SpudGPU instance of which to get GPU API.
 * * @return SPUDGPU_API The enum value representing the active backend.
 * Returns `SPUDGPU_API_NONE` if instance is NULL.
 */
SPUDGPU_NATIVE_API spudgpu_get_native_gpu_api(spudgpu_instance instance);


spudgpu_command_queue spudgpu_get_graphics_queue(spudgpu_device device);

void spudgpu_submit_command_lists(
    spudgpu_command_queue queue,
    spudgpu_command_list *cmd_lists,
    uint32_t cmd_list_count);

// Submit command lists with full swap chain synchronization.
// Waits on the swap chain's image_available semaphore,
// signals its render_finished semaphore, and signals the in-flight fence.
// Call this instead of spudgpu_submit_command_lists when rendering to a swap chain.
void spudgpu_submit_command_lists_synced(
    spudgpu_command_queue    queue,
    spudgpu_command_list    *cmd_lists,
    uint32_t                 cmd_list_count,
    spudgpu_swap_chain       swap_chain);

spudgpu_command_allocator spudgpu_create_command_allocator(spudgpu_device device);

void spudgpu_reset_command_allocator(spudgpu_command_allocator allocator);

void spudgpu_destroy_command_allocator(spudgpu_command_allocator allocator);

spudgpu_command_list spudgpu_create_command_list(spudgpu_command_allocator allocator);

void spudgpu_destroy_command_list(spudgpu_command_list cmd);


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
 * @brief Retrieves the immutable configuration descriptor of an active buffer view.
 * * @param[in] view The GPU buffer view to get the descriptor of.
 * @return spudgpu_buffer_view_desc A copy of the view descriptor structure.
 */
spudgpu_buffer_view_desc spudgpu_get_buffer_view_desc(spudgpu_buffer_view view);

/**
 * @brief Destroys a GPU buffer view.
 * * Frees the virtual view mapping context. Does **not** impact the lifecycle or data
 * retention of the underlying parent GPU buffer.
 * @param[in] buffer The target GPU buffer view to destroy.
 */
void spudgpu_destroy_buffer_view(spudgpu_buffer_view buffer);

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
 * @return true on success, false if buffer is NULL or not HOST_VISIBLE.
 */
bool spudgpu_map_buffer(
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
 * @brief Invalidates CPU caches for a mapped buffer range before reading GPU-written data.
 *
 * Only required for HOST_CACHED readback buffers.
 * Equivalent to vkInvalidateMappedMemoryRanges().
 *
 * @param buffer  The buffer to invalidate.
 * @param offset  Byte offset of the region to invalidate.
 * @param size    Byte length of the region. Pass 0 for entire buffer.
 */
void spudgpu_invalidate_buffer(
    spudgpu_buffer buffer,
    uint64_t offset,
    uint64_t size);


typedef uint32_t SPUDGPU_IMAGE_TYPE;

enum {
    SPUDGPU_IMAGE_TYPE_NONE = 0,
    SPUDGPU_IMAGE_TYPE_1D = 1 << 0,
    SPUDGPU_IMAGE_TYPE_2D = 1 << 1,
    SPUDGPU_IMAGE_TYPE_3D = 1 << 2
    //SPUDGPU_IMAGE_TYPE_CUBE = 1 << 3
};

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

    SPUDGPU_IMAGE_USAGE_SAMPLED = 1 << 0,

    SPUDGPU_IMAGE_USAGE_COLOR_ATTACHMENT = 1 << 1,
    SPUDGPU_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 1 << 2,
    SPUDGPU_IMAGE_USAGE_STORAGE = 1 << 3,
    SPUDGPU_IMAGE_USAGE_TRANSFER_SRC = 1 << 4,
    SPUDGPU_IMAGE_USAGE_TRANSFER_DST = 1 << 5
};

///@}

/**
 * @brief Configuration descriptor used to allocate a physical GPU image resource (texture/surface).
 */
typedef struct spudgpu_image_desc {
    /// Bitmask configuration specifying texture layout type (2D, 3D, etc.).
    /// @see SPUDGPU_IMAGE_USAGE
    uint32_t usage;

    uint32_t type;

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
    const char *debug_name;
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
 * @param[in] device The GPU device that originally allocated this resource.
 * @param[in] image  The target GPU image to destroy.
 * * @warning Any active command streams, image views, descriptors, or render passes currently
 * binding or writing to this texture must finish execution on the GPU timeline before invocation.
 */
void spudgpu_destroy_image(spudgpu_device device, spudgpu_image image);


typedef uint32_t SPUDGPU_IMAGE_VIEW_TYPE;

enum {
    SPUDGPU_IMAGE_VIEW_TYPE_NONE = 0,
    SPUDGPU_IMAGE_VIEW_TYPE_1D = 1 << 0,
    SPUDGPU_IMAGE_VIEW_TYPE_2D = 1 << 1,
    SPUDGPU_IMAGE_VIEW_TYPE_3D = 1 << 2,
    SPUDGPU_IMAGE_VIEW_TYPE_CUBE = 1 << 3,
    SPUDGPU_IMAGE_VIEW_TYPE_1D_ARRAY = 1 << 4,
    SPUDGPU_IMAGE_VIEW_TYPE_2D_ARRAY = 1 << 5,
    SPUDGPU_IMAGE_VIEW_TYPE_CUBE_ARRAY = 1 << 6
};

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

    /// The image view type
    /// @see SPUDGPU_IMAGE_VIEW_TYPE
    uint32_t type;

    // TODO: Swizzle indentities? Learn about that

    /// The isolated subresource layer and mip boundary definitions for this view.
    spudgpu_image_view_desc_subresource_range subresource_range;
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
 * @param[in] image_view  The GPU image view to destroy.
 */
void spudgpu_destroy_image_view(spudgpu_device device, spudgpu_image_view image_view);

/**
 * @brief Retrieves the configuration descriptor of the GPU image view.
 * * @param[in] image_view The GPU image view handle to query.
 * @return spudgpu_image_view_desc A copy of the GPU image view configuration descriptor structure.
 */
spudgpu_image_view_desc spudgpu_get_image_view_desc(spudgpu_image_view image_view);

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
    float x;

    /// Y-coordinate of the upper-left corner of the viewport region in pixels.
    float y;

    /// Total width of the targeted viewport frame in pixels.
    float width;

    /// Total height of the targeted viewport frame in pixels.
    float height;

    /// Minimum depth boundary slice. Usually maps to `0.0f` (near clipping plane).
    float minDepth;

    /// Maximum depth boundary slice. Usually maps to `1.0f` (far clipping plane).
    float maxDepth;
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
    uint32_t viewport_count,
    const SPUDGPU_VIEWPORT *viewports);

/**
 * @brief Defines a dynamic screen space bounding box for rasterization discarding.
 * * Pixels falling outside the scissor rectangle bounds are completely dropped by the
 * rasterizer, optimizing rendering execution for partial UI elements or UI windows.
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
    int32_t base_vertex_location);

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
    int32_t base_vertex_location,
    uint32_t start_instance_location);



typedef struct spudgpu_surface_t *spudgpu_surface;


/** window_handle Opaque generic pointer targeting the host operating system's native window object.
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
spudgpu_surface spudgpu_create_surface(spudgpu_instance instance, void *window_handle, void *display_handle);

void spudgpu_destroy_surface(spudgpu_surface surface);

typedef bool (*spudgpu_surface_create_fn)(
    void *vk_instance,   // VkInstance, typed as void* to keep spudgpu.h Vulkan-free
    void *user_data,
    void *out_surface);  // VkSurfaceKHR*, typed as void*

spudgpu_surface spudgpu_create_surface_from_callback(
    spudgpu_instance instance,
    void *user_data,
    spudgpu_surface_create_fn create_fn);


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
    spudgpu_surface surface;

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

spudgpu_swap_chain_desc spudgpu_get_swap_chain_desc(spudgpu_swap_chain swap_chain);

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

/**
 * @brief Returns the image view for the given swap chain image index.
 * Use the index returned by vkAcquireNextImageKHR (or your equivalent).
 */
spudgpu_image_view spudgpu_get_swap_chain_image_view(
    spudgpu_swap_chain swap_chain,
    uint32_t image_index);


typedef uint32_t SPUDGPU_SHADER_STAGE;

enum {
    SPUDGPU_SHADER_STAGE_NONE = 0,
    SPUDGPU_SHADER_STAGE_VERTEX = 1 << 0,
    SPUDGPU_SHADER_STAGE_FRAGMENT = 1 << 1,
    SPUDGPU_SHADER_STAGE_COMPUTE = 1 << 2,
    SPUDGPU_SHADER_STAGE_GEOMETRY = 1 << 3,
    SPUDGPU_SHADER_STAGE_TESSELLATION_CONTROL = 1 << 4,
    SPUDGPU_SHADER_STAGE_TESSELLATION_EVALUATION = 1 << 5
};

typedef uint32_t SPUDGPU_PRIMITIVE_TOPOLOGY;

enum {
    SPUDGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 0,
    SPUDGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 1,
    SPUDGPU_PRIMITIVE_TOPOLOGY_LINE_LIST = 2,
    SPUDGPU_PRIMITIVE_TOPOLOGY_LINE_STRIP = 3,
    SPUDGPU_PRIMITIVE_TOPOLOGY_POINT_LIST = 4,
    SPUDGPU_PRIMITIVE_TOPOLOGY_PATCH_LIST = 5 // Tessellation
};


typedef uint32_t SPUDGPU_CULL_MODE;

enum {
    SPUDGPU_CULL_MODE_NONE = 0,
    SPUDGPU_CULL_MODE_FRONT = 1,
    SPUDGPU_CULL_MODE_BACK = 2
};


// Depth Compare Op
typedef uint32_t SPUDGPU_COMPARE_OP;

enum {
    SPUDGPU_COMPARE_OP_NEVER = 0,
    SPUDGPU_COMPARE_OP_LESS = 1,
    SPUDGPU_COMPARE_OP_EQUAL = 2,
    SPUDGPU_COMPARE_OP_LESS_OR_EQUAL = 3,
    SPUDGPU_COMPARE_OP_GREATER = 4,
    SPUDGPU_COMPARE_OP_NOT_EQUAL = 5,
    SPUDGPU_COMPARE_OP_GREATER_OR_EQUAL = 6,
    SPUDGPU_COMPARE_OP_ALWAYS = 7
};

// Capacity Limits
#define SPUDGPU_MAX_VERTEX_ATTRIBUTES      16
#define SPUDGPU_MAX_VERTEX_BINDINGS         8
#define SPUDGPU_MAX_PUSH_CONSTANT_RANGES    4
#define SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS  4

typedef struct spudgpu_vertex_attribute_desc {
    /// Which shader location slot this attribute binds to (layout(location = N)).
    uint32_t location;

    /// Which vertex buffer binding slot this attribute is sourced from.
    uint32_t binding;

    /// The data layout and channel bit-depth of this attribute (e.g., SPUDGPU_FORMAT_R32G32B32_FLOAT).
    SPUDGPU_FORMAT format;

    /// Byte offset of this attribute from the start of a single vertex element.
    uint32_t offset;
} spudgpu_vertex_attribute_desc;

typedef struct spudgpu_vertex_binding_desc {
    /// The binding slot index this entry targets.
    uint32_t binding;

    /// Byte distance between consecutive elements in the buffer (sizeof your vertex struct).
    uint32_t stride;

    /// When true, advances per-instance rather than per-vertex (instanced rendering).
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
    SPUDGPU_BLEND_FACTOR_ZERO = 0,
    SPUDGPU_BLEND_FACTOR_ONE = 1,
    SPUDGPU_BLEND_FACTOR_SRC_ALPHA = 2,
    SPUDGPU_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA = 3,
    SPUDGPU_BLEND_FACTOR_DST_ALPHA = 4,
    SPUDGPU_BLEND_FACTOR_ONE_MINUS_DST_ALPHA = 5,
    SPUDGPU_BLEND_FACTOR_SRC_COLOR = 6,
    SPUDGPU_BLEND_FACTOR_ONE_MINUS_SRC_COLOR = 7,
    SPUDGPU_BLEND_FACTOR_DST_COLOR = 8,
    SPUDGPU_BLEND_FACTOR_ONE_MINUS_DST_COLOR = 9
};

typedef uint32_t SPUDGPU_BLEND_OP;

enum {
    SPUDGPU_BLEND_OP_ADD = 0,
    SPUDGPU_BLEND_OP_SUBTRACT = 1,
    SPUDGPU_BLEND_OP_REVERSE_SUBTRACT = 2,
    SPUDGPU_BLEND_OP_MIN = 3,
    SPUDGPU_BLEND_OP_MAX = 4
};

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

spudgpu_shader_module spudgpu_create_shader_module(
    spudgpu_device device,
    const spudgpu_shader_module_desc *desc);

void spudgpu_destroy_shader_module(spudgpu_device device, spudgpu_shader_module shader_module);

/**
 * @brief Complete configuration descriptor for creating a graphics shader pipeline.
 *
 * Fill in the stage modules you need, leave optional ones NULL.
 * All arrays are inline and fixed-capacity; use the corresponding _count
 * field to indicate how many entries are valid.
 */
typedef struct spudgpu_shader_pipeline_desc {
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

    /// Compiled tessellation control shader module. Optional — leave NULL to skip.
    spudgpu_shader_module tess_control_module;
    const char *tess_control_entry_point;

    /// Compiled tessellation evaluation shader module. Optional — leave NULL to skip.
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

    /// When true, triangles with counter-clockwise winding are treated as front-facing.
    bool front_face_ccw;

    /// When true, geometry is rasterized as wireframe lines instead of filled triangles.
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

    /// Pixel format of the depth attachment. Set to SPUDGPU_FORMAT_UNKNOWN for no depth.
    /// @see SPUDGPU_FORMAT
    SPUDGPU_FORMAT depth_format;

    // -----------------------------------------------------------------------
    // Pipeline layout
    // -----------------------------------------------------------------------

    /// Opaque descriptor set layout handles. Cast to VkDescriptorSetLayout* internally.
    void *descriptor_set_layouts[SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS];
    uint32_t descriptor_set_layout_count;

    spudgpu_push_constant_range_desc push_constant_ranges[SPUDGPU_MAX_PUSH_CONSTANT_RANGES];
    uint32_t push_constant_range_count;

    // -----------------------------------------------------------------------
    // Tessellation
    // -----------------------------------------------------------------------

    /// Number of control points per patch. Only used when both tess stages are present.
    /// Defaults to 3 when set to 0.
    uint32_t patch_control_points;

#if _DEBUG
    /// @brief A string identifier used for diagnostic tracking.
    const char *debug_name;
#endif
} spudgpu_shader_pipeline_desc;


spudgpu_shader_pipeline spudgpu_create_shader_pipeline(
    spudgpu_device device,
    const spudgpu_shader_pipeline_desc *desc);

void spudgpu_destroy_shader_pipeline(spudgpu_device device, spudgpu_shader_pipeline pipeline);

void spudgpu_cmd_bind_pipeline(
    spudgpu_command_list cmd,
    spudgpu_shader_pipeline pipeline);

/**
 * @brief Complete configuration descriptor for creating a compute shader pipeline.
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

    /// Opaque descriptor set layout handles. Cast to VkDescriptorSetLayout* internally.
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
 *
 * @return spudgpu_compute_pipeline A valid handle on success, or NULL if
 * the module is missing, the layout is invalid, or SPIR-V compilation fails.
 */
spudgpu_compute_pipeline spudgpu_create_compute_pipeline(
    spudgpu_device device,
    const spudgpu_compute_pipeline_desc *desc);

/**
 * @brief Destroys a compute pipeline and frees all associated driver resources.
 *
 * @param[in] device   The GPU device that originally created this pipeline.
 * @param[in] pipeline The compute pipeline to destroy.
 *
 * @warning Any command lists currently recording dispatches against this
 * pipeline must have finished execution on the GPU timeline before calling this.
 */
void spudgpu_destroy_compute_pipeline(
    spudgpu_device device,
    spudgpu_compute_pipeline pipeline);


// ============================================================================
//  Descriptor Set Layout
//  Maps to: VkDescriptorSetLayout (Vulkan) / MTLArgumentEncoder schema (Metal)
// ============================================================================

typedef struct spudgpu_descriptor_set_layout_t *spudgpu_descriptor_set_layout;

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

/**
 * @brief Describes a single binding slot within a descriptor set layout.
 *
 * Each entry corresponds to one `layout(set=N, binding=M)` declaration in GLSL.
 */
typedef struct spudgpu_descriptor_binding_desc {
    /// Slot index matching `layout(binding = N)` in GLSL. Must be unique within a layout.
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

#define SPUDGPU_MAX_DESCRIPTOR_BINDINGS_PER_SET  16

/**
 * @brief Configuration descriptor for a descriptor set layout.
 *
 * Describes the binding slots that make up one set. Pass this to
 * spudgpu_create_descriptor_set_layout(), then hand the resulting
 * handle into spudgpu_shader_pipeline_desc::descriptor_set_layouts[].
 */
typedef struct spudgpu_descriptor_set_layout_desc {
    spudgpu_descriptor_binding_desc bindings[SPUDGPU_MAX_DESCRIPTOR_BINDINGS_PER_SET];
    uint32_t binding_count;

#if _DEBUG
    const char *debug_name;
#endif
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
 * @return A valid handle, or NULL on failure.
 */
spudgpu_descriptor_set_layout spudgpu_create_descriptor_set_layout(
    spudgpu_device device,
    const spudgpu_descriptor_set_layout_desc *desc);

/**
 * @brief Destroys a descriptor set layout.
 *
 * @warning All descriptor sets allocated from this layout, and all pipelines
 * referencing it, must be destroyed before calling this.
 */
void spudgpu_destroy_descriptor_set_layout(
    spudgpu_device device,
    spudgpu_descriptor_set_layout layout);


// ============================================================================
//  Descriptor Pool
//  Maps to: VkDescriptorPool (Vulkan) / heap of argument buffers (Metal)
// ============================================================================

typedef struct spudgpu_descriptor_pool_t *spudgpu_descriptor_pool;

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

#define SPUDGPU_MAX_DESCRIPTOR_POOL_SIZES  8

/**
 * @brief Configuration descriptor for a descriptor pool.
 */
typedef struct spudgpu_descriptor_pool_desc {
    /// Maximum number of descriptor sets that can be allocated from this pool.
    uint32_t max_sets;

    spudgpu_descriptor_pool_size pool_sizes[SPUDGPU_MAX_DESCRIPTOR_POOL_SIZES];
    uint32_t pool_size_count;

#if _DEBUG
    const char *debug_name;
#endif
} spudgpu_descriptor_pool_desc;

/**
 * @brief Allocates a descriptor pool — the backing memory for descriptor sets.
 *
 * Create one pool per frame-in-flight (or one large shared pool) and reset it
 * each frame rather than allocating/freeing individual sets every frame.
 *
 * @param[in] device The GPU device to create this pool on.
 * @param[in] desc   Pointer to the capacity configuration.
 * @return A valid handle, or NULL on failure.
 */
spudgpu_descriptor_pool spudgpu_create_descriptor_pool(
    spudgpu_device device,
    const spudgpu_descriptor_pool_desc *desc);

/**
 * @brief Resets the pool, freeing all sets allocated from it in bulk.
 *
 * Cheaper than freeing sets individually. Call once per frame before
 * re-recording new descriptor writes.
 *
 * @param[in] device The GPU device that owns this pool.
 * @param[in] pool   The pool to reset.
 */
void spudgpu_reset_descriptor_pool(
    spudgpu_device device,
    spudgpu_descriptor_pool pool);

/**
 * @brief Destroys a descriptor pool and all sets allocated from it.
 *
 * @warning All command lists currently using sets from this pool must have
 * finished GPU execution before calling this.
 */
void spudgpu_destroy_descriptor_pool(
    spudgpu_device device,
    spudgpu_descriptor_pool pool);


// ============================================================================
//  Descriptor Set
//  Maps to: VkDescriptorSet (Vulkan) / MTLBuffer argument buffer (Metal)
// ============================================================================

typedef struct spudgpu_descriptor_set_t *spudgpu_descriptor_set;

/**
 * @brief Allocation descriptor for one or more descriptor sets.
 *
 * All sets in a single call are allocated from the same pool in one
 * driver round-trip (matches vkAllocateDescriptorSets semantics).
 */
typedef struct spudgpu_descriptor_set_alloc_desc {
    spudgpu_descriptor_pool pool;

    /// Each element describes the layout for one set being allocated.
    spudgpu_descriptor_set_layout layouts[SPUDGPU_MAX_DESCRIPTOR_SET_LAYOUTS];
    uint32_t set_count;
} spudgpu_descriptor_set_alloc_desc;

/**
 * @brief Allocates descriptor sets from a pool.
 *
 * @param[in]  device    The GPU device that owns the pool.
 * @param[in]  desc      Allocation configuration (pool + layouts).
 * @param[out] out_sets  Caller-supplied array that receives the allocated handles.
 *                       Must be at least desc->set_count elements wide.
 * @return true on success; false if the pool is out of capacity.
 */
bool spudgpu_allocate_descriptor_sets(
    spudgpu_device device,
    const spudgpu_descriptor_set_alloc_desc *desc,
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
 * @brief A single write operation targeting one binding slot in a descriptor set.
 *
 * Fill either buffer_info or image_info depending on the descriptor_type.
 * The unused field is ignored by the backend.
 */
typedef struct spudgpu_write_descriptor_set {
    /// The descriptor set to write into.
    spudgpu_descriptor_set dst_set;

    /// The binding slot index to update (matches spudgpu_descriptor_binding_desc::binding).
    uint32_t dst_binding;

    /// First array element to update. Use 0 for non-array bindings.
    uint32_t dst_array_element;

    /// Number of descriptors to update starting at dst_array_element.
    uint32_t descriptor_count;

    /// The type of descriptor being written. Must match the layout's declared type.
    SPUDGPU_DESCRIPTOR_TYPE descriptor_type;

    /// Set when writing UNIFORM_BUFFER or STORAGE_BUFFER descriptors.
    const spudgpu_descriptor_buffer_info *buffer_info;

    /// Set when writing SAMPLED_IMAGE, STORAGE_IMAGE, or COMBINED_IMAGE_SAMPLER descriptors.
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
 * @brief Binds descriptor sets to the pipeline for subsequent draw or dispatch calls.
 *
 * Maps to vkCmdBindDescriptorSets. Call after binding the pipeline and before
 * the draw/dispatch.
 *
 * @param[in] cmd          The active command list.
 * @param[in] pipeline     The graphics pipeline whose layout defines the set slots.
 * @param[in] first_set    The set index of the first element in sets[] (usually 0).
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
//  Image Layout
//  Maps to: VkImageLayout (Vulkan)
// ============================================================================

typedef uint32_t SPUDGPU_IMAGE_LAYOUT;

enum {
    SPUDGPU_IMAGE_LAYOUT_UNDEFINED = 0,
    SPUDGPU_IMAGE_LAYOUT_GENERAL = 1,
    SPUDGPU_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2,
    SPUDGPU_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
    SPUDGPU_IMAGE_LAYOUT_SHADER_READ_ONLY = 4,
    SPUDGPU_IMAGE_LAYOUT_TRANSFER_SRC = 5,
    SPUDGPU_IMAGE_LAYOUT_TRANSFER_DST = 6,
    SPUDGPU_IMAGE_LAYOUT_PRESENT_SRC = 7,
};


// ============================================================================
//  Image Barrier
//  Maps to: vkCmdPipelineBarrier (Vulkan)
// ============================================================================

/**
 * @brief Transitions an image from one layout to another, inserting the
 * necessary pipeline barrier so dependent stages wait correctly.
 *
 * Call before spudgpu_cmd_begin_render_pass to move a swap chain image from
 * UNDEFINED/PRESENT_SRC to COLOR_ATTACHMENT_OPTIMAL, and after
 * spudgpu_cmd_end_render_pass to move it back to PRESENT_SRC.
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
 * Useful for swap chain images, which are not created through spudgpu_create_image
 * and so don't have a spudgpu_image handle. The image view must have been
 * created from the target image (e.g. a swap chain image view).
 *
 * @param[in] cmd          The active recording command list.
 * @param[in] image_view   An image view whose parent image will be transitioned.
 * @param[in] old_layout   The current layout of the image.
 * @param[in] new_layout   The target layout to transition into.
 */
void spudgpu_cmd_image_barrier_view(
    spudgpu_command_list cmd,
    spudgpu_image_view image_view,
    SPUDGPU_IMAGE_LAYOUT old_layout,
    SPUDGPU_IMAGE_LAYOUT new_layout);


// ============================================================================
//  Render Pass Begin / End
//  Maps to: vkCmdBeginRenderPass / vkCmdEndRenderPass (Vulkan)
// ============================================================================

/**
 * @brief Describes the render targets and clear values for one render pass.
 */
typedef struct spudgpu_render_pass_begin_desc {
    /**
     * @brief The image view to render into (e.g. a swap chain image view).
     *
     * On Vulkan, this is used to create (or look up a cached) VkFramebuffer
     * that wraps this view for the pipeline's VkRenderPass.
     */
    spudgpu_image_view color_attachment;

    /**
     * @brief Optional depth/stencil attachment. Set to NULL for no depth.
     */
    spudgpu_image_view depth_attachment;

    /**
     * @brief The pipeline whose VkRenderPass compatibility this pass must match.
     *
     * SpudGPU uses the VkRenderPass stored in the pipeline to begin the pass.
     * The color and depth attachment formats must match what the pipeline
     * was created with.
     */
    spudgpu_shader_pipeline pipeline;

    /// RGBA clear color applied when the color attachment loadOp is CLEAR.
    float clear_color[4];

    /// Clear value for the depth buffer (typically 1.0f).
    float clear_depth;

    /// Clear value for the stencil buffer (typically 0).
    uint32_t clear_stencil;

    /// Render area width in pixels. Usually the swap chain width.
    uint32_t width;

    /// Render area height in pixels. Usually the swap chain height.
    uint32_t height;
} spudgpu_render_pass_begin_desc;

/**
 * @brief Begins a render pass, binding the color (and optional depth)
 * attachments and issuing the configured clear operations.
 *
 * Must be matched with spudgpu_cmd_end_render_pass before submitting.
 *
 * @param[in] cmd  The active recording command list.
 * @param[in] desc Render pass configuration and attachment handles.
 */
void spudgpu_cmd_begin_render_pass(
    spudgpu_command_list cmd,
    const spudgpu_render_pass_begin_desc *desc);

/**
 * @brief Ends the current render pass.
 *
 * @param[in] cmd The active recording command list.
 */
void spudgpu_cmd_end_render_pass(spudgpu_command_list cmd);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif //SPUDLIB_SPUDGPU_H
