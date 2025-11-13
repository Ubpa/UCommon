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
    using EForceInit = UCommon::EForceInit; \
    using ETextureAddress = UCommon::ETextureAddress; \
    constexpr float Pi = UCommon::Pi; \
    using EElementType = UCommon::EElementType; \
    using EOwnership = UCommon::EOwnership; \
    template<typename T> constexpr bool IsDirectSupported = UCommon::IsDirectSupported<T>; \
    template<typename T> constexpr bool IsSupported = UCommon::IsSupported<T>; \
    template<typename T> constexpr EElementType ElementTypeOf = UCommon::ElementTypeOf<T>; \
    using FPackedHue = UCommon::FPackedHue; \
    using FPackedHemiOct = UCommon::FPackedHemiOct; \
}

namespace UCommon
{
	enum class EForceInit
	{
		Default,
		Zero
	};

	enum class ETextureAddress
	{
		Wrap,
		Clamp,
		Mirror,
	};

	// Apply texture address mode to a coordinate
	// For float: Coord is normalized coordinate, returns wrapped coordinate in [0, 1]
	// For int64_t: Coord is integer coordinate, Size is texture size, returns wrapped coordinate in [0, Size-1]
	UBPA_UCOMMON_API float ApplyAddressMode(float Coord, ETextureAddress AddressMode);
	UBPA_UCOMMON_API uint64_t ApplyAddressMode(int64_t Coord, uint64_t Size, ETextureAddress AddressMode);

	// Vector versions
	UBPA_UCOMMON_API FVector2f ApplyAddressMode(const FVector2f& Coord, ETextureAddress AddressMode);
	UBPA_UCOMMON_API FUint64Vector2 ApplyAddressMode(const FInt64Vector2& Coord, const FUint64Vector2& Size, ETextureAddress AddressMode);

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

	// co in [-2, 2], cg in [-1, 1]
	static inline void RGBToYCoCg(float R, float G, float B, float& Y, float& Co, float& Cg)
	{
		UBPA_UCOMMON_ASSERT(R >= 0 && G >= 0 && B >= 0);
		const float RB = R + B;
		Y = (2 * G + RB) / 4;
		if (Y < UBPA_UCOMMON_DELTA)
		{
			Co = 0.f;
			Cg = 0.f;
			return;
		}
		Co = (R - B) / Y / 2;
		Cg = (2 * G - RB) / Y / 4;
	}

	static inline FLinearColorRGB RGBToYCoCg(const FLinearColorRGB& RGB)
	{
		FLinearColorRGB YCoCg;
		RGBToYCoCg(RGB.X, RGB.Y, RGB.Z, YCoCg.X, YCoCg.Y, YCoCg.Z);
		return YCoCg;
	}

	static inline void YCoCgToRGB(float Y, float Co, float Cg, float& R, float& G, float& B)
	{
		const float Tmp = 1.f - Cg;
		R = (Tmp + Co) * Y;
		G = (1.f + Cg) * Y;
		B = (Tmp - Co) * Y;
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
		const float Y = float(n) * 2.3283064365386963e-10f; // / 0x100000000

		return { X,Y };
	}

	// [Jarzynski 2020, "Hash Functions for GPU Rendering"]
	static inline uint32_t PCGHash(uint32_t Input)
	{
		uint32_t state = Input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	static inline float PCGHashRand(uint32_t Input)
	{
		return (PCGHash(Input) % 0x7FFF) / (float)0x7FFF;
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
		static constexpr float Table[256]
		{
			-127.f/127.f, -125.f/127.f, -123.f/127.f, -121.f/127.f, -119.f/127.f, -117.f/127.f, -115.f/127.f, -113.f/127.f,
			-111.f/127.f, -109.f/127.f, -107.f/127.f, -105.f/127.f, -103.f/127.f, -101.f/127.f,  -99.f/127.f,  -97.f/127.f,
			 -95.f/127.f,  -93.f/127.f,  -91.f/127.f,  -89.f/127.f,  -87.f/127.f,  -85.f/127.f,  -83.f/127.f,  -81.f/127.f,
			 -79.f/127.f,  -77.f/127.f,  -75.f/127.f,  -73.f/127.f,  -71.f/127.f,  -69.f/127.f,  -67.f/127.f,  -65.f/127.f,
			 -63.f/127.f,  -61.f/127.f,  -59.f/127.f,  -57.f/127.f,  -55.f/127.f,  -53.f/127.f,  -51.f/127.f,  -49.f/127.f,
			 -47.f/127.f,  -45.f/127.f,  -43.f/127.f,  -41.f/127.f,  -39.f/127.f,  -37.f/127.f,  -35.f/127.f,  -33.f/127.f,
			 -31.f/127.f,  -29.f/127.f,  -27.f/127.f,  -25.f/127.f,  -23.f/127.f,  -21.f/127.f,  -19.f/127.f,  -17.f/127.f,
			 -15.f/127.f,  -13.f/127.f,  -11.f/127.f,   -9.f/127.f,   -7.f/127.f,   -5.f/127.f,   -3.f/127.f,   -1.f/127.f,
			   1.f/127.f,    3.f/127.f,    5.f/127.f,    7.f/127.f,    9.f/127.f,   11.f/127.f,   13.f/127.f,   15.f/127.f,
			  17.f/127.f,   19.f/127.f,   21.f/127.f,   23.f/127.f,   25.f/127.f,   27.f/127.f,   29.f/127.f,   31.f/127.f,
			  33.f/127.f,   35.f/127.f,   37.f/127.f,   39.f/127.f,   41.f/127.f,   43.f/127.f,   45.f/127.f,   47.f/127.f,
			  49.f/127.f,   51.f/127.f,   53.f/127.f,   55.f/127.f,   57.f/127.f,   59.f/127.f,   61.f/127.f,   63.f/127.f,
			  65.f/127.f,   67.f/127.f,   69.f/127.f,   71.f/127.f,   73.f/127.f,   75.f/127.f,   77.f/127.f,   79.f/127.f,
			  81.f/127.f,   83.f/127.f,   85.f/127.f,   87.f/127.f,   89.f/127.f,   91.f/127.f,   93.f/127.f,   95.f/127.f,
			  97.f/127.f,   99.f/127.f,  101.f/127.f,  103.f/127.f,  105.f/127.f,  107.f/127.f,  109.f/127.f,  111.f/127.f,
			 113.f/127.f,  115.f/127.f,  117.f/127.f,  119.f/127.f,  121.f/127.f,  123.f/127.f,  125.f/127.f,  127.f/127.f,
			-127.f/127.f, -125.f/127.f, -123.f/127.f, -121.f/127.f, -119.f/127.f, -117.f/127.f, -115.f/127.f, -113.f/127.f,
			-111.f/127.f, -109.f/127.f, -107.f/127.f, -105.f/127.f, -103.f/127.f, -101.f/127.f,  -99.f/127.f,  -97.f/127.f,
			 -95.f/127.f,  -93.f/127.f,  -91.f/127.f,  -89.f/127.f,  -87.f/127.f,  -85.f/127.f,  -83.f/127.f,  -81.f/127.f,
			 -79.f/127.f,  -77.f/127.f,  -75.f/127.f,  -73.f/127.f,  -71.f/127.f,  -69.f/127.f,  -67.f/127.f,  -65.f/127.f,
			 -63.f/127.f,  -61.f/127.f,  -59.f/127.f,  -57.f/127.f,  -55.f/127.f,  -53.f/127.f,  -51.f/127.f,  -49.f/127.f,
			 -47.f/127.f,  -45.f/127.f,  -43.f/127.f,  -41.f/127.f,  -39.f/127.f,  -37.f/127.f,  -35.f/127.f,  -33.f/127.f,
			 -31.f/127.f,  -29.f/127.f,  -27.f/127.f,  -25.f/127.f,  -23.f/127.f,  -21.f/127.f,  -19.f/127.f,  -17.f/127.f,
			 -15.f/127.f,  -13.f/127.f,  -11.f/127.f,   -9.f/127.f,   -7.f/127.f,   -5.f/127.f,   -3.f/127.f,   -1.f/127.f,
			   1.f/127.f,    3.f/127.f,    5.f/127.f,    7.f/127.f,    9.f/127.f,   11.f/127.f,   13.f/127.f,   15.f/127.f,
			  17.f/127.f,   19.f/127.f,   21.f/127.f,   23.f/127.f,   25.f/127.f,   27.f/127.f,   29.f/127.f,   31.f/127.f,
			  33.f/127.f,   35.f/127.f,   37.f/127.f,   39.f/127.f,   41.f/127.f,   43.f/127.f,   45.f/127.f,   47.f/127.f,
			  49.f/127.f,   51.f/127.f,   53.f/127.f,   55.f/127.f,   57.f/127.f,   59.f/127.f,   61.f/127.f,   63.f/127.f,
			  65.f/127.f,   67.f/127.f,   69.f/127.f,   71.f/127.f,   73.f/127.f,   75.f/127.f,   77.f/127.f,   79.f/127.f,
			  81.f/127.f,   83.f/127.f,   85.f/127.f,   87.f/127.f,   89.f/127.f,   91.f/127.f,   93.f/127.f,   95.f/127.f,
			  97.f/127.f,   99.f/127.f,  101.f/127.f,  103.f/127.f,  105.f/127.f,  107.f/127.f,  109.f/127.f,  111.f/127.f,
			 113.f/127.f,  115.f/127.f,  117.f/127.f,  119.f/127.f,  121.f/127.f,  123.f/127.f,  125.f/127.f,  127.f/127.f,
		};
		return Table[Element];
	}
	static float ElementUint8SNormToFloat(uint8_t Element) noexcept
	{
		static constexpr float Table[256] =
		{
			-255.f/255.f, -253.f/255.f, -251.f/255.f, -249.f/255.f, -247.f/255.f, -245.f/255.f, -243.f/255.f, -241.f/255.f,
			-239.f/255.f, -237.f/255.f, -235.f/255.f, -233.f/255.f, -231.f/255.f, -229.f/255.f, -227.f/255.f, -225.f/255.f,
			-223.f/255.f, -221.f/255.f, -219.f/255.f, -217.f/255.f, -215.f/255.f, -213.f/255.f, -211.f/255.f, -209.f/255.f,
			-207.f/255.f, -205.f/255.f, -203.f/255.f, -201.f/255.f, -199.f/255.f, -197.f/255.f, -195.f/255.f, -193.f/255.f,
			-191.f/255.f, -189.f/255.f, -187.f/255.f, -185.f/255.f, -183.f/255.f, -181.f/255.f, -179.f/255.f, -177.f/255.f,
			-175.f/255.f, -173.f/255.f, -171.f/255.f, -169.f/255.f, -167.f/255.f, -165.f/255.f, -163.f/255.f, -161.f/255.f,
			-159.f/255.f, -157.f/255.f, -155.f/255.f, -153.f/255.f, -151.f/255.f, -149.f/255.f, -147.f/255.f, -145.f/255.f,
			-143.f/255.f, -141.f/255.f, -139.f/255.f, -137.f/255.f, -135.f/255.f, -133.f/255.f, -131.f/255.f, -129.f/255.f,
			-127.f/255.f, -125.f/255.f, -123.f/255.f, -121.f/255.f, -119.f/255.f, -117.f/255.f, -115.f/255.f, -113.f/255.f,
			-111.f/255.f, -109.f/255.f, -107.f/255.f, -105.f/255.f, -103.f/255.f, -101.f/255.f,  -99.f/255.f,  -97.f/255.f,
			 -95.f/255.f,  -93.f/255.f,  -91.f/255.f,  -89.f/255.f,  -87.f/255.f,  -85.f/255.f,  -83.f/255.f,  -81.f/255.f,
			 -79.f/255.f,  -77.f/255.f,  -75.f/255.f,  -73.f/255.f,  -71.f/255.f,  -69.f/255.f,  -67.f/255.f,  -65.f/255.f,
			 -63.f/255.f,  -61.f/255.f,  -59.f/255.f,  -57.f/255.f,  -55.f/255.f,  -53.f/255.f,  -51.f/255.f,  -49.f/255.f,
			 -47.f/255.f,  -45.f/255.f,  -43.f/255.f,  -41.f/255.f,  -39.f/255.f,  -37.f/255.f,  -35.f/255.f,  -33.f/255.f,
			 -31.f/255.f,  -29.f/255.f,  -27.f/255.f,  -25.f/255.f,  -23.f/255.f,  -21.f/255.f,  -19.f/255.f,  -17.f/255.f,
			 -15.f/255.f,  -13.f/255.f,  -11.f/255.f,   -9.f/255.f,   -7.f/255.f,   -5.f/255.f,   -3.f/255.f,   -1.f/255.f,
			   1.f/255.f,    3.f/255.f,    5.f/255.f,    7.f/255.f,    9.f/255.f,   11.f/255.f,   13.f/255.f,   15.f/255.f,
			  17.f/255.f,   19.f/255.f,   21.f/255.f,   23.f/255.f,   25.f/255.f,   27.f/255.f,   29.f/255.f,   31.f/255.f,
			  33.f/255.f,   35.f/255.f,   37.f/255.f,   39.f/255.f,   41.f/255.f,   43.f/255.f,   45.f/255.f,   47.f/255.f,
			  49.f/255.f,   51.f/255.f,   53.f/255.f,   55.f/255.f,   57.f/255.f,   59.f/255.f,   61.f/255.f,   63.f/255.f,
			  65.f/255.f,   67.f/255.f,   69.f/255.f,   71.f/255.f,   73.f/255.f,   75.f/255.f,   77.f/255.f,   79.f/255.f,
			  81.f/255.f,   83.f/255.f,   85.f/255.f,   87.f/255.f,   89.f/255.f,   91.f/255.f,   93.f/255.f,   95.f/255.f,
			  97.f/255.f,   99.f/255.f,  101.f/255.f,  103.f/255.f,  105.f/255.f,  107.f/255.f,  109.f/255.f,  111.f/255.f,
			 113.f/255.f,  115.f/255.f,  117.f/255.f,  119.f/255.f,  121.f/255.f,  123.f/255.f,  125.f/255.f,  127.f/255.f,
			 129.f/255.f,  131.f/255.f,  133.f/255.f,  135.f/255.f,  137.f/255.f,  139.f/255.f,  141.f/255.f,  143.f/255.f,
			 145.f/255.f,  147.f/255.f,  149.f/255.f,  151.f/255.f,  153.f/255.f,  155.f/255.f,  157.f/255.f,  159.f/255.f,
			 161.f/255.f,  163.f/255.f,  165.f/255.f,  167.f/255.f,  169.f/255.f,  171.f/255.f,  173.f/255.f,  175.f/255.f,
			 177.f/255.f,  179.f/255.f,  181.f/255.f,  183.f/255.f,  185.f/255.f,  187.f/255.f,  189.f/255.f,  191.f/255.f,
			 193.f/255.f,  195.f/255.f,  197.f/255.f,  199.f/255.f,  201.f/255.f,  203.f/255.f,  205.f/255.f,  207.f/255.f,
			 209.f/255.f,  211.f/255.f,  213.f/255.f,  215.f/255.f,  217.f/255.f,  219.f/255.f,  221.f/255.f,  223.f/255.f,
			 225.f/255.f,  227.f/255.f,  229.f/255.f,  231.f/255.f,  233.f/255.f,  235.f/255.f,  237.f/255.f,  239.f/255.f,
			 241.f/255.f,  243.f/255.f,  245.f/255.f,  247.f/255.f,  249.f/255.f,  251.f/255.f,  253.f/255.f,  255.f/255.f,
		};
		return Table[Element];
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

	struct FPackedHue
	{
		FPackedHue() {} // uninitialize
		FPackedHue(EForceInit) : Co(128), Cg(128) {} // zero hue
		FPackedHue(uint8_t InCo, uint8_t InCg) : Co(InCo), Cg(InCg) {}
		FPackedHue(const FVector2f& CoCg)
		{
			Co = ElementFloatClampToUint8((CoCg.X + 2.f) / 4.f);
			Cg = ElementFloatClampToUint8((CoCg.Y + 1.f) / 2.f);
		}
		FPackedHue(const FVector3f& Hue)
		{
			//require R+2G+B==4
			UBPA_UCOMMON_ASSERT(std::abs(Hue.X + 2.f * Hue.Y + Hue.Z - 4.f) < UBPA_UCOMMON_DELTA);
			Co = ElementFloatClampToUint8((Hue.X - Hue.Z + 4.f) / 8.f);
			Cg = ElementFloatClampToUint8((2.f * Hue.Y - (Hue.X + Hue.Z) + 4.f) / 8.f);
		}

		FVector3f Unpack() const
		{
			FVector3f Hue;
			constexpr float Y = 1.f;
			const float Cof = ElementUint8ToFloat(Co) * 4.f - 2.f;
			const float Cgf = ElementUint8ToFloat(Cg) * 2.f - 1.f;
			const float YSubCg = Y - Cgf;
			Hue.X = YSubCg + Cof;
			Hue.Y = Y + Cgf;
			Hue.Z = YSubCg - Cof;
			return Hue;
		}

		// (Y = (R+2G+B)/4) == 1
		uint8_t Co; // / 255 * 4 - 2 => [-2,2]
		uint8_t Cg; // / 255 * 2 - 1 => [-1,1]
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

		uint8_t U, V; // Rotated hemispherical octahedral coordinates: u = (x+y)/2+0.5, v = (x-y)/2+0.5
	};
}

UBPA_UCOMMON_UTILS_TO_NAMESPACE(UCommonTest)
