//
// Created by nathanmoore on 3/27/26.
//

#ifndef SPUDLIB_SPUDGPUFORMATS_HPP
#define SPUDLIB_SPUDGPUFORMATS_HPP

#include <cstdint>
#include <string>

namespace spud::gpu {
    // Format for textures and buffers, copied from DirectX API DXGI_FORMAT
    enum class SPUDGPU_FORMAT {
        _UNKNOWN = 0,
        _R32G32B32A32_TYPELESS = 1,
        _R32G32B32A32_FLOAT = 2,
        _R32G32B32A32_UINT = 3,
        _R32G32B32A32_SINT = 4,
        _R32G32B32_TYPELESS = 5,
        _R32G32B32_FLOAT = 6,
        _R32G32B32_UINT = 7,
        _R32G32B32_SINT = 8,
        _R16G16B16A16_TYPELESS = 9,
        _R16G16B16A16_FLOAT = 10,
        _R16G16B16A16_UNORM = 11,
        _R16G16B16A16_UINT = 12,
        _R16G16B16A16_SNORM = 13,
        _R16G16B16A16_SINT = 14,
        _R32G32_TYPELESS = 15,
        _R32G32_FLOAT = 16,
        _R32G32_UINT = 17,
        _R32G32_SINT = 18,
        _R32G8X24_TYPELESS = 19,
        _D32_FLOAT_S8X24_UINT = 20,
        _R32_FLOAT_X8X24_TYPELESS = 21,
        _X32_TYPELESS_G8X24_UINT = 22,
        _R10G10B10A2_TYPELESS = 23,
        _R10G10B10A2_UNORM = 24,
        _R10G10B10A2_UINT = 25,
        _R11G11B10_FLOAT = 26,
        _R8G8B8A8_TYPELESS = 27,
        _R8G8B8A8_UNORM = 28,
        _R8G8B8A8_UNORM_SRGB = 29,
        _R8G8B8A8_UINT = 30,
        _R8G8B8A8_SNORM = 31,
        _R8G8B8A8_SINT = 32,
        _R16G16_TYPELESS = 33,
        _R16G16_FLOAT = 34,
        _R16G16_UNORM = 35,
        _R16G16_UINT = 36,
        _R16G16_SNORM = 37,
        _R16G16_SINT = 38,
        _R32_TYPELESS = 39,
        _D32_FLOAT = 40,
        _R32_FLOAT = 41,
        _R32_UINT = 42,
        _R32_SINT = 43,
        _R24G8_TYPELESS = 44,
        _D24_UNORM_S8_UINT = 45,
        _R24_UNORM_X8_TYPELESS = 46,
        _X24_TYPELESS_G8_UINT = 47,
        _R8G8_TYPELESS = 48,
        _R8G8_UNORM = 49,
        _R8G8_UINT = 50,
        _R8G8_SNORM = 51,
        _R8G8_SINT = 52,
        _R16_TYPELESS = 53,
        _R16_FLOAT = 54,
        _D16_UNORM = 55,
        _R16_UNORM = 56,
        _R16_UINT = 57,
        _R16_SNORM = 58,
        _R16_SINT = 59,
        _R8_TYPELESS = 60,
        _R8_UNORM = 61,
        _R8_UINT = 62,
        _R8_SNORM = 63,
        _R8_SINT = 64,
        _A8_UNORM = 65,
        _R1_UNORM = 66,
        _R9G9B9E5_SHAREDEXP = 67,
        _R8G8_B8G8_UNORM = 68,
        _G8R8_G8B8_UNORM = 69,
        _BC1_TYPELESS = 70,
        _BC1_UNORM = 71,
        _BC1_UNORM_SRGB = 72,
        _BC2_TYPELESS = 73,
        _BC2_UNORM = 74,
        _BC2_UNORM_SRGB = 75,
        _BC3_TYPELESS = 76,
        _BC3_UNORM = 77,
        _BC3_UNORM_SRGB = 78,
        _BC4_TYPELESS = 79,
        _BC4_UNORM = 80,
        _BC4_SNORM = 81,
        _BC5_TYPELESS = 82,
        _BC5_UNORM = 83,
        _BC5_SNORM = 84,
        _B5G6R5_UNORM = 85,
        _B5G5R5A1_UNORM = 86,
        _B8G8R8A8_UNORM = 87,
        _B8G8R8X8_UNORM = 88,
        _R10G10B10_XR_BIAS_A2_UNORM = 89,
        _B8G8R8A8_TYPELESS = 90,
        _B8G8R8A8_UNORM_SRGB = 91,
        _B8G8R8X8_TYPELESS = 92,
        _B8G8R8X8_UNORM_SRGB = 93,
        _BC6H_TYPELESS = 94,
        _BC6H_UF16 = 95,
        _BC6H_SF16 = 96,
        _BC7_TYPELESS = 97,
        _BC7_UNORM = 98,
        _BC7_UNORM_SRGB = 99,
        _AYUV = 100,
        _Y410 = 101,
        _Y416 = 102,
        _NV12 = 103,
        _P010 = 104,
        _P016 = 105,
        _420_OPAQUE = 106,
        _YUY2 = 107,
        _Y210 = 108,
        _Y216 = 109,
        _NV11 = 110,
        _AI44 = 111,
        _IA44 = 112,
        _P8 = 113,
        _A8P8 = 114,
        _B4G4R4A4_UNORM = 115,

        _P208 = 130,
        _V208 = 131,
        _V408 = 132,


        _FORCE_UINT = -1 //0xffffffff
    };


    // Use the return value / 8 for byte size.
    // @return Pixel bit size according to the format.
    uint32_t format_bit_count(const SPUDGPU_FORMAT &fmt);

    // @return SPUDGPU_FORMAT according to string.
    SPUDGPU_FORMAT gpu_format_from_string(const std::string &fmtString);
}

#endif //SPUDLIB_SPUDGPUFORMATS_HPP
