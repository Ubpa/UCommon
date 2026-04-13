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

#include <UCommon/Codec.h>

//===========================================
// RGBM Codec Implementation
//===========================================

UCommon::FLinearColor UCommon::EncodeRGBM(FLinearColorRGB Color, float Multiplier, float InLowClamp)
{
	Color = Color.Max(0.f);

	if (Color.MaxComponent() == 0.f || Multiplier == 0.f)
	{
		return FLinearColor(0.f, 0.f, 0.f, InLowClamp);
	}

	const FLinearColorRGB RGB = Color / Multiplier;
	const float MaxRGB = RGB.MaxComponent();

	float SqrtMaxRGB = std::sqrt(MaxRGB);

	// Keep well above zero to avoid clamps in the compressor
	SqrtMaxRGB = std::max(SqrtMaxRGB, InLowClamp);

	// Ensure we always round up to next largest M
	const float SqrtMScale = std::min(1.0f, std::ceil(SqrtMaxRGB * 255.0f) / 255.0f);
	const float MScale = Pow2(SqrtMScale);
	const float Ratio = std::min(1.f, MScale / MaxRGB);

	FLinearColorRGB RGBScale = (RGB * Ratio / MScale).Clamp(0.f, 1.f);

	return FLinearColor(RGBScale, SqrtMScale);
}

float UCommon::EncodeRGBM(float M, float Multiplier, float InLowClamp)
{
	const float MScale = M / Multiplier;

	float SqrtMScale = std::sqrt(std::max(0.f, MScale));

	// Keep well above zero to avoid clamps in the compressor
	SqrtMScale = std::max(SqrtMScale, InLowClamp);

	// Ensure we always round up to next largest M
	SqrtMScale = std::min(1.0f, std::ceil(SqrtMScale * 255.0f) / 255.0f);

	return SqrtMScale;
}

UCommon::FLinearColor UCommon::EncodeRGBMWithM(FLinearColorRGB Color, float Multiplier, float SqrtMScale)
{
	Color = Color.Max(0.f);

	if (Color.MaxComponent() == 0.f || Multiplier == 0.f)
	{
		return FLinearColor(0.f, 0.f, 0.f, SqrtMScale);
	}

	const FLinearColorRGB RGB = Color / Multiplier;
	const float MaxRGB = RGB.MaxComponent();
	const float MScale = Pow2(SqrtMScale);
	const float Ratio = std::min(1.f, MScale / MaxRGB);

	FLinearColorRGB RGBScale = (RGB * Ratio / MScale).Clamp(0.f, 1.f);

	return FLinearColor(RGBScale, SqrtMScale);
}

UCommon::FLinearColorRGB UCommon::MapToValidColorRGBM(FLinearColorRGB Color, float Multiplier, float InLowClamp)
{
	Color = Color.Max(0.f);

	if (Color.MaxComponent() == 0.f || Multiplier == 0.f)
	{
		return FLinearColorRGB(0.f, 0.f, 0.f);
	}

	const FLinearColorRGB RGB = Color / Multiplier;
	const float MaxRGB = RGB.MaxComponent();

	float SqrtMaxRGB = std::sqrt(MaxRGB);

	// Keep well above zero to avoid clamps in the compressor
	SqrtMaxRGB = std::max(SqrtMaxRGB, InLowClamp);

	// Clamp to valid range
	const float SqrtMScale = std::min(1.0f, SqrtMaxRGB);
	const float MScale = Pow2(SqrtMScale);
	const float Ratio = std::min(1.f, MScale / MaxRGB);

	FLinearColorRGB RGBScale = (RGB * Ratio / MScale).Clamp(0.f, 1.f);

	return RGBScale * MScale * Multiplier;
}

//===========================================
// RGBD Codec Implementation
//===========================================

float UCommon::RGBD_GetK(float MaxValue)
{
	return 1.f / std::sqrt(MaxValue) - 1.f;
}

float UCommon::RGBD_GetMaxValue(float K)
{
	return 1.f / (Pow2(K + 1.f));
}

UCommon::FLinearColor UCommon::EncodeRGBD(FLinearColorRGB Color, float MaxValue, float InLowClamp)
{
	Color = Color.Max(0.f);

	const float MaxRGB = Color.MaxComponent();
	if (MaxRGB == 0.f)
	{
		return FLinearColor(0.f, 0.f, 0.f, InLowClamp);
	}
	if (MaxValue == 0.f)
	{
		return FLinearColor(0.f, 0.f, 0.f, InLowClamp);
	}
	const float k = RGBD_GetK(MaxValue);
	const float SqrtMaxRGB = std::sqrt(MaxRGB);
	float D = SqrtMaxRGB / (1.f - SqrtMaxRGB * k);

	// Keep well above zero to avoid clamps in the compressor
	D = std::max(D, InLowClamp);

	// Ensure we always round up to next largest D
	D = std::min(1.0f, std::ceil(D * 255.0f) / 255.0f);

	float Multiplier = Pow2(D / (k * D + 1.f));
	float Ratio = std::min(1.f, Multiplier / MaxRGB);

	FLinearColorRGB RGBScale = (Color * Ratio / Multiplier).Clamp(0.f, 1.f);

	return FLinearColor(RGBScale, D);
}

UCommon::FLinearColor UCommon::EncodeRGBDWithD(FLinearColorRGB Color, float MaxValue, float D)
{
	Color = Color.Max(0.f);

	const float MaxRGB = Color.MaxComponent();
	if (MaxRGB == 0.f || MaxValue == 0.f)
	{
		return FLinearColor(0.f, 0.f, 0.f, D);
	}

	const float k = RGBD_GetK(MaxValue);
	float Multiplier = Pow2(D / (k * D + 1.f));
	float Ratio = std::min(1.f, Multiplier / MaxRGB);

	FLinearColorRGB RGBScale = (Color * Ratio / Multiplier).Clamp(0.f, 1.f);

	return FLinearColor(RGBScale, D);
}

UCommon::FLinearColorRGB UCommon::MapToValidColorRGBD(FLinearColorRGB Color, float MaxValue, float InLowClamp)
{
	Color = Color.Max(0.f);

	const float MaxRGB = Color.MaxComponent();
	if (MaxRGB == 0.f)
	{
		return FLinearColorRGB(0.f, 0.f, 0.f);
	}

	const float k = RGBD_GetK(MaxValue);
	const float SqrtMaxRGB = std::sqrt(MaxRGB);
	float D = SqrtMaxRGB / (1.f - SqrtMaxRGB * k);

	// Keep well above zero to avoid clamps in the compressor
	D = std::max(D, InLowClamp);

	// Clamp D to valid range
	D = std::min(1.0f, D);

	// Compute multiplier = (D/(k*D+1))^2
	float Multiplier = Pow2(D / (k * D + 1.f));

	float Ratio = std::min(1.f, Multiplier / MaxRGB);

	FLinearColorRGB RGBScale = (Color * Ratio / Multiplier).Clamp(0.f, 1.f);

	return RGBScale * Multiplier;
}

//===========================================
// RGBV Codec Implementation
//===========================================

float UCommon::RGBV_ComputeIntegral(float MaxValue, float S)
{
	UBPA_UCOMMON_ASSERT(MaxValue > 0.f);
	UBPA_UCOMMON_ASSERT(S > -1.f / MaxValue);

	float InvM = 1.f / MaxValue;

	// Case s = 0: I = M/3
	if (std::abs(S) < UBPA_UCOMMON_DELTA)
	{
		return MaxValue / 3.f;
	}

	float b = S + InvM; // b = s + 1/M

	// Case s > 0: I = (1/s) * (sqrt((s+1/M)/s) * artanh(sqrt(s/(s+1/M))) - 1)
	if (S > 0.f)
	{
		float ratio = b / S;            // (s + 1/M) / s
		float sqrtRatio = std::sqrt(ratio);
		float arg = std::sqrt(S / b);   // sqrt(s / (s + 1/M))
		// artanh(x) = 0.5 * ln((1+x)/(1-x))
		float artanhVal = std::atanh(arg);
		return (sqrtRatio * artanhVal - 1.f) / S;
	}

	// Case -1/M < s < 0: I = (1/s) * (sqrt((s+1/M)/(-s)) * arctan(sqrt(-s/(s+1/M))) - 1)
	float negS = -S;
	float ratio = b / negS;          // (s + 1/M) / (-s)
	float sqrtRatio = std::sqrt(ratio);
	float arg = std::sqrt(negS / b); // sqrt(-s / (s + 1/M))
	float arctanVal = std::atan(arg);
	return (sqrtRatio * arctanVal - 1.f) / S; // Note: S < 0, so division by S gives correct sign
}

float UCommon::RGBV_ComputeIntegral2(float MaxValue, float S)
{
	UBPA_UCOMMON_ASSERT(MaxValue > 0.f);
	UBPA_UCOMMON_ASSERT(S > -1.f / MaxValue);

	// Case s = 0: I2 = M^2 / 5
	if (std::abs(S) < UBPA_UCOMMON_DELTA)
	{
		return Pow2(MaxValue) / 5.f;
	}

	// General case: I2 = (M - 3*I1) / (2*s)
	// where I1 = RGBV_ComputeIntegral(M, s)
	float I1 = RGBV_ComputeIntegral(MaxValue, S);
	return (MaxValue - 3.f * I1) / (2.f * S);
}

float UCommon::RGBV_SolveS2(float MaxValue, float Integral2Value, float Tolerance, uint64_t MaxIterations)
{
	UBPA_UCOMMON_ASSERT(MaxValue > 0.f);
	UBPA_UCOMMON_ASSERT(Integral2Value > 0.f);

	float InvM = 1.f / MaxValue;
	float I2AtZero = Pow2(MaxValue) / 5.f; // I2(s=0) = M^2/5

	// If target I2 equals M^2/5, return s = 0
	if (std::abs(Integral2Value - I2AtZero) < Tolerance)
	{
		return 0.f;
	}

	// I2(s) is monotonically decreasing:
	// - As s -> -1/M+, I2 -> +infinity
	// - As s -> +infinity, I2 -> 0
	// So if I2 > M^2/5, we need s < 0; if I2 < M^2/5, we need s > 0

	float sLow, sHigh;

	if (Integral2Value > I2AtZero)
	{
		// Need s < 0 (closer to -1/M gives larger I2)
		sLow = -InvM + Tolerance; // Just above -1/M
		sHigh = 0.f;
	}
	else
	{
		// Need s > 0 (larger s gives smaller I2)
		// Find upper bound by exponential search
		sLow = 0.f;
		sHigh = 1.f;
		// Double sHigh until I2(sHigh) < target
		while (RGBV_ComputeIntegral2(MaxValue, sHigh) > Integral2Value && sHigh < 1e10f)
		{
			sLow = sHigh;
			sHigh *= 2.f;
		}
	}

	// Use bisection method for robustness
	float sMid = 0.f;
	for (uint64_t i = 0; i < MaxIterations; ++i)
	{
		sMid = (sLow + sHigh) * 0.5f;

		float currentI2 = RGBV_ComputeIntegral2(MaxValue, sMid);

		float error = currentI2 - Integral2Value;

		if (std::abs(error) < Tolerance)
		{
			return sMid;
		}

		// I2(s) is decreasing, so:
		// If currentI2 > target, we need larger s (to get smaller I2)
		// If currentI2 < target, we need smaller s (to get larger I2)
		if (error > 0.f)
		{
			sLow = sMid;
		}
		else
		{
			sHigh = sMid;
		}
	}

	return sMid;
}

float UCommon::RGBV_SolveS(float MaxValue, float IntegralValue, float Tolerance, uint64_t MaxIterations)
{
	UBPA_UCOMMON_ASSERT(MaxValue > 0.f);
	UBPA_UCOMMON_ASSERT(IntegralValue > 0.f);

	float InvM = 1.f / MaxValue;
	float SAtZero = MaxValue / 3.f; // I(s=0) = M/3

	// If target I equals M/3, return s = 0
	if (std::abs(IntegralValue - SAtZero) < Tolerance)
	{
		return 0.f;
	}

	// I(s) is monotonically decreasing:
	// - As s -> -1/M+, I -> +infinity
	// - As s -> +infinity, I -> 0
	// So if I > M/3, we need s < 0; if I < M/3, we need s > 0

	float sLow, sHigh;

	if (IntegralValue > SAtZero)
	{
		// Need s < 0 (closer to -1/M gives larger I)
		sLow = -InvM + Tolerance; // Just above -1/M
		sHigh = 0.f;
	}
	else
	{
		// Need s > 0 (larger s gives smaller I)
		// Find upper bound by exponential search
		sLow = 0.f;
		sHigh = 1.f;
		// Double sHigh until I(sHigh) < target
		while (RGBV_ComputeIntegral(MaxValue, sHigh) > IntegralValue && sHigh < 1e10f)
		{
			sLow = sHigh;
			sHigh *= 2.f;
		}
	}

	// Use bisection method for robustness
	float sMid = 0.f;
	for (uint64_t i = 0; i < MaxIterations; ++i)
	{
		sMid = (sLow + sHigh) * 0.5f;

		float currentI = RGBV_ComputeIntegral(MaxValue, sMid);

		float error = currentI - IntegralValue;

		if (std::abs(error) < Tolerance)
		{
			return sMid;
		}

		// I(s) is decreasing, so:
		// If currentI > target, we need larger s (to get smaller I)
		// If currentI < target, we need smaller s (to get larger I)
		if (error > 0.f)
		{
			sLow = sMid;
		}
		else
		{
			sHigh = sMid;
		}
	}

	return sMid;
}

UCommon::FLinearColor UCommon::EncodeRGBV(FLinearColorRGB Color, float MaxValue, float S, float InLowClamp)
{
	UBPA_UCOMMON_ASSERT(S >= -1.f / MaxValue);
	if (S == -1.f / MaxValue)
	{
		// Degenerate case: v = 1 for all L
		return FLinearColor(1.f, 1.f, 1.f, InLowClamp);
	}

	Color = Color.Max(0.f);

	float L = Color.MaxComponent();
	if (L == 0.f)
	{
		return FLinearColor(0.f, 0.f, 0.f, InLowClamp);
	}

	// Clamp L to [0, MaxValue]
	L = std::min(L, MaxValue);

	// v = sqrt((sM+1)/(sL+1) * L/M)
	float sM = S * MaxValue;
	float sL = S * L;
	float V = std::sqrt((sM + 1.f) / (sL + 1.f) * L / MaxValue);

	// Keep well above zero to avoid clamps in the compressor
	V = std::max(V, InLowClamp);

	// Ensure we always round up to next largest V
	V = std::min(1.0f, std::ceil(V * 255.0f) / 255.0f);

	// Decode back to get the actual L after quantization
	float DecodedL = DecodeRGBV(V, MaxValue, S);

	FLinearColorRGB RGBScale = (Color / DecodedL).Clamp(0.f, 1.f);

	return FLinearColor(RGBScale, V);
}

UCommon::FLinearColor UCommon::EncodeRGBVWithV(FLinearColorRGB Color, float MaxValue, float S, float V)
{
	UBPA_UCOMMON_ASSERT(MaxValue > 0.f);
	UBPA_UCOMMON_ASSERT(S >= -1.f / MaxValue);

	Color = Color.Max(0.f);

	float L = Color.MaxComponent();
	if (L == 0.f)
	{
		return FLinearColor(0.f, 0.f, 0.f, V);
	}

	// Decode back to get the actual L from V
	float DecodedL = DecodeRGBV(V, MaxValue, S);

	FLinearColorRGB RGBScale = (Color / DecodedL).Clamp(0.f, 1.f);

	return FLinearColor(RGBScale, V);
}

float UCommon::EncodeRGBV(float L, float MaxValue, float S)
{
	UBPA_UCOMMON_ASSERT(MaxValue > 0.f);
	UBPA_UCOMMON_ASSERT(S >= -1.f / MaxValue);
	UBPA_UCOMMON_ASSERT(L >= 0.f);

	// Clamp L to [0, MaxValue]
	L = std::min(L, MaxValue);

	// v = sqrt((sM+1)/(sL+1) * L/M)
	float sM = S * MaxValue;
	float sL = S * L;
	float V = std::sqrt((sM + 1.f) / (sL + 1.f) * L / MaxValue);

	return V;
}

UCommon::FLinearColorRGB UCommon::MapToValidColorRGBV(FLinearColorRGB Color, float MaxValue, float S, float InLowClamp)
{
	UBPA_UCOMMON_ASSERT(MaxValue > 0.f);
	UBPA_UCOMMON_ASSERT(S >= -1.f / MaxValue);

	Color = Color.Max(0.f);

	float L = Color.MaxComponent();
	if (L == 0.f)
	{
		return FLinearColorRGB(0.f, 0.f, 0.f);
	}

	// Clamp L to [0, MaxValue]
	L = std::min(L, MaxValue);

	// v = sqrt((sM+1)/(sL+1) * L/M)
	float sM = S * MaxValue;
	float sL = S * L;
	float V = std::sqrt((sM + 1.f) / (sL + 1.f) * L / MaxValue);

	// Keep well above zero to avoid clamps in the compressor
	V = std::max(V, InLowClamp);

	// Clamp V to valid range
	V = std::min(1.0f, V);

	// Decode back to get the actual L
	float DecodedL = DecodeRGBV(V, MaxValue, S);

	FLinearColorRGB RGBScale = (Color / DecodedL).Clamp(0.f, 1.f);

	return RGBScale * DecodedL;
}
