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

template<uint8_t E, uint8_t M, int8_t B>
UCommon::FFP8<E, M, B>::FFP8() noexcept : Data(0) {}

template<uint8_t E, uint8_t M, int8_t B>
UCommon::FFP8<E, M, B>::FFP8(float Value, ERound Round) noexcept
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

template<uint8_t E, uint8_t M, int8_t B>
UCommon::FFP8<E, M, B>::operator float() const noexcept
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

template<uint8_t E, uint8_t M, int8_t B>
bool UCommon::FFP8<E, M, B>::operator==(const FFP8& rhs) const noexcept
{
    return (Data == 0 && rhs.Data == 0x80)
        || (Data == 0x80 && rhs.Data == 0)
        || (Data == rhs.Data);
}

template<uint8_t E, uint8_t M, int8_t B>
bool UCommon::FFP8<E, M, B>::operator!=(const FFP8& rhs) const noexcept
{
    return !(*this == rhs);
}

template<uint8_t E, uint8_t M, int8_t B>
UCommon::FUFP8<E, M, B>::FUFP8() noexcept : Data(0) {}

template<uint8_t E, uint8_t M, int8_t B>
UCommon::FUFP8<E, M, B>::FUFP8(float Value, ERound Round) noexcept
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

template<uint8_t E, uint8_t M, int8_t B>
UCommon::FUFP8<E, M, B>::operator float() const noexcept
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

template<uint8_t E, uint8_t M, int8_t B>
bool UCommon::FUFP8<E, M, B>::operator==(const FUFP8& rhs) const noexcept
{
    return Data == rhs.Data;
}

template<uint8_t E, uint8_t M, int8_t B>
bool UCommon::FUFP8<E, M, B>::operator!=(const FUFP8& rhs) const noexcept
{
    return !(*this == rhs);
}
