#include "gpu/spudgpuformats.hpp"

namespace spud::gpu {
    uint32_t format_bit_count(SPUDGPU_FORMAT fmt) {
        switch (fmt) {
            case SPUDGPU_FORMAT::_R32G32B32A32_TYPELESS:
            case SPUDGPU_FORMAT::_R32G32B32A32_FLOAT:
            case SPUDGPU_FORMAT::_R32G32B32A32_UINT:
            case SPUDGPU_FORMAT::_R32G32B32A32_SINT:
                return 128;

            case SPUDGPU_FORMAT::_R32G32B32_TYPELESS:
            case SPUDGPU_FORMAT::_R32G32B32_FLOAT:
            case SPUDGPU_FORMAT::_R32G32B32_UINT:
            case SPUDGPU_FORMAT::_R32G32B32_SINT:
                return 96;

            case SPUDGPU_FORMAT::_R16G16B16A16_TYPELESS:
            case SPUDGPU_FORMAT::_R16G16B16A16_FLOAT:
            case SPUDGPU_FORMAT::_R16G16B16A16_UNORM:
            case SPUDGPU_FORMAT::_R16G16B16A16_UINT:
            case SPUDGPU_FORMAT::_R16G16B16A16_SNORM:
            case SPUDGPU_FORMAT::_R16G16B16A16_SINT:
            case SPUDGPU_FORMAT::_R32G32_TYPELESS:
            case SPUDGPU_FORMAT::_R32G32_FLOAT:
            case SPUDGPU_FORMAT::_R32G32_UINT:
            case SPUDGPU_FORMAT::_R32G32_SINT:
            case SPUDGPU_FORMAT::_R32G8X24_TYPELESS:
            case SPUDGPU_FORMAT::_D32_FLOAT_S8X24_UINT:
            case SPUDGPU_FORMAT::_R32_FLOAT_X8X24_TYPELESS:
            case SPUDGPU_FORMAT::_X32_TYPELESS_G8X24_UINT:
            case SPUDGPU_FORMAT::_Y416:
            case SPUDGPU_FORMAT::_Y210:
            case SPUDGPU_FORMAT::_Y216:
                return 64;

            case SPUDGPU_FORMAT::_R10G10B10A2_TYPELESS:
            case SPUDGPU_FORMAT::_R10G10B10A2_UNORM:
            case SPUDGPU_FORMAT::_R10G10B10A2_UINT:
            case SPUDGPU_FORMAT::_R11G11B10_FLOAT:
            case SPUDGPU_FORMAT::_R8G8B8A8_TYPELESS:
            case SPUDGPU_FORMAT::_R8G8B8A8_UNORM:
            case SPUDGPU_FORMAT::_R8G8B8A8_UNORM_SRGB:
            case SPUDGPU_FORMAT::_R8G8B8A8_UINT:
            case SPUDGPU_FORMAT::_R8G8B8A8_SNORM:
            case SPUDGPU_FORMAT::_R8G8B8A8_SINT:
            case SPUDGPU_FORMAT::_R16G16_TYPELESS:
            case SPUDGPU_FORMAT::_R16G16_FLOAT:
            case SPUDGPU_FORMAT::_R16G16_UNORM:
            case SPUDGPU_FORMAT::_R16G16_UINT:
            case SPUDGPU_FORMAT::_R16G16_SNORM:
            case SPUDGPU_FORMAT::_R16G16_SINT:
            case SPUDGPU_FORMAT::_R32_TYPELESS:
            case SPUDGPU_FORMAT::_D32_FLOAT:
            case SPUDGPU_FORMAT::_R32_FLOAT:
            case SPUDGPU_FORMAT::_R32_UINT:
            case SPUDGPU_FORMAT::_R32_SINT:
            case SPUDGPU_FORMAT::_R24G8_TYPELESS:
            case SPUDGPU_FORMAT::_D24_UNORM_S8_UINT:
            case SPUDGPU_FORMAT::_R24_UNORM_X8_TYPELESS:
            case SPUDGPU_FORMAT::_X24_TYPELESS_G8_UINT:
            case SPUDGPU_FORMAT::_R9G9B9E5_SHAREDEXP:
            case SPUDGPU_FORMAT::_R8G8_B8G8_UNORM:
            case SPUDGPU_FORMAT::_G8R8_G8B8_UNORM:
            case SPUDGPU_FORMAT::_B8G8R8A8_UNORM:
            case SPUDGPU_FORMAT::_B8G8R8X8_UNORM:
            case SPUDGPU_FORMAT::_R10G10B10_XR_BIAS_A2_UNORM:
            case SPUDGPU_FORMAT::_B8G8R8A8_TYPELESS:
            case SPUDGPU_FORMAT::_B8G8R8A8_UNORM_SRGB:
            case SPUDGPU_FORMAT::_B8G8R8X8_TYPELESS:
            case SPUDGPU_FORMAT::_B8G8R8X8_UNORM_SRGB:
            case SPUDGPU_FORMAT::_AYUV:
            case SPUDGPU_FORMAT::_Y410:
            case SPUDGPU_FORMAT::_YUY2:
                return 32;

            case SPUDGPU_FORMAT::_P010:
            case SPUDGPU_FORMAT::_P016:
                return 24;

            case SPUDGPU_FORMAT::_R8G8_TYPELESS:
            case SPUDGPU_FORMAT::_R8G8_UNORM:
            case SPUDGPU_FORMAT::_R8G8_UINT:
            case SPUDGPU_FORMAT::_R8G8_SNORM:
            case SPUDGPU_FORMAT::_R8G8_SINT:
            case SPUDGPU_FORMAT::_R16_TYPELESS:
            case SPUDGPU_FORMAT::_R16_FLOAT:
            case SPUDGPU_FORMAT::_D16_UNORM:
            case SPUDGPU_FORMAT::_R16_UNORM:
            case SPUDGPU_FORMAT::_R16_UINT:
            case SPUDGPU_FORMAT::_R16_SNORM:
            case SPUDGPU_FORMAT::_R16_SINT:
            case SPUDGPU_FORMAT::_B5G6R5_UNORM:
            case SPUDGPU_FORMAT::_B5G5R5A1_UNORM:
            case SPUDGPU_FORMAT::_A8P8:
            case SPUDGPU_FORMAT::_B4G4R4A4_UNORM:
                return 16;

            case SPUDGPU_FORMAT::_NV12:
            case SPUDGPU_FORMAT::_420_OPAQUE:
            case SPUDGPU_FORMAT::_NV11:
                return 12;

            case SPUDGPU_FORMAT::_R8_TYPELESS:
            case SPUDGPU_FORMAT::_R8_UNORM:
            case SPUDGPU_FORMAT::_R8_UINT:
            case SPUDGPU_FORMAT::_R8_SNORM:
            case SPUDGPU_FORMAT::_R8_SINT:
            case SPUDGPU_FORMAT::_A8_UNORM:
            case SPUDGPU_FORMAT::_AI44:
            case SPUDGPU_FORMAT::_IA44:
            case SPUDGPU_FORMAT::_P8:
                return 8;

            case SPUDGPU_FORMAT::_R1_UNORM:
                return 1;

            case SPUDGPU_FORMAT::_BC1_TYPELESS:
            case SPUDGPU_FORMAT::_BC1_UNORM:
            case SPUDGPU_FORMAT::_BC1_UNORM_SRGB:
            case SPUDGPU_FORMAT::_BC4_TYPELESS:
            case SPUDGPU_FORMAT::_BC4_UNORM:
            case SPUDGPU_FORMAT::_BC4_SNORM:
                return 4;

            case SPUDGPU_FORMAT::_BC2_TYPELESS:
            case SPUDGPU_FORMAT::_BC2_UNORM:
            case SPUDGPU_FORMAT::_BC2_UNORM_SRGB:
            case SPUDGPU_FORMAT::_BC3_TYPELESS:
            case SPUDGPU_FORMAT::_BC3_UNORM:
            case SPUDGPU_FORMAT::_BC3_UNORM_SRGB:
            case SPUDGPU_FORMAT::_BC5_TYPELESS:
            case SPUDGPU_FORMAT::_BC5_UNORM:
            case SPUDGPU_FORMAT::_BC5_SNORM:
            case SPUDGPU_FORMAT::_BC6H_TYPELESS:
            case SPUDGPU_FORMAT::_BC6H_UF16:
            case SPUDGPU_FORMAT::_BC6H_SF16:
            case SPUDGPU_FORMAT::_BC7_TYPELESS:
            case SPUDGPU_FORMAT::_BC7_UNORM:
            case SPUDGPU_FORMAT::_BC7_UNORM_SRGB:
                return 8;

            default:
                return 0;
        }
    }

    SPUDGPU_FORMAT gpu_format_from_string(const std::string &fmtString) {
        if (fmtString == "R32G32B32A32_TYPELESS") return SPUDGPU_FORMAT::_R32G32B32A32_TYPELESS;
        if (fmtString == "R32G32B32A32_FLOAT") return SPUDGPU_FORMAT::_R32G32B32A32_FLOAT;
        if (fmtString == "R32G32B32A32_UINT") return SPUDGPU_FORMAT::_R32G32B32A32_UINT;
        if (fmtString == "R32G32B32A32_SINT") return SPUDGPU_FORMAT::_R32G32B32A32_SINT;

        if (fmtString == "R32G32B32_TYPELESS") return SPUDGPU_FORMAT::_R32G32B32_TYPELESS;
        if (fmtString == "R32G32B32_FLOAT") return SPUDGPU_FORMAT::_R32G32B32_FLOAT;
        if (fmtString == "R32G32B32_UINT") return SPUDGPU_FORMAT::_R32G32B32_UINT;
        if (fmtString == "R32G32B32_SINT") return SPUDGPU_FORMAT::_R32G32B32_SINT;

        if (fmtString == "R16G16B16A16_TYPELESS") return SPUDGPU_FORMAT::_R16G16B16A16_TYPELESS;
        if (fmtString == "R16G16B16A16_FLOAT") return SPUDGPU_FORMAT::_R16G16B16A16_FLOAT;
        if (fmtString == "R16G16B16A16_UNORM") return SPUDGPU_FORMAT::_R16G16B16A16_UNORM;
        if (fmtString == "R16G16B16A16_UINT") return SPUDGPU_FORMAT::_R16G16B16A16_UINT;
        if (fmtString == "R16G16B16A16_SNORM") return SPUDGPU_FORMAT::_R16G16B16A16_SNORM;
        if (fmtString == "R16G16B16A16_SINT") return SPUDGPU_FORMAT::_R16G16B16A16_SINT;

        if (fmtString == "R32G32_TYPELESS") return SPUDGPU_FORMAT::_R32G32_TYPELESS;
        if (fmtString == "R32G32_FLOAT") return SPUDGPU_FORMAT::_R32G32_FLOAT;
        if (fmtString == "R32G32_UINT") return SPUDGPU_FORMAT::_R32G32_UINT;
        if (fmtString == "R32G32_SINT") return SPUDGPU_FORMAT::_R32G32_SINT;

        if (fmtString == "R32G8X24_TYPELESS") return SPUDGPU_FORMAT::_R32G8X24_TYPELESS;
        if (fmtString == "D32_FLOAT_S8X24_UINT") return SPUDGPU_FORMAT::_D32_FLOAT_S8X24_UINT;
        if (fmtString == "R32_FLOAT_X8X24_TYPELESS") return SPUDGPU_FORMAT::_R32_FLOAT_X8X24_TYPELESS;
        if (fmtString == "X32_TYPELESS_G8X24_UINT") return SPUDGPU_FORMAT::_X32_TYPELESS_G8X24_UINT;

        if (fmtString == "Y416") return SPUDGPU_FORMAT::_Y416;
        if (fmtString == "Y210") return SPUDGPU_FORMAT::_Y210;
        if (fmtString == "Y216") return SPUDGPU_FORMAT::_Y216;

        if (fmtString == "R10G10B10A2_TYPELESS") return SPUDGPU_FORMAT::_R10G10B10A2_TYPELESS;
        if (fmtString == "R10G10B10A2_UNORM") return SPUDGPU_FORMAT::_R10G10B10A2_UNORM;
        if (fmtString == "R10G10B10A2_UINT") return SPUDGPU_FORMAT::_R10G10B10A2_UINT;

        if (fmtString == "R11G11B10_FLOAT") return SPUDGPU_FORMAT::_R11G11B10_FLOAT;

        if (fmtString == "R8G8B8A8_TYPELESS") return SPUDGPU_FORMAT::_R8G8B8A8_TYPELESS;
        if (fmtString == "R8G8B8A8_UNORM") return SPUDGPU_FORMAT::_R8G8B8A8_UNORM;
        if (fmtString == "R8G8B8A8_UNORM_SRGB") return SPUDGPU_FORMAT::_R8G8B8A8_UNORM_SRGB;
        if (fmtString == "R8G8B8A8_UINT") return SPUDGPU_FORMAT::_R8G8B8A8_UINT;
        if (fmtString == "R8G8B8A8_SNORM") return SPUDGPU_FORMAT::_R8G8B8A8_SNORM;
        if (fmtString == "R8G8B8A8_SINT") return SPUDGPU_FORMAT::_R8G8B8A8_SINT;

        if (fmtString == "R16G16_TYPELESS") return SPUDGPU_FORMAT::_R16G16_TYPELESS;
        if (fmtString == "R16G16_FLOAT") return SPUDGPU_FORMAT::_R16G16_FLOAT;
        if (fmtString == "R16G16_UNORM") return SPUDGPU_FORMAT::_R16G16_UNORM;
        if (fmtString == "R16G16_UINT") return SPUDGPU_FORMAT::_R16G16_UINT;
        if (fmtString == "R16G16_SNORM") return SPUDGPU_FORMAT::_R16G16_SNORM;
        if (fmtString == "R16G16_SINT") return SPUDGPU_FORMAT::_R16G16_SINT;

        if (fmtString == "R32_TYPELESS") return SPUDGPU_FORMAT::_R32_TYPELESS;
        if (fmtString == "D32_FLOAT") return SPUDGPU_FORMAT::_D32_FLOAT;
        if (fmtString == "R32_FLOAT") return SPUDGPU_FORMAT::_R32_FLOAT;
        if (fmtString == "R32_UINT") return SPUDGPU_FORMAT::_R32_UINT;
        if (fmtString == "R32_SINT") return SPUDGPU_FORMAT::_R32_SINT;

        if (fmtString == "R24G8_TYPELESS") return SPUDGPU_FORMAT::_R24G8_TYPELESS;
        if (fmtString == "D24_UNORM_S8_UINT") return SPUDGPU_FORMAT::_D24_UNORM_S8_UINT;
        if (fmtString == "R24_UNORM_X8_TYPELESS") return SPUDGPU_FORMAT::_R24_UNORM_X8_TYPELESS;
        if (fmtString == "X24_TYPELESS_G8_UINT") return SPUDGPU_FORMAT::_X24_TYPELESS_G8_UINT;

        if (fmtString == "R9G9B9E5_SHAREDEXP") return SPUDGPU_FORMAT::_R9G9B9E5_SHAREDEXP;

        if (fmtString == "R8G8_B8G8_UNORM") return SPUDGPU_FORMAT::_R8G8_B8G8_UNORM;
        if (fmtString == "G8R8_G8B8_UNORM") return SPUDGPU_FORMAT::_G8R8_G8B8_UNORM;

        if (fmtString == "B8G8R8A8_UNORM") return SPUDGPU_FORMAT::_B8G8R8A8_UNORM;
        if (fmtString == "B8G8R8X8_UNORM") return SPUDGPU_FORMAT::_B8G8R8X8_UNORM;

        if (fmtString == "R10G10B10_XR_BIAS_A2_UNORM") return SPUDGPU_FORMAT::_R10G10B10_XR_BIAS_A2_UNORM;

        if (fmtString == "B8G8R8A8_TYPELESS") return SPUDGPU_FORMAT::_B8G8R8A8_TYPELESS;
        if (fmtString == "B8G8R8A8_UNORM_SRGB") return SPUDGPU_FORMAT::_B8G8R8A8_UNORM_SRGB;
        if (fmtString == "B8G8R8X8_TYPELESS") return SPUDGPU_FORMAT::_B8G8R8X8_TYPELESS;
        if (fmtString == "B8G8R8X8_UNORM_SRGB") return SPUDGPU_FORMAT::_B8G8R8X8_UNORM_SRGB;

        if (fmtString == "AYUV") return SPUDGPU_FORMAT::_AYUV;
        if (fmtString == "Y410") return SPUDGPU_FORMAT::_Y410;
        if (fmtString == "YUY2") return SPUDGPU_FORMAT::_YUY2;

        if (fmtString == "P010") return SPUDGPU_FORMAT::_P010;
        if (fmtString == "P016") return SPUDGPU_FORMAT::_P016;

        if (fmtString == "R8G8_TYPELESS") return SPUDGPU_FORMAT::_R8G8_TYPELESS;
        if (fmtString == "R8G8_UNORM") return SPUDGPU_FORMAT::_R8G8_UNORM;
        if (fmtString == "R8G8_UINT") return SPUDGPU_FORMAT::_R8G8_UINT;
        if (fmtString == "R8G8_SNORM") return SPUDGPU_FORMAT::_R8G8_SNORM;
        if (fmtString == "R8G8_SINT") return SPUDGPU_FORMAT::_R8G8_SINT;

        if (fmtString == "R16_TYPELESS") return SPUDGPU_FORMAT::_R16_TYPELESS;
        if (fmtString == "R16_FLOAT") return SPUDGPU_FORMAT::_R16_FLOAT;
        if (fmtString == "D16_UNORM") return SPUDGPU_FORMAT::_D16_UNORM;
        if (fmtString == "R16_UNORM") return SPUDGPU_FORMAT::_R16_UNORM;
        if (fmtString == "R16_UINT") return SPUDGPU_FORMAT::_R16_UINT;
        if (fmtString == "R16_SNORM") return SPUDGPU_FORMAT::_R16_SNORM;
        if (fmtString == "R16_SINT") return SPUDGPU_FORMAT::_R16_SINT;

        if (fmtString == "B5G6R5_UNORM") return SPUDGPU_FORMAT::_B5G6R5_UNORM;
        if (fmtString == "B5G5R5A1_UNORM") return SPUDGPU_FORMAT::_B5G5R5A1_UNORM;

        if (fmtString == "A8P8") return SPUDGPU_FORMAT::_A8P8;

        if (fmtString == "B4G4R4A4_UNORM") return SPUDGPU_FORMAT::_B4G4R4A4_UNORM;

        if (fmtString == "NV12") return SPUDGPU_FORMAT::_NV12;
        if (fmtString == "420_OPAQUE") return SPUDGPU_FORMAT::_420_OPAQUE;
        if (fmtString == "NV11") return SPUDGPU_FORMAT::_NV11;

        if (fmtString == "R8_TYPELESS") return SPUDGPU_FORMAT::_R8_TYPELESS;
        if (fmtString == "R8_UNORM") return SPUDGPU_FORMAT::_R8_UNORM;
        if (fmtString == "R8_UINT") return SPUDGPU_FORMAT::_R8_UINT;
        if (fmtString == "R8_SNORM") return SPUDGPU_FORMAT::_R8_SNORM;
        if (fmtString == "R8_SINT") return SPUDGPU_FORMAT::_R8_SINT;
        if (fmtString == "A8_UNORM") return SPUDGPU_FORMAT::_A8_UNORM;

        if (fmtString == "AI44") return SPUDGPU_FORMAT::_AI44;
        if (fmtString == "IA44") return SPUDGPU_FORMAT::_IA44;
        if (fmtString == "P8") return SPUDGPU_FORMAT::_P8;

        if (fmtString == "R1_UNORM") return SPUDGPU_FORMAT::_R1_UNORM;

        if (fmtString == "BC1_TYPELESS") return SPUDGPU_FORMAT::_BC1_TYPELESS;
        if (fmtString == "BC1_UNORM") return SPUDGPU_FORMAT::_BC1_UNORM;
        if (fmtString == "BC1_UNORM_SRGB") return SPUDGPU_FORMAT::_BC1_UNORM_SRGB;
        if (fmtString == "BC4_TYPELESS") return SPUDGPU_FORMAT::_BC4_TYPELESS;
        if (fmtString == "BC4_UNORM") return SPUDGPU_FORMAT::_BC4_UNORM;
        if (fmtString == "BC4_SNORM") return SPUDGPU_FORMAT::_BC4_SNORM;

        if (fmtString == "BC2_TYPELESS") return SPUDGPU_FORMAT::_BC2_TYPELESS;
        if (fmtString == "BC2_UNORM") return SPUDGPU_FORMAT::_BC2_UNORM;
        if (fmtString == "BC2_UNORM_SRGB") return SPUDGPU_FORMAT::_BC2_UNORM_SRGB;
        if (fmtString == "BC3_TYPELESS") return SPUDGPU_FORMAT::_BC3_TYPELESS;
        if (fmtString == "BC3_UNORM") return SPUDGPU_FORMAT::_BC3_UNORM;
        if (fmtString == "BC3_UNORM_SRGB") return SPUDGPU_FORMAT::_BC3_UNORM_SRGB;
        if (fmtString == "BC5_TYPELESS") return SPUDGPU_FORMAT::_BC5_TYPELESS;
        if (fmtString == "BC5_UNORM") return SPUDGPU_FORMAT::_BC5_UNORM;
        if (fmtString == "BC5_UNORM_SRGB") return SPUDGPU_FORMAT::_BC5_SNORM;

        if (fmtString == "BC6H_TYPELESS") return SPUDGPU_FORMAT::_BC6H_TYPELESS;
        if (fmtString == "BC6H_UF16") return SPUDGPU_FORMAT::_BC6H_UF16;
        if (fmtString == "BC6H_SF16") return SPUDGPU_FORMAT::_BC6H_SF16;

        if (fmtString == "BC7_TYPELESS") return SPUDGPU_FORMAT::_BC7_TYPELESS;
        if (fmtString == "BC7_UNORM") return SPUDGPU_FORMAT::_BC7_UNORM;
        if (fmtString == "BC7_UNORM_SRGB") return SPUDGPU_FORMAT::_BC7_UNORM_SRGB;

        return SPUDGPU_FORMAT::_UNKNOWN;
    }
}
