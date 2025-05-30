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

#include "SH.h"

namespace UCommon
{
	namespace Details
	{
		// normalization constants
		template<int l, int m>
		constexpr float SHKImpl()
		{
			static_assert(0 <= l && l <= 2, "l=0,1,2");
			static_assert(-l <= m && m <= l, "m in [-l, l]");
			constexpr int SHIndex = l * l + m + l;
			constexpr float SHKTable[9] =
			{
				0.2820948f,
				0.34549415f,
				0.48860252f,
				0.34549415f,
				0.12875807f,
				0.25751615f,
				0.63078314f,
				0.25751615f,
				0.12875807f,
			};
			return SHKTable[SHIndex];
		}

		//Reference: Stupid Spherical Harmonics (SH) Tricks
		// different from wiki
		template<int l, int m> struct SHImpl;
		template<> struct SHImpl<0,  0> { static constexpr float Func(float x, float y, float z) { return 0.28209478f; } };
		template<> struct SHImpl<1, -1> { static constexpr float Func(float x, float y, float z) { return -0.48860252f * y; } };
		template<> struct SHImpl<1,  0> { static constexpr float Func(float x, float y, float z) { return 0.48860252f * z; } };
		template<> struct SHImpl<1,  1> { static constexpr float Func(float x, float y, float z) { return -0.48860252f * x; } };
		template<> struct SHImpl<2, -2> { static constexpr float Func(float x, float y, float z) { return 1.0925485f * x * y; } };
		template<> struct SHImpl<2, -1> { static constexpr float Func(float x, float y, float z) { return -1.0925485f * y * z; } };
		template<> struct SHImpl<2,  0> { static constexpr float Func(float x, float y, float z) { return 0.31539157f * (3.f * z * z - 1.f); } };
		template<> struct SHImpl<2,  1> { static constexpr float Func(float x, float y, float z) { return -1.0925485f * x * z; } };
		template<> struct SHImpl<2,  2> { static constexpr float Func(float x, float y, float z) { return 0.54627424f * (x * x - y * y); } };

		template<int SHIndexOffset, int MaxSHBasis>
		inline void SHs(float(&V)[MaxSHBasis], float X, float Y, float Z, std::integer_sequence<int>) {}

		template<int SHIndexOffset, int MaxSHBasis, int FirstIndex, int... LeftIndices>
		inline void SHs(float(&V)[MaxSHBasis], float X, float Y, float Z, std::integer_sequence<int, FirstIndex, LeftIndices...>)
		{
			V[FirstIndex] = SH<SHIndexToL<SHIndexOffset + FirstIndex>, SHIndexToM<SHIndexOffset + FirstIndex>>(X, Y, Z);
			SHs<SHIndexOffset>(V, X, Y, Z, std::integer_sequence<int, LeftIndices...>());
		}

		template<int SHIndexOffset, int MaxSHBasis>
		inline void SHs(float(&V)[MaxSHBasis], float X, float Y, float Z)
		{
			SHs<SHIndexOffset>(V, X, Y, Z, std::make_integer_sequence<int, MaxSHBasis>());
		}
	}
}

template<int l, int m>
constexpr float UCommon::SH(float x, float y, float z)
{
	static_assert(0 <= l && l <= 2, "l=0,1,2");
	static_assert(-l <= m && m <= l, "m in [-l, l]");
	return Details::SHImpl<l, m>::Func(x, y, z);
}

template<typename DerivedType, int InMaxSHOrder, int InMaxSHBasis>
DerivedType UCommon::TSHVectorBase<DerivedType, InMaxSHOrder, InMaxSHBasis>::SHBasisFunction(const FVector3f& Vector)
{
	constexpr int SHIndexOffset = InMaxSHOrder * InMaxSHOrder - InMaxSHBasis;
	DerivedType Result;
	Details::SHs<SHIndexOffset>(Result.V, Vector.X, Vector.Y, Vector.Z);
	return Result;
}

template<typename DerivedType, int InMaxSHOrder, int InMaxSHBasis>
float UCommon::TSHVectorBase<DerivedType, InMaxSHOrder, InMaxSHBasis>::operator()(const FVector3f& Vector) const
{
	const DerivedType SHBasises = DerivedType::SHBasisFunction(Vector);
	return Dot(static_cast<const DerivedType&>(*this), SHBasises);
}

template<typename DerivedType, template<int> class TElement, int InMaxSHOrder, int InMaxSHBasis>
UCommon::FVector3f UCommon::TSHVectorRGBBase<DerivedType, TElement, InMaxSHOrder, InMaxSHBasis>::operator()(const FVector3f& Vector) const
{
	const TElement<MaxSHOrder> SHBasises = TElement<MaxSHOrder>::SHBasisFunction(Vector);
	return { TElement<MaxSHOrder>::Dot(R, SHBasises), TElement<MaxSHOrder>::Dot(G, SHBasises), TElement<MaxSHOrder>::Dot(B, SHBasises) };
}
