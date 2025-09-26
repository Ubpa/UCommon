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
	template<typename T, int64_t V>
	constexpr T TypedValue = static_cast<T>(V);
	template<typename T>
	constexpr T TypedZero = TypedValue<T, 0>;
	template<typename T>
	constexpr T TypedOne = TypedValue<T, 1>;
	template<typename T>
	constexpr T TypedInvalid = TypedValue<T, -1>;

	template<typename T>
	constexpr T Saturate(const T& V) noexcept { return V < 0 ? 0 : (V > 1 ? 1 : V); }

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
	static inline FVector4f FibonacciSpherePoint(uint64_t N, uint64_t n)
	{
		constexpr float Phi = 2.f * Pi / GlodenRatio;
		const float Z = (2.f * n + 1.f) / N - 1.f;
		const float r = std::sqrt(1.f - Z * Z);
		const float X = r * std::cos(Phi * n);
		const float Y = r * std::sin(Phi * n);

		constexpr float PDF = 1.0f / (4.f * Pi);

		return FVector4f(X, Y, Z, PDF);
	}
	static inline FVector4f FibonacciHemispherePoint(uint64_t N, uint64_t n)
	{
		constexpr float Phi = 2.f * Pi / GlodenRatio;
		const float Z = (n + 0.5f) / N;
		const float r = std::sqrt(1.f - Z * Z);
		const float X = r * std::cos(Phi * n);
		const float Y = r * std::sin(Phi * n);

		constexpr float PDF = 1.0f / (2.f * Pi);

		return FVector4f(X, Y, Z, PDF);
	}

	static inline FVector2f Hammersley(uint64_t N, uint64_t n) {
		const float X = float(n) / float(N);

		// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
		// efficient VanDerCorpus calculation.
		n = (n << 16u) | (n >> 16u);
		n = ((n & 0x55555555u) << 1u) | ((n & 0xAAAAAAAAu) >> 1u);
		n = ((n & 0x33333333u) << 2u) | ((n & 0xCCCCCCCCu) >> 2u);
		n = ((n & 0x0F0F0F0Fu) << 4u) | ((n & 0xF0F0F0F0u) >> 4u);
		n = ((n & 0x00FF00FFu) << 8u) | ((n & 0xFF00FF00u) >> 8u);
		const float Y = float(n) * 2.3283064365386963e-10; // / 0x100000000

		return { X,Y };
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
		BQ,           /** 128 bits / block => 8.00 bits / channel */
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
	static float ElementUint7SNormToFloat(uint8_t Element) noexcept
	{
		static constexpr uint32_t Table[256]
		{
			0xBF800000,0xBF7BF7F0,0xBF77EFE0,0xBF73E7D0,0xBF6FDFBF,0xBF6BD7AF,0xBF67CF9F,0xBF63C78F,
			0xBF5FBF7F,0xBF5BB76F,0xBF57AF5F,0xBF53A74F,0xBF4F9F3E,0xBF4B972E,0xBF478F1E,0xBF43870E,
			0xBF3F7EFE,0xBF3B76EE,0xBF376EDE,0xBF3366CE,0xBF2F5EBD,0xBF2B56AD,0xBF274E9D,0xBF23468D,
			0xBF1F3E7D,0xBF1B366D,0xBF172E5D,0xBF13264D,0xBF0F1E3C,0xBF0B162C,0xBF070E1C,0xBF03060C,
			0xBEFDFBF8,0xBEF5EBD8,0xBEEDDBB7,0xBEE5CB97,0xBEDDBB77,0xBED5AB57,0xBECD9B36,0xBEC58B16,
			0xBEBD7AF6,0xBEB56AD6,0xBEAD5AB5,0xBEA54A95,0xBE9D3A75,0xBE952A55,0xBE8D1A34,0xBE850A14,
			0xBE79F3E8,0xBE69D3A7,0xBE59B367,0xBE499326,0xBE3972E6,0xBE2952A5,0xBE193265,0xBE091224,
			0xBDF1E3C8,0xBDD1A347,0xBDB162C6,0xBD912245,0xBD61C387,0xBD214285,0xBCC18306,0xBC010204,
			0x3C010204,0x3CC18306,0x3D214285,0x3D61C387,0x3D912245,0x3DB162C6,0x3DD1A347,0x3DF1E3C8,
			0x3E091224,0x3E193265,0x3E2952A5,0x3E3972E6,0x3E499326,0x3E59B367,0x3E69D3A7,0x3E79F3E8,
			0x3E850A14,0x3E8D1A34,0x3E952A55,0x3E9D3A75,0x3EA54A95,0x3EAD5AB5,0x3EB56AD6,0x3EBD7AF6,
			0x3EC58B16,0x3ECD9B36,0x3ED5AB57,0x3EDDBB77,0x3EE5CB97,0x3EEDDBB7,0x3EF5EBD8,0x3EFDFBF8,
			0x3F03060C,0x3F070E1C,0x3F0B162C,0x3F0F1E3C,0x3F13264D,0x3F172E5D,0x3F1B366D,0x3F1F3E7D,
			0x3F23468D,0x3F274E9D,0x3F2B56AD,0x3F2F5EBD,0x3F3366CE,0x3F376EDE,0x3F3B76EE,0x3F3F7EFE,
			0x3F43870E,0x3F478F1E,0x3F4B972E,0x3F4F9F3E,0x3F53A74F,0x3F57AF5F,0x3F5BB76F,0x3F5FBF7F,
			0x3F63C78F,0x3F67CF9F,0x3F6BD7AF,0x3F6FDFBF,0x3F73E7D0,0x3F77EFE0,0x3F7BF7F0,0x3F800000,

			0xBF800000,0xBF7BF7F0,0xBF77EFE0,0xBF73E7D0,0xBF6FDFBF,0xBF6BD7AF,0xBF67CF9F,0xBF63C78F,
			0xBF5FBF7F,0xBF5BB76F,0xBF57AF5F,0xBF53A74F,0xBF4F9F3E,0xBF4B972E,0xBF478F1E,0xBF43870E,
			0xBF3F7EFE,0xBF3B76EE,0xBF376EDE,0xBF3366CE,0xBF2F5EBD,0xBF2B56AD,0xBF274E9D,0xBF23468D,
			0xBF1F3E7D,0xBF1B366D,0xBF172E5D,0xBF13264D,0xBF0F1E3C,0xBF0B162C,0xBF070E1C,0xBF03060C,
			0xBEFDFBF8,0xBEF5EBD8,0xBEEDDBB7,0xBEE5CB97,0xBEDDBB77,0xBED5AB57,0xBECD9B36,0xBEC58B16,
			0xBEBD7AF6,0xBEB56AD6,0xBEAD5AB5,0xBEA54A95,0xBE9D3A75,0xBE952A55,0xBE8D1A34,0xBE850A14,
			0xBE79F3E8,0xBE69D3A7,0xBE59B367,0xBE499326,0xBE3972E6,0xBE2952A5,0xBE193265,0xBE091224,
			0xBDF1E3C8,0xBDD1A347,0xBDB162C6,0xBD912245,0xBD61C387,0xBD214285,0xBCC18306,0xBC010204,
			0x3C010204,0x3CC18306,0x3D214285,0x3D61C387,0x3D912245,0x3DB162C6,0x3DD1A347,0x3DF1E3C8,
			0x3E091224,0x3E193265,0x3E2952A5,0x3E3972E6,0x3E499326,0x3E59B367,0x3E69D3A7,0x3E79F3E8,
			0x3E850A14,0x3E8D1A34,0x3E952A55,0x3E9D3A75,0x3EA54A95,0x3EAD5AB5,0x3EB56AD6,0x3EBD7AF6,
			0x3EC58B16,0x3ECD9B36,0x3ED5AB57,0x3EDDBB77,0x3EE5CB97,0x3EEDDBB7,0x3EF5EBD8,0x3EFDFBF8,
			0x3F03060C,0x3F070E1C,0x3F0B162C,0x3F0F1E3C,0x3F13264D,0x3F172E5D,0x3F1B366D,0x3F1F3E7D,
			0x3F23468D,0x3F274E9D,0x3F2B56AD,0x3F2F5EBD,0x3F3366CE,0x3F376EDE,0x3F3B76EE,0x3F3F7EFE,
			0x3F43870E,0x3F478F1E,0x3F4B972E,0x3F4F9F3E,0x3F53A74F,0x3F57AF5F,0x3F5BB76F,0x3F5FBF7F,
			0x3F63C78F,0x3F67CF9F,0x3F6BD7AF,0x3F6FDFBF,0x3F73E7D0,0x3F77EFE0,0x3F7BF7F0,0x3F800000,
		};
		return reinterpret_cast<const float&>(Table[Element]);
	}
	static float ElementUint8SNormToFloat(uint8_t Element) noexcept
	{
		static constexpr uint32_t Table[256] =
		{
			0xBF800000,0xBF7DFDFE,0xBF7BFBFC,0xBF79F9FA,0xBF77F7F8,0xBF75F5F6,0xBF73F3F4,0xBF71F1F2,
			0xBF6FEFF0,0xBF6DEDEE,0xBF6BEBEC,0xBF69E9EA,0xBF67E7E8,0xBF65E5E6,0xBF63E3E4,0xBF61E1E2,
			0xBF5FDFE0,0xBF5DDDDE,0xBF5BDBDC,0xBF59D9DA,0xBF57D7D8,0xBF55D5D6,0xBF53D3D4,0xBF51D1D2,
			0xBF4FCFD0,0xBF4DCDCE,0xBF4BCBCC,0xBF49C9CA,0xBF47C7C8,0xBF45C5C6,0xBF43C3C4,0xBF41C1C2,
			0xBF3FBFC0,0xBF3DBDBE,0xBF3BBBBC,0xBF39B9BA,0xBF37B7B8,0xBF35B5B6,0xBF33B3B4,0xBF31B1B2,
			0xBF2FAFB0,0xBF2DADAE,0xBF2BABAC,0xBF29A9AA,0xBF27A7A8,0xBF25A5A6,0xBF23A3A4,0xBF21A1A2,
			0xBF1F9FA0,0xBF1D9D9E,0xBF1B9B9C,0xBF19999A,0xBF179798,0xBF159596,0xBF139394,0xBF119192,
			0xBF0F8F90,0xBF0D8D8E,0xBF0B8B8C,0xBF09898A,0xBF078788,0xBF058586,0xBF038384,0xBF018182,
			0xBEFEFEFF,0xBEFAFAFB,0xBEF6F6F7,0xBEF2F2F3,0xBEEEEEEF,0xBEEAEAEB,0xBEE6E6E7,0xBEE2E2E3,
			0xBEDEDEDF,0xBEDADADB,0xBED6D6D7,0xBED2D2D3,0xBECECECF,0xBECACACB,0xBEC6C6C7,0xBEC2C2C3,
			0xBEBEBEBF,0xBEBABABB,0xBEB6B6B7,0xBEB2B2B3,0xBEAEAEAF,0xBEAAAAAB,0xBEA6A6A7,0xBEA2A2A3,
			0xBE9E9E9F,0xBE9A9A9B,0xBE969697,0xBE929293,0xBE8E8E8F,0xBE8A8A8B,0xBE868687,0xBE828283,
			0xBE7CFCFD,0xBE74F4F5,0xBE6CECED,0xBE64E4E5,0xBE5CDCDD,0xBE54D4D5,0xBE4CCCCD,0xBE44C4C5,
			0xBE3CBCBD,0xBE34B4B5,0xBE2CACAD,0xBE24A4A5,0xBE1C9C9D,0xBE149495,0xBE0C8C8D,0xBE048485,
			0xBDF8F8F9,0xBDE8E8E9,0xBDD8D8D9,0xBDC8C8C9,0xBDB8B8B9,0xBDA8A8A9,0xBD989899,0xBD888889,
			0xBD70F0F1,0xBD50D0D1,0xBD30B0B1,0xBD109091,0xBCE0E0E1,0xBCA0A0A1,0xBC40C0C1,0xBB808081,
			0x3B808081,0x3C40C0C1,0x3CA0A0A1,0x3CE0E0E1,0x3D109091,0x3D30B0B1,0x3D50D0D1,0x3D70F0F1,
			0x3D888889,0x3D989899,0x3DA8A8A9,0x3DB8B8B9,0x3DC8C8C9,0x3DD8D8D9,0x3DE8E8E9,0x3DF8F8F9,
			0x3E048485,0x3E0C8C8D,0x3E149495,0x3E1C9C9D,0x3E24A4A5,0x3E2CACAD,0x3E34B4B5,0x3E3CBCBD,
			0x3E44C4C5,0x3E4CCCCD,0x3E54D4D5,0x3E5CDCDD,0x3E64E4E5,0x3E6CECED,0x3E74F4F5,0x3E7CFCFD,
			0x3E828283,0x3E868687,0x3E8A8A8B,0x3E8E8E8F,0x3E929293,0x3E969697,0x3E9A9A9B,0x3E9E9E9F,
			0x3EA2A2A3,0x3EA6A6A7,0x3EAAAAAB,0x3EAEAEAF,0x3EB2B2B3,0x3EB6B6B7,0x3EBABABB,0x3EBEBEBF,
			0x3EC2C2C3,0x3EC6C6C7,0x3ECACACB,0x3ECECECF,0x3ED2D2D3,0x3ED6D6D7,0x3EDADADB,0x3EDEDEDF,
			0x3EE2E2E3,0x3EE6E6E7,0x3EEAEAEB,0x3EEEEEEF,0x3EF2F2F3,0x3EF6F6F7,0x3EFAFAFB,0x3EFEFEFF,
			0x3F018182,0x3F038384,0x3F058586,0x3F078788,0x3F09898A,0x3F0B8B8C,0x3F0D8D8E,0x3F0F8F90,
			0x3F119192,0x3F139394,0x3F159596,0x3F179798,0x3F19999A,0x3F1B9B9C,0x3F1D9D9E,0x3F1F9FA0,
			0x3F21A1A2,0x3F23A3A4,0x3F25A5A6,0x3F27A7A8,0x3F29A9AA,0x3F2BABAC,0x3F2DADAE,0x3F2FAFB0,
			0x3F31B1B2,0x3F33B3B4,0x3F35B5B6,0x3F37B7B8,0x3F39B9BA,0x3F3BBBBC,0x3F3DBDBE,0x3F3FBFC0,
			0x3F41C1C2,0x3F43C3C4,0x3F45C5C6,0x3F47C7C8,0x3F49C9CA,0x3F4BCBCC,0x3F4DCDCE,0x3F4FCFD0,
			0x3F51D1D2,0x3F53D3D4,0x3F55D5D6,0x3F57D7D8,0x3F59D9DA,0x3F5BDBDC,0x3F5DDDDE,0x3F5FDFE0,
			0x3F61E1E2,0x3F63E3E4,0x3F65E5E6,0x3F67E7E8,0x3F69E9EA,0x3F6BEBEC,0x3F6DEDEE,0x3F6FEFF0,
			0x3F71F1F2,0x3F73F3F4,0x3F75F5F6,0x3F77F7F8,0x3F79F9FA,0x3F7BFBFC,0x3F7DFDFE,0x3F800000,
		};
		return reinterpret_cast<const float&>(Table[Element]);
	}
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
	static uint8_t ElementFloatClampToUint8(float Element) noexcept { return static_cast<uint8_t>(std::roundf(Clamp(Element * 255.f, 0.f, 255.f))); }
	static FHalf ElementFloatToHalf(float Element) noexcept { return static_cast<FHalf>(Element); }
	static FHalf ElementUint8ToHalf(uint8_t Element) noexcept { return static_cast<FHalf>(ElementUint8ToFloat(Element)); }
	static uint8_t ElementHalfToUint8(FHalf Element) noexcept { return ElementFloatToUint8(ElementHalfToFloat(Element)); }
	static uint8_t ElementHalfClampToUint8(FHalf Element) noexcept { return ElementFloatClampToUint8(ElementHalfToFloat(Element)); }
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

	static constexpr uint8_t MSB64(uint64_t Value) noexcept
	{
		constexpr uint8_t Table[256] =
		{
			TypedInvalid<uint8_t>,
			0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
			5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
			6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
			6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
			7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		};

		uint8_t MSB = 0;
		if (Value >> 32) { Value >>= 32; MSB |= 0x20; }
		if (Value >> 16) { Value >>= 16; MSB |= 0x10; }
		if (Value >> 8) { Value >>= 8;  MSB |= 0x8; }
		return MSB + Table[Value];
	}
}

UBPA_UCOMMON_UTILS_TO_NAMESPACE(UCommonTest)
