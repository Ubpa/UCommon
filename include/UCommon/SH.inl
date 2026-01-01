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

template<int l, int m>
float UCommon::SH(const FVector3f& w)
{
	return SH<l, m>(w.X, w.Y, w.Z);
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

// ============================================================================
// TSHBandView Mathematical Operations
// ============================================================================

// operator+= (mutable view only)
template<int Order>
UCommon::TSHBandView<Order, false>& UCommon::TSHBandView<Order, false>::operator+=(const TSHBandView& Other) noexcept
{
	UBPA_UCOMMON_ASSERT(Data != nullptr);
	UBPA_UCOMMON_ASSERT(Other.Data != nullptr);
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Data[i] += Other.Data[i];
	}
	return *this;
}

// operator-= (mutable view only)
template<int Order>
UCommon::TSHBandView<Order, false>& UCommon::TSHBandView<Order, false>::operator-=(const TSHBandView& Other) noexcept
{
	UBPA_UCOMMON_ASSERT(Data != nullptr);
	UBPA_UCOMMON_ASSERT(Other.Data != nullptr);
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Data[i] -= Other.Data[i];
	}
	return *this;
}

// operator*= (mutable view only)
template<int Order>
UCommon::TSHBandView<Order, false>& UCommon::TSHBandView<Order, false>::operator*=(float Scalar) noexcept
{
	UBPA_UCOMMON_ASSERT(Data != nullptr);
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Data[i] *= Scalar;
	}
	return *this;
}

// operator/= (mutable view only)
template<int Order>
UCommon::TSHBandView<Order, false>& UCommon::TSHBandView<Order, false>::operator/=(float Scalar) noexcept
{
	UBPA_UCOMMON_ASSERT(Data != nullptr);
	UBPA_UCOMMON_ASSERT(Scalar != 0.0f);
	const float InvScalar = 1.0f / Scalar;
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Data[i] *= InvScalar;
	}
	return *this;
}

// Non-member Dot function (template handles all const combinations)
namespace UCommon
{
	template<int Order, bool bConst1, bool bConst2>
	float Dot(const TSHBandView<Order, bConst1>& A, const TSHBandView<Order, bConst2>& B) noexcept
	{
		UBPA_UCOMMON_ASSERT(A.GetData() != nullptr);
		UBPA_UCOMMON_ASSERT(B.GetData() != nullptr);
		float Result = 0.0f;
		for (uint64_t i = 0; i < TSHBandView<Order, bConst1>::MaxSHBasis; ++i)
		{
			Result += A[i] * B[i];
		}
		return Result;
	}
}

// Member Dot function (calls non-member)
template<typename Derived, int Order, bool bConst>
template<bool bOtherConst>
float UCommon::TSHBandViewCommon<Derived, Order, bConst>::Dot(const TSHBandView<Order, bOtherConst>& Other) const noexcept
{
	return UCommon::Dot(static_cast<const TSHBandView<Order, bConst>&>(AsDerived()), Other);
}

// ============================================================================
// TSHBandVector Implementation
// ============================================================================

// Default constructor - zero-initialize
template<int Order>
UCommon::TSHBandVector<Order>::TSHBandVector() noexcept
{
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Data[i] = 0.0f;
	}
}

// Initializer list constructor
template<int Order>
UCommon::TSHBandVector<Order>::TSHBandVector(std::initializer_list<float> InitList) noexcept
{
	UBPA_UCOMMON_ASSERT(InitList.size() == MaxSHBasis);
	uint64_t i = 0;
	for (float Value : InitList)
	{
		Data[i++] = Value;
	}
}

// Copy constructor
template<int Order>
UCommon::TSHBandVector<Order>::TSHBandVector(const TSHBandVector& Other) noexcept
{
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Data[i] = Other.Data[i];
	}
}

// Move constructor
template<int Order>
UCommon::TSHBandVector<Order>::TSHBandVector(TSHBandVector&& Other) noexcept
{
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Data[i] = Other.Data[i];
	}
}

// Constructor from view (deep copy)
template<int Order>
template<bool bConst>
UCommon::TSHBandVector<Order>::TSHBandVector(const TSHBandView<Order, bConst>& View) noexcept
{
	UBPA_UCOMMON_ASSERT(View.GetData() != nullptr);
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Data[i] = View[i];
	}
}

// Copy assignment
template<int Order>
UCommon::TSHBandVector<Order>& UCommon::TSHBandVector<Order>::operator=(const TSHBandVector& Other) noexcept
{
	if (this != &Other)
	{
		for (uint64_t i = 0; i < MaxSHBasis; ++i)
		{
			Data[i] = Other.Data[i];
		}
	}
	return *this;
}

// Move assignment
template<int Order>
UCommon::TSHBandVector<Order>& UCommon::TSHBandVector<Order>::operator=(TSHBandVector&& Other) noexcept
{
	if (this != &Other)
	{
		for (uint64_t i = 0; i < MaxSHBasis; ++i)
		{
			Data[i] = Other.Data[i];
		}
	}
	return *this;
}

// Element access (non-const)
template<int Order>
float& UCommon::TSHBandVector<Order>::operator[](uint64_t Index) noexcept
{
	UBPA_UCOMMON_ASSERT(Index < MaxSHBasis);
	return Data[Index];
}

// Element access (const)
template<int Order>
const float& UCommon::TSHBandVector<Order>::operator[](uint64_t Index) const noexcept
{
	UBPA_UCOMMON_ASSERT(Index < MaxSHBasis);
	return Data[Index];
}

// Compound assignment operators
template<int Order>
UCommon::TSHBandVector<Order>& UCommon::TSHBandVector<Order>::operator+=(const TSHBandView<Order, true>& Other) noexcept
{
	UBPA_UCOMMON_ASSERT(Other.GetData() != nullptr);
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Data[i] += Other[i];
	}
	return *this;
}

template<int Order>
UCommon::TSHBandVector<Order>& UCommon::TSHBandVector<Order>::operator-=(const TSHBandView<Order, true>& Other) noexcept
{
	UBPA_UCOMMON_ASSERT(Other.GetData() != nullptr);
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Data[i] -= Other[i];
	}
	return *this;
}

template<int Order>
UCommon::TSHBandVector<Order>& UCommon::TSHBandVector<Order>::operator*=(float Scalar) noexcept
{
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Data[i] *= Scalar;
	}
	return *this;
}

template<int Order>
UCommon::TSHBandVector<Order>& UCommon::TSHBandVector<Order>::operator/=(float Scalar) noexcept
{
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Data[i] /= Scalar;
	}
	return *this;
}

// Dot product (member function)
template<int Order>
template<bool bConst>
float UCommon::TSHBandVector<Order>::Dot(const TSHBandView<Order, bConst>& Other) const noexcept
{
	UBPA_UCOMMON_ASSERT(Other.GetData() != nullptr);
	float Result = 0.0f;
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Result += Data[i] * Other[i];
	}
	return Result;
}

// Implicit conversion to mutable view
template<int Order>
UCommon::TSHBandVector<Order>::operator UCommon::TSHBandView<Order, false>() noexcept
{
	return TSHBandView<Order, false>(Data);
}

// Implicit conversion to const view
template<int Order>
UCommon::TSHBandVector<Order>::operator UCommon::TSHBandView<Order, true>() const noexcept
{
	return TSHBandView<Order, true>(Data);
}

// ============================================================================
// Binary operators for TSHBandView (return TSHBandVector)
// ============================================================================

// Addition
template<int Order, bool bConst1, bool bConst2>
UCommon::TSHBandVector<Order> UCommon::operator+(const TSHBandView<Order, bConst1>& Lhs, const TSHBandView<Order, bConst2>& Rhs) noexcept
{
	UBPA_UCOMMON_ASSERT(Lhs.GetData() != nullptr);
	UBPA_UCOMMON_ASSERT(Rhs.GetData() != nullptr);
	TSHBandVector<Order> Result;
	for (uint64_t i = 0; i < TSHBandVector<Order>::MaxSHBasis; ++i)
	{
		Result[i] = Lhs[i] + Rhs[i];
	}
	return Result;
}

// Subtraction
template<int Order, bool bConst1, bool bConst2>
UCommon::TSHBandVector<Order> UCommon::operator-(const TSHBandView<Order, bConst1>& Lhs, const TSHBandView<Order, bConst2>& Rhs) noexcept
{
	UBPA_UCOMMON_ASSERT(Lhs.GetData() != nullptr);
	UBPA_UCOMMON_ASSERT(Rhs.GetData() != nullptr);
	TSHBandVector<Order> Result;
	for (uint64_t i = 0; i < TSHBandVector<Order>::MaxSHBasis; ++i)
	{
		Result[i] = Lhs[i] - Rhs[i];
	}
	return Result;
}

// Scalar multiplication (view * scalar)
template<int Order, bool bConst>
UCommon::TSHBandVector<Order> UCommon::operator*(const TSHBandView<Order, bConst>& View, float Scalar) noexcept
{
	UBPA_UCOMMON_ASSERT(View.GetData() != nullptr);
	TSHBandVector<Order> Result;
	for (uint64_t i = 0; i < TSHBandVector<Order>::MaxSHBasis; ++i)
	{
		Result[i] = View[i] * Scalar;
	}
	return Result;
}

// Scalar multiplication (scalar * view)
template<int Order, bool bConst>
UCommon::TSHBandVector<Order> UCommon::operator*(float Scalar, const TSHBandView<Order, bConst>& View) noexcept
{
	return View * Scalar;
}

// Scalar division
template<int Order, bool bConst>
UCommon::TSHBandVector<Order> UCommon::operator/(const TSHBandView<Order, bConst>& View, float Scalar) noexcept
{
	UBPA_UCOMMON_ASSERT(View.GetData() != nullptr);
	TSHBandVector<Order> Result;
	for (uint64_t i = 0; i < TSHBandVector<Order>::MaxSHBasis; ++i)
	{
		Result[i] = View[i] / Scalar;
	}
	return Result;
}

// Non-member Dot function for TSHBandVector
template<int Order>
float UCommon::Dot(const TSHBandVector<Order>& A, const TSHBandVector<Order>& B) noexcept
{
	float Result = 0.0f;
	for (uint64_t i = 0; i < TSHBandVector<Order>::MaxSHBasis; ++i)
	{
		Result += A[i] * B[i];
	}
	return Result;
}

// Binary operators for TSHBandVector (return TSHBandVector)
template<int Order>
UCommon::TSHBandVector<Order> UCommon::operator+(const TSHBandVector<Order>& Lhs, const TSHBandVector<Order>& Rhs) noexcept
{
	TSHBandVector<Order> Result;
	for (uint64_t i = 0; i < TSHBandVector<Order>::MaxSHBasis; ++i)
	{
		Result[i] = Lhs[i] + Rhs[i];
	}
	return Result;
}

template<int Order>
UCommon::TSHBandVector<Order> UCommon::operator-(const TSHBandVector<Order>& Lhs, const TSHBandVector<Order>& Rhs) noexcept
{
	TSHBandVector<Order> Result;
	for (uint64_t i = 0; i < TSHBandVector<Order>::MaxSHBasis; ++i)
	{
		Result[i] = Lhs[i] - Rhs[i];
	}
	return Result;
}

template<int Order>
UCommon::TSHBandVector<Order> UCommon::operator*(const TSHBandVector<Order>& Vec, float Scalar) noexcept
{
	TSHBandVector<Order> Result;
	for (uint64_t i = 0; i < TSHBandVector<Order>::MaxSHBasis; ++i)
	{
		Result[i] = Vec[i] * Scalar;
	}
	return Result;
}

template<int Order>
UCommon::TSHBandVector<Order> UCommon::operator*(float Scalar, const TSHBandVector<Order>& Vec) noexcept
{
	return Vec * Scalar;
}

template<int Order>
UCommon::TSHBandVector<Order> UCommon::operator/(const TSHBandVector<Order>& Vec, float Scalar) noexcept
{
	TSHBandVector<Order> Result;
	for (uint64_t i = 0; i < TSHBandVector<Order>::MaxSHBasis; ++i)
	{
		Result[i] = Vec[i] / Scalar;
	}
	return Result;
}

// ============================================================================
// TSHBandVectorRGB Implementation
// ============================================================================

// Default constructor
template<int Order>
UCommon::TSHBandVectorRGB<Order>::TSHBandVectorRGB() noexcept
	: R(), G(), B()
{
}

// Constructor from three TSHBandVector
template<int Order>
UCommon::TSHBandVectorRGB<Order>::TSHBandVectorRGB(const TSHBandVector<Order>& InR, const TSHBandVector<Order>& InG, const TSHBandVector<Order>& InB) noexcept
	: R(InR), G(InG), B(InB)
{
}

// Copy constructor
template<int Order>
UCommon::TSHBandVectorRGB<Order>::TSHBandVectorRGB(const TSHBandVectorRGB& Other) noexcept
	: R(Other.R), G(Other.G), B(Other.B)
{
}

// Move constructor
template<int Order>
UCommon::TSHBandVectorRGB<Order>::TSHBandVectorRGB(TSHBandVectorRGB&& Other) noexcept
	: R(std::move(Other.R)), G(std::move(Other.G)), B(std::move(Other.B))
{
}

// Constructor from view (deep copy)
template<int Order>
template<bool bConst>
UCommon::TSHBandVectorRGB<Order>::TSHBandVectorRGB(const TSHBandViewRGB<Order, bConst>& View) noexcept
	: R(View.R), G(View.G), B(View.B)
{
}

// Copy assignment
template<int Order>
UCommon::TSHBandVectorRGB<Order>& UCommon::TSHBandVectorRGB<Order>::operator=(const TSHBandVectorRGB& Other) noexcept
{
	if (this != &Other)
	{
		R = Other.R;
		G = Other.G;
		B = Other.B;
	}
	return *this;
}

// Move assignment
template<int Order>
UCommon::TSHBandVectorRGB<Order>& UCommon::TSHBandVectorRGB<Order>::operator=(TSHBandVectorRGB&& Other) noexcept
{
	if (this != &Other)
	{
		R = std::move(Other.R);
		G = std::move(Other.G);
		B = std::move(Other.B);
	}
	return *this;
}

// Compound assignment operators
template<int Order>
UCommon::TSHBandVectorRGB<Order>& UCommon::TSHBandVectorRGB<Order>::operator+=(const TSHBandViewRGB<Order, true>& Other) noexcept
{
	R += Other.R;
	G += Other.G;
	B += Other.B;
	return *this;
}

template<int Order>
UCommon::TSHBandVectorRGB<Order>& UCommon::TSHBandVectorRGB<Order>::operator-=(const TSHBandViewRGB<Order, true>& Other) noexcept
{
	R -= Other.R;
	G -= Other.G;
	B -= Other.B;
	return *this;
}

template<int Order>
UCommon::TSHBandVectorRGB<Order>& UCommon::TSHBandVectorRGB<Order>::operator*=(float Scalar) noexcept
{
	R *= Scalar;
	G *= Scalar;
	B *= Scalar;
	return *this;
}

template<int Order>
UCommon::TSHBandVectorRGB<Order>& UCommon::TSHBandVectorRGB<Order>::operator/=(float Scalar) noexcept
{
	R /= Scalar;
	G /= Scalar;
	B /= Scalar;
	return *this;
}

// Implicit conversion to mutable view
template<int Order>
UCommon::TSHBandVectorRGB<Order>::operator UCommon::TSHBandViewRGB<Order, false>() noexcept
{
	return TSHBandViewRGB<Order, false>(R, G, B);
}

// Implicit conversion to const view
template<int Order>
UCommon::TSHBandVectorRGB<Order>::operator UCommon::TSHBandViewRGB<Order, true>() const noexcept
{
	return TSHBandViewRGB<Order, true>(R, G, B);
}

// ============================================================================
// Binary operators for TSHBandViewRGB (return TSHBandVectorRGB)
// ============================================================================

// Addition
template<int Order, bool bConst1, bool bConst2>
UCommon::TSHBandVectorRGB<Order> UCommon::operator+(const TSHBandViewRGB<Order, bConst1>& Lhs, const TSHBandViewRGB<Order, bConst2>& Rhs) noexcept
{
	return TSHBandVectorRGB<Order>(
		Lhs.R + Rhs.R,
		Lhs.G + Rhs.G,
		Lhs.B + Rhs.B
	);
}

// Subtraction
template<int Order, bool bConst1, bool bConst2>
UCommon::TSHBandVectorRGB<Order> UCommon::operator-(const TSHBandViewRGB<Order, bConst1>& Lhs, const TSHBandViewRGB<Order, bConst2>& Rhs) noexcept
{
	return TSHBandVectorRGB<Order>(
		Lhs.R - Rhs.R,
		Lhs.G - Rhs.G,
		Lhs.B - Rhs.B
	);
}

// Scalar multiplication (view * scalar)
template<int Order, bool bConst>
UCommon::TSHBandVectorRGB<Order> UCommon::operator*(const TSHBandViewRGB<Order, bConst>& View, float Scalar) noexcept
{
	return TSHBandVectorRGB<Order>(
		View.R * Scalar,
		View.G * Scalar,
		View.B * Scalar
	);
}

// Scalar multiplication (scalar * view)
template<int Order, bool bConst>
UCommon::TSHBandVectorRGB<Order> UCommon::operator*(float Scalar, const TSHBandViewRGB<Order, bConst>& View) noexcept
{
	return View * Scalar;
}

// Scalar division
template<int Order, bool bConst>
UCommon::TSHBandVectorRGB<Order> UCommon::operator/(const TSHBandViewRGB<Order, bConst>& View, float Scalar) noexcept
{
	return TSHBandVectorRGB<Order>(
		View.R / Scalar,
		View.G / Scalar,
		View.B / Scalar
	);
}

// Binary operators for TSHBandVectorRGB (return TSHBandVectorRGB)
template<int Order>
UCommon::TSHBandVectorRGB<Order> UCommon::operator+(const TSHBandVectorRGB<Order>& Lhs, const TSHBandVectorRGB<Order>& Rhs) noexcept
{
	return TSHBandVectorRGB<Order>(
		Lhs.R + Rhs.R,
		Lhs.G + Rhs.G,
		Lhs.B + Rhs.B
	);
}

template<int Order>
UCommon::TSHBandVectorRGB<Order> UCommon::operator-(const TSHBandVectorRGB<Order>& Lhs, const TSHBandVectorRGB<Order>& Rhs) noexcept
{
	return TSHBandVectorRGB<Order>(
		Lhs.R - Rhs.R,
		Lhs.G - Rhs.G,
		Lhs.B - Rhs.B
	);
}

template<int Order>
UCommon::TSHBandVectorRGB<Order> UCommon::operator*(const TSHBandVectorRGB<Order>& Vec, float Scalar) noexcept
{
	return TSHBandVectorRGB<Order>(
		Vec.R * Scalar,
		Vec.G * Scalar,
		Vec.B * Scalar
	);
}

template<int Order>
UCommon::TSHBandVectorRGB<Order> UCommon::operator*(float Scalar, const TSHBandVectorRGB<Order>& Vec) noexcept
{
	return Vec * Scalar;
}

template<int Order>
UCommon::TSHBandVectorRGB<Order> UCommon::operator/(const TSHBandVectorRGB<Order>& Vec, float Scalar) noexcept
{
	return TSHBandVectorRGB<Order>(
		Vec.R / Scalar,
		Vec.G / Scalar,
		Vec.B / Scalar
	);
}
