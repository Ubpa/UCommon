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

#include "UCommonSH.h"

namespace UCommon
{
	namespace Details
	{
		template<int l, int m>
		struct SHImpl;

		template<>
		struct SHImpl<0, 0>
		{
			static constexpr float Func(float x, float y, float z) { return 0.28209478f; }
		};
		template<>
		struct SHImpl<1, -1>
		{
			static constexpr float Func(float x, float y, float z) { return 0.48860252f * y; }
		};
		template<>
		struct SHImpl<1, 0>
		{
			static constexpr float Func(float x, float y, float z) { return 0.48860252f * z; }
		};
		template<>
		struct SHImpl<1, 1>
		{
			static constexpr float Func(float x, float y, float z) { return 0.48860252f * x; }
		};
		template<>
		struct SHImpl<2, -2>
		{
			static constexpr float Func(float x, float y, float z) { return 1.0925485f * x * y; }
		};
		template<>
		struct SHImpl<2, -1>
		{
			static constexpr float Func(float x, float y, float z) { return 1.0925485f * y * z; }
		};
		template<>
		struct SHImpl<2, 0>
		{
			static constexpr float Func(float x, float y, float z) { return 0.31539157f * (3.f * z * z - 1.f); }
		};
		template<>
		struct SHImpl<2, 1>
		{
			static constexpr float Func(float x, float y, float z) { return 1.0925485f * x * z; }
		};
		template<>
		struct SHImpl<2, 2>
		{
			static constexpr float Func(float x, float y, float z) { return 0.54627424f * (x * x - y * y); }
		};

		template<int MaxSHBasis>
		inline void SHs(float(&V)[MaxSHBasis], float X, float Y, float Z, std::integer_sequence<int>) {}

		template<int MaxSHBasis, int FirstIndex, int... LeftIndices>
		inline void SHs(float(&V)[MaxSHBasis], float X, float Y, float Z, std::integer_sequence<int, FirstIndex, LeftIndices...>)
		{
			V[FirstIndex] = SH<SHIndexToL<FirstIndex>, SHIndexToM<FirstIndex>>(X, Y, Z);
			SHs(V, X, Y, Z, std::integer_sequence<int, LeftIndices...>());
		}

		template<int MaxSHBasis>
		inline void SHs(float(&V)[MaxSHBasis], float X, float Y, float Z)
		{
			SHs(V, X, Y, Z, std::make_integer_sequence<int, MaxSHBasis>());
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

template<int Order>
UCommon::TSHVector<Order> UCommon::TSHVector<Order>::SHBasisFunction(const FVector& Vector)
{
	TSHVector Result;
	Details::SHs(Result.V, Vector.X, Vector.Y, Vector.Z);
	return Result;
}

template<int Order>
float UCommon::TSHVector<Order>::operator()(const FVector& Vector) const
{
	const TSHVector<Order> SHBasises = TSHVector<Order>::SHBasisFunction(Vector);
	return Dot(*this, SHBasises);
}

template<int Order>
UCommon::FVector UCommon::TSHVectorRGB<Order>::operator()(const FVector& Vector) const
{
	const TSHVector<Order> SHBasises = TSHVector<Order>::SHBasisFunction(Vector);
	return { TSHVector<Order>::Dot(R, SHBasises), TSHVector<Order>::Dot(G, SHBasises), TSHVector<Order>::Dot(B, SHBasises) };
}
