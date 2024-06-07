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
	template<int i> constexpr int SHIndexToL = UCommon::SHIndexToL<i>; \
	template<int i> constexpr int SHIndexToM = UCommon::SHIndexToM<i>; \
	template<int Order> using TSHVector = UCommon::TSHVector<Order>; \
	template<int Order> using TSHVectorRGB = UCommon::TSHVectorRGB<Order>; \
	using FSHVector2 = UCommon::FSHVector2; \
	using FSHVector3 = UCommon::FSHVector3; \
	using FSHVectorRGB2 = UCommon::FSHVectorRGB2; \
	using FSHVectorRGB3 = UCommon::FSHVectorRGB3; \
}

namespace UCommon
{
	template<int l, int m>
	constexpr float SH(float x, float y, float z);

	template<int i>
	constexpr int SHIndexToL = i == 0 ? 0 : (i < 4 ? 1 : 2);

	template<int i>
	constexpr int SHIndexToM = i == 0 ? 0 : (i < 4 ? i - 2 : i - 6);

	/** A vector of spherical harmonic coefficients. */
	template<int Order>
	class TSHVector
	{
	public:
		static constexpr int MaxSHOrder = Order;
		static constexpr int MaxSHBasis = MaxSHOrder * MaxSHOrder;
		float V[MaxSHBasis];

		/** Default constructor. */
		TSHVector() : V{ 0 } {}

		TSHVector(float V0, float V1, float V2, float V3) : V{ V0,V1,V2,V3 } {}

		explicit TSHVector(const FVector4& Vector) : TSHVector(Vector.X, Vector.Y, Vector.Z, Vector.W) {}

		template<int OtherOrder>
		explicit TSHVector(const TSHVector<OtherOrder>&Other)
		{
			if (MaxSHBasis <= TSHVector<OtherOrder>::MaxSHBasis)
			{
				for (int i = 0; i < MaxSHBasis; i++)
				{
					V[i] = Other.V[i];
				}
			}
			else
			{
				for (int i = 0; i < TSHVector<OtherOrder>::MaxSHBasis; i++)
				{
					V[i] = Other.V[i];
				}
				for (int i = TSHVector<OtherOrder>::MaxSHBasis; i < MaxSHBasis; i++)
				{
					V[i] = 0.f;
				}
			}
		}

		/** Returns the value of the SH basis L,M at the point on the sphere defined by the unit vector Vector. */
		static TSHVector SHBasisFunction(const FVector& Vector);

		/** Scalar multiplication operator. */
		/** Changed to float& from float to avoid LHS **/
		friend inline TSHVector operator*(const TSHVector & A, const float& B)
		{
			TSHVector Result;
			for (int i = 0; i < MaxSHBasis; i++)
			{
				Result.V[i] = A.V[i] * B;
			}
			return Result;
		}

		/** Scalar division operator. */
		friend inline TSHVector operator/(const TSHVector & A, const float& Scalar)
		{
			const float B = 1.f / Scalar;
			return A * B;
		}

		/** Addition operator. */
		friend inline TSHVector operator+(const TSHVector & A, const TSHVector & B)
		{
			TSHVector Result;
			for (int i = 0; i < MaxSHBasis; i++)
			{
				Result.V[i] = A.V[i] * B.V[i];
			}
			return Result;
		}

		/** Subtraction operator. */
		friend inline TSHVector operator-(const TSHVector & A, const TSHVector & B)
		{
			TSHVector Result;
			for (int i = 0; i < MaxSHBasis; i++)
			{
				Result.V[i] = A.V[i] - B.V[i];
			}
			return Result;
		}

		/** Dot product operator. */
		static inline float Dot(const TSHVector & A, const TSHVector & B)
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
		inline TSHVector& operator+=(const TSHVector & B)
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
		inline TSHVector& operator-=(const TSHVector & B)
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
		inline TSHVector& operator*=(const float& B)
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
		inline TSHVector& operator/=(const float& Scalar)
		{
			const float B = 1.f / Scalar;
			return operator*=(B);
		}

		inline float operator()(const FVector& Vector) const;
	};

	/** A vector of colored spherical harmonic coefficients. */
	template<int Order>
	class TSHVectorRGB
	{
	public:
		static constexpr int MaxSHOrder = Order;
		static constexpr int MaxSHBasis = MaxSHOrder * MaxSHOrder;

		TSHVector<Order> R;
		TSHVector<Order> G;
		TSHVector<Order> B;

		TSHVectorRGB() {}

		template<int OtherOrder>
		explicit TSHVectorRGB(const TSHVectorRGB<OtherOrder>& Other)
		{
			R = (TSHVector<Order>)Other.R;
			G = (TSHVector<Order>)Other.G;
			B = (TSHVector<Order>)Other.B;
		}

		template<int OtherOrder>
		explicit TSHVectorRGB(const TSHVector<OtherOrder>& Other)
		{
			R = (TSHVector<Order>)Other;
			G = (TSHVector<Order>)Other;
			B = (TSHVector<Order>)Other;
		}

		/** Calculates greyscale spherical harmonic coefficients. */
		TSHVector<Order> GetLuminance() const
		{
			return R * 0.3f + G * 0.59f + B * 0.11f;
		}

		void Desaturate(float DesaturateFraction)
		{
			TSHVector<Order> Desaturated = GetLuminance() * DesaturateFraction;

			R = R * (1 - DesaturateFraction) + Desaturated;
			G = G * (1 - DesaturateFraction) + Desaturated;
			B = B * (1 - DesaturateFraction) + Desaturated;
		}

		/** Scalar multiplication operator. */
		/** Changed to float& from float to avoid LHS **/
		friend inline TSHVectorRGB operator*(const TSHVectorRGB& A, const float& Scalar)
		{
			TSHVectorRGB Result;
			Result.R = A.R * Scalar;
			Result.G = A.G * Scalar;
			Result.B = A.B * Scalar;
			return Result;
		}

		/** Scalar multiplication operator. */
		/** Changed to float& from float to avoid LHS **/
		friend inline TSHVectorRGB operator*(const float& Scalar, const TSHVectorRGB& A)
		{
			TSHVectorRGB Result;
			Result.R = A.R * Scalar;
			Result.G = A.G * Scalar;
			Result.B = A.B * Scalar;
			return Result;
		}

		/** Color multiplication operator. */
		friend inline TSHVectorRGB operator*(const TSHVectorRGB& A, const FVector& Color)
		{
			TSHVectorRGB Result;
			Result.R = A.R * Color.X;
			Result.G = A.G * Color.Y;
			Result.B = A.B * Color.Z;
			return Result;
		}

		/** Color multiplication operator. */
		friend inline TSHVectorRGB operator*(const FVector& Color, const TSHVectorRGB& A)
		{
			TSHVectorRGB Result;
			Result.R = A.R * Color.X;
			Result.G = A.G * Color.Y;
			Result.B = A.B * Color.Z;
			return Result;
		}

		/** Division operator. */
		friend inline TSHVectorRGB operator/(const TSHVectorRGB& A, const float& InB)
		{
			TSHVectorRGB Result;
			Result.R = A.R / InB;
			Result.G = A.G / InB;
			Result.B = A.B / InB;
			return Result;
		}

		/** Addition operator. */
		friend inline TSHVectorRGB operator+(const TSHVectorRGB& A, const TSHVectorRGB& InB)
		{
			TSHVectorRGB Result;
			Result.R = A.R + InB.R;
			Result.G = A.G + InB.G;
			Result.B = A.B + InB.B;
			return Result;
		}

		/** Subtraction operator. */
		friend inline TSHVectorRGB operator-(const TSHVectorRGB& A, const TSHVectorRGB& InB)
		{
			TSHVectorRGB Result;
			Result.R = A.R - InB.R;
			Result.G = A.G - InB.G;
			Result.B = A.B - InB.B;
			return Result;
		}

		/** Dot product operator. */
		static inline FVector Dot(const TSHVectorRGB& A, const TSHVector<Order>& InB)
		{
			FVector Result;
			Result.X = TSHVector<Order>::Dot(A.R, InB);
			Result.Y = TSHVector<Order>::Dot(A.G, InB);
			Result.Z = TSHVector<Order>::Dot(A.B, InB);
			return Result;
		}

		/** In-place addition operator. */
		/** Changed from (*this = *this + InB;) to separate all calc to avoid LHS **/

		/** Now it calls directly += operator in TSHVector (avoid TSHVectorRGB + operator) **/
		inline TSHVectorRGB& operator+=(const TSHVectorRGB& InB)
		{
			R += InB.R;
			G += InB.G;
			B += InB.B;

			return *this;
		}

		/** In-place subtraction operator. */
		/** Changed from (*this = *this - InB;) to separate all calc to avoid LHS **/
		/** Now it calls directly -= operator in TSHVector (avoid TSHVectorRGB - operator) **/
		inline TSHVectorRGB& operator-=(const TSHVectorRGB& InB)
		{
			R -= InB.R;
			G -= InB.G;
			B -= InB.B;

			return *this;
		}

		/** In-place scalar multiplication operator. */
		/** Changed from (*this = *this * InB;) to separate all calc to avoid LHS **/
		/** Now it calls directly *= operator in TSHVector (avoid TSHVectorRGB * operator) **/
		inline TSHVectorRGB& operator*=(const float& Scalar)
		{
			R *= Scalar;
			G *= Scalar;
			B *= Scalar;

			return *this;
		}

		inline TSHVector<Order>& operator[](int Index)
		{
			UBPA_UCOMMON_ASSERT(Index >= 0 && Index < 3);
			return (&R)[Index];
		}

		inline const TSHVector<Order>& operator[](int Index) const
		{
			return const_cast<TSHVectorRGB*>(this)->operator[](Index);
		}

		inline FVector operator()(const FVector& Vector) const;

		inline TSHVectorRGB ToYCoCg() const
		{
			TSHVectorRGB SHVectorYCoCg;
			for (int i = 0; i < MaxSHBasis; i++)
			{
				RGBToYCoCg(R.V[i], G.V[i], B.V[i], SHVectorYCoCg.R.V[i], SHVectorYCoCg.G.V[i], SHVectorYCoCg.B.V[i]);
			}
			return SHVectorYCoCg;
		}
	};

	using FSHVector2 = TSHVector<2>;
	using FSHVector3 = TSHVector<3>;
	using FSHVectorRGB2 = TSHVectorRGB<2>;
	using FSHVectorRGB3 = TSHVectorRGB<3>;
} // namespace UCommon

UBPA_UCOMMON_SH_TO_NAMESPACE(UCommonTest)

#include "UCommonSH.inl"
