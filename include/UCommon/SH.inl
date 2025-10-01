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
			static_assert(0 <= l && l <= 4, "l=0,1,2,3,4");
			static_assert(-l <= m && m <= l, "m in [-l, l]");
			constexpr int SHIndex = l * l + m + l;
			constexpr float SHKTable[25] =
			{
				// l = 0
				0.28209480f,

				// l = 1
				0.34549415f,
				0.48860252f,
				0.34549415f,

				// l = 2
				0.12875807f,
				0.25751615f,
				0.63078314f,
				0.25751615f,
				0.12875807f,

				// l = 3
				0.02781492f,
				0.06813236f,
				0.21545345f,
				0.74635267f,
				0.21545345f,
				0.06813236f,
				0.02781492f,

				// l = 4
				0.00421460f,
				0.01192068f,
				0.04460310f,
				0.18923494f,
				0.84628440f,
				0.18923494f,
				0.04460310f,
				0.01192068f,
				0.00421460f,
			};
			return SHKTable[SHIndex];
		}

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
	static_assert(0 <= l && l <= 4, "l=0,1,2,3,4");
	static_assert(-l <= m && m <= l, "m in [-l, l]");

	//Reference: Stupid Spherical Harmonics (SH) Tricks
	// different from wiki
	if constexpr (l == 0)
	{
		return 0.28209480f;
	}
	else if constexpr (l == 1)
	{
		if constexpr (m == -1) return -0.48860252f * y;
		else if constexpr (m == 0) return 0.48860252f * z;
		else /*m == 1*/ return -0.48860252f * x;
	}
	else if constexpr (l == 2)
	{
		if constexpr (m == -2) return 1.0925485f * x * y;
		else if constexpr (m == -1) return -1.0925485f * y * z;
		else if constexpr (m == 0) return 0.31539157f * (3.f * z * z - 1.f);
		else if constexpr (m == 1) return -1.0925485f * x * z;
		else /*m == 2*/  return 0.54627424f * (x * x - y * y);
	}
	else if constexpr (l == 3)
	{
		if constexpr (m == -3) return -0.59004360f * y * (3.f * x * x - y * y);
		else if constexpr (m == -2) return 2.8906114f * (x * y * z);
		else if constexpr (m == -1) return -0.45704580f * y * (5.f * z * z - 1.f);
		else if constexpr (m == 0) return 0.37317634f * (5.f * z * z - 3.f) * z;
		else if constexpr (m == 1) return -0.45704580f * x * (5.f * z * z - 1.f);
		else if constexpr (m == 2) return 1.4453057f * (x * x - y * y) * z;
		else /*m == 3*/ return -0.5900436f * x * (x * x - 3.f * y * y);
	}
	else if constexpr (l == 4)
	{
		if constexpr (m == -4) return 2.5033429f * x * y * (x * x - y * y);
		else if constexpr (m == -3) return -1.7701308f * y * (3.f * x * x - y * y) * z;
		else if constexpr (m == -2) return 0.94617470f * x * y * (7.f * z * z - 1.f);
		else if constexpr (m == -1) return 0.66904650f * y * (7.f * z * z - 3.f) * z;
		else if constexpr (m == 0)
		{
			const float z2 = z * z;
			return 0.10578555f * (35.f * z2 * z2 - 30.f * z2 + 3.f);
		}
		else if constexpr (m == 1) return -0.66904650f * x * (7.f * z * z - 3.f) * z;
		else if constexpr (m == 2) return 0.47308734f * (x * x - y * y) * (7.f * z * z - 1.f);
		else if constexpr (m == 3) return -1.7701308f * x * (x * x - 3.f * y * y) * z;
		else /*m == 4*/
		{
			const float x2 = x * x;
			const float y2 = y * y;
			return 0.62583572f * (x2 * x2 - 6.f * x2 * y2 + y2 * y2);
		}
	}
	else
	{
		//UBPA_UCOMMON_NO_ENTRY();
		return 0.f;
	}
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
