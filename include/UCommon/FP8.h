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

#include "_deps/half.hpp"

#define UBPA_UCOMMON_FP8_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
    template<uint32_t E, uint32_t M, int32_t B = 0> \
	using FFP8 = UCommon::FFP8<E, M, B>; \
    using FFP8_E4M3 = UCommon::FFP8<4, 3>; \
    using FFP8_E5M2 = UCommon::FFP8<5, 2>; \
    using FUFP8_E4M4 = UCommon::FUFP8<4, 4>; \
    using FUFP8_E5M3 = UCommon::FUFP8<5, 3>; \
}

namespace UCommon
{
    struct FFloat
    {
        uint32_t Frac : 23;
        uint32_t Exp  : 8;
        uint32_t Sign : 1;
    };

    enum class ERound : uint8_t
    {
        Nearest = 0,
        Down    = 1,
        Up      = 2,
	};

    template<uint8_t E, uint8_t M, int8_t B = 0>
    struct FFP8
    {
        static_assert(E + M == 7, "FFP8 must have E + M = 7");

        static constexpr uint8_t BiasE = (1 << (E - 1)) - 1 + B;
        static constexpr uint32_t SubNormalScale = 1 << (BiasE + M - 1);
        static constexpr uint8_t MaskE = (1 << E) - 1;
        static constexpr uint8_t MaskM = (1 << M) - 1;
        static constexpr uint32_t MaskR = (1 << (23 - M)) - 1;

        FFP8() noexcept;

        explicit FFP8(float Value, ERound Round = ERound::Nearest) noexcept;

        operator float() const noexcept;

        bool operator==(const FFP8& rhs) const noexcept;
        bool operator!=(const FFP8& rhs) const noexcept;

        union
        {
            struct
            {
                uint8_t Frac : M;
                uint8_t Exp  : E;
                uint8_t Sign : 1;
            } Components;
            uint8_t Data;
        };
    };

    template<uint8_t E, uint8_t M, int8_t B = 0>
    struct FUFP8
    {
        static_assert(E + M == 8, "FUFP8 must have E + M = 8");

        static constexpr uint8_t BiasE = (1 << (E - 1)) - 1 + B;
        static constexpr uint32_t SubNormalScale = 1 << (BiasE + M - 1);
        static constexpr uint8_t MaskE = (1 << E) - 1;
        static constexpr uint8_t MaskM = (1 << M) - 1;
        static constexpr uint32_t MaskR = (1 << (23 - M)) - 1;

        FUFP8() noexcept;

        explicit FUFP8(float Value, ERound Round = ERound::Nearest) noexcept;

        operator float() const noexcept;

        bool operator==(const FUFP8& rhs) const noexcept;
        bool operator!=(const FUFP8& rhs) const noexcept;

        union
        {
            struct
            {
                uint8_t Frac : M;
                uint8_t Exp  : E;
            } Components;
            uint8_t Data;
        };
    };

    using FFP8_E3M4 = FFP8<3, 4>;
    using FFP8_E4M3 = FFP8<4, 3>;
    using FFP8_E5M2 = FFP8<5, 2>;
    using FUFP8_E3M5 = FUFP8<3, 5>;
    using FUFP8_E4M4 = FUFP8<4, 4>;
    using FUFP8_E5M3 = FUFP8<5, 3>;
}

UBPA_UCOMMON_FP8_TO_NAMESPACE(UCommonTest)
#include "FP8.inl"
