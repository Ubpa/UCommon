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

/*
 * SH Type System Overview
 * =======================
 *
 * This file defines template types for Spherical Harmonics (SH),
 * organized along four orthogonal dimensions:
 *   - Scope      : Band (single order) vs Vector (all orders, L0..L(N-1))
 *   - Ownership  : View (non-owning reference) vs Vector/Owning (inline float array)
 *   - Channel    : single-channel (grayscale) vs RGB (three single-channel members)
 *   - DC         : with DC (includes L0) vs AC-only (excludes L0)
 *
 * --------------------------------------------------------------------
 * 1. Full-order coefficients (spanning all SH orders 0 .. Order-1,
 *    Order*Order basis functions total)
 *
 *   TSHVectorCommon<Derived, Order, Basis>       -- CRTP base, stores float V[Basis]
 *     +-- TSHVector<N>                              N*N coefficients (with DC, L0..L(N-1))
 *     +-- TSHVectorAC<N>                            N*N-1 coefficients (no DC, L1..L(N-1))
 *
 *   TSHVectorRGBCommon<Derived, TElem, Order, Basis>  -- CRTP base, stores TElem<N> R,G,B
 *     +-- TSHVectorRGB<N>      R/G/B each a TSHVector<N>
 *     +-- TSHVectorACRGB<N>    R/G/B each a TSHVectorAC<N> (no DC)
 *
 * --------------------------------------------------------------------
 * 2. Single-band coefficients (single SH band, order N-1 has 2N-1 coefficients)
 *
 *   TSHBandCommon<Derived, Order>                -- CRTP base
 *     +-- TSHBandViewCommon<Order, bConst>          -- View base (holds Data pointer, non-owning)
 *     |   +-- TSHBandView<Order, false>              mutable view
 *     |   +-- TSHBandView<Order, true>               const view
 *     +-- TSHBandVector<Order>                       owning (inline float Data[2N-1])
 *
 *   TSHBandRGBCommon<Derived, Order>             -- CRTP base
 *     +-- TSHBandViewRGBCommon<Order, bConst>       -- RGB View base (R, G, B as TSHBandView)
 *     |   +-- TSHBandViewRGB<Order, false>           mutable RGB view
 *     |   +-- TSHBandViewRGB<Order, true>            const RGB view
 *     +-- TSHBandVectorRGB<Order>                    owning (R/G/B each a TSHBandVector<N>)
 *
 *   Const view aliases:
 *     TSHBandConstView<N>    = TSHBandView<N, true>
 *     TSHBandConstViewRGB<N> = TSHBandViewRGB<N, true>
 *
 * --------------------------------------------------------------------
 * 3. Common F-prefixed aliases (suffix digit = order, range 2..5)
 *
 *   FSHVector{2..5}           = TSHVector<N>
 *   FSHVectorAC{2..5}         = TSHVectorAC<N>
 *   FSHVectorRGB{2..5}        = TSHVectorRGB<N>
 *   FSHVectorACRGB{2..5}      = TSHVectorACRGB<N>
 *   FSHBandView{2..5}         = TSHBandView<N, false>
 *   FSHBandConstView{2..5}    = TSHBandView<N, true>
 *   FSHBandVector{2..5}       = TSHBandVector<N>
 *   FSHBandViewRGB{2..5}      = TSHBandViewRGB<N, false>
 *   FSHBandConstViewRGB{2..5} = TSHBandViewRGB<N, true>
 *   FSHBandVectorRGB{2..5}    = TSHBandVectorRGB<N>
 *
 * --------------------------------------------------------------------
 * 4. Key relationships
 *
 *   TSHVector<N>::GetBand<K>()        -> TSHBandView<K>        (ref into internal data)
 *   TSHVectorAC<N>::GetBand<K>()      -> TSHBandView<K>        (K >= 2, skips L0)
 *   TSHVectorRGB<N>::GetBand<K>()     -> TSHBandViewRGB<K>
 *   TSHBandView<N>::Vector()          -> TSHBandVector<N>&     (reinterpret, requires contiguous data)
 *
 *   TSHBandVector<N>     -> TSHBandView<N, false>  : implicit (operator TSHBandView<N,false>())
 *   TSHBandView<N,false> -> TSHBandView<N, true>   : implicit (mutable to const)
 *   TSHVector<N>(TSHVector<K>)                     : explicit truncation or zero-padding
 */

#include "Config.h"
#include "Cpp17.h"
#include "Matrix.h"
#include "Utils.h"

#define UBPA_UCOMMON_SH_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
	template<int l, int m> constexpr float SHK = UCommon::SHK<l, m>; \
	template<int i> constexpr int SHIndexToL = UCommon::SHIndexToL<i>; \
	template<int i> constexpr int SHIndexToM = UCommon::SHIndexToM<i>; \
	template<int Order, bool bConst = false> using TSHBandView = UCommon::TSHBandView<Order, bConst>; \
	template<int Order, bool bConst = false> using TSHBandViewRGB = UCommon::TSHBandViewRGB<Order, bConst>; \
	template<int Order> using TSHBandConstView = UCommon::TSHBandConstView<Order>; \
	template<int Order> using TSHBandConstViewRGB = UCommon::TSHBandConstViewRGB<Order>; \
	template<int Order> using TSHBandVector = UCommon::TSHBandVector<Order>; \
	template<int Order> using TSHBandVectorRGB = UCommon::TSHBandVectorRGB<Order>; \
	template<int Order> using TSHVector = UCommon::TSHVector<Order>; \
	template<int Order> using TSHVectorAC = UCommon::TSHVectorAC<Order>; \
	template<int Order> using TSHVectorRGB = UCommon::TSHVectorRGB<Order>; \
	template<int Order> using TSHVectorACRGB = UCommon::TSHVectorACRGB<Order>; \
	using FSHBandView2 = UCommon::FSHBandView2; \
	using FSHBandView3 = UCommon::FSHBandView3; \
	using FSHBandView4 = UCommon::FSHBandView4; \
	using FSHBandView5 = UCommon::FSHBandView5; \
	using FSHBandViewRGB2 = UCommon::FSHBandViewRGB2; \
	using FSHBandViewRGB3 = UCommon::FSHBandViewRGB3; \
	using FSHBandViewRGB4 = UCommon::FSHBandViewRGB4; \
	using FSHBandViewRGB5 = UCommon::FSHBandViewRGB5; \
	using FSHBandConstView2 = UCommon::FSHBandConstView2; \
	using FSHBandConstView3 = UCommon::FSHBandConstView3; \
	using FSHBandConstView4 = UCommon::FSHBandConstView4; \
	using FSHBandConstView5 = UCommon::FSHBandConstView5; \
	using FSHBandConstViewRGB2 = UCommon::FSHBandConstViewRGB2; \
	using FSHBandConstViewRGB3 = UCommon::FSHBandConstViewRGB3; \
	using FSHBandConstViewRGB4 = UCommon::FSHBandConstViewRGB4; \
	using FSHBandConstViewRGB5 = UCommon::FSHBandConstViewRGB5; \
	using FSHBandVector2 = UCommon::FSHBandVector2; \
	using FSHBandVector3 = UCommon::FSHBandVector3; \
	using FSHBandVector4 = UCommon::FSHBandVector4; \
	using FSHBandVector5 = UCommon::FSHBandVector5; \
	using FSHBandVectorRGB2 = UCommon::FSHBandVectorRGB2; \
	using FSHBandVectorRGB3 = UCommon::FSHBandVectorRGB3; \
	using FSHBandVectorRGB4 = UCommon::FSHBandVectorRGB4; \
	using FSHBandVectorRGB5 = UCommon::FSHBandVectorRGB5; \
	using FSHVector2 = UCommon::FSHVector2; \
	using FSHVector3 = UCommon::FSHVector3; \
	using FSHVector4 = UCommon::FSHVector4; \
	using FSHVector5 = UCommon::FSHVector5; \
	using FSHVectorAC2 = UCommon::FSHVectorAC2; \
	using FSHVectorAC3 = UCommon::FSHVectorAC3; \
	using FSHVectorAC4 = UCommon::FSHVectorAC4; \
	using FSHVectorAC5 = UCommon::FSHVectorAC5; \
	using FSHVectorRGB2 = UCommon::FSHVectorRGB2; \
	using FSHVectorRGB3 = UCommon::FSHVectorRGB3; \
	using FSHVectorRGB4 = UCommon::FSHVectorRGB4; \
	using FSHVectorRGB5 = UCommon::FSHVectorRGB5; \
	using FSHVectorACRGB2 = UCommon::FSHVectorACRGB2; \
	using FSHVectorACRGB3 = UCommon::FSHVectorACRGB3; \
	using FSHVectorACRGB4 = UCommon::FSHVectorACRGB4; \
	using FSHVectorACRGB5 = UCommon::FSHVectorACRGB5; \
	template<int Order> using TSHRotateMatrices = UCommon::TSHRotateMatrices<Order>; \
	using FSHRotateMatrices2 = UCommon::FSHRotateMatrices2; \
	using FSHRotateMatrices3 = UCommon::FSHRotateMatrices3; \
}

namespace UCommon { namespace Details { template<int l, int m> constexpr float SHKImpl(); } }

namespace UCommon
{
	// SH normalization constants
	// K(l,m) = sqrt((2l+1)/(4pi) * (l-|m|)!/(l+|m|)!)
	template<int l, int m>
	constexpr float SHK = Details::SHKImpl<l, m>();

	template<int l, int m>
	constexpr float SH(float x, float y, float z);

	template<int l, int m>
	float SH(const FVector3f& w);

	template<int i>
	constexpr int SHIndexToL = i == 0 ? 0 : (i < 4 ? 1 : (i < 9 ? 2 : (i < 16 ? 3 : 4)));

	template<int i>
	constexpr int SHIndexToM = i == 0 ? 0 : (i < 4 ? i - 2 : (i < 9 ? i - 6 : (i < 16 ? i - 12 : i - 20)));

	// Flat container for all SH band rotation matrices (bands 2..Order).
	// Band k's matrix is (2k-1) x (2k-1), stored row-major in Data at offset BandOffset<k>.
	// Construct from a 3x3 rotation matrix: TSHRotateMatrices<Order> mats(rotMatrix);
	//
	// Closed-form sizes (derived from the odd-square sum: 1^2+3^2+...+(2n-1)^2 = n(4n^2-1)/3):
	//
	//   TotalSize          = sum_{k=2}^{Order} (2k-1)^2
	//                      = Order*(4*Order^2-1)/3 - 1
	//
	//   BandOffset<B>      = sum_{k=2}^{B-1} (2k-1)^2           (empty sum = 0 when B=2)
	//                      = (B-1)*(4*(B-1)^2-1)/3 - 1          (evaluates to 0 for B=2)
	template<int Order>
	struct TSHRotateMatrices
	{
		static_assert(Order >= 2, "TSHRotateMatrices requires Order >= 2");

		// Offset (in floats) of band BandOrder's block within Data.
		// BandOrder=2 -> 0, BandOrder=3 -> 9, BandOrder=4 -> 34, ...
		template<int BandOrder>
		static constexpr int BandOffset =
			(BandOrder - 1) * (4 * (BandOrder - 1) * (BandOrder - 1) - 1) / 3 - 1;

		// Total number of floats: sum_{k=2}^{Order} (2k-1)^2 = BandOffset<Order+1>
		static constexpr int TotalSize = BandOffset<Order + 1>;

		// Flat storage: band 2 matrix, then band 3 matrix, ..., then band Order matrix
		float Data[TotalSize];

		// Construct from a 3x3 rotation matrix: fills all band rotation matrices in one call.
		explicit TSHRotateMatrices(const FMatrix3x3f& RotateMatrix);

		// Returns a TSpan over the row-major (2*BandOrder-1) x (2*BandOrder-1) rotation matrix for band BandOrder.
		// Indexed as [row * (2*BandOrder-1) + col], size = (2*BandOrder-1)^2.
		template<int BandOrder>
		TSpan<float> GetBand() noexcept
		{
			static_assert(BandOrder >= 2 && BandOrder <= Order);
			constexpr int N = 2 * BandOrder - 1;
			return { Data + BandOffset<BandOrder>, N * N };
		}

		template<int BandOrder>
		TSpan<const float> GetBand() const noexcept
		{
			static_assert(BandOrder >= 2 && BandOrder <= Order);
			constexpr int N = 2 * BandOrder - 1;
			return { Data + BandOffset<BandOrder>, N * N };
		}

		// Implicit zero-copy conversion to a lower-order view.
		// Valid because TSHRotateMatrices<LowerOrder>::Data is a strict prefix of this->Data.
		template<int LowerOrder,
			typename = std::enable_if_t<(LowerOrder >= 2 && LowerOrder < Order)>>
		operator TSHRotateMatrices<LowerOrder>&() noexcept
		{
			static_assert(sizeof(TSHRotateMatrices<LowerOrder>) <= sizeof(TSHRotateMatrices<Order>),
			"Lower-order matrices must be strictly smaller");
			return reinterpret_cast<TSHRotateMatrices<LowerOrder>&>(*this);
		}

		template<int LowerOrder,
			typename = std::enable_if_t<(LowerOrder >= 2 && LowerOrder < Order)>>
		operator const TSHRotateMatrices<LowerOrder>&() const noexcept
		{
			static_assert(sizeof(TSHRotateMatrices<LowerOrder>) <= sizeof(TSHRotateMatrices<Order>),
			"Lower-order matrices must be strictly smaller");
			return reinterpret_cast<const TSHRotateMatrices<LowerOrder>&>(*this);
		}
	};

	template<int Order> class TSHVectorRGB;
	template<int Order> class TSHVectorACRGB;
	template<int Order> class TSHBandVectorRGB;

	template<typename DerivedType, int InMaxSHOrder, int InMaxSHBasis>
	class TSHVectorCommon
	{
	public:
		static constexpr int MaxSHOrder = InMaxSHOrder;
		static constexpr int MaxSHBasis = InMaxSHBasis;
		float V[MaxSHBasis];

		static_assert(MaxSHOrder > 0 && MaxSHBasis > 0, "Invalid MaxSHOrder or MaxSHBasis");

		/** Default constructor. */
		TSHVectorCommon() : V{ 0 } {}

		TSHVectorCommon(std::initializer_list<float> InitList)
		{
			UBPA_UCOMMON_ASSERT(InitList.size() == MaxSHBasis);
			int i = 0;
			for (float Value : InitList)
			{
				V[i++] = Value;
			}
		}

		template<typename U>
		U& As()&
		{
			static_assert(sizeof(U) == sizeof(TSHVectorCommon), "The size of U is not same with TSHVectorCommon");
			return *reinterpret_cast<U*>(this);
		}

		template<typename U>
		const U& As() const&
		{
			return const_cast<TSHVectorCommon*>(this)->As<U>();
		}

		template<typename U>
		const U&& As() const &&
		{
			static_assert(sizeof(U) == sizeof(TSHVectorCommon), "The size of U is not same with TSHVectorCommon");
			static_assert(alignof(TSHVectorCommon) % alignof(U) == 0, "The alignment of U is not compatible with TSHVectorCommon");
			return reinterpret_cast<const U&&>(*this);
		}

		/** Returns the value of the SH basis L,M at the point on the sphere defined by the unit vector Vector. */
		static DerivedType SHBasisFunction(const FVector3f& Vector);

		/** Unnormalized */
		FVector3f GetLinearVector() const
		{
			constexpr int BaseIndex = 2 - (MaxSHOrder * MaxSHOrder - MaxSHBasis);
			static_assert(BaseIndex >= 1 && BaseIndex + 1 <= MaxSHBasis, "invalid base index");
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
			return As<DerivedType>();
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
			return As<DerivedType>();
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
			return As<DerivedType>();
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

		inline float operator()(const FVector3f& Vector) const;

		inline DerivedType Exp() const
		{
			DerivedType Result;
			for (int i = 0; i < MaxSHBasis; i++)
			{
				Result.V[i] = std::exp(V[i]);
			}
			return Result;
		}

		inline DerivedType Exp2() const
		{
			DerivedType Result;
			for (int i = 0; i < MaxSHBasis; i++)
			{
				Result.V[i] = std::exp2(V[i]);
			}
			return Result;
		}

		// Returns a new DerivedType with all bands 2..MaxSHOrder rotated by the given precomputed matrices.
		// L0 (DC) is rotationally invariant and is left unchanged.
		DerivedType ApplySHRotateMatrix(const TSHRotateMatrices<MaxSHOrder>& Matrices) const;
	};

	template<typename DerivedType, template<int> class TElement, int InMaxSHOrder, int InMaxSHBasis>
	class TSHVectorRGBCommon
	{
	public:
		static constexpr int MaxSHOrder = InMaxSHOrder;
		static constexpr int MaxSHBasis = InMaxSHBasis;

		static_assert(MaxSHOrder > 0, "Invalid MaxSHOrder");

		TElement<MaxSHOrder> R;
		TElement<MaxSHOrder> G;
		TElement<MaxSHOrder> B;

		TSHVectorRGBCommon() {}

		TSHVectorRGBCommon(const TElement<MaxSHOrder>& InR, const TElement<MaxSHOrder>& InG, const TElement<MaxSHOrder>& InB)
			: R(InR), G(InG), B(InB) {}

		template<typename U>
		U& As()&
		{
			static_assert(sizeof(U) == sizeof(TSHVectorRGBCommon), "The size of U is not same with TSHVectorRGBCommon");
			return *reinterpret_cast<U*>(this);
		}

		template<typename U>
		const U& As() const&
		{
			return const_cast<TSHVectorRGBCommon*>(this)->As<U>();
		}

		template<typename U>
		const U&& As() const &&
		{
			static_assert(sizeof(U) == sizeof(TSHVectorRGBCommon), "The size of U is not same with TSHVectorRGBCommon");
			static_assert(alignof(TSHVectorRGBCommon) % alignof(U) == 0, "The alignment of U is not compatible with TSHVectorRGBCommon");
			return reinterpret_cast<const U&&>(*this);
		}

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
		friend inline DerivedType operator*(const DerivedType& A, const FVector3f& Color)
		{
			DerivedType Result;
			Result.R = A.R * Color.X;
			Result.G = A.G * Color.Y;
			Result.B = A.B * Color.Z;
			return Result;
		}

		/** Color multiplication operator. */
		friend inline DerivedType operator*(const FVector3f& Color, const DerivedType& A)
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
		static inline FVector3f Dot(const DerivedType& A, const TElement<MaxSHOrder>& InB)
		{
			FVector3f Result;
			Result.X = TElement<MaxSHOrder>::Dot(A.R, InB);
			Result.Y = TElement<MaxSHOrder>::Dot(A.G, InB);
			Result.Z = TElement<MaxSHOrder>::Dot(A.B, InB);
			return Result;
		}

		/** Dot product operator. */
		static inline TElement<MaxSHOrder> Dot(const DerivedType& A, const FVector3f& InB)
		{
			return A.R * InB.X + A.G * InB.Y + A.B * InB.Z;
		}

		/** In-place addition operator. */
		/** Changed from (*this = *this + InB;) to separate all calc to avoid LHS **/

		/** Now it calls directly += operator in TSHVector (avoid TSHVectorRGBCommon + operator) **/
		inline DerivedType& operator+=(const DerivedType& InB)
		{
			R += InB.R;
			G += InB.G;
			B += InB.B;

			return As<DerivedType>();
		}

		/** In-place subtraction operator. */
		/** Changed from (*this = *this - InB;) to separate all calc to avoid LHS **/
		/** Now it calls directly -= operator in TSHVector (avoid TSHVectorRGBCommon - operator) **/
		inline DerivedType& operator-=(const DerivedType& InB)
		{
			R -= InB.R;
			G -= InB.G;
			B -= InB.B;

			return As<DerivedType>();
		}

		/** In-place scalar multiplication operator. */
		/** Changed from (*this = *this * InB;) to separate all calc to avoid LHS **/
		/** Now it calls directly *= operator in TSHVector (avoid TSHVectorRGBCommon * operator) **/
		inline DerivedType& operator*=(const float& Scalar)
		{
			R *= Scalar;
			G *= Scalar;
			B *= Scalar;

			return As<DerivedType>();
		}

		inline TElement<MaxSHOrder>& operator[](uint64_t Index)
		{
			UBPA_UCOMMON_ASSERT(Index < 3);
			return (&R)[Index];
		}

		inline const TElement<MaxSHOrder>& operator[](uint64_t Index) const
		{
			return const_cast<TSHVectorRGBCommon*>(this)->operator[](Index);
		}

		inline FVector3f operator()(const FVector3f& Vector) const;

		inline DerivedType ToYCoCg() const
		{
			DerivedType SHVectorRGBBaseYCoCg;
			for (int i = 0; i < MaxSHBasis; i++)
			{
				RGBToYCoCg(R.V[i], G.V[i], B.V[i], SHVectorRGBBaseYCoCg.R.V[i], SHVectorRGBBaseYCoCg.G.V[i], SHVectorRGBBaseYCoCg.B.V[i]);
			}
			return SHVectorRGBBaseYCoCg;
		}

		// Returns a new DerivedType with all bands 2..MaxSHOrder rotated by the given precomputed matrices.
		// L0 (DC) is rotationally invariant and is left unchanged.
		DerivedType ApplySHRotateMatrix(const TSHRotateMatrices<MaxSHOrder>& Matrices) const;
	};

	// Forward declarations for CRTP base class
	template<typename DerivedType, int Order>
	class TSHBandCommon;

	template<int Order, bool bConst = false>
	class TSHBandView;

	template<int Order, bool bConst = false>
	class TSHBandViewRGB;

	template<int Order>
	class TSHBandVector;

	template<int Order>
	class TSHBandVectorRGB;

	// Template aliases for const views (must be after forward declarations)
	template<int Order>
	using TSHBandConstView = TSHBandView<Order, true>;

	template<int Order>
	using TSHBandConstViewRGB = TSHBandViewRGB<Order, true>;

	// Common base class for TSHBandView and TSHBandVector (const-agnostic operations)
	// Uses CRTP to avoid virtual functions and maintain memory layout
	template<typename DerivedType, int Order>
	class TSHBandCommon
	{
	public:
		static constexpr int MaxSHOrder = Order;
		static constexpr int MaxSHBasis = 2 * Order - 1;

		static constexpr uint64_t GetSize() noexcept { return MaxSHBasis; }

		// Returns the value of the SH basis for this band at the point on the sphere defined by the unit vector Vector
		static TSHBandVector<Order> SHBasisFunction(const FVector3f& Vector);

		// Static Dot function (accepts any combination of views and vectors)
		static float Dot(TSHBandConstView<Order> A, TSHBandConstView<Order> B) noexcept
		{
			UBPA_UCOMMON_ASSERT(A.GetData() != nullptr);
			UBPA_UCOMMON_ASSERT(B.GetData() != nullptr);
			float Result = 0.0f;
			for (uint64_t i = 0; i < MaxSHBasis; ++i)
			{
				Result += A[i] * B[i];
			}
			return Result;
		}

		// Common operations (implemented in terms of DerivedType class's GetData() and operator[])
		float Dot(TSHBandConstView<Order> Other) const noexcept;

		TSHBandVector<Order> operator+(TSHBandConstView<Order> Other) const noexcept;
		TSHBandVector<Order> operator-(TSHBandConstView<Order> Other) const noexcept;
		TSHBandVector<Order> operator*(float Scalar) const noexcept;
		TSHBandVector<Order> operator/(float Scalar) const noexcept;

		// Multiply by FVector3f to create RGB band (each component multiplied by R, G, B)
		TSHBandVectorRGB<Order> operator*(const FVector3f& Color) const noexcept
		{
			const DerivedType& Derived = AsDerived();
			TSHBandVectorRGB<Order> Result;
			for (uint64_t i = 0; i < MaxSHBasis; ++i)
			{
				Result.R[i] = Derived[i] * Color.X;
				Result.G[i] = Derived[i] * Color.Y;
				Result.B[i] = Derived[i] * Color.Z;
			}
			return Result;
		}

		// GetLinearVector - only available for Order 2 (L1 band)
		template<int O = Order>
		std::enable_if_t<O == 2, FVector3f> GetLinearVector() const
		{
			const DerivedType& Derived = AsDerived();
			// For Order 2 band, we have 3 coefficients (2*2-1 = 3)
			// Data[0] = L1, m=-1
			// Data[1] = L1, m=0
			// Data[2] = L1, m=1
			// Return: { -m=1, -m=-1, m=0 } to match TSHVectorCommon::GetLinearVector pattern
			return FVector3f{ -Derived[2], -Derived[0], Derived[1] };
		}

	protected:
		// CRTP helper methods
		DerivedType& AsDerived() noexcept { return static_cast<DerivedType&>(*this); }
		const DerivedType& AsDerived() const noexcept { return static_cast<const DerivedType&>(*this); }

		~TSHBandCommon() = default;  // Protected destructor to prevent deletion through base pointer
	};

	// Base class for TSHBandView
	template<int Order, bool bConst>
	class TSHBandViewCommon : public TSHBandCommon<TSHBandView<Order, bConst>, Order>
	{
	public:
		using DataType = std::conditional_t<bConst, const float*, float*>;
		using RGBType = TSHBandViewRGB<Order, bConst>;

		// Constructors
		constexpr TSHBandViewCommon() noexcept : Data(nullptr) {}
		explicit constexpr TSHBandViewCommon(DataType InData) noexcept : Data(InData) {}

		// Data access
		DataType GetData() const noexcept { return Data; }

		// Array access
		decltype(auto) operator[](uint64_t Index) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Data != nullptr);
			UBPA_UCOMMON_ASSERT(Index < (TSHBandCommon<TSHBandView<Order, bConst>, Order>::MaxSHBasis));
			return Data[Index];
		}

		// Convert to vector reference (for convenient assignment)
		decltype(auto) Vector() const noexcept
		{
			UBPA_UCOMMON_ASSERT(Data != nullptr);
			return *reinterpret_cast<std::conditional_t<bConst, const TSHBandVector<Order>*, TSHBandVector<Order>*>>(Data);
		}

		// Assignment (shallow copy - rebind pointer)
		TSHBandView<Order, bConst>& operator=(const TSHBandView<Order, bConst>& Other) noexcept
		{
			Data = Other.Data;
			return static_cast<TSHBandView<Order, bConst>&>(*this);
		}

	protected:
		DataType Data;
	};

	// TSHBandView<Order, false> - Mutable specialization
	template<int Order>
	class TSHBandView<Order, false> : public TSHBandViewCommon<Order, false>
	{
		using Super = TSHBandViewCommon<Order, false>;

	public:
		// Constructors
		using Super::Super;

		// Mathematical operators (in-place) - only in mutable version
		TSHBandView& operator+=(TSHBandConstView<Order> Other) const noexcept;
		TSHBandView& operator-=(TSHBandConstView<Order> Other) const noexcept;
		TSHBandView& operator*=(float Scalar) const noexcept;
		TSHBandView& operator/=(float Scalar) const noexcept;
	};

	// TSHBandView<Order, true> - Const specialization
	template<int Order>
	class TSHBandView<Order, true> : public TSHBandViewCommon<Order, true>
	{
		using Super = TSHBandViewCommon<Order, true>;

	public:
		// Constructors
		using Super::Super;

		// Implicit conversion from mutable view
		constexpr TSHBandView(const TSHBandView<Order, false>& Other) noexcept
			: Super(Other.GetData()) {}
	};

	// Common base class for RGB band types (provides Dot functionality)
	template<typename DerivedType, int Order>
	class TSHBandRGBCommon
	{
	public:
		static constexpr int MaxSHOrder = Order;
		static constexpr int MaxSHBasis = 2 * Order - 1;

		// Dot product with single-channel band - static version (returns RGB result)
		static FVector3f Dot(TSHBandConstViewRGB<Order> A, TSHBandConstView<Order> B) noexcept
		{
			return FVector3f{ A.R.Dot(B), A.G.Dot(B), A.B.Dot(B) };
		}

		// Dot product with single-channel band - member version (returns RGB result)
		FVector3f Dot(TSHBandConstView<Order> Other) const noexcept
		{
			const DerivedType& Derived = static_cast<const DerivedType&>(*this);
			return FVector3f{ Derived.R.Dot(Other), Derived.G.Dot(Other), Derived.B.Dot(Other) };
		}

		// Dot product with FVector3f - static version (returns single-channel band)
		static TSHBandVector<Order> Dot(TSHBandConstViewRGB<Order> A, const FVector3f& Color) noexcept
		{
			TSHBandVector<Order> Result;
			for (uint64_t i = 0; i < MaxSHBasis; ++i)
			{
				Result[i] = A.R[i] * Color.X + A.G[i] * Color.Y + A.B[i] * Color.Z;
			}
			return Result;
		}

		// Dot product with FVector3f - member version (returns single-channel band)
		TSHBandVector<Order> Dot(const FVector3f& Color) const noexcept
		{
			const DerivedType& Derived = static_cast<const DerivedType&>(*this);
			TSHBandVector<Order> Result;
			for (uint64_t i = 0; i < MaxSHBasis; ++i)
			{
				Result[i] = Derived.R[i] * Color.X + Derived.G[i] * Color.Y + Derived.B[i] * Color.Z;
			}
			return Result;
		}

		// Channel access via operator[] (similar to TSHVectorRGBCommon)
		// Note: Return type depends on Derived class (could be TSHBandView or TSHBandVector)
		auto& operator[](uint64_t Index) noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 3);
			DerivedType& Derived = static_cast<DerivedType&>(*this);
			return (&Derived.R)[Index];
		}

		auto& operator[](uint64_t Index) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 3);
			const DerivedType& Derived = static_cast<const DerivedType&>(*this);
			return (&Derived.R)[Index];
		}

		static constexpr uint64_t GetSize() noexcept { return MaxSHBasis; }

	protected:
		~TSHBandRGBCommon() = default;
	};

	// Base class for TSHBandViewRGB
	template<int Order, bool bConst>
	class TSHBandViewRGBCommon : public TSHBandRGBCommon<TSHBandViewRGBCommon<Order, bConst>, Order>
	{
	public:
		using Super = TSHBandRGBCommon<TSHBandViewRGBCommon<Order, bConst>, Order>;
		using ViewType = TSHBandView<Order, bConst>;
		using DataType = typename ViewType::DataType;

		ViewType R;
		ViewType G;
		ViewType B;

		// Constructors
		constexpr TSHBandViewRGBCommon() noexcept : R(), G(), B() {}

		constexpr TSHBandViewRGBCommon(DataType InR, DataType InG, DataType InB) noexcept
			: R(InR), G(InG), B(InB) {}

		constexpr TSHBandViewRGBCommon(ViewType InR, ViewType InG, ViewType InB) noexcept
			: R(InR), G(InG), B(InB) {}

		// Assignment (shallow copy - rebind all three pointers)
		TSHBandViewRGB<Order, bConst>& operator=(const TSHBandViewRGB<Order, bConst>& Other) noexcept
		{
			R = Other.R;
			G = Other.G;
			B = Other.B;
			return static_cast<TSHBandViewRGB<Order, bConst>&>(*this);
		}

		// Note: operator[] is inherited from TSHBandRGBCommon

		// Luminance calculation methods
		TSHBandVector<Order> GetLuminance() const noexcept
		{
			return R * 0.3f + G * 0.59f + B * 0.11f;
		}

		TSHBandVector<Order> GetSrgbLuminance() const noexcept
		{
			return R * 0.2126f + G * 0.7152f + B * 0.0722f;
		}

		// Note: Dot functions are inherited from TSHBandRGBCommon

		// Binary operators (return TSHBandVectorRGB) - implemented in .inl
		TSHBandVectorRGB<Order> operator+(TSHBandConstViewRGB<Order> Other) const noexcept;
		TSHBandVectorRGB<Order> operator-(TSHBandConstViewRGB<Order> Other) const noexcept;
		TSHBandVectorRGB<Order> operator*(float Scalar) const noexcept;
		TSHBandVectorRGB<Order> operator/(float Scalar) const noexcept;
	};

	// TSHBandViewRGB<Order, false> - Mutable RGB view specialization
	template<int Order>
	class TSHBandViewRGB<Order, false> : public TSHBandViewRGBCommon<Order, false>
	{
		using Super = TSHBandViewRGBCommon<Order, false>;

	public:
		// Constructors
		using Super::Super;

		// Note: operator[] is inherited from TSHBandRGBCommon
	};

	// TSHBandViewRGB<Order, true> - Const RGB view specialization
	template<int Order>
	class TSHBandViewRGB<Order, true> : public TSHBandViewRGBCommon<Order, true>
	{
		using Super = TSHBandViewRGBCommon<Order, true>;

	public:
		// Constructors
		using Super::Super;

		// Implicit conversion from mutable RGB view
		constexpr TSHBandViewRGB(const TSHBandViewRGB<Order, false>& Other) noexcept
			: Super(Other.R, Other.G, Other.B) {}
	};

	// Forward declarations for owning containers
	template<int Order>
	class TSHBandVector;

	template<int Order>
	class TSHBandVectorRGB;

	/** Owning container for spherical harmonic band coefficients. */
	template<int Order>
	class TSHBandVector : public TSHBandCommon<TSHBandVector<Order>, Order>
	{
		// Allow base class to access Data for SHBasisFunction
		friend class TSHBandCommon<TSHBandVector<Order>, Order>;

	public:
		// Constructors
		TSHBandVector() noexcept;
		TSHBandVector(std::initializer_list<float> InitList) noexcept;
		TSHBandVector(const TSHBandVector& Other) noexcept = default;
		explicit TSHBandVector(TSHBandConstView<Order> View) noexcept;

		// Assignment operators
		TSHBandVector& operator=(TSHBandConstView<Order> View) noexcept;
		TSHBandVector& operator=(const TSHBandVector& Other) noexcept = default;

		// Element access (non-const)
		float& operator[](uint64_t Index) noexcept;

		// Element access (const)
		const float& operator[](uint64_t Index) const noexcept;

		// Data access (non-const)
		float* GetData() noexcept { return Data; }

		// Data access (const)
		const float* GetData() const noexcept { return Data; }

		// Compound assignment operators
		TSHBandVector& operator+=(TSHBandConstView<Order> Other) noexcept;
		TSHBandVector& operator-=(TSHBandConstView<Order> Other) noexcept;
		TSHBandVector& operator*=(float Scalar) noexcept;
		TSHBandVector& operator/=(float Scalar) noexcept;

		// Implicit conversion to view
		operator TSHBandView<Order, false>() noexcept;
		operator TSHBandConstView<Order>() const noexcept;

		// Type conversion (similar to TSHVectorCommon::As)
		template<typename U>
		U& As()&
		{
			static_assert(sizeof(U) == sizeof(TSHBandVector), "The size of U is not same with TSHBandVector");
			return *reinterpret_cast<U*>(this);
		}

		template<typename U>
		const U& As() const&
		{
			return const_cast<TSHBandVector*>(this)->As<U>();
		}

		template<typename U>
		const U&& As() const&&
		{
			static_assert(sizeof(U) == sizeof(TSHBandVector), "The size of U is not same with TSHBandVector");
			static_assert(alignof(TSHBandVector) % alignof(U) == 0, "The alignment of U is not compatible with TSHBandVector");
			return reinterpret_cast<const U&&>(*this);
		}

		// Note: GetLinearVector is inherited from TSHBandCommon

	private:
		float Data[TSHBandCommon<TSHBandVector<Order>, Order>::MaxSHBasis];
	};

	/** Owning container for RGB spherical harmonic band coefficients. */
	template<int Order>
	class TSHBandVectorRGB : public TSHBandRGBCommon<TSHBandVectorRGB<Order>, Order>
	{
	public:
		TSHBandVector<Order> R;
		TSHBandVector<Order> G;
		TSHBandVector<Order> B;

		// Constructors
		TSHBandVectorRGB() noexcept;
		TSHBandVectorRGB(const TSHBandVector<Order>& InR, const TSHBandVector<Order>& InG, const TSHBandVector<Order>& InB) noexcept;
		TSHBandVectorRGB(const TSHBandVectorRGB& Other) noexcept = default;
		explicit TSHBandVectorRGB(TSHBandConstViewRGB<Order> View) noexcept;

		// Assignment operators
		TSHBandVectorRGB& operator=(TSHBandConstViewRGB<Order> View) noexcept;
		TSHBandVectorRGB& operator=(const TSHBandVectorRGB& Other) noexcept = default;

		// Compound assignment operators
		TSHBandVectorRGB& operator+=(TSHBandConstViewRGB<Order> Other) noexcept;
		TSHBandVectorRGB& operator-=(TSHBandConstViewRGB<Order> Other) noexcept;
		TSHBandVectorRGB& operator*=(float Scalar) noexcept;
		TSHBandVectorRGB& operator/=(float Scalar) noexcept;

		// Note: Dot functions are inherited from TSHBandRGBCommon

		// Binary operators (return TSHBandVectorRGB)
		TSHBandVectorRGB operator+(TSHBandConstViewRGB<Order> Other) const noexcept;
		TSHBandVectorRGB operator-(TSHBandConstViewRGB<Order> Other) const noexcept;
		TSHBandVectorRGB operator*(float Scalar) const noexcept;
		TSHBandVectorRGB operator/(float Scalar) const noexcept;

		// Implicit conversion to view
		operator TSHBandViewRGB<Order, false>() noexcept;
		operator TSHBandConstViewRGB<Order>() const noexcept;
	};

	// Scalar multiplication (scalar * vector) - non-member for commutativity
	template<int Order>
	inline TSHBandVectorRGB<Order> operator*(float Scalar, const TSHBandVectorRGB<Order>& Vec) noexcept
	{
		return Vec * Scalar;
	}

	/** A vector of spherical harmonic coefficients. */
	template<int Order>
	class TSHVector : public TSHVectorCommon<TSHVector<Order>, Order, Order* Order>
	{
	public:
		using Super = TSHVectorCommon<TSHVector<Order>, Order, Order* Order>;
		using TSHVectorCommon<TSHVector<Order>, Order, Order* Order>::TSHVectorCommon;

		using RGBType = TSHVectorRGB<Order>;

		TSHVector(float V0, float V1, float V2, float V3) : Super({ V0, V1, V2, V3 }) {}

		explicit TSHVector(const FVector4f& Vector) : TSHVector(Vector.X, Vector.Y, Vector.Z, Vector.W) {}

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

		TSHVector(const TSHVector<Order - 1>& Other, TSHBandConstView<Order> Band)
		{
			for (int i = 0; i < TSHVector<Order - 1>::Super::MaxSHBasis; i++)
			{
				Super::V[i] = Other.V[i];
			}
			for (int i = TSHVector<Order - 1>::Super::MaxSHBasis; i < Super::MaxSHBasis; i++)
			{
				Super::V[i] = Band[i - TSHVector<Order - 1>::Super::MaxSHBasis];
			}
		}

		/** Returns the DC component (L0, m=0) */
		float DC() const noexcept
		{
			return Super::V[0];
		}

		template<int BandOrder>
		TSHBandView<BandOrder, false> GetBand() noexcept
		{
			static_assert(BandOrder <= Order, "BandOrder must be <= Order");
			constexpr int IndexBase = Pow2(BandOrder - 1);
			return TSHBandView<BandOrder, false>(&Super::V[IndexBase]);
		}

		template<int BandOrder>
		TSHBandView<BandOrder, true> GetBand() const noexcept
		{
			static_assert(BandOrder <= Order, "BandOrder must be <= Order");
			constexpr int IndexBase = Pow2(BandOrder - 1);
			return TSHBandView<BandOrder, true>(&Super::V[IndexBase]);
		}
	};

	/** A vector of spherical harmonic coefficients without DC. */
	template<int Order>
	class TSHVectorAC : public TSHVectorCommon<TSHVectorAC<Order>, Order, Order* Order - 1>
	{
	public:
		using Super = TSHVectorCommon<TSHVectorAC<Order>, Order, Order* Order - 1>;
		using TSHVectorCommon<TSHVectorAC<Order>, Order, Order* Order - 1>::TSHVectorCommon;

		using RGBType = TSHVectorACRGB<Order>;

		TSHVectorAC(float V1, float V2, float V3) : Super{ V1,V2,V3 } {}

		explicit TSHVectorAC(const FVector3f& Vector) : TSHVectorAC(Vector.X, Vector.Y, Vector.Z) {}

		template<int OtherOrder>
		explicit TSHVectorAC(const TSHVectorAC<OtherOrder>& Other)
		{
			if (Super::MaxSHBasis <= TSHVectorAC<OtherOrder>::Super::MaxSHBasis)
			{
				for (int i = 0; i < Super::MaxSHBasis; i++)
				{
					Super::V[i] = Other.V[i];
				}
			}
			else
			{
				for (int i = 0; i < TSHVectorAC<OtherOrder>::Super::MaxSHBasis; i++)
				{
					Super::V[i] = Other.V[i];
				}
				for (int i = TSHVectorAC<OtherOrder>::Super::MaxSHBasis; i < Super::MaxSHBasis; i++)
				{
					Super::V[i] = 0.f;
				}
			}
		}

		template<bool bConst>
		TSHVectorAC(const TSHVectorAC<Order - 1>& Other, const TSHBandView<Order, bConst>& Band)
		{
			for (int i = 0; i < TSHVectorAC<Order - 1>::Super::MaxSHBasis; i++)
			{
				Super::V[i] = Other.V[i];
			}
			for (int i = TSHVectorAC<Order - 1>::Super::MaxSHBasis; i < Super::MaxSHBasis; i++)
			{
				Super::V[i] = Band[i - TSHVectorAC<Order - 1>::Super::MaxSHBasis];
			}
		}

		template<int BandOrder>
		TSHBandView<BandOrder, false> GetBand() noexcept
		{
			static_assert(BandOrder <= Order, "BandOrder must be <= Order");
			constexpr int IndexBase = Pow2(BandOrder - 1) - 1;
			return TSHBandView<BandOrder, false>(&Super::V[IndexBase]);
		}

		template<int BandOrder>
		TSHBandView<BandOrder, true> GetBand() const noexcept
		{
			static_assert(BandOrder <= Order, "BandOrder must be <= Order");
			constexpr int IndexBase = Pow2(BandOrder - 1) - 1;
			return TSHBandView<BandOrder, true>(&Super::V[IndexBase]);
		}
	};

	/** A vector of colored spherical harmonic coefficients. */
	template<int Order>
	class TSHVectorRGB : public TSHVectorRGBCommon<TSHVectorRGB<Order>, TSHVector, Order, Order * Order>
	{
	public:
		using Super = TSHVectorRGBCommon<TSHVectorRGB<Order>, TSHVector, Order, Order* Order>;
		using TSHVectorRGBCommon<TSHVectorRGB<Order>, TSHVector, Order, Order* Order>::TSHVectorRGBCommon;

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

		TSHVectorRGB(const TSHVectorRGB<Order - 1>& Other, TSHBandConstViewRGB<Order> Band)
		{
			Super::R = TSHVector<Order>(Other.R, Band.R);
			Super::G = TSHVector<Order>(Other.G, Band.G);
			Super::B = TSHVector<Order>(Other.B, Band.B);
		}

		/** Returns the DC component (L0, m=0) as RGB */
		FVector3f DC() const noexcept
		{
			return FVector3f{ Super::R.DC(), Super::G.DC(), Super::B.DC() };
		}

		template<int BandOrder>
		TSHBandViewRGB<BandOrder, false> GetBand() noexcept
		{
			static_assert(BandOrder <= Order, "BandOrder must be <= Order");
			return TSHBandViewRGB<BandOrder, false>(
				Super::R.template GetBand<BandOrder>(),
				Super::G.template GetBand<BandOrder>(),
				Super::B.template GetBand<BandOrder>()
			);
		}

		template<int BandOrder>
		TSHBandViewRGB<BandOrder, true> GetBand() const noexcept
		{
			static_assert(BandOrder <= Order, "BandOrder must be <= Order");
			return TSHBandViewRGB<BandOrder, true>(
				Super::R.template GetBand<BandOrder>(),
				Super::G.template GetBand<BandOrder>(),
				Super::B.template GetBand<BandOrder>()
			);
		}

		template<int BandOrder>
		void SetBand(const TSHBandViewRGB<BandOrder, false>& SHBandView) noexcept
		{
			static_assert(BandOrder <= Order, "BandOrder must be <= Order");
			auto RBand = Super::R.template GetBand<BandOrder>();
			auto GBand = Super::G.template GetBand<BandOrder>();
			auto BBand = Super::B.template GetBand<BandOrder>();
			for (uint64_t i = 0; i < TSHBandView<BandOrder, false>::MaxSHBasis; ++i)
			{
				RBand[i] = SHBandView.R[i];
				GBand[i] = SHBandView.G[i];
				BBand[i] = SHBandView.B[i];
			}
		}
	};

	/** A vector of colored spherical harmonic coefficients without DC. */
	template<int Order>
	class TSHVectorACRGB : public TSHVectorRGBCommon<TSHVectorACRGB<Order>, TSHVectorAC, Order, Order* Order>
	{
	public:
		using Super = TSHVectorRGBCommon<TSHVectorACRGB<Order>, TSHVectorAC, Order, Order* Order>;
		using TSHVectorRGBCommon<TSHVectorACRGB<Order>, TSHVectorAC, Order, Order* Order>::TSHVectorRGBCommon;

		template<int OtherOrder>
		explicit TSHVectorACRGB(const TSHVectorACRGB<OtherOrder>& Other)
		{
			Super::R = (TSHVectorAC<Order>)Other.R;
			Super::G = (TSHVectorAC<Order>)Other.G;
			Super::B = (TSHVectorAC<Order>)Other.B;
		}

		template<int OtherOrder>
		explicit TSHVectorACRGB(const TSHVectorAC<OtherOrder>& Other)
		{
			Super::R = (TSHVectorAC<Order>)Other;
			Super::G = (TSHVectorAC<Order>)Other;
			Super::B = (TSHVectorAC<Order>)Other;
		}

		template<bool bConst>
		TSHVectorACRGB(const TSHVectorACRGB<Order - 1>& Other, const TSHBandViewRGB<Order, bConst>& Band)
		{
			Super::R = TSHVector<Order>(Other.R, Band.R);
			Super::G = TSHVector<Order>(Other.G, Band.G);
			Super::B = TSHVector<Order>(Other.B, Band.B);
		}

		template<int BandOrder>
		TSHBandViewRGB<BandOrder, false> GetBand() noexcept
		{
			static_assert(BandOrder <= Order, "BandOrder must be <= Order");
			return TSHBandViewRGB<BandOrder, false>(
				Super::R.template GetBand<BandOrder>(),
				Super::G.template GetBand<BandOrder>(),
				Super::B.template GetBand<BandOrder>()
			);
		}

		template<int BandOrder>
		TSHBandViewRGB<BandOrder, true> GetBand() const noexcept
		{
			static_assert(BandOrder <= Order, "BandOrder must be <= Order");
			return TSHBandViewRGB<BandOrder, true>(
				Super::R.template GetBand<BandOrder>(),
				Super::G.template GetBand<BandOrder>(),
				Super::B.template GetBand<BandOrder>()
			);
		}

		template<int BandOrder>
		void SetBand(const TSHBandViewRGB<BandOrder, false>& SHBandView) noexcept
		{
			static_assert(BandOrder <= Order, "BandOrder must be <= Order");
			auto RBand = Super::R.template GetBand<BandOrder>();
			auto GBand = Super::G.template GetBand<BandOrder>();
			auto BBand = Super::B.template GetBand<BandOrder>();
			for (uint64_t i = 0; i < TSHBandView<BandOrder, false>::MaxSHBasis; ++i)
			{
				RBand[i] = SHBandView.R[i];
				GBand[i] = SHBandView.G[i];
				BBand[i] = SHBandView.B[i];
			}
		}
	};

	// Type aliases for SHBand views (mutable by default)
	using FSHBandView2 = TSHBandView<2, false>;
	using FSHBandView3 = TSHBandView<3, false>;
	using FSHBandView4 = TSHBandView<4, false>;
	using FSHBandView5 = TSHBandView<5, false>;
	using FSHBandViewRGB2 = TSHBandViewRGB<2, false>;
	using FSHBandViewRGB3 = TSHBandViewRGB<3, false>;
	using FSHBandViewRGB4 = TSHBandViewRGB<4, false>;
	using FSHBandViewRGB5 = TSHBandViewRGB<5, false>;

	// Const view aliases
	using FSHBandConstView2 = TSHBandConstView<2>;
	using FSHBandConstView3 = TSHBandConstView<3>;
	using FSHBandConstView4 = TSHBandConstView<4>;
	using FSHBandConstView5 = TSHBandConstView<5>;
	using FSHBandConstViewRGB2 = TSHBandConstViewRGB<2>;
	using FSHBandConstViewRGB3 = TSHBandConstViewRGB<3>;
	using FSHBandConstViewRGB4 = TSHBandConstViewRGB<4>;
	using FSHBandConstViewRGB5 = TSHBandConstViewRGB<5>;

	// Owning container type aliases
	using FSHBandVector2 = TSHBandVector<2>;
	using FSHBandVector3 = TSHBandVector<3>;
	using FSHBandVector4 = TSHBandVector<4>;
	using FSHBandVector5 = TSHBandVector<5>;
	using FSHBandVectorRGB2 = TSHBandVectorRGB<2>;
	using FSHBandVectorRGB3 = TSHBandVectorRGB<3>;
	using FSHBandVectorRGB4 = TSHBandVectorRGB<4>;
	using FSHBandVectorRGB5 = TSHBandVectorRGB<5>;

	using FSHRotateMatrices2 = TSHRotateMatrices<2>;
	using FSHRotateMatrices3 = TSHRotateMatrices<3>;

	using FSHVector2 = TSHVector<2>;
	using FSHVector3 = TSHVector<3>;
	using FSHVector4 = TSHVector<4>;
	using FSHVector5 = TSHVector<5>;
	using FSHVectorAC2 = TSHVectorAC<2>;
	using FSHVectorAC3 = TSHVectorAC<3>;
	using FSHVectorAC4 = TSHVectorAC<4>;
	using FSHVectorAC5 = TSHVectorAC<5>;
	using FSHVectorRGB2 = TSHVectorRGB<2>;
	using FSHVectorRGB3 = TSHVectorRGB<3>;
	using FSHVectorRGB4 = TSHVectorRGB<4>;
	using FSHVectorRGB5 = TSHVectorRGB<5>;
	using FSHVectorACRGB2 = TSHVectorACRGB<2>;
	using FSHVectorACRGB3 = TSHVectorACRGB<3>;
	using FSHVectorACRGB4 = TSHVectorACRGB<4>;
	using FSHVectorACRGB5 = TSHVectorACRGB<5>;

	template<template<int> class SHVectorType, int Order>
	typename SHVectorType<Order>::RGBType operator*(const SHVectorType<Order>& SHVector, const FVector3f& Color)
	{
		return typename SHVectorType<Order>::RGBType(SHVector) * Color;
	}

	template<template<int> class SHVectorType, int Order>
	typename SHVectorType<Order>::RGBType operator*(const FVector3f& Color, const SHVectorType<Order>& SHVector)
	{
		return SHVector * Color;
	}

	// z1 = dot(Buffer.xyz, n)
	// Buffer.w + z1 + z1*z1*k
	// == Buffer.w + z1*(1 + z1*k)
	UBPA_UCOMMON_API float HallucinateZH(const FSHVector2& SHVector2, float t, FVector4f& Buffer, float Delta = UBPA_UCOMMON_DELTA);

	// SHBand2RotateMatrix: row-major 3x3 (float[9], [row*3+col])
	UBPA_UCOMMON_API void ComputeSHBand2RotateMatrix(float* SHBand2RotateMatrix, const FMatrix3x3f& RotateMatrix);
	// SHBand3RotateMatrix: row-major 5x5 (float[25], [row*5+col])
	UBPA_UCOMMON_API void ComputeSHBand3RotateMatrix(float* SHBand3RotateMatrix, const FMatrix3x3f& RotateMatrix);
	// SHBand4RotateMatrix: row-major 7x7 (float[49], [row*7+col])
	UBPA_UCOMMON_API void ComputeSHBand4RotateMatrix(float* SHBand4RotateMatrix, const FMatrix3x3f& RotateMatrix);

	// Apply a precomputed SH band rotation matrix to a mutable SH band view (in-place).
	// SHBand          : mutable view of 2*Order-1 coefficients, modified in place
	// SHBandRotateMatrix: row-major (2*Order-1) x (2*Order-1) float array, [row*(2*Order-1)+col]
	template<int Order>
	void ApplySHRotateMatrix(TSHBandView<Order> SHBand, const float* SHBandRotateMatrix);

	// ============================================================================
	// Binary operators for TSHBandView (return TSHBandVector)
	// ============================================================================
	// Note: operator+, operator-, operator*, operator/ are now member functions in TSHBandViewCommon

	// Scalar multiplication (scalar * view) - non-member for commutativity
	template<int Order>
	TSHBandVector<Order> operator*(float Scalar, TSHBandConstView<Order> View) noexcept
	{
		return View * Scalar;
	}

	// FVector3f multiplication (color * view) - non-member for commutativity
	template<int Order, bool bConst>
	inline TSHBandVectorRGB<Order> operator*(const FVector3f& Color, TSHBandView<Order, bConst> View) noexcept
	{
		return View * Color;
	}

	// Note: Dot function is now a static member of TSHBandCommon
	// Use TSHBandCommon<DerivedType, Order>::Dot(A, B) or the member function A.Dot(B)

	// Scalar multiplication (scalar * vector) - non-member for commutativity
	// Note: Cannot rely on implicit conversion to TSHBandConstView because template argument deduction doesn't consider user-defined conversions
	template<int Order>
	inline TSHBandVector<Order> operator*(float Scalar, const TSHBandVector<Order>& Vec) noexcept
	{
		return Vec * Scalar;
	}

	// FVector3f multiplication (color * vector) - non-member for commutativity
	// Note: Cannot rely on implicit conversion to TSHBandConstView because template argument deduction doesn't consider user-defined conversions
	template<int Order>
	inline TSHBandVectorRGB<Order> operator*(const FVector3f& Color, const TSHBandVector<Order>& Vec) noexcept
	{
		return Vec * Color;
	}

	// ============================================================================
	// Binary operators for TSHBandViewRGB (return TSHBandVectorRGB)
	// ============================================================================

	// Scalar multiplication (scalar * view) - non-member for commutativity
	template<int Order, bool bConst>
	inline TSHBandVectorRGB<Order> operator*(float Scalar, TSHBandViewRGB<Order, bConst> View) noexcept
	{
		return View * Scalar;
	}

	// ============================================================================
	// Binary operators for TSHBandVectorRGB (return TSHBandVectorRGB)
	// ============================================================================

	// Scalar multiplication (scalar * vector) - non-member for commutativity
	template<int Order>
	TSHBandVectorRGB<Order> operator*(float Scalar, const TSHBandVectorRGB<Order>& Vec) noexcept;

} // namespace UCommon

UBPA_UCOMMON_SH_TO_NAMESPACE(UCommonTest)

#include "SH.inl"
