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

#include "Vector.h"

#define UBPA_UCOMMON_UTILS_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
    constexpr float Pi = UCommon::Pi; \
}

namespace UCommon
{
	template<typename T>
	constexpr T Pow2(const T& V) noexcept { return V * V; }

	template<typename T>
	constexpr T Pow3(const T& V) noexcept { return V * V * V; }

	template<typename T>
	constexpr T Pow5(const T& V) noexcept { return Pow2(Pow2(V)) * V; }

	template<typename T, typename U>
	inline T LinearInterpolate(const T& val0, const T& val1, const U& Texcoord) noexcept
	{
		const U OneMinusTexcoord = static_cast<U>(1) - Texcoord;

		return val1 * Texcoord + val0 * OneMinusTexcoord;
	}

	template<typename T, typename U>
	inline T BilinearInterpolate(const T val2[4], const U Weights[4]) noexcept
	{
		T val1[2];
		T val;

		val1[0b0] = val2[0b01] * Weights[0b01] + val2[0b00] * Weights[0b00];
		val1[0b1] = val2[0b11] * Weights[0b11] + val2[0b10] * Weights[0b10];

		val = val1[0b1] + val1[0b0];

		return val;
	}

	template<typename T, typename U>
	inline T BilinearInterpolate(const T val2[4], const TVector2<U>& Texcoord, const TVector2<U>& OneMinusTexcoord) noexcept
	{
		T val1[2];
		T val;

		val1[0b0] = val2[0b01] * Texcoord.Y + val2[0b00] * OneMinusTexcoord.Y;
		val1[0b1] = val2[0b11] * Texcoord.Y + val2[0b10] * OneMinusTexcoord.Y;

		val = val1[0b1] * Texcoord.X + val1[0b0] * OneMinusTexcoord.X;

		return val;
	}

	template<typename T, typename U>
	inline T BilinearInterpolate(const T val2[4], const TVector2<U>& Texcoord) noexcept
	{
		const TVector2<U> OneMinusTexcoord = TVector2<U>(static_cast<U>(1)) - Texcoord;

		return BilinearInterpolate(val2, Texcoord, OneMinusTexcoord);
	}

	template<typename T, typename U>
	inline T TrilinearInterpolate(const T val3[8], const U Weights[8]) noexcept
	{
		T val2[4];
		T val1[2];
		T val;

		val2[0b00] = val3[0b001] * Weights[0b001] + val3[0b000] * Weights[0b000];
		val2[0b01] = val3[0b011] * Weights[0b011] + val3[0b010] * Weights[0b010];
		val2[0b10] = val3[0b101] * Weights[0b101] + val3[0b100] * Weights[0b100];
		val2[0b11] = val3[0b111] * Weights[0b111] + val3[0b110] * Weights[0b110];

		val1[0b0] = val2[0b01] + val2[0b00];
		val1[0b1] = val2[0b11] + val2[0b10];

		val = val1[0b1] + val1[0b0];

		return val;
	}

	template<typename T, typename U>
	inline T TrilinearInterpolate(const T val3[8], const TVector<U>& Texcoord, const TVector<U>& OneMinusTexcoord) noexcept
	{
		T val2[4];
		T val1[2];
		T val;

		val2[0b00] = val3[0b001] * Texcoord.Z + val3[0b000] * OneMinusTexcoord.Z;
		val2[0b01] = val3[0b011] * Texcoord.Z + val3[0b010] * OneMinusTexcoord.Z;
		val2[0b10] = val3[0b101] * Texcoord.Z + val3[0b100] * OneMinusTexcoord.Z;
		val2[0b11] = val3[0b111] * Texcoord.Z + val3[0b110] * OneMinusTexcoord.Z;

		val1[0b0] = val2[0b01] * Texcoord.Y + val2[0b00] * OneMinusTexcoord.Y;
		val1[0b1] = val2[0b11] * Texcoord.Y + val2[0b10] * OneMinusTexcoord.Y;

		val = val1[0b1] * Texcoord.X + val1[0b0] * OneMinusTexcoord.X;

		return val;
	}

	template<typename T, typename U>
	inline T TrilinearInterpolate(const T val3[8], const TVector<U>& Texcoord) noexcept
	{
		const TVector<U> OneMinusTexcoord = TVector<U>(static_cast<U>(1)) - Texcoord;

		return TrilinearInterpolate(val3, Texcoord, OneMinusTexcoord);
	}

	template<typename T, typename U>
	inline T TrilinearInterpolateZ(const T val3[8], const TVector<U>& Texcoord, const TVector<U>& OneMinusTexcoord) noexcept
	{
		T val2[4];
		T val1[2];
		T val;

		val2[0b00] = val3[0b001] * Texcoord.Y + val3[0b000] * OneMinusTexcoord.Y;
		val2[0b01] = val3[0b011] * Texcoord.Y + val3[0b010] * OneMinusTexcoord.Y;
		val2[0b10] = val3[0b101] * Texcoord.Y + val3[0b100] * OneMinusTexcoord.Y;
		val2[0b11] = val3[0b111] * Texcoord.Y + val3[0b110] * OneMinusTexcoord.Y;

		val1[0b0] = val2[0b01] * Texcoord.X + val2[0b00] * OneMinusTexcoord.X;
		val1[0b1] = val2[0b11] * Texcoord.X + val2[0b10] * OneMinusTexcoord.X;

		val = val1[0b1] * Texcoord.Z + val1[0b0] * OneMinusTexcoord.Z;

		return val;
	}

	template<typename T, typename U, uint64_t Stride = 1>
	inline T TrilinearInterpolateZ(const T val3[8], const TVector<U>& Texcoord) noexcept
	{
		const TVector<U> OneMinusTexcoord = TVector<U>(static_cast<U>(1)) - Texcoord;

		return TrilinearInterpolateZ(val3, Texcoord, OneMinusTexcoord);
	}

	static inline void RGBToYCoCg(float R, float G, float B, float& Y, float& Co, float& Cg)
	{
		const float RB = R + B;
		Y = (2 * G + RB) / 4;
		Co = (R - B) / 2;
		Cg = (2 * G - RB) / 4;
	}

	static inline FLinearColorRGB RGBToYCoCg(const FLinearColorRGB& RGB)
	{
		FLinearColorRGB YCoCg;
		RGBToYCoCg(RGB.X, RGB.Y, RGB.Z, YCoCg.X, YCoCg.Y, YCoCg.Z);
		return YCoCg;
	}

	static inline void YCoCgToRGB(float Y, float Co, float Cg, float& R, float& G, float& B)
	{
		const float Tmp = Y - Cg;
		R = Tmp + Co;
		G = Y + Cg;
		B = Tmp - Co;
	}

	static inline FLinearColorRGB YCoCgToRGB(const FLinearColorRGB& YCoCg)
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

	static constexpr float Pi = 3.1415926f;

	static inline FVector4f UniformSampleSphere(FVector2f E)
	{
		float Phi = 2 * Pi * E.X;
		float CosTheta = 1 - 2 * E.Y;
		float SinTheta = std::sqrt(1 - CosTheta * CosTheta);

		FVector3f H;
		H.X = SinTheta * cos(Phi);
		H.Y = SinTheta * sin(Phi);
		H.Z = CosTheta;

		constexpr float PDF = 1.0f / (4 * Pi);

		return FVector4f(H, PDF);
	}

	constexpr float GlodenRatio = 1.618034f;

	//Reference: https://zhuanlan.zhihu.com/p/25988652?group_id=828963677192491008
	static inline FVector4f FibonacciSpherePoint(int N, int n)
	{
		constexpr float Phi = 2.f * Pi / GlodenRatio;
		const float Z = (2.f * n + 1.f) / N - 1.f;
		const float r = std::sqrt(1.f - Z * Z);
		const float X = r * std::cos(Phi * n);
		const float Y = r * std::sin(Phi * n);

		constexpr float PDF = 1.0f / (4.f * Pi);

		return FVector4f(X, Y, Z, PDF);
	}
}

UBPA_UCOMMON_UTILS_TO_NAMESPACE(UCommonTest)
