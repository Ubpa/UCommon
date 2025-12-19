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

#define UBPA_UCOMMON_CODEC_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
    using FPackedHue = UCommon::FPackedHue; \
    using FPackedHemiOct = UCommon::FPackedHemiOct; \
}

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
		UBPA_UCOMMON_ASSERT(L >= 0.f);
		float K = RGBD_GetK(MaxValue);
		float SqrtL = std::sqrt(L);
		return SqrtL / (1.f - K * SqrtL);
	}

	/**
	 * Decode D back to luminance L.
	 * L = (D / (k*D + 1))^2
	 */
	inline float DecodeRGBD(float D, float MaxValue) noexcept
	{
		UBPA_UCOMMON_ASSERT(D >= 0.f);
		float K = RGBD_GetK(MaxValue);
		float SqrtL = D / (K * D + 1.f);
		return Pow2(SqrtL);
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
	// [0, 1]
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

	[[nodiscard]] static inline FVector2f CoCgToSquareCoCg(const FVector2f& CoCg)
	{
		float Cg = Clamp(CoCg[1], -1.f, 1.f);
		float CoRange = 1.f - Cg;
		float SqCo = CoRange > UBPA_UCOMMON_DELTA ? Saturate((CoCg[0] + 0.5f * CoRange) / CoRange) : 0.5f;
		float SqCg = (Cg + 1.f) / 2.f;
		return { SqCo,SqCg };
	}

	[[nodiscard]] static inline FVector2f SquareCoCgToCoCg(const FVector2f& SqCoCg)
	{
		float Cg = Saturate(SqCoCg[1]) * 2.f - 1.f;
		float CoRange = 1.f - Cg;
		float Co = (Saturate(SqCoCg[0]) - 0.5f) * CoRange;
		return { Co,Cg };
	}

	[[nodiscard]] static inline FVector2f ClampCoCg(const FVector2f& CoCg)
	{
		float Cg = Clamp(CoCg[1], -1.f, 1.f);
		const float HalfCoRange = (1.f - Cg) / 2;
		float Co = Clamp(CoCg[0], -HalfCoRange, HalfCoRange);
		return { Co,Cg };
	}

	// co in [-1, 1], cg in [-1, 1]
	static inline void RGBToCoCg(float R, float G, float B, float& Co, float& Cg)
	{
		UBPA_UCOMMON_ASSERT(R >= 0 && G >= 0 && B >= 0);
		const float RB = R + B;
		const float Y = (2.f * G + RB) / 4.f;
		if (Y < UBPA_UCOMMON_DELTA)
		{
			Co = 0.f;
			Cg = 0.f;
			return;
		}
		Co = (R - B) / Y / 4.f;
		Cg = (2.f * G - RB) / Y / 4.f;
	}

	// co in [-1, 1], cg in [-1, 1]
	static inline void RGBToYCoCg(float R, float G, float B, float& Y, float& Co, float& Cg)
	{
		UBPA_UCOMMON_ASSERT(R >= 0 && G >= 0 && B >= 0);
		const float RB = R + B;
		Y = (2.f * G + RB) / 4.f;
		if (Y < UBPA_UCOMMON_DELTA)
		{
			Co = 0.f;
			Cg = 0.f;
			return;
		}
		Co = (R - B) / Y / 4.f;
		Cg = (2.f * G - RB) / Y / 4.f;
	}

	static inline FVector2f RGBToCoCg(const FLinearColorRGB& RGB)
	{
		FVector2f CoCg;
		RGBToCoCg(RGB.X, RGB.Y, RGB.Z, CoCg[0], CoCg[1]);
		return CoCg;
	}

	[[nodiscard]] static inline FLinearColorRGB RGBToYCoCg(const FLinearColorRGB& RGB)
	{
		FLinearColorRGB YCoCg;
		RGBToYCoCg(RGB.X, RGB.Y, RGB.Z, YCoCg.X, YCoCg.Y, YCoCg.Z);
		return YCoCg;
	}

	static inline void CoCgToRGB(float Co, float Cg, float& R, float& G, float& B)
	{
		const float Tmp = 1.f - Cg;
		R = (Tmp + 2.f * Co);
		G = (1.f + Cg);
		B = (Tmp - 2.f * Co);
	}

	static inline void YCoCgToRGB(float Y, float Co, float Cg, float& R, float& G, float& B)
	{
		CoCgToRGB(Co, Cg, R, G, B);
		R *= Y;
		G *= Y;
		B *= Y;
	}

	static inline FLinearColorRGB CoCgToRGB(const FVector2f& CoCg)
	{
		FLinearColorRGB RGB;
		CoCgToRGB(CoCg[0], CoCg[1], RGB.X, RGB.Y, RGB.Z);
		return RGB;
	}

	[[nodiscard]] static inline FLinearColorRGB YCoCgToRGB(const FLinearColorRGB& YCoCg)
	{
		FLinearColorRGB RGB;
		YCoCgToRGB(YCoCg.X, YCoCg.Y, YCoCg.Z, RGB.X, RGB.Y, RGB.Z);
		return RGB;
	}

	static inline FLinearColorRGB ClampRGBwithYCoCg(const FLinearColorRGB& RGB, const FLinearColorRGB& MinYCoCg, const FLinearColorRGB& MaxYCoCg)
	{
		const FLinearColorRGB YCoCg = RGBToYCoCg(RGB);
		const FLinearColorRGB ClampYCoCg = YCoCg.Clamp(MinYCoCg, MaxYCoCg);
		return YCoCgToRGB(ClampYCoCg);
	}

	static inline FLinearColorRGB ClampRGBwithYCoCg(const FLinearColorRGB& RGB, const FBox& YCoCgBox)
	{
		return ClampRGBwithYCoCg(RGB, YCoCgBox.Min, YCoCgBox.Max);
	}

	struct FPackedHue
	{
		FPackedHue() {} // uninitialize
		FPackedHue(EForceInit) : U(128), V(128) {} // zero hue (U=0.5, V=0.5 in UV space)
		FPackedHue(uint8_t InU, uint8_t InV) : U(InU), V(InV) {}
		FPackedHue(const FVector2f& CoCg)
		{
			// Triangle mapping: map (Co, Cg) in valid triangle to UV [0,1]x[0,1]
			// Valid region: Cg - 1 <= 2*Co <= 1 - Cg
			// Inverse mapping: given (Co, Cg), find UV

			// Step 1: Quantize Cg first
			const float Vf = (CoCg.Y + 1.f) * 0.5f;
			V = ElementFloatClampToUint8(Vf);

			// Step 2: Map Co to U within valid range based on quantized Cg
			// Note: V = 255 (triangle top vertex) is degenerate case where CoRange = 0
			float Uf;
			if (V != 255)
			{
				// Use quantized Cg to compute Co range for better accuracy
				// CoRange is symmetric: Co in [-CoRange/2, CoRange/2]
				const float QuantizedCg = ElementUint8ToFloat(V) * 2.f - 1.f;
				const float CoRange = 1.f - QuantizedCg;
				Uf = (CoCg.X + 0.5f * CoRange) / CoRange;
			}
			else
			{
				Uf = 0.5f; // Degenerate case: triangle top vertex, Co must be 0
			}

			U = ElementFloatClampToUint8(Uf);
		}

		FVector2f Unpack() const
		{
			// Triangle mapping: map UV [0,1]x[0,1] to valid (Co, Cg) triangle
			const float Uf = ElementUint8ToFloat(U);
			const float Vf = ElementUint8ToFloat(V);

			// Map V to Cg: [0, 1] -> [-1, 1]
			const float CgValue = Vf * 2.f - 1.f;

			// CoRange is symmetric: Co in [-CoRange/2, CoRange/2]
			const float CoRange = 1.f - CgValue;
			const float CoValue = (Uf - 0.5f) * CoRange;

			return { CoValue, CgValue };
		}

		operator FUint8Vector2() const { return reinterpret_cast<const FUint8Vector2&>(*this); }

		// Triangle mapping: UV [0,1]x[0,1] -> Valid CoCg triangle
		// U maps to Co position within valid range for given Cg
		// V maps to Cg in [-1, 1]
		// Valid region: Cg - 1 <= 2*Co <= 1 - Cg
		// Space utilization: 100%
		uint8_t U; // U coordinate in triangle mapping (maps to Co)
		uint8_t V; // V coordinate in triangle mapping (maps to Cg)
	};

	static inline FVector3f VectorToHemiOctL(const FVector3f& V)
	{
		// Require Z >= 0 (upper hemisphere only)
		UBPA_UCOMMON_ASSERT(V.Z >= 0.f);

		const float L = std::abs(V.X) + std::abs(V.Y) + V.Z;
		if (L < UBPA_UCOMMON_DELTA)
		{
			return FVector3f(0.f, 0.f, 0.f);
		}

		// Normalize by L1 norm to get hemispherical octahedral coordinates
		const float HemiOctX = V.X / L;
		const float HemiOctY = V.Y / L;

		return { HemiOctX, HemiOctY, L };
	}

	static inline FVector3f HemiOctToDir(const FVector2f& HemiOct)
	{
		// Reconstruct Z from hemispherical octahedral constraint: |x| + |y| + z = 1
		const float Z = 1.f - std::abs(HemiOct.X) - std::abs(HemiOct.Y);
		UBPA_UCOMMON_ASSERT(Z > -UBPA_UCOMMON_DELTA);

		// Reconstruct vector with original length
		return FVector3f(HemiOct.X, HemiOct.Y, Z);
	}

	static inline FVector3f HemiOctLToVector(const FVector3f& HemiOctL)
	{
		return HemiOctToDir(HemiOctL) * HemiOctL.Z;
	}

	struct FPackedHemiOct
	{
		FPackedHemiOct() {} // uninitialize
		FPackedHemiOct(EForceInit) : U(213), V(128) {} // ~(1/3,1/3,1/3)
		FPackedHemiOct(uint8_t InU, uint8_t InV) : U(InU), V(InV) {}
		FPackedHemiOct(const FVector2f& HemiOct)
		{
			// Rotate 45 degrees to map diamond [-1,1]x[-1,1] with |x|+|y|<=1 to square [0,1]x[0,1]
			// u = (x + y) / 2 + 0.5  maps to [0, 1]
			// v = (x - y) / 2 + 0.5  maps to [0, 1]
			const float Uf = (HemiOct.X + HemiOct.Y) * 0.5f + 0.5f;
			const float Vf = (HemiOct.X - HemiOct.Y) * 0.5f + 0.5f;
			U = ElementFloatClampToUint8(Uf);
			V = ElementFloatClampToUint8(Vf);
		}

		FVector2f Unpack() const
		{
			// Inverse rotation: recover HemiOct.X and HemiOct.Y from U and V
			// x = u + v - 1
			// y = u - v
			const float Uf = ElementUint8ToFloat(U);
			const float Vf = ElementUint8ToFloat(V);
			const float HemiOctX = Uf + Vf - 1.f;
			const float HemiOctY = Uf - Vf;
			return { HemiOctX, HemiOctY };
		}

		operator FUint8Vector2() const { return reinterpret_cast<const FUint8Vector2&>(*this); }

		uint8_t U, V; // Rotated hemispherical octahedral coordinates: u = (x+y)/2+0.5, v = (x-y)/2+0.5
	};
} // namespace UCommon

UBPA_UCOMMON_CODEC_TO_NAMESPACE(UCommonTest)
