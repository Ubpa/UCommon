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
#include "Half.h"

#define UBPA_UCOMMON_UTILS_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
    constexpr float Pi = UCommon::Pi; \
    using EElementType = UCommon::EElementType; \
    using EOwnership = UCommon::EOwnership; \
    template<typename T> constexpr bool IsDirectSupported = UCommon::IsDirectSupported<T>; \
    template<typename T> constexpr bool IsSupported = UCommon::IsSupported<T>; \
    template<typename T> constexpr EElementType ElementTypeOf = UCommon::ElementTypeOf<T>; \
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

	enum class EElementType : std::uint64_t
	{
		Unknown,
		Uint8,        /** 1 byte unsigned integer number, 0-255 */
		Half,         /** 2 bytes floating point number, IEEE 754 */
		Float,        /** 4 bytes floating point number */
		Double,       /** 8 bytes floating point number */
		ASTC_4x4,     /** 128 bits / block => 8.00 bits / pixel */
		ASTC_6x6,     /** 128 bits / block => 3.56 bits / pixel */
		ASTC_8x8,     /** 128 bits / block => 2.00 bits / pixel */
		ASTC_10x10,   /** 128 bits / block => 1.28 bits / pixel */
		ASTC_12x12,   /** 128 bits / block => 0.89 bits / pixel */
		AdaptiveASTC, /** Adative Uint8/ASTC_4x4/ASTC_8x8 */
		BC7,          /** 128 bits / block => 8.00 bits / pixel */
		BQ,           /** 128 bits / block => 8.00 bits / channel*/
	};
	inline uint64_t ElementGetSize(EElementType ElementType) noexcept
	{
		switch (ElementType)
		{
		case EElementType::Uint8:
			return 1;
		case EElementType::Half:
			return sizeof(FHalf);
		case EElementType::Float:
			return sizeof(float);
		case EElementType::Double:
			return sizeof(double);
		default:
			UBPA_UCOMMON_NO_ENTRY();
			return 0;
		}
	}
	template<typename T>
	constexpr bool IsDirectSupported = false
		|| std::is_same<T, std::uint8_t>::value
		|| std::is_same<T, FHalf>::value
		|| std::is_same<T, float>::value
		|| std::is_same<T, double>::value;
	template<typename T>
	constexpr bool IsSupported = (IsVector_v<T> && IsDirectSupported<typename TRemoveVector<T>::value_type>) || IsDirectSupported<T>;
	template<typename T>
	constexpr EElementType ElementTypeOf = std::is_same<typename TRemoveVector<T>::value_type, std::uint8_t>::value ? EElementType::Uint8 :
		(std::is_same<typename TRemoveVector<T>::value_type, FHalf>::value ? EElementType::Half :
		(std::is_same<typename TRemoveVector<T>::value_type, float>::value ? EElementType::Float :
		(std::is_same<typename TRemoveVector<T>::value_type, double>::value ? EElementType::Double :
		EElementType::Unknown)));
	static float ElementUint6ToFloat(uint8_t Element) noexcept { return Element / 63.f; }
	static float ElementUint7ToFloat(uint8_t Element) noexcept { return Element / 127.f; }
	static float ElementUint8ToFloat(uint8_t Element) noexcept { return Element / 255.f; }
	static inline float ElementHalfToFloat(FHalf Element) noexcept { return static_cast<float>(Element); }
	static uint8_t ElementFloatToUint7(float Element) noexcept
	{
		UBPA_UCOMMON_ASSERT(0.f <= Element && Element <= 1.f);
		return static_cast<uint8_t>(std::roundf(Element * 127.f));
	}
	static uint8_t ElementFloatClampToUint7(float Element) noexcept { return static_cast<uint8_t>(std::roundf(Clamp(Element * 127.f, 0.f, 127.f))); }
	static uint8_t ElementFloatToUint8(float Element) noexcept
	{
		UBPA_UCOMMON_ASSERT(0.f <= Element && Element <= 1.f);
		return static_cast<uint8_t>(std::roundf(Element * 255.f));
	}
	static uint8_t ElementFloatClampToUint6(float Element) noexcept { return static_cast<uint8_t>(std::roundf(Clamp(Element * 63.f, 0.f, 63.f))); }
	static uint8_t ElementFloatClampToUint8(float Element) noexcept { return static_cast<uint8_t>(std::roundf(Clamp(Element * 255.f, 0.f, 255.f))); }
	static FHalf ElementFloatToHalf(float Element) noexcept { return static_cast<FHalf>(Element); }
	static FHalf ElementUint7ToHalf(uint8_t Element) noexcept { return static_cast<FHalf>(ElementUint7ToFloat(Element)); }
	static FHalf ElementUint8ToHalf(uint8_t Element) noexcept { return static_cast<FHalf>(ElementUint8ToFloat(Element)); }
	static uint8_t ElementHalfToUint8(FHalf Element) noexcept { return ElementFloatToUint8(ElementHalfToFloat(Element)); }
	static uint8_t ElementHalfClampToUint8(FHalf Element) noexcept { return ElementFloatClampToUint8(ElementHalfToFloat(Element)); }
	static double ElementUint7ToDouble(uint8_t Element) noexcept { return Element / 127.; }
	static double ElementUint8ToDouble(uint8_t Element) noexcept { return Element / 255.; }
	static double ElementHalfToDouble(FHalf Element) noexcept { return static_cast<double>(Element); }
	static uint8_t ElementDoubleToUint8(double Element) noexcept
	{
		UBPA_UCOMMON_ASSERT(0. <= Element && Element <= 1.);
		return static_cast<uint8_t>(std::round(Element * 255.));
	}
	static uint8_t ElementDoubleClampToUint8(double Element) noexcept { return static_cast<uint8_t>(std::round(Clamp(Element, 0., 1.) * 255.)); }
	static FHalf ElementDoubleToHalf(double Element) noexcept { return ElementFloatToHalf(static_cast<float>(Element)); }

	static FLinearColorRGB ElementColorToLinearColor(const FColorRGB& Element) noexcept
	{ return { ElementUint8ToFloat(Element.X), ElementUint8ToFloat(Element.Y), ElementUint8ToFloat(Element.Z) }; }

	static FColorRGB ElementLinearColorToColor(const FLinearColorRGB& Element) noexcept
	{ return { ElementFloatToUint8(Element.X), ElementFloatToUint8(Element.Y), ElementFloatToUint8(Element.Z) }; }

	static FColorRGB ElementLinearColorClampToColor(const FLinearColorRGB& Element) noexcept
	{ return { ElementFloatClampToUint8(Element.X), ElementFloatClampToUint8(Element.Y), ElementFloatClampToUint8(Element.Z) }; }

	static FLinearColor ElementColorToLinearColor(const FColor& Element) noexcept
	{ return { ElementUint8ToFloat(Element.X), ElementUint8ToFloat(Element.Y), ElementUint8ToFloat(Element.Z), ElementUint8ToFloat(Element.W) }; }

	static FColor ElementLinearColorToColor(const FLinearColor& Element) noexcept
	{ return { ElementFloatToUint8(Element.X), ElementFloatToUint8(Element.Y), ElementFloatToUint8(Element.Z), ElementFloatToUint8(Element.W) }; }

	static FColor ElementLinearColorClampToColor(const FLinearColor& Element) noexcept
	{ return { ElementFloatClampToUint8(Element.X), ElementFloatClampToUint8(Element.Y), ElementFloatClampToUint8(Element.Z), ElementFloatClampToUint8(Element.W) }; }

	static FDoubleColorRGB ElementColorToDoubleColor(const FColorRGB& Element) noexcept
	{ return { ElementUint8ToDouble(Element.X), ElementUint8ToDouble(Element.Y), ElementUint8ToDouble(Element.Z) }; }

	static FColorRGB ElementDoubleColorToColor(const FDoubleColorRGB& Element) noexcept
	{ return { ElementDoubleToUint8(Element.X), ElementDoubleToUint8(Element.Y), ElementDoubleToUint8(Element.Z) }; }

	static FColorRGB ElementDoubleColorClampToColor(const FDoubleColorRGB& Element) noexcept
	{ return { ElementDoubleClampToUint8(Element.X), ElementDoubleClampToUint8(Element.Y), ElementDoubleClampToUint8(Element.Z) }; }

	static FDoubleColor ElementColorToDoubleColor(const FColor& Element) noexcept
	{ return { ElementUint8ToDouble(Element.X), ElementUint8ToDouble(Element.Y), ElementUint8ToDouble(Element.Z), ElementUint8ToDouble(Element.W) }; }

	static FColor ElementDoubleColorToColor(const FDoubleColor& Element) noexcept
	{ return { ElementDoubleToUint8(Element.X), ElementDoubleToUint8(Element.Y), ElementDoubleToUint8(Element.Z), ElementDoubleToUint8(Element.W) }; }

	static FColor ElementDoubleColorClampToColor(const FDoubleColor& Element) noexcept
	{ return { ElementDoubleClampToUint8(Element.X), ElementDoubleClampToUint8(Element.Y), ElementDoubleClampToUint8(Element.Z), ElementDoubleClampToUint8(Element.W) }; }

	static bool ElementIsASTC(EElementType ElementType) noexcept
	{
		return (uint64_t)ElementType >= (uint64_t)EElementType::ASTC_4x4
			&& (uint64_t)ElementType <= (uint64_t)EElementType::ASTC_12x12;
	}
	static FUint64Vector2 ElementGetBlockSize(EElementType ElementType) noexcept
	{
		switch (ElementType)
		{
		case EElementType::ASTC_4x4: return FUint64Vector2(4);
		case EElementType::ASTC_6x6: return FUint64Vector2(6);
		case EElementType::ASTC_8x8: return FUint64Vector2(8);
		case EElementType::ASTC_10x10: return FUint64Vector2(10);
		case EElementType::ASTC_12x12: return FUint64Vector2(12);
		case EElementType::AdaptiveASTC: return FUint64Vector2(8);
		case EElementType::BC7: return FUint64Vector2(4);
		case EElementType::BQ: return FUint64Vector2(4);
		default: return FUint64Vector2(1);
		}
	}
	static EElementType ElementGetASTC(uint64_t BlockSize) noexcept
	{
		switch (BlockSize)
		{
		case 1: return EElementType::Uint8;
		case 4: return EElementType::ASTC_4x4;
		case 6: return EElementType::ASTC_6x6;
		case 8: return EElementType::ASTC_8x8;
		case 10: return EElementType::ASTC_10x10;
		case 12: return EElementType::ASTC_12x12;
		default: return EElementType::Unknown;
		}
	}

	enum class EOwnership : std::uint64_t
	{
		TakeOwnership, /** used in some APIs, the object will take the ownership of pointer. */
		DoNotTakeOwnership, /** used in some APIs, the object will not take the ownership of pointer. */
	};

	static void* CreateCopy(const void* Buffer, uint64_t SizeInBytes)
	{
		void* NewBuffer = UBPA_UCOMMON_MALLOC(SizeInBytes);
		if (NewBuffer == nullptr)
		{
			return nullptr;
		}
		std::memcpy(NewBuffer, Buffer, SizeInBytes);
		return NewBuffer;
	}
}

UBPA_UCOMMON_UTILS_TO_NAMESPACE(UCommonTest)
