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
DerivedType UCommon::TSHVectorCommon<DerivedType, InMaxSHOrder, InMaxSHBasis>::SHBasisFunction(const FVector3f& Vector)
{
	constexpr int SHIndexOffset = InMaxSHOrder * InMaxSHOrder - InMaxSHBasis;
	DerivedType Result;
	Details::SHs<SHIndexOffset>(Result.V, Vector.X, Vector.Y, Vector.Z);
	return Result;
}

template<typename DerivedType, int InMaxSHOrder, int InMaxSHBasis>
float UCommon::TSHVectorCommon<DerivedType, InMaxSHOrder, InMaxSHBasis>::operator()(const FVector3f& Vector) const
{
	const DerivedType SHBasises = DerivedType::SHBasisFunction(Vector);
	return Dot(static_cast<const DerivedType&>(*this), SHBasises);
}

template<typename DerivedType, template<int> class TElement, int InMaxSHOrder, int InMaxSHBasis>
UCommon::FVector3f UCommon::TSHVectorRGBCommon<DerivedType, TElement, InMaxSHOrder, InMaxSHBasis>::operator()(const FVector3f& Vector) const
{
	const TElement<MaxSHOrder> SHBasises = TElement<MaxSHOrder>::SHBasisFunction(Vector);
	return { TElement<MaxSHOrder>::Dot(R, SHBasises), TElement<MaxSHOrder>::Dot(G, SHBasises), TElement<MaxSHOrder>::Dot(B, SHBasises) };
}

// ============================================================================
// TSHBandView Mathematical Operations
// ============================================================================

// operator+= (mutable view only)
template<int Order>
UCommon::TSHBandView<Order, false>& UCommon::TSHBandView<Order, false>::operator+=(TSHBandConstView<Order> Other) const noexcept
{
	UBPA_UCOMMON_ASSERT(this->Data != nullptr);
	UBPA_UCOMMON_ASSERT(Other.GetData() != nullptr);
	for (uint64_t i = 0; i < TSHBandCommon<TSHBandView<Order, false>, Order>::MaxSHBasis; ++i)
	{
		this->Data[i] += Other[i];
	}
	return const_cast<TSHBandView&>(*this);
}

// operator-= (mutable view only)
template<int Order>
UCommon::TSHBandView<Order, false>& UCommon::TSHBandView<Order, false>::operator-=(TSHBandConstView<Order> Other) const noexcept
{
	UBPA_UCOMMON_ASSERT(this->Data != nullptr);
	UBPA_UCOMMON_ASSERT(Other.GetData() != nullptr);
	for (uint64_t i = 0; i < TSHBandCommon<TSHBandView<Order, false>, Order>::MaxSHBasis; ++i)
	{
		this->Data[i] -= Other[i];
	}
	return const_cast<TSHBandView&>(*this);
}

// operator*= (mutable view only)
template<int Order>
UCommon::TSHBandView<Order, false>& UCommon::TSHBandView<Order, false>::operator*=(float Scalar) const noexcept
{
	UBPA_UCOMMON_ASSERT(this->Data != nullptr);
	for (uint64_t i = 0; i < TSHBandCommon<TSHBandView<Order, false>, Order>::MaxSHBasis; ++i)
	{
		this->Data[i] *= Scalar;
	}
	return const_cast<TSHBandView&>(*this);
}

// operator/= (mutable view only)
template<int Order>
UCommon::TSHBandView<Order, false>& UCommon::TSHBandView<Order, false>::operator/=(float Scalar) const noexcept
{
	UBPA_UCOMMON_ASSERT(this->Data != nullptr);
	UBPA_UCOMMON_ASSERT(Scalar != 0.0f);
	const float InvScalar = 1.0f / Scalar;
	for (uint64_t i = 0; i < TSHBandCommon<TSHBandView<Order, false>, Order>::MaxSHBasis; ++i)
	{
		this->Data[i] *= InvScalar;
	}
	return const_cast<TSHBandView&>(*this);
}

// Dot function is now a static member of TSHBandCommon (inline in header)

// ============================================================================
// TSHBandCommon member functions (common operations for View and Vector)
// ============================================================================

// SHBasisFunction - compute SH basis values for this band at given direction
template<typename DerivedType, int Order>
UCommon::TSHBandVector<Order> UCommon::TSHBandCommon<DerivedType, Order>::SHBasisFunction(const FVector3f& Vector)
{
	// For band Order, we need coefficients for L = Order-1
	// SHIndexOffset is the starting index in the global SH array
	// L=0: index 0, L=1: index 1-3, L=2: index 4-8, L=3: index 9-15, L=4: index 16-24
	// Formula: SHIndexOffset = (Order-1)^2
	constexpr int SHIndexOffset = (Order - 1) * (Order - 1);
	TSHBandVector<Order> Result;
	Details::SHs<SHIndexOffset>(Result.Data, Vector.X, Vector.Y, Vector.Z);
	return Result;
}

// Dot product - delegates to static Dot function
template<typename DerivedType, int Order>
float UCommon::TSHBandCommon<DerivedType, Order>::Dot(TSHBandConstView<Order> Other) const noexcept
{
	// Convert to const view and call static Dot function
	return TSHBandCommon<DerivedType, Order>::Dot(static_cast<TSHBandConstView<Order>>(AsDerived()), Other);
}

// Binary operators
template<typename DerivedType, int Order>
UCommon::TSHBandVector<Order> UCommon::TSHBandCommon<DerivedType, Order>::operator+(TSHBandConstView<Order> Other) const noexcept
{
	UBPA_UCOMMON_ASSERT(AsDerived().GetData() != nullptr);
	UBPA_UCOMMON_ASSERT(Other.GetData() != nullptr);
	TSHBandVector<Order> Result;
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Result[i] = AsDerived()[i] + Other[i];
	}
	return Result;
}

template<typename DerivedType, int Order>
UCommon::TSHBandVector<Order> UCommon::TSHBandCommon<DerivedType, Order>::operator-(TSHBandConstView<Order> Other) const noexcept
{
	UBPA_UCOMMON_ASSERT(AsDerived().GetData() != nullptr);
	UBPA_UCOMMON_ASSERT(Other.GetData() != nullptr);
	TSHBandVector<Order> Result;
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Result[i] = AsDerived()[i] - Other[i];
	}
	return Result;
}

template<typename DerivedType, int Order>
UCommon::TSHBandVector<Order> UCommon::TSHBandCommon<DerivedType, Order>::operator*(float Scalar) const noexcept
{
	UBPA_UCOMMON_ASSERT(AsDerived().GetData() != nullptr);
	TSHBandVector<Order> Result;
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Result[i] = AsDerived()[i] * Scalar;
	}
	return Result;
}

template<typename DerivedType, int Order>
UCommon::TSHBandVector<Order> UCommon::TSHBandCommon<DerivedType, Order>::operator/(float Scalar) const noexcept
{
	UBPA_UCOMMON_ASSERT(AsDerived().GetData() != nullptr);
	TSHBandVector<Order> Result;
	for (uint64_t i = 0; i < MaxSHBasis; ++i)
	{
		Result[i] = AsDerived()[i] / Scalar;
	}
	return Result;
}

// ============================================================================
// TSHBandVector Implementation
// ============================================================================

// Default constructor - zero-initialize
template<int Order>
UCommon::TSHBandVector<Order>::TSHBandVector() noexcept
{
	for (uint64_t i = 0; i < TSHBandCommon<TSHBandVector<Order>, Order>::MaxSHBasis; ++i)
	{
		Data[i] = 0.0f;
	}
}

// Initializer list constructor
template<int Order>
UCommon::TSHBandVector<Order>::TSHBandVector(std::initializer_list<float> InitList) noexcept
{
	UBPA_UCOMMON_ASSERT(InitList.size() == (TSHBandCommon<TSHBandVector<Order>, Order>::MaxSHBasis));
	uint64_t i = 0;
	for (float Value : InitList)
	{
		Data[i++] = Value;
	}
}

// Constructor from view (deep copy)
template<int Order>
UCommon::TSHBandVector<Order>::TSHBandVector(TSHBandConstView<Order> View) noexcept
{
	UBPA_UCOMMON_ASSERT(View.GetData() != nullptr);
	for (uint64_t i = 0; i < TSHBandCommon<TSHBandVector<Order>, Order>::MaxSHBasis; ++i)
	{
		Data[i] = View[i];
	}
}

// Assignment from view
template<int Order>
UCommon::TSHBandVector<Order>& UCommon::TSHBandVector<Order>::operator=(TSHBandConstView<Order> View) noexcept
{
	UBPA_UCOMMON_ASSERT(View.GetData() != nullptr);
	for (uint64_t i = 0; i < TSHBandCommon<TSHBandVector<Order>, Order>::MaxSHBasis; ++i)
	{
		Data[i] = View[i];
	}
	return *this;
}

// Element access (non-const)
template<int Order>
float& UCommon::TSHBandVector<Order>::operator[](uint64_t Index) noexcept
{
	UBPA_UCOMMON_ASSERT(Index < (TSHBandCommon<TSHBandVector<Order>, Order>::MaxSHBasis));
	return Data[Index];
}

// Element access (const)
template<int Order>
const float& UCommon::TSHBandVector<Order>::operator[](uint64_t Index) const noexcept
{
	UBPA_UCOMMON_ASSERT(Index < (TSHBandCommon<TSHBandVector<Order>, Order>::MaxSHBasis));
	return Data[Index];
}

// Compound assignment operators
template<int Order>
UCommon::TSHBandVector<Order>& UCommon::TSHBandVector<Order>::operator+=(TSHBandConstView<Order> Other) noexcept
{
	UBPA_UCOMMON_ASSERT(Other.GetData() != nullptr);
	for (uint64_t i = 0; i < TSHBandCommon<TSHBandVector<Order>, Order>::MaxSHBasis; ++i)
	{
		Data[i] += Other[i];
	}
	return *this;
}

template<int Order>
UCommon::TSHBandVector<Order>& UCommon::TSHBandVector<Order>::operator-=(TSHBandConstView<Order> Other) noexcept
{
	UBPA_UCOMMON_ASSERT(Other.GetData() != nullptr);
	for (uint64_t i = 0; i < TSHBandCommon<TSHBandVector<Order>, Order>::MaxSHBasis; ++i)
	{
		Data[i] -= Other[i];
	}
	return *this;
}

template<int Order>
UCommon::TSHBandVector<Order>& UCommon::TSHBandVector<Order>::operator*=(float Scalar) noexcept
{
	for (uint64_t i = 0; i < TSHBandCommon<TSHBandVector<Order>, Order>::MaxSHBasis; ++i)
	{
		Data[i] *= Scalar;
	}
	return *this;
}

template<int Order>
UCommon::TSHBandVector<Order>& UCommon::TSHBandVector<Order>::operator/=(float Scalar) noexcept
{
	for (uint64_t i = 0; i < TSHBandCommon<TSHBandVector<Order>, Order>::MaxSHBasis; ++i)
	{
		Data[i] /= Scalar;
	}
	return *this;
}

// Dot and binary operators are inherited from TSHBandCommon - no need to reimplement

// Implicit conversion to mutable view
template<int Order>
UCommon::TSHBandVector<Order>::operator UCommon::TSHBandView<Order, false>() noexcept
{
	return TSHBandView<Order, false>(Data);
}

// Implicit conversion to const view
template<int Order>
UCommon::TSHBandVector<Order>::operator UCommon::TSHBandConstView<Order>() const noexcept
{
	return TSHBandConstView<Order>(Data);
}

// Binary operators and Dot for TSHBandView are inherited from TSHBandCommon - no need to reimplement
// ============================================================================

// Dot function is now a static member of TSHBandCommon (inline in header)

// Binary operators for TSHBandVector are inherited from TSHBandCommon - no need to reimplement
// Global operator* (scalar * view) is defined inline in the header
// Note: operator*(float, TSHBandVector) is handled by operator*(float, TSHBandConstView) via implicit conversion
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

// Constructor from view (deep copy)
template<int Order>
UCommon::TSHBandVectorRGB<Order>::TSHBandVectorRGB(TSHBandConstViewRGB<Order> View) noexcept
	: R(View.R), G(View.G), B(View.B)
{
}

// Assignment from view
template<int Order>
UCommon::TSHBandVectorRGB<Order>& UCommon::TSHBandVectorRGB<Order>::operator=(TSHBandConstViewRGB<Order> View) noexcept
{
	R = View.R;
	G = View.G;
	B = View.B;
	return *this;
}

// Compound assignment operators
template<int Order>
UCommon::TSHBandVectorRGB<Order>& UCommon::TSHBandVectorRGB<Order>::operator+=(TSHBandConstViewRGB<Order> Other) noexcept
{
	R += Other.R;
	G += Other.G;
	B += Other.B;
	return *this;
}

template<int Order>
UCommon::TSHBandVectorRGB<Order>& UCommon::TSHBandVectorRGB<Order>::operator-=(TSHBandConstViewRGB<Order> Other) noexcept
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
UCommon::TSHBandVectorRGB<Order>::operator UCommon::TSHBandConstViewRGB<Order>() const noexcept
{
	return TSHBandConstViewRGB<Order>(R, G, B);
}

// ============================================================================
// Member binary operators for TSHBandViewRGBCommon
// ============================================================================

// operator+ (member)
template<int Order, bool bConst>
UCommon::TSHBandVectorRGB<Order> UCommon::TSHBandViewRGBCommon<Order, bConst>::operator+(TSHBandConstViewRGB<Order> Other) const noexcept
{
	return TSHBandVectorRGB<Order>(
		R + Other.R,
		G + Other.G,
		B + Other.B
	);
}

// operator- (member)
template<int Order, bool bConst>
UCommon::TSHBandVectorRGB<Order> UCommon::TSHBandViewRGBCommon<Order, bConst>::operator-(TSHBandConstViewRGB<Order> Other) const noexcept
{
	return TSHBandVectorRGB<Order>(
		R - Other.R,
		G - Other.G,
		B - Other.B
	);
}

// operator* (member - view * scalar)
template<int Order, bool bConst>
UCommon::TSHBandVectorRGB<Order> UCommon::TSHBandViewRGBCommon<Order, bConst>::operator*(float Scalar) const noexcept
{
	return TSHBandVectorRGB<Order>(
		R * Scalar,
		G * Scalar,
		B * Scalar
	);
}

// operator/ (member)
template<int Order, bool bConst>
UCommon::TSHBandVectorRGB<Order> UCommon::TSHBandViewRGBCommon<Order, bConst>::operator/(float Scalar) const noexcept
{
	return TSHBandVectorRGB<Order>(
		R / Scalar,
		G / Scalar,
		B / Scalar
	);
}

// Binary operators for TSHBandViewRGB are now inline in the header
// ============================================================================

// Member binary operators for TSHBandVectorRGB
template<int Order>
UCommon::TSHBandVectorRGB<Order> UCommon::TSHBandVectorRGB<Order>::operator+(TSHBandConstViewRGB<Order> Other) const noexcept
{
	return TSHBandVectorRGB<Order>(
		R + Other.R,
		G + Other.G,
		B + Other.B
	);
}

template<int Order>
UCommon::TSHBandVectorRGB<Order> UCommon::TSHBandVectorRGB<Order>::operator-(TSHBandConstViewRGB<Order> Other) const noexcept
{
	return TSHBandVectorRGB<Order>(
		R - Other.R,
		G - Other.G,
		B - Other.B
	);
}

template<int Order>
UCommon::TSHBandVectorRGB<Order> UCommon::TSHBandVectorRGB<Order>::operator*(float Scalar) const noexcept
{
	return TSHBandVectorRGB<Order>(
		R * Scalar,
		G * Scalar,
		B * Scalar
	);
}

template<int Order>
UCommon::TSHBandVectorRGB<Order> UCommon::TSHBandVectorRGB<Order>::operator/(float Scalar) const noexcept
{
	return TSHBandVectorRGB<Order>(
		R / Scalar,
		G / Scalar,
		B / Scalar
	);
}

// Binary operators for TSHBandVectorRGB (return TSHBandVectorRGB)
// Global operator* (scalar * vector) is defined inline in the header
// ============================================================================

template<int Order>
void UCommon::ApplySHRotateMatrix(TSHBandView<Order> SHBand, const float* SHBandRotateMatrix)
{
	UBPA_UCOMMON_ASSERT(SHBand.GetData() != nullptr);
	UBPA_UCOMMON_ASSERT(SHBandRotateMatrix != nullptr);
	constexpr int N = TSHBandView<Order>::MaxSHBasis;
	float Tmp[N];
	for (int Row = 0; Row < N; ++Row)
	{
		Tmp[Row] = 0.f;
		for (int Col = 0; Col < N; ++Col)
			Tmp[Row] += SHBandRotateMatrix[Row * N + Col] * SHBand[Col];
	}
	for (int i = 0; i < N; ++i)
		SHBand[i] = Tmp[i];
}

// ============================================================================
// TSHRotateMatrices constructor / TSHVectorCommon::ApplySHRotateMatrix /
// TSHVectorRGBCommon::ApplySHRotateMatrix
// ============================================================================

namespace UCommon::Details
{
	// Dispatch: fill the rotation matrix for band BandOrder into Matrices.
	// Only bands 2 and 3 are supported via ComputeSHBand2/3RotateMatrix.
	// For higher bands (4, 5, ...) add explicit specializations here.
	template<int BandOrder, int Order>
	void ComputeOneBandRotateMatrix(TSHRotateMatrices<Order>& Matrices, const FMatrix3x3f& RotateMatrix)
	{
		static_assert(BandOrder >= 2 && BandOrder <= Order);
		if constexpr (BandOrder == 2)
			ComputeSHBand2RotateMatrix(Matrices.template GetBand<2>().GetData(), RotateMatrix);
		else if constexpr (BandOrder == 3)
			ComputeSHBand3RotateMatrix(Matrices.template GetBand<3>().GetData(), RotateMatrix);
		else
			static_assert(BandOrder < 4, "ComputeOneBandRotateMatrix: no implementation for BandOrder >= 4");
	}

	// Fold over bands 2..Order using integer_sequence (Is = 0, 1, ..., Order-2 => Band = 2, 3, ..., Order)
	template<int Order, int... Is>
	void FillSHRotateMatrices(TSHRotateMatrices<Order>& Matrices, const FMatrix3x3f& RotateMatrix,
	                          std::integer_sequence<int, Is...>)
	{
		(ComputeOneBandRotateMatrix<Is + 2>(Matrices, RotateMatrix), ...);
	}

	// Apply per-band rotation for a single-channel V[] array.
	// SHIndexOffset = Order*Order - Basis  (0 for TSHVector, 1 for TSHVectorAC)
	// Band k's coefficients start at V[(k-1)^2 - SHIndexOffset].
	template<int Order, int SHIndexOffset, int... Is>
	void ApplyBandRotations(float* V, const TSHRotateMatrices<Order>& Matrices,
	                        std::integer_sequence<int, Is...>)
	{
		// Is = 0, 1, ..., Order-2  =>  Band = Is+2 = 2, 3, ..., Order
		// Band k base in V: (k-1)^2 - SHIndexOffset  =  Is*Is+2*Is+1 - SHIndexOffset
		(ApplySHRotateMatrix(
			TSHBandView<Is + 2>(&V[(Is + 1) * (Is + 1) - SHIndexOffset]),
			Matrices.template GetBand<Is + 2>().GetData()
		), ...);
	}
}

template<int Order>
UCommon::TSHRotateMatrices<Order>::TSHRotateMatrices(const FMatrix3x3f& RotateMatrix)
{
	Details::FillSHRotateMatrices(*this, RotateMatrix, std::make_integer_sequence<int, Order - 1>{});
}

template<typename DerivedType, int InMaxSHOrder, int InMaxSHBasis>
DerivedType UCommon::TSHVectorCommon<DerivedType, InMaxSHOrder, InMaxSHBasis>::ApplySHRotateMatrix(
	const TSHRotateMatrices<MaxSHOrder>& Matrices) const
{
	static_assert(MaxSHOrder >= 2, "ApplySHRotateMatrix requires MaxSHOrder >= 2");
	DerivedType Result = static_cast<const DerivedType&>(*this);
	// SHIndexOffset: how many leading coefficients are absent from V[]
	//   TSHVector  (Basis = Order^2):      Offset = 0  (V[0] = L0)
	//   TSHVectorAC (Basis = Order^2 - 1): Offset = 1  (V[0] = L1,m=-1)
	constexpr int SHIndexOffset = InMaxSHOrder * InMaxSHOrder - InMaxSHBasis;
	Details::ApplyBandRotations<MaxSHOrder, SHIndexOffset>(
		Result.V, Matrices, std::make_integer_sequence<int, MaxSHOrder - 1>{});
	return Result;
}

template<typename DerivedType, template<int> class TElement, int InMaxSHOrder, int InMaxSHBasis>
DerivedType UCommon::TSHVectorRGBCommon<DerivedType, TElement, InMaxSHOrder, InMaxSHBasis>::ApplySHRotateMatrix(
	const TSHRotateMatrices<MaxSHOrder>& Matrices) const
{
	static_assert(MaxSHOrder >= 2, "ApplySHRotateMatrix requires MaxSHOrder >= 2");
	DerivedType Result;
	Result.R = static_cast<const DerivedType&>(*this).R.ApplySHRotateMatrix(Matrices);
	Result.G = static_cast<const DerivedType&>(*this).G.ApplySHRotateMatrix(Matrices);
	Result.B = static_cast<const DerivedType&>(*this).B.ApplySHRotateMatrix(Matrices);
	return Result;
}
