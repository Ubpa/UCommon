/*
MIT License

Copyright (c) 2024 Ubpa

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "FP8.h"

namespace UCommon
{
    template<typename FP8Type>
    struct TFloatTable
    {
        static constexpr bool bSupport = false;
    };

    template<>
    struct TFloatTable<FFP8_E4M3>
    {
        static constexpr bool bSupport = true;
        static constexpr uint32_t Data[256] = {
0x00000000,0x3B000000,0x3B800000,0x3BC00000,0x3C000000,0x3C200000,0x3C400000,0x3C600000,
0x3C800000,0x3C900000,0x3CA00000,0x3CB00000,0x3CC00000,0x3CD00000,0x3CE00000,0x3CF00000,
0x3D000000,0x3D100000,0x3D200000,0x3D300000,0x3D400000,0x3D500000,0x3D600000,0x3D700000,
0x3D800000,0x3D900000,0x3DA00000,0x3DB00000,0x3DC00000,0x3DD00000,0x3DE00000,0x3DF00000,
0x3E000000,0x3E100000,0x3E200000,0x3E300000,0x3E400000,0x3E500000,0x3E600000,0x3E700000,
0x3E800000,0x3E900000,0x3EA00000,0x3EB00000,0x3EC00000,0x3ED00000,0x3EE00000,0x3EF00000,
0x3F000000,0x3F100000,0x3F200000,0x3F300000,0x3F400000,0x3F500000,0x3F600000,0x3F700000,
0x3F800000,0x3F900000,0x3FA00000,0x3FB00000,0x3FC00000,0x3FD00000,0x3FE00000,0x3FF00000,
0x40000000,0x40100000,0x40200000,0x40300000,0x40400000,0x40500000,0x40600000,0x40700000,
0x40800000,0x40900000,0x40A00000,0x40B00000,0x40C00000,0x40D00000,0x40E00000,0x40F00000,
0x41000000,0x41100000,0x41200000,0x41300000,0x41400000,0x41500000,0x41600000,0x41700000,
0x41800000,0x41900000,0x41A00000,0x41B00000,0x41C00000,0x41D00000,0x41E00000,0x41F00000,
0x42000000,0x42100000,0x42200000,0x42300000,0x42400000,0x42500000,0x42600000,0x42700000,
0x42800000,0x42900000,0x42A00000,0x42B00000,0x42C00000,0x42D00000,0x42E00000,0x42F00000,
0x43000000,0x43100000,0x43200000,0x43300000,0x43400000,0x43500000,0x43600000,0x43700000,
0x43800000,0x43900000,0x43A00000,0x43B00000,0x43C00000,0x43D00000,0x43E00000,0x43F00000,
0x80000000,0xBB000000,0xBB800000,0xBBC00000,0xBC000000,0xBC200000,0xBC400000,0xBC600000,
0xBC800000,0xBC900000,0xBCA00000,0xBCB00000,0xBCC00000,0xBCD00000,0xBCE00000,0xBCF00000,
0xBD000000,0xBD100000,0xBD200000,0xBD300000,0xBD400000,0xBD500000,0xBD600000,0xBD700000,
0xBD800000,0xBD900000,0xBDA00000,0xBDB00000,0xBDC00000,0xBDD00000,0xBDE00000,0xBDF00000,
0xBE000000,0xBE100000,0xBE200000,0xBE300000,0xBE400000,0xBE500000,0xBE600000,0xBE700000,
0xBE800000,0xBE900000,0xBEA00000,0xBEB00000,0xBEC00000,0xBED00000,0xBEE00000,0xBEF00000,
0xBF000000,0xBF100000,0xBF200000,0xBF300000,0xBF400000,0xBF500000,0xBF600000,0xBF700000,
0xBF800000,0xBF900000,0xBFA00000,0xBFB00000,0xBFC00000,0xBFD00000,0xBFE00000,0xBFF00000,
0xC0000000,0xC0100000,0xC0200000,0xC0300000,0xC0400000,0xC0500000,0xC0600000,0xC0700000,
0xC0800000,0xC0900000,0xC0A00000,0xC0B00000,0xC0C00000,0xC0D00000,0xC0E00000,0xC0F00000,
0xC1000000,0xC1100000,0xC1200000,0xC1300000,0xC1400000,0xC1500000,0xC1600000,0xC1700000,
0xC1800000,0xC1900000,0xC1A00000,0xC1B00000,0xC1C00000,0xC1D00000,0xC1E00000,0xC1F00000,
0xC2000000,0xC2100000,0xC2200000,0xC2300000,0xC2400000,0xC2500000,0xC2600000,0xC2700000,
0xC2800000,0xC2900000,0xC2A00000,0xC2B00000,0xC2C00000,0xC2D00000,0xC2E00000,0xC2F00000,
0xC3000000,0xC3100000,0xC3200000,0xC3300000,0xC3400000,0xC3500000,0xC3600000,0xC3700000,
0xC3800000,0xC3900000,0xC3A00000,0xC3B00000,0xC3C00000,0xC3D00000,0xC3E00000,0xC3F00000,
		};
    };

    template<>
    struct TFloatTable<FUFP8_E4M4>
    {
        static constexpr bool bSupport = true;
        static constexpr uint32_t Data[256] = {
0x00000000,0x3A800000,0x3B000000,0x3B400000,0x3B800000,0x3BA00000,0x3BC00000,0x3BE00000,
0x3C000000,0x3C100000,0x3C200000,0x3C300000,0x3C400000,0x3C500000,0x3C600000,0x3C700000,
0x3C800000,0x3C880000,0x3C900000,0x3C980000,0x3CA00000,0x3CA80000,0x3CB00000,0x3CB80000,
0x3CC00000,0x3CC80000,0x3CD00000,0x3CD80000,0x3CE00000,0x3CE80000,0x3CF00000,0x3CF80000,
0x3D000000,0x3D080000,0x3D100000,0x3D180000,0x3D200000,0x3D280000,0x3D300000,0x3D380000,
0x3D400000,0x3D480000,0x3D500000,0x3D580000,0x3D600000,0x3D680000,0x3D700000,0x3D780000,
0x3D800000,0x3D880000,0x3D900000,0x3D980000,0x3DA00000,0x3DA80000,0x3DB00000,0x3DB80000,
0x3DC00000,0x3DC80000,0x3DD00000,0x3DD80000,0x3DE00000,0x3DE80000,0x3DF00000,0x3DF80000,
0x3E000000,0x3E080000,0x3E100000,0x3E180000,0x3E200000,0x3E280000,0x3E300000,0x3E380000,
0x3E400000,0x3E480000,0x3E500000,0x3E580000,0x3E600000,0x3E680000,0x3E700000,0x3E780000,
0x3E800000,0x3E880000,0x3E900000,0x3E980000,0x3EA00000,0x3EA80000,0x3EB00000,0x3EB80000,
0x3EC00000,0x3EC80000,0x3ED00000,0x3ED80000,0x3EE00000,0x3EE80000,0x3EF00000,0x3EF80000,
0x3F000000,0x3F080000,0x3F100000,0x3F180000,0x3F200000,0x3F280000,0x3F300000,0x3F380000,
0x3F400000,0x3F480000,0x3F500000,0x3F580000,0x3F600000,0x3F680000,0x3F700000,0x3F780000,
0x3F800000,0x3F880000,0x3F900000,0x3F980000,0x3FA00000,0x3FA80000,0x3FB00000,0x3FB80000,
0x3FC00000,0x3FC80000,0x3FD00000,0x3FD80000,0x3FE00000,0x3FE80000,0x3FF00000,0x3FF80000,
0x40000000,0x40080000,0x40100000,0x40180000,0x40200000,0x40280000,0x40300000,0x40380000,
0x40400000,0x40480000,0x40500000,0x40580000,0x40600000,0x40680000,0x40700000,0x40780000,
0x40800000,0x40880000,0x40900000,0x40980000,0x40A00000,0x40A80000,0x40B00000,0x40B80000,
0x40C00000,0x40C80000,0x40D00000,0x40D80000,0x40E00000,0x40E80000,0x40F00000,0x40F80000,
0x41000000,0x41080000,0x41100000,0x41180000,0x41200000,0x41280000,0x41300000,0x41380000,
0x41400000,0x41480000,0x41500000,0x41580000,0x41600000,0x41680000,0x41700000,0x41780000,
0x41800000,0x41880000,0x41900000,0x41980000,0x41A00000,0x41A80000,0x41B00000,0x41B80000,
0x41C00000,0x41C80000,0x41D00000,0x41D80000,0x41E00000,0x41E80000,0x41F00000,0x41F80000,
0x42000000,0x42080000,0x42100000,0x42180000,0x42200000,0x42280000,0x42300000,0x42380000,
0x42400000,0x42480000,0x42500000,0x42580000,0x42600000,0x42680000,0x42700000,0x42780000,
0x42800000,0x42880000,0x42900000,0x42980000,0x42A00000,0x42A80000,0x42B00000,0x42B80000,
0x42C00000,0x42C80000,0x42D00000,0x42D80000,0x42E00000,0x42E80000,0x42F00000,0x42F80000,
0x43000000,0x43080000,0x43100000,0x43180000,0x43200000,0x43280000,0x43300000,0x43380000,
0x43400000,0x43480000,0x43500000,0x43580000,0x43600000,0x43680000,0x43700000,0x43780000,
0x43800000,0x43880000,0x43900000,0x43980000,0x43A00000,0x43A80000,0x43B00000,0x43B80000,
0x43C00000,0x43C80000,0x43D00000,0x43D80000,0x43E00000,0x43E80000,0x43F00000,0x43F80000,
        };
    };

    template<typename FP8Type>
	static constexpr bool TFloatTable_bSupport = TFloatTable<FP8Type>::bSupport;

    template<typename FP8Type>
    const float& FloatTableGet(const FP8Type& FP8) noexcept
    {
		static_assert(sizeof(FP8Type) == sizeof(uint8_t), "FP8Type must be a single byte type");
        return reinterpret_cast<const float&>(TFloatTable<FP8Type>::Data[reinterpret_cast<const uint8_t&>(FP8)]);
    }
}

template<uint8_t E, uint8_t M>
UCommon::FFP8<E, M>::FFP8() noexcept : Data(0) {}

template<uint8_t E, uint8_t M>
UCommon::FFP8<E, M>::FFP8(float Value, ERound Round) noexcept
{
    const auto& Float = reinterpret_cast<const FFloat&>(Value);
    const uint8_t Sign = Float.Sign;

    if (Value == 0.0f)
    {
        Data = Sign << 7;
        return;
    }

    if (Float.Exp == 0xFF)
    {
        Data = (Sign << 7) | 0x7F;
        return;
    }

    const int32_t e_f = Float.Exp - 127;

    int32_t e = e_f + BiasE;

    if (e <= 0)
    {
        const float abs_f = std::abs(Value);
        constexpr float scale = SubNormalScale;
        uint16_t m;
        switch (Round)
        {
        default:
        case UCommon::ERound::Nearest:
            m = static_cast<uint16_t>(abs_f * scale + 0.5f);
            break;
        case UCommon::ERound::Down:
            m = static_cast<uint16_t>(abs_f * scale);
            break;
        case UCommon::ERound::Up:
            m = static_cast<uint16_t>(std::ceil(abs_f * scale));
            break;
        }

        if (m > MaskM)
        {
            Data = (Sign << 7) | (1 << M);
            return;
        }

        Data = (Sign << 7) | (uint8_t)m;
        return;
    }

    uint32_t m24 = (1 << 23) | Float.Frac;
    bool bAddBit;
    switch (Round)
    {
    default:
    case UCommon::ERound::Nearest:
        bAddBit = (m24 >> (22 - M)) & 0x01;
        break;
    case UCommon::ERound::Down:
        bAddBit = 0;
        break;
    case UCommon::ERound::Up:
        bAddBit = (m24 & MaskR) != 0;
        break;
    }

    uint8_t m = (m24 >> (23 - M)) & MaskM;
    if (bAddBit)
    {
        if (m == MaskM)
        {
            m = 0;
            e++;
        }
        else
        {
            m++;
        }
    }

    if (e > MaskE)
    {
        Data = (Sign << 7) | 0x7F;
        return;
    }

    Data = (Sign << 7) | ((uint8_t)e << M) | m;
}

template<uint8_t E, uint8_t M>
UCommon::FFP8<E, M>::operator float() const noexcept
{
    if constexpr (TFloatTable_bSupport<FFP8<E, M>>)
    {
        return FloatTableGet(*this);
    }
    else
    {
        const uint32_t frac = Components.Frac;

        if (Components.Exp == 0)
        {
            constexpr float scale = SubNormalScale;
            float value = static_cast<float>(frac) / scale;

            return Components.Sign ? -value : value;
        }
        else
        {
            const int32_t e = Components.Exp - BiasE;
            uint32_t bits = 0;
            bits |= ((uint32_t)Components.Sign << 31);
            bits |= static_cast<uint32_t>(e + 127) << 23;
            bits |= (frac << (23 - M));

            auto f = reinterpret_cast<const float&>(bits);
            return f;
        }
    }
}

template<uint8_t E, uint8_t M>
bool UCommon::FFP8<E, M>::operator==(const FFP8& rhs) const noexcept
{
    return (Data == 0 && rhs.Data == 0x80)
        || (Data == 0x80 && rhs.Data == 0)
        || (Data == rhs.Data);
}

template<uint8_t E, uint8_t M>
bool UCommon::FFP8<E, M>::operator!=(const FFP8& rhs) const noexcept
{
    return !(*this == rhs);
}

template<uint8_t E, uint8_t M>
UCommon::FUFP8<E, M>::FUFP8() noexcept : Data(0) {}

template<uint8_t E, uint8_t M>
UCommon::FUFP8<E, M>::FUFP8(float Value, ERound Round) noexcept
{
    const auto& Float = reinterpret_cast<const FFloat&>(Value);

    if (Value <= 0.0f)
    {
        Data = 0;
        return;
    }

    if (Float.Exp == 0xFF)
    {
        Data = 0xFF;
        return;
    }

    const int32_t e_f = Float.Exp - 127;

    int32_t e = e_f + BiasE;

    if (e <= 0)
    {
        const float abs_f = std::abs(Value);
        constexpr float scale = SubNormalScale;
        uint16_t m;
        switch (Round)
        {
        default:
        case UCommon::ERound::Nearest:
            m = static_cast<uint16_t>(abs_f * scale + 0.5f);
            break;
        case UCommon::ERound::Down:
            m = static_cast<uint16_t>(abs_f * scale);
            break;
        case UCommon::ERound::Up:
            m = static_cast<uint16_t>(std::ceil(abs_f * scale));
            break;
        }

        if (m > MaskM)
        {
            Data = 1 << M;
            return;
        }

        Data = (uint8_t)m;
        return;
    }

    uint32_t m24 = (1 << 23) | Float.Frac;
    bool bAddBit;
    switch (Round)
    {
    default:
    case UCommon::ERound::Nearest:
        bAddBit = (m24 >> (22 - M)) & 0x01;
        break;
    case UCommon::ERound::Down:
        bAddBit = 0;
        break;
    case UCommon::ERound::Up:
        bAddBit = (m24 & MaskR) != 0;
        break;
    }

    uint8_t m = (m24 >> (23 - M)) & MaskM;
    if (bAddBit)
    {
        if (m == MaskM)
        {
            m = 0;
            e++;
        }
        else
        {
            m++;
        }
    }

    if (e > MaskE)
    {
        Data = 0xFF;
        return;
    }

    Data = ((uint8_t)e << M) | m;
}

template<uint8_t E, uint8_t M>
UCommon::FUFP8<E, M>::operator float() const noexcept
{
    if constexpr (TFloatTable_bSupport<FUFP8<E, M>>)
    {
        return FloatTableGet(*this);
	}
    else
    {
        const uint32_t frac = Components.Frac;

        if (Components.Exp == 0)
        {
            constexpr float scale = SubNormalScale;
            float value = static_cast<float>(frac) / scale;

            return value;
        }
        else
        {
            const int32_t e = Components.Exp - BiasE;
            uint32_t bits = 0;
            bits |= static_cast<uint32_t>(e + 127) << 23;
            bits |= (frac << (23 - M));

            auto f = reinterpret_cast<const float&>(bits);
            return f;
        }
    }
}

template<uint8_t E, uint8_t M>
bool UCommon::FUFP8<E, M>::operator==(const FUFP8& rhs) const noexcept
{
    return Data == rhs.Data;
}

template<uint8_t E, uint8_t M>
bool UCommon::FUFP8<E, M>::operator!=(const FUFP8& rhs) const noexcept
{
    return !(*this == rhs);
}
