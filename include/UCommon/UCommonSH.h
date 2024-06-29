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

#include "UCommonConfig.h"
#include "UCommonUtils.h"

#define UBPA_UCOMMON_SH_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
	template<int l, int m> constexpr float SHK = UCommon::SHK<l, m>; \
	template<int i> constexpr int SHIndexToL = UCommon::SHIndexToL<i>; \
	template<int i> constexpr int SHIndexToM = UCommon::SHIndexToM<i>; \
	template<int Order> using TSHBandVector = UCommon::TSHBandVector<Order>; \
	template<int Order> using TSHBandVectorRGB = UCommon::TSHBandVectorRGB<Order>; \
	template<int Order> using TSHVector = UCommon::TSHVector<Order>; \
	template<int Order> using TSHVectorRGB = UCommon::TSHVectorRGB<Order>; \
	using FSHBandVector2 = UCommon::FSHBandVector2; \
	using FSHBandVector3 = UCommon::FSHBandVector3; \
	using FSHVector2 = UCommon::FSHVector2; \
	using FSHVector3 = UCommon::FSHVector3; \
	using FSHBandVectorRGB2 = UCommon::FSHBandVectorRGB2; \
	using FSHBandVectorRGB3 = UCommon::FSHBandVectorRGB3; \
	using FSHVectorRGB2 = UCommon::FSHVectorRGB2; \
	using FSHVectorRGB3 = UCommon::FSHVectorRGB3; \
}

namespace UCommon { namespace Details { template<int l, int m> constexpr float SHKImpl(); } }

namespace UCommon
{
	// SH normalization constants
	template<int l, int m>
	constexpr float SHK = Details::SHKImpl<l, m>();

	template<int l, int m>
	constexpr float SH(float x, float y, float z);

	template<int i>
	constexpr int SHIndexToL = i == 0 ? 0 : (i < 4 ? 1 : 2);

	template<int i>
	constexpr int SHIndexToM = i == 0 ? 0 : (i < 4 ? i - 2 : i - 6);

	template<int Order> class TSHVectorRGB;
	template<int Order> class TSHBandVectorRGB;

	template<typename DerivedType, int InMaxSHOrder, int InMaxSHBasis>
	class TSHVectorBase
	{
	public:
		static constexpr int MaxSHOrder = InMaxSHOrder;
		static constexpr int MaxSHBasis = InMaxSHBasis;
		float V[MaxSHBasis];

		static_assert(MaxSHOrder > 1, "Invalid MaxSHOrder");

		/** Default constructor. */
		TSHVectorBase() : V{ 0 } {}

		template<typename ArgType0, typename... ArgTypes>
		TSHVectorBase(ArgType0 Arg0, ArgTypes... Args) : V{ Arg0, Args... } {}

		/** Returns the value of the SH basis L,M at the point on the sphere defined by the unit vector Vector. */
		static DerivedType SHBasisFunction(const FVector& Vector);

		/** Unnormalized */
		FVector GetLinearVector() const
		{
			constexpr int BaseIndex = 2 - (InMaxSHOrder * InMaxSHOrder - InMaxSHBasis);
			static_assert(BaseIndex >= 1, "invalid base index");
			return { -V[BaseIndex + 1], -V[BaseIndex - 1], V[BaseIndex] };
		}

		/** Scalar multiplication operator. */
		/** Changed to float& from float to avoid LHS **/
		friend inline DerivedType operator*(const DerivedType& A, const float& B)
		{
			DerivedType Result;
			for (int i = 0; i < MaxSHBasis; i++)
			{
				Result.V[i] = A.V[i] * B;
			}
			return Result;
		}

		/** Scalar multiplication operator. */
		/** Changed to float& from float to avoid LHS **/
		friend inline DerivedType operator*(const float& A, const DerivedType& B)
		{
			return B * A;
		}

		/** Scalar division operator. */
		friend inline DerivedType operator/(const DerivedType& A, const float& Scalar)
		{
			const float B = 1.f / Scalar;
			return A * B;
		}

		/** Addition operator. */
		friend inline DerivedType operator+(const DerivedType& A, const DerivedType& B)
		{
			DerivedType Result;
			for (int i = 0; i < MaxSHBasis; i++)
			{
				Result.V[i] = A.V[i] + B.V[i];
			}
			return Result;
		}

		/** Subtraction operator. */
		friend inline DerivedType operator-(const DerivedType& A, const DerivedType& B)
		{
			DerivedType Result;
			for (int i = 0; i < MaxSHBasis; i++)
			{
				Result.V[i] = A.V[i] - B.V[i];
			}
			return Result;
		}

		/** Dot product operator. */
		static inline float Dot(const DerivedType& A, const DerivedType& B)
		{
			float Result = 0.f;
			for (int i = 0; i < MaxSHBasis; i++)
			{
				Result += A.V[i] * B.V[i];
			}
			return Result;
		}

		/** In-place addition operator. */
		/** Changed from (*this = *this + B;} to calculate here to avoid LHS **/
		/** Now this avoids TSHVector + operator thus LHS on *this as well as Result and more **/
		inline DerivedType& operator+=(const DerivedType& B)
		{
			for (int i = 0; i < MaxSHBasis; i++)
			{
				V[i] += B.V[i];
			}
			return *this;
		}

		/** In-place subtraction operator. */
		/** Changed from (*this = *this - B;} to calculate here to avoid LHS **/
		/** Now this avoids TSHVector - operator thus LHS on *this as well as Result and **/
		inline DerivedType& operator-=(const DerivedType& B)
		{
			for (int i = 0; i < MaxSHBasis; i++)
			{
				V[i] -= B.V[i];
			}
			return *this;
		}

		/** In-place scalar multiplication operator. */
		/** Changed to float& from float to avoid LHS **/
		/** Changed from (*this = *this * B;) to calculate here to avoid LHS **/
		/** Now this avoids TSHVector * operator thus LHS on *this as well as Result and LHS **/
		inline DerivedType& operator*=(const float& B)
		{
			for (int i = 0; i < MaxSHBasis; i++)
			{
				V[i] *= B;
			}
			return *this;
		}

		/** In-place scalar division operator. */
		/** Changed to float& from float to avoid LHS **/
		/** Changed from (*this = *this * (1.0f/B);) to calculate here to avoid LHS **/
		/** Now this avoids TSHVector * operator thus LHS on *this as well as Result and LHS **/
		inline DerivedType& operator/=(const float& Scalar)
		{
			const float B = 1.f / Scalar;
			return operator*=(B);
		}

		inline float operator()(const FVector& Vector) const;
	};

	template<typename DerivedType, template<int> typename TElement, int InMaxSHOrder, int InMaxSHBasis>
	class TSHVectorRGBBase
	{
	public:
		static constexpr int MaxSHOrder = InMaxSHOrder;
		static constexpr int MaxSHBasis = InMaxSHBasis;

		static_assert(MaxSHOrder > 0, "Invalid MaxSHOrder");

		TElement<MaxSHOrder> R;
		TElement<MaxSHOrder> G;
		TElement<MaxSHOrder> B;

		TSHVectorRGBBase() {}

		TSHVectorRGBBase(const TElement<MaxSHOrder>& InR, const TElement<MaxSHOrder>& InG, const TElement<MaxSHOrder>& InB)
			: R(InR), G(InG), B(InB) {}

		/** Calculates greyscale spherical harmonic coefficients. */
		TElement<MaxSHOrder> GetLuminance() const
		{
			return R * 0.3f + G * 0.59f + B * 0.11f;
		}

		/** Calculates srgb spherical harmonic coefficients. */
		TElement<MaxSHOrder> GetSrgbLuminance() const
		{
			return R * 0.2126f + G * 0.7152f + B * 0.0722f;
		}

		void Desaturate(float DesaturateFraction)
		{
			TElement<MaxSHOrder> Desaturated = GetLuminance() * DesaturateFraction;

			R = R * (1 - DesaturateFraction) + Desaturated;
			G = G * (1 - DesaturateFraction) + Desaturated;
			B = B * (1 - DesaturateFraction) + Desaturated;
		}

		/** Scalar multiplication operator. */
		/** Changed to float& from float to avoid LHS **/
		friend inline DerivedType operator*(const DerivedType& A, const float& Scalar)
		{
			DerivedType Result;
			Result.R = A.R * Scalar;
			Result.G = A.G * Scalar;
			Result.B = A.B * Scalar;
			return Result;
		}

		/** Scalar multiplication operator. */
		/** Changed to float& from float to avoid LHS **/
		friend inline DerivedType operator*(const float& Scalar, const DerivedType& A)
		{
			DerivedType Result;
			Result.R = A.R * Scalar;
			Result.G = A.G * Scalar;
			Result.B = A.B * Scalar;
			return Result;
		}

		/** Color multiplication operator. */
		friend inline DerivedType operator*(const DerivedType& A, const FVector& Color)
		{
			DerivedType Result;
			Result.R = A.R * Color.X;
			Result.G = A.G * Color.Y;
			Result.B = A.B * Color.Z;
			return Result;
		}

		/** Color multiplication operator. */
		friend inline DerivedType operator*(const FVector& Color, const DerivedType& A)
		{
			DerivedType Result;
			Result.R = A.R * Color.X;
			Result.G = A.G * Color.Y;
			Result.B = A.B * Color.Z;
			return Result;
		}

		/** Division operator. */
		friend inline DerivedType operator/(const DerivedType& A, const float& InB)
		{
			DerivedType Result;
			Result.R = A.R / InB;
			Result.G = A.G / InB;
			Result.B = A.B / InB;
			return Result;
		}

		/** Addition operator. */
		friend inline DerivedType operator+(const DerivedType& A, const DerivedType& InB)
		{
			DerivedType Result;
			Result.R = A.R + InB.R;
			Result.G = A.G + InB.G;
			Result.B = A.B + InB.B;
			return Result;
		}

		/** Subtraction operator. */
		friend inline DerivedType operator-(const DerivedType& A, const DerivedType& InB)
		{
			DerivedType Result;
			Result.R = A.R - InB.R;
			Result.G = A.G - InB.G;
			Result.B = A.B - InB.B;
			return Result;
		}

		/** Dot product operator. */
		static inline FVector Dot(const DerivedType& A, const TElement<MaxSHOrder>& InB)
		{
			FVector Result;
			Result.X = TElement<MaxSHOrder>::Dot(A.R, InB);
			Result.Y = TElement<MaxSHOrder>::Dot(A.G, InB);
			Result.Z = TElement<MaxSHOrder>::Dot(A.B, InB);
			return Result;
		}

		/** In-place addition operator. */
		/** Changed from (*this = *this + InB;) to separate all calc to avoid LHS **/

		/** Now it calls directly += operator in TSHVector (avoid TSHVectorRGBBase + operator) **/
		inline DerivedType& operator+=(const DerivedType& InB)
		{
			R += InB.R;
			G += InB.G;
			B += InB.B;

			return *this;
		}

		/** In-place subtraction operator. */
		/** Changed from (*this = *this - InB;) to separate all calc to avoid LHS **/
		/** Now it calls directly -= operator in TSHVector (avoid TSHVectorRGBBase - operator) **/
		inline DerivedType& operator-=(const DerivedType& InB)
		{
			R -= InB.R;
			G -= InB.G;
			B -= InB.B;

			return *this;
		}

		/** In-place scalar multiplication operator. */
		/** Changed from (*this = *this * InB;) to separate all calc to avoid LHS **/
		/** Now it calls directly *= operator in TSHVector (avoid TSHVectorRGBBase * operator) **/
		inline DerivedType& operator*=(const float& Scalar)
		{
			R *= Scalar;
			G *= Scalar;
			B *= Scalar;

			return *this;
		}

		inline TElement<MaxSHOrder>& operator[](int Index)
		{
			UBPA_UCOMMON_ASSERT(Index >= 0 && Index < 3);
			return (&R)[Index];
		}

		inline const TElement<MaxSHOrder>& operator[](int Index) const
		{
			return const_cast<TSHVectorRGBBase*>(this)->operator[](Index);
		}

		inline FVector operator()(const FVector& Vector) const;

		inline DerivedType ToYCoCg() const
		{
			DerivedType SHVectorRGBBaseYCoCg;
			for (int i = 0; i < MaxSHBasis; i++)
			{
				RGBToYCoCg(R.V[i], G.V[i], B.V[i], SHVectorRGBBaseYCoCg.R.V[i], SHVectorRGBBaseYCoCg.G.V[i], SHVectorRGBBaseYCoCg.B.V[i]);
			}
			return SHVectorRGBBaseYCoCg;
		}
	};

	template<int Order>
	class TSHBandVector : public TSHVectorBase<TSHBandVector<Order>, Order, 2 * Order - 1>
	{
	public:
		using Super = TSHVectorBase<TSHBandVector<Order>, Order, 2 * Order - 1>;
		using Super::TSHVectorBase;

		using RGBType = TSHBandVectorRGB<Order>;

		TSHBandVector(float V1, float V2, float V3) : Super{ V1,V2,V3 } {}

		explicit TSHBandVector(const FVector& Vector) : TSHBandVector(Vector.X, Vector.Y, Vector.Z) {}

		/** Returns the value of the SH basis L,M at the point on the sphere defined by the unit vector Vector and ZH coefficient. */
		static TSHBandVector ZHToSHBasisFunction(float ZHCoefficient, const FVector& Vector)
		{
			return Super::SHBasisFunction(Vector) * ZHCoefficient;
		}
	};

	template<int Order>
	class TSHBandVectorRGB : public TSHVectorRGBBase<TSHBandVectorRGB<Order>, TSHBandVector, Order, Order* Order>
	{
	public:
		using Super = TSHVectorRGBBase<TSHBandVectorRGB<Order>, TSHBandVector, Order, Order* Order>;
		using Super::TSHVectorRGBBase;

		/** Returns the value of the SH basis L,M at the point on the sphere defined by the unit vector Vector and ZH coefficient. */
		static TSHBandVectorRGB ZHToSHBasisFunction(const FVector& ZHCoefficient, const FVector& Vector)
		{
			return Super::SHBasisFunction(Vector) * ZHCoefficient;
		}
	};

	/** A vector of spherical harmonic coefficients. */
	template<int Order>
	class TSHVector : public TSHVectorBase<TSHVector<Order>, Order, Order* Order>
	{
	public:
		using Super = TSHVectorBase<TSHVector<Order>, Order, Order* Order>;
		using Super::TSHVectorBase;

		using RGBType = TSHVectorRGB<Order>;

		TSHVector(float V0, float V1, float V2, float V3) : Super{ V0,V1,V2,V3 } {}

		explicit TSHVector(const FVector4& Vector) : TSHVector(Vector.X, Vector.Y, Vector.Z, Vector.W) {}

		template<int OtherOrder>
		explicit TSHVector(const TSHVector<OtherOrder>& Other)
		{
			if (Super::MaxSHBasis <= TSHVector<OtherOrder>::Super::MaxSHBasis)
			{
				for (int i = 0; i < Super::MaxSHBasis; i++)
				{
					Super::V[i] = Other.V[i];
				}
			}
			else
			{
				for (int i = 0; i < TSHVector<OtherOrder>::Super::MaxSHBasis; i++)
				{
					Super::V[i] = Other.V[i];
				}
				for (int i = TSHVector<OtherOrder>::Super::MaxSHBasis; i < Super::MaxSHBasis; i++)
				{
					Super::V[i] = 0.f;
				}
			}
		}

		template<int BandOrder>
		TSHBandVector<BandOrder>& GetBand()
		{
			constexpr int IndexBase = Pow2(BandOrder - 1);
			return *reinterpret_cast<TSHBandVector<BandOrder>*>(&Super::V[IndexBase]);
		}

		template<int BandOrder>
		const TSHBandVector<BandOrder>& GetBand() const
		{
			return const_cast<TSHVector*>(this)->GetBand<BandOrder>();
		}
	};

	/** A vector of colored spherical harmonic coefficients. */
	template<int Order>
	class TSHVectorRGB : public TSHVectorRGBBase<TSHVectorRGB<Order>, TSHVector, Order, Order * Order>
	{
	public:
		using Super = TSHVectorRGBBase<TSHVectorRGB<Order>, TSHVector, Order, Order* Order>;
		using Super::TSHVectorRGBBase;

		template<int OtherOrder>
		explicit TSHVectorRGB(const TSHVectorRGB<OtherOrder>& Other)
		{
			Super::R = (TSHVector<Order>)Other.R;
			Super::G = (TSHVector<Order>)Other.G;
			Super::B = (TSHVector<Order>)Other.B;
		}

		template<int OtherOrder>
		explicit TSHVectorRGB(const TSHVector<OtherOrder>& Other)
		{
			Super::R = (TSHVector<Order>)Other;
			Super::G = (TSHVector<Order>)Other;
			Super::B = (TSHVector<Order>)Other;
		}

		template<int BandOrder>
		const TSHBandVectorRGB<BandOrder> GetBand() const
		{
			return { Super::R.template GetBand<BandOrder>(),Super::G.template GetBand<BandOrder>(),Super::B.template GetBand<BandOrder>() };
		}
	};

	using FSHBandVector2 = TSHBandVector<2>;
	using FSHBandVector3 = TSHBandVector<3>;
	using FSHBandVectorRGB2 = TSHBandVectorRGB<2>;
	using FSHBandVectorRGB3 = TSHBandVectorRGB<3>;

	using FSHVector2 = TSHVector<2>;
	using FSHVector3 = TSHVector<3>;
	using FSHVectorRGB2 = TSHVectorRGB<2>;
	using FSHVectorRGB3 = TSHVectorRGB<3>;

	template<template<int> typename SHVectorType, int Order>
	typename SHVectorType<Order>::RGBType operator*(const SHVectorType<Order>& SHVector, const FVector& Color)
	{
		return typename SHVectorType<Order>::RGBType(SHVector) * Color;
	}
} // namespace UCommon

UBPA_UCOMMON_SH_TO_NAMESPACE(UCommonTest)

#include "UCommonSH.inl"
