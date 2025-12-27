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

	for (uint64_t Index = 0; Index < 3; Index++)
	{
		float a = std::clamp(std::floor(RGBScale[Index] * 255.f) / 255.f, 0.f, 1.f);
		float b = std::clamp(std::ceil(RGBScale[Index] * 255.f) / 255.f, 0.f, 1.f);
		if (std::abs(a * MScale * Multiplier - Color[Index]) < std::abs(b * MScale * Multiplier - Color[Index]))
		{
			RGBScale[Index] = a;
		}
		else
		{
			RGBScale[Index] = b;
		}
	}

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

	for (uint64_t Index = 0; Index < 3; Index++)
	{
		float a = std::clamp(std::floor(RGBScale[Index] * 255.f) / 255.f, 0.f, 1.f);
		float b = std::clamp(std::ceil(RGBScale[Index] * 255.f) / 255.f, 0.f, 1.f);
		if (std::abs(a * MScale * Multiplier - Color[Index]) < std::abs(b * MScale * Multiplier - Color[Index]))
		{
			RGBScale[Index] = a;
		}
		else
		{
			RGBScale[Index] = b;
		}
	}

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

	// Ensure we always round up to next largest M
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
	if (MaxRGB == 0)
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

	float Multipiler = Pow2(D / (k * D + 1.f));
	float Ratio = std::min(1.f, Multipiler / MaxRGB);

	FLinearColorRGB RGBScale = (Color * Ratio / Multipiler).Clamp(0.f, 1.f);

	for (uint64_t Index = 0; Index < 3; Index++)
	{
		float a = std::clamp(std::floor(RGBScale[Index] * 255.f) / 255.f, 0.f, 1.f);
		float b = std::clamp(std::ceil(RGBScale[Index] * 255.f) / 255.f, 0.f, 1.f);
		if (std::abs(a * Multipiler - Color[Index]) < std::abs(b * Multipiler - Color[Index]))
		{
			RGBScale[Index] = a;
		}
		else
		{
			RGBScale[Index] = b;
		}
	}

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

	for (uint64_t Index = 0; Index < 3; Index++)
	{
		float a = std::clamp(std::floor(RGBScale[Index] * 255.f) / 255.f, 0.f, 1.f);
		float b = std::clamp(std::ceil(RGBScale[Index] * 255.f) / 255.f, 0.f, 1.f);
		if (std::abs(a * Multiplier - Color[Index]) < std::abs(b * Multiplier - Color[Index]))
		{
			RGBScale[Index] = a;
		}
		else
		{
			RGBScale[Index] = b;
		}
	}

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

	// Compute multiplier: L = (D/(k*D+1))^2
	float Multiplier = Pow2(D / (k * D + 1.f));

	float Ratio = std::min(1.f, Multiplier / MaxRGB);

	FLinearColorRGB RGBScale = (Color * Ratio / Multiplier).Clamp(0.f, 1.f);

	return RGBScale * Multiplier;
}

//===========================================
// RGBV Codec Implementation
//===========================================

UCommon::FLinearColor UCommon::EncodeRGBV(FLinearColorRGB Color, float MaxValue, float S, float InLowClamp)
{
	Color = Color.Max(0.f);

	float L = Color.MaxComponent();
	if (L == 0.f)
	{
		return FLinearColor(0.f, 0.f, 0.f, InLowClamp);
	}

	// Clamp L to [0, MaxValue]
	L = std::min(L, MaxValue);

	// v = sqrt((sM+1)/(sM) * sL/(sL+1))
	float sM = S * MaxValue;
	float sL = S * L;
	float V = std::sqrt((sM + 1.f) / sM * sL / (sL + 1.f));

	// Keep well above zero to avoid clamps in the compressor
	V = std::max(V, InLowClamp);

	// Ensure we always round up to next largest V
	V = std::min(1.0f, std::ceil(V * 255.0f) / 255.0f);

	// Decode back to get the actual L after quantization: L = V^2 / (k*V^2 + b)
	float k = RGBV_GetK(S);
	float b = RGBV_GetB(MaxValue, S);
	float V2 = Pow2(V);
	float LDecoded = V2 / (k * V2 + b);

	// RGB_encoded = Color / LDecoded, decode: RGB = RGB_encoded * L
	FLinearColorRGB RGBScale = (Color / LDecoded).Clamp(0.f, 1.f);

	// Quantization optimization: choose floor or ceil for each channel
	for (uint64_t Index = 0; Index < 3; Index++)
	{
		float FloorVal = std::clamp(std::floor(RGBScale[Index] * 255.f) / 255.f, 0.f, 1.f);
		float CeilVal = std::clamp(std::ceil(RGBScale[Index] * 255.f) / 255.f, 0.f, 1.f);
		float FloorDecoded = FloorVal * LDecoded;
		float CeilDecoded = CeilVal * LDecoded;
		if (std::abs(FloorDecoded - Color[Index]) < std::abs(CeilDecoded - Color[Index]))
		{
			RGBScale[Index] = FloorVal;
		}
		else
		{
			RGBScale[Index] = CeilVal;
		}
	}

	return FLinearColor(RGBScale, V);
}

UCommon::FLinearColor UCommon::EncodeRGBVWithV(FLinearColorRGB Color, float MaxValue, float S, float V)
{
	Color = Color.Max(0.f);

	float L = Color.MaxComponent();
	if (L == 0.f || MaxValue == 0.f)
	{
		return FLinearColor(0.f, 0.f, 0.f, V);
	}

	// Decode back to get the actual L from V: L = V^2 / (k*V^2 + b)
	float k = RGBV_GetK(S);
	float b = RGBV_GetB(MaxValue, S);
	float V2 = Pow2(V);
	float LDecoded = V2 / (k * V2 + b);

	// RGB_encoded = Color / LDecoded, decode: RGB = RGB_encoded * L
	FLinearColorRGB RGBScale = (Color / LDecoded).Clamp(0.f, 1.f);

	// Quantization optimization: choose floor or ceil for each channel
	for (uint64_t Index = 0; Index < 3; Index++)
	{
		float FloorVal = std::clamp(std::floor(RGBScale[Index] * 255.f) / 255.f, 0.f, 1.f);
		float CeilVal = std::clamp(std::ceil(RGBScale[Index] * 255.f) / 255.f, 0.f, 1.f);
		float FloorDecoded = FloorVal * LDecoded;
		float CeilDecoded = CeilVal * LDecoded;
		if (std::abs(FloorDecoded - Color[Index]) < std::abs(CeilDecoded - Color[Index]))
		{
			RGBScale[Index] = FloorVal;
		}
		else
		{
			RGBScale[Index] = CeilVal;
		}
	}

	return FLinearColor(RGBScale, V);
}

float UCommon::EncodeRGBV(float L, float MaxValue, float S)
{
	if (L <= 0.f)
	{
		return 0.f;
	}

	// Clamp L to [0, MaxValue]
	L = std::min(L, MaxValue);

	// v = sqrt((sM+1)/(sM) * sL/(sL+1))
	float sM = S * MaxValue;
	float sL = S * L;
	float V = std::sqrt((sM + 1.f) / sM * sL / (sL + 1.f));

	return V;
}

UCommon::FLinearColorRGB UCommon::MapToValidColorRGBV(FLinearColorRGB Color, float MaxValue, float S, float InLowClamp)
{
	Color = Color.Max(0.f);

	float L = Color.MaxComponent();
	if (L == 0.f)
	{
		return FLinearColorRGB(0.f, 0.f, 0.f);
	}

	// Clamp L to [0, MaxValue]
	L = std::min(L, MaxValue);

	// v = sqrt((sM+1)/(sM) * sL/(sL+1))
	float sM = S * MaxValue;
	float sL = S * L;
	float V = std::sqrt((sM + 1.f) / sM * sL / (sL + 1.f));

	// Keep well above zero to avoid clamps in the compressor
	V = std::max(V, InLowClamp);

	// Clamp V to valid range
	V = std::min(1.0f, V);

	// Decode back to get the actual L: L = V^2 / (k*V^2 + b)
	float k = RGBV_GetK(S);
	float b = RGBV_GetB(MaxValue, S);
	float V2 = Pow2(V);
	float LDecoded = V2 / (k * V2 + b);

	// RGB_encoded = Color / LDecoded, then decode: RGB = RGB_encoded * LDecoded
	FLinearColorRGB RGBScale = (Color / LDecoded).Clamp(0.f, 1.f);

	return RGBScale * LDecoded;
}
