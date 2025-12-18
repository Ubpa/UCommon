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

#include "Config.h"
#include "Vector.h"
#include "Utils.h"

namespace UCommon
{
	//===========================================
	// RGBM Codec
	//===========================================

	/**
	 * Unity use 34.49(5^2.2).
	 * Because M is in sqrt space instead of gamma 2.2 space,
	 * So we use 36(6^2) which colse to Untiy's value.
	 * But 36 is not enough, 100 is.
	 */
	constexpr float RGBM_DefaultMaxValue = 10.f;
	constexpr float RGBM_DefaultMaxMultiplier = RGBM_DefaultMaxValue * RGBM_DefaultMaxValue; // 100.f

	/**
	 * @astc-encoder-help
	 * The compression behavior of the ASTC format for RGBM data
	 * requires that the user's RGBM encoding preprocess keeps values
	 * of M above a lower threshold to avoid them quantizing to zero
	 * during compression. We recommend trying 16/255 or 32/255.
	 */
	constexpr float LowClamp = 16.f / 255.f;

	inline float RGBM_GetMultiplier(const FLinearColorRGB& Color) noexcept { return Color.MaxComponent(); }

	/**
	 * Reference
	 * - Unity shader
	 * - Arm: https://github.com/ARM-software/astc-encoder/blob/main/Utils/astc_rgbm_codec.cpp
	 *
	 * @param Color input color
	 * @param Multiplier Use same multiplier for all pixel.
	 * @param LowClamp low clamp for M to avoid quantization issues
	 * @return RGBM M is in sqrt space (referenced from Unity)
	 */
	UBPA_UCOMMON_API FLinearColor EncodeRGBM(FLinearColorRGB Color, float Multiplier = RGBM_DefaultMaxMultiplier, float InLowClamp = LowClamp);
	UBPA_UCOMMON_API float EncodeRGBM(float M, float Multiplier, float InLowClamp = LowClamp);
	UBPA_UCOMMON_API FLinearColor EncodeRGBMWithM(FLinearColorRGB Color, float Multiplier = RGBM_DefaultMaxMultiplier, float SqrtMScale = RGBM_DefaultMaxValue);
	UBPA_UCOMMON_API FLinearColorRGB MapToValidColorRGBM(FLinearColorRGB Color, float Multiplier = RGBM_DefaultMaxMultiplier, float InLowClamp = LowClamp);

	inline FLinearColorRGB DecodeRGBM(const FLinearColor& RGBM, float Multiplier) noexcept
	{
		return FLinearColorRGB(RGBM.X, RGBM.Y, RGBM.Z) * (Pow2(RGBM.W) * Multiplier);
	}

	inline FLinearColorRGB DecodeRGBM(const FColor& RGBM, float Multiplier) noexcept
	{
		return DecodeRGBM(ElementColorToLinearColor(RGBM), Multiplier);
	}

	inline FDoubleColorRGB DecodeRGBM(const FDoubleColor& RGBM, double Multiplier) noexcept
	{
		return FDoubleColorRGB(RGBM.X, RGBM.Y, RGBM.Z) * (Pow2(RGBM.W) * Multiplier);
	}

	//===========================================
	// RGBD Codec
	//===========================================

	constexpr float RGBD_DefaultMaxValue = 2048.f;

	UBPA_UCOMMON_API float RGBD_GetK(float MaxValue);
	UBPA_UCOMMON_API float RGBD_GetMaxValue(float K);

	/**
	 * RGBD
	 * D means divide
	 * https://www.desmos.com/calculator/ioylfelonf
	 */
	UBPA_UCOMMON_API FLinearColor EncodeRGBD(FLinearColorRGB Color, float MaxValue, float InLowClamp = LowClamp);

	/**
	 * Map color to valid RGBD-encodable color.
	 * This ensures the color can be represented within the RGBD encoding scheme.
	 */
	UBPA_UCOMMON_API FLinearColorRGB MapToValidColorRGBD(FLinearColorRGB Color, float MaxValue, float InLowClamp = LowClamp);

	inline FLinearColorRGB DecodeRGBD(FLinearColor RGBD, float MaxValue) noexcept
	{
		float Factor = Pow2(RGBD.W / (RGBD_GetK(MaxValue) * RGBD.W + 1.f));
		return FLinearColorRGB(RGBD.X, RGBD.Y, RGBD.Z) * Factor;
	}

	inline FDoubleColorRGB DecodeRGBD(FDoubleColor RGBD, double MaxValue) noexcept
	{
		double Factor = Pow2(RGBD.W / ((double)RGBD_GetK((float)MaxValue) * RGBD.W + 1.));
		return FDoubleColorRGB(RGBD.X, RGBD.Y, RGBD.Z) * Factor;
	}

	/**
	 * Encode single luminance value L to D.
	 * D = sqrt(L) / (1 - k * sqrt(L)), where k = 1/sqrt(MaxValue) - 1
	 */
	inline float EncodeRGBD(float L, float MaxValue) noexcept
	{
		if (L <= 0.f)
			return 0.f;
		float k = RGBD_GetK(MaxValue);
		float sqrtL = std::sqrt(L);
		return sqrtL / (1.f - k * sqrtL);
	}

	/**
	 * Decode D back to luminance L.
	 * L = (D / (k*D + 1))^2
	 */
	inline float DecodeRGBD(float D, float MaxValue) noexcept
	{
		if (D <= 0.f)
			return 0.f;
		float k = RGBD_GetK(MaxValue);
		float sqrtL = D / (k * D + 1.f);
		return Pow2(sqrtL);
	}

	//===========================================
	// RGBV Codec
	//===========================================

	/** Default max luminance value. L is in [0, MaxValue]. */
	constexpr float RGBV_DefaultMaxValue = 128.f;
	/** Default optimal V value for GlobalEncode. */
	constexpr float RGBV_DefaultOptimalV = 0.8f;

	/**
	 * Get the b factor for the RGBV encoding.
	 * b = 1/M + 1
	 * Used in formula: L = V^2 / (b - V^2)
	 */
	inline float RGBV_GetB(float MaxValue) noexcept { return 1.f / MaxValue + 1.f; }

	/**
	 * RGBV encoding
	 * V = sqrt((M+1)/M * L/(L+1)) = sqrt(b * L/(L+1))
	 * where L = max(R,G,B), L in [0, M]
	 * L/(L+1) simulates tonemapping, sqrt simulates gamma correction.
	 */
	UBPA_UCOMMON_API FLinearColor EncodeRGBV(FLinearColorRGB Color, float MaxValue = RGBV_DefaultMaxValue, float InLowClamp = LowClamp);
	UBPA_UCOMMON_API float EncodeRGBV(float L, float MaxValue = RGBV_DefaultMaxValue);

	/**
	 * Map color to valid RGBV-encodable color.
	 * This ensures the color can be represented within the RGBV encoding scheme.
	 */
	UBPA_UCOMMON_API FLinearColorRGB MapToValidColorRGBV(FLinearColorRGB Color, float MaxValue, float InLowClamp = LowClamp);

	/**
	 * Decode RGBV to linear HDR color.
	 * L = V^2 / (b - V^2), where b = 1/M + 1
	 * RGB = RGB_encoded * L
	 */
	inline FLinearColorRGB DecodeRGBV(FLinearColor RGBV, float MaxValue) noexcept
	{
		float b = RGBV_GetB(MaxValue);
		float V2 = Pow2(RGBV.W);
		float L = V2 / (b - V2);
		return FLinearColorRGB(RGBV.X, RGBV.Y, RGBV.Z) * L;
	}

	inline float DecodeRGBV(float V, float MaxValue) noexcept
	{
		float b = RGBV_GetB(MaxValue);
		float V2 = Pow2(V);
		return V2 / (b - V2);
	}

	inline FLinearColorRGB DecodeRGBV(FColor RGBV, float MaxValue) noexcept
	{
		return DecodeRGBV(ElementColorToLinearColor(RGBV), MaxValue);
	}

	inline FDoubleColorRGB DecodeRGBV(FDoubleColor RGBV, double MaxValue) noexcept
	{
		double b = 1. / MaxValue + 1.;
		double V2 = Pow2(RGBV.W);
		double L = V2 / (b - V2);
		return FDoubleColorRGB(RGBV.X, RGBV.Y, RGBV.Z) * L;
	}
} // namespace UCommon

