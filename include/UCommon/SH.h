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

#include "Config.h"
#include "Utils.h"

#define UBPA_UCOMMON_SH_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
	template<int l, int m> constexpr float SHK = UCommon::SHK<l, m>; \
	template<int i> constexpr int SHIndexToL = UCommon::SHIndexToL<i>; \
	template<int i> constexpr int SHIndexToM = UCommon::SHIndexToM<i>; \
	template<int Order, bool bConst = false> using TSHBandView = UCommon::TSHBandView<Order, bConst>; \
	template<int Order, bool bConst = false> using TSHBandViewRGB = UCommon::TSHBandViewRGB<Order, bConst>; \
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

	template<int Order> class TSHVectorRGB;
	template<int Order> class TSHVectorACRGB;
	template<int Order> class TSHBandVectorRGB;

	template<typename DerivedType, int InMaxSHOrder, int InMaxSHBasis>
	class TSHVectorBase
	{
	public:
		static constexpr int MaxSHOrder = InMaxSHOrder;
		static constexpr int MaxSHBasis = InMaxSHBasis;
		float V[MaxSHBasis];

		static_assert(MaxSHOrder > 0 && MaxSHBasis > 0, "Invalid MaxSHOrder or MaxSHBasis");

		/** Default constructor. */
		TSHVectorBase() : V{ 0 } {}

		template<typename ArgType0, typename... ArgTypes>
		TSHVectorBase(ArgType0 Arg0, ArgTypes... Args) : V{ Arg0, Args... } {}

		template<typename U>
		U& As()&
		{
			static_assert(sizeof(U) == sizeof(TSHVectorBase), "The size of U is not same with TSHVectorBase");
			return *reinterpret_cast<U*>(this);
		}

		template<typename U>
		const U& As() const&
		{
			return const_cast<TSHVectorBase*>(this)->As<U>();
		}

		template<typename U>
		const U&& As() const &&
		{
			static_assert(sizeof(U) == sizeof(TSHVectorBase), "The size of U is not same with TSHVectorBase");
			static_assert(alignof(TSHVectorBase) % alignof(U) == 0, "The alignment of U is not compatible with TSHVectorBase");
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
	};

	template<typename DerivedType, template<int> class TElement, int InMaxSHOrder, int InMaxSHBasis>
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

		template<typename U>
		U& As()&
		{
			static_assert(sizeof(U) == sizeof(TSHVectorRGBBase), "The size of U is not same with TSHVectorRGBBase");
			return *reinterpret_cast<U*>(this);
		}

		template<typename U>
		const U& As() const&
		{
			return const_cast<TSHVectorRGBBase*>(this)->As<U>();
		}

		template<typename U>
		const U&& As() const &&
		{
			static_assert(sizeof(U) == sizeof(TSHVectorRGBBase), "The size of U is not same with TSHVectorRGBBase");
			static_assert(alignof(TSHVectorRGBBase) % alignof(U) == 0, "The alignment of U is not compatible with TSHVectorRGBBase");
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

		/** Now it calls directly += operator in TSHVector (avoid TSHVectorRGBBase + operator) **/
		inline DerivedType& operator+=(const DerivedType& InB)
		{
			R += InB.R;
			G += InB.G;
			B += InB.B;

			return As<DerivedType>();
		}

		/** In-place subtraction operator. */
		/** Changed from (*this = *this - InB;) to separate all calc to avoid LHS **/
		/** Now it calls directly -= operator in TSHVector (avoid TSHVectorRGBBase - operator) **/
		inline DerivedType& operator-=(const DerivedType& InB)
		{
			R -= InB.R;
			G -= InB.G;
			B -= InB.B;

			return As<DerivedType>();
		}

		/** In-place scalar multiplication operator. */
		/** Changed from (*this = *this * InB;) to separate all calc to avoid LHS **/
		/** Now it calls directly *= operator in TSHVector (avoid TSHVectorRGBBase * operator) **/
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
			return const_cast<TSHVectorRGBBase*>(this)->operator[](Index);
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
	};

	// Forward declarations for CRTP base class
	template<int Order, bool bConst = false>
	class TSHBandView;

	template<int Order, bool bConst = false>
	class TSHBandViewRGB;

	// CRTP base class for TSHBandView
	template<typename Derived, int Order, bool bConst>
	class TSHBandViewCommon
	{
	public:
		static constexpr int MaxSHOrder = Order;
		static constexpr int MaxSHBasis = 2 * Order - 1;

		using DataType = std::conditional_t<bConst, const float*, float*>;
		using RGBType = TSHBandViewRGB<Order, bConst>;

		// Array access (const version - always available)
		const float& operator[](uint64_t Index) const noexcept
		{
			UBPA_UCOMMON_ASSERT(GetData() != nullptr);
			UBPA_UCOMMON_ASSERT(Index < MaxSHBasis);
			return GetData()[Index];
		}

		static constexpr uint64_t GetSize() noexcept { return MaxSHBasis; }

		// Dot product (member - calls non-member)
		template<bool bOtherConst>
		float Dot(const TSHBandView<Order, bOtherConst>& Other) const noexcept;

	protected:
		constexpr TSHBandViewCommon() noexcept = default;

		const Derived& AsDerived() const noexcept { return static_cast<const Derived&>(*this); }
		Derived& AsDerived() noexcept { return static_cast<Derived&>(*this); }

		// Derived class must provide GetData()
		DataType GetData() const noexcept { return AsDerived().GetData(); }
	};

	// TSHBandView<Order, false> - Mutable specialization
	template<int Order>
	class TSHBandView<Order, false> : public TSHBandViewCommon<TSHBandView<Order, false>, Order, false>
	{
		using Super = TSHBandViewCommon<TSHBandView<Order, false>, Order, false>;

	public:
		// Constructors
		constexpr TSHBandView() noexcept : Data(nullptr) {}
		explicit constexpr TSHBandView(float* InData) noexcept : Data(InData) {}

		// Assignment (shallow copy - rebind pointer)
		TSHBandView& operator=(const TSHBandView& Other) noexcept
		{
			Data = Other.Data;
			return *this;
		}

		// Array access (mutable version)
		float& operator[](uint64_t Index) noexcept
		{
			UBPA_UCOMMON_ASSERT(Data != nullptr);
			UBPA_UCOMMON_ASSERT(Index < Super::MaxSHBasis);
			return Data[Index];
		}

		// Inherit const version from base
		using Super::operator[];

		float* GetData() noexcept { return Data; }
		const float* GetData() const noexcept { return Data; }

		// Mathematical operators (in-place) - only in mutable version
		TSHBandView& operator+=(const TSHBandView& Other) noexcept;
		TSHBandView& operator-=(const TSHBandView& Other) noexcept;
		TSHBandView& operator*=(float Scalar) noexcept;
		TSHBandView& operator/=(float Scalar) noexcept;

	private:
		float* Data;
	};

	// TSHBandView<Order, true> - Const specialization
	template<int Order>
	class TSHBandView<Order, true> : public TSHBandViewCommon<TSHBandView<Order, true>, Order, true>
	{
		using Super = TSHBandViewCommon<TSHBandView<Order, true>, Order, true>;

	public:
		// Constructors
		constexpr TSHBandView() noexcept : Data(nullptr) {}
		explicit constexpr TSHBandView(const float* InData) noexcept : Data(InData) {}

		// Implicit conversion from mutable view
		constexpr TSHBandView(const TSHBandView<Order, false>& Other) noexcept
			: Data(Other.GetData()) {}

		// Assignment (shallow copy - rebind pointer)
		TSHBandView& operator=(const TSHBandView& Other) noexcept
		{
			Data = Other.Data;
			return *this;
		}

		const float* GetData() const noexcept { return Data; }

		// Array access inherited from base (const only)
		using Super::operator[];

	private:
		const float* Data;
	};

	// TSHBandViewRGB<Order, false> - Mutable RGB view specialization
	template<int Order>
	class TSHBandViewRGB<Order, false>
	{
	public:
		static constexpr int MaxSHOrder = Order;
		static constexpr int MaxSHBasis = 2 * Order - 1;

		TSHBandView<Order, false> R;
		TSHBandView<Order, false> G;
		TSHBandView<Order, false> B;

		// Constructors
		constexpr TSHBandViewRGB() noexcept : R(), G(), B() {}

		constexpr TSHBandViewRGB(float* InR, float* InG, float* InB) noexcept
			: R(InR), G(InG), B(InB) {}

		constexpr TSHBandViewRGB(TSHBandView<Order, false> InR, TSHBandView<Order, false> InG, TSHBandView<Order, false> InB) noexcept
			: R(InR), G(InG), B(InB) {}

		// Assignment (shallow copy - rebind all three pointers)
		TSHBandViewRGB& operator=(const TSHBandViewRGB& Other) noexcept
		{
			R = Other.R;
			G = Other.G;
			B = Other.B;
			return *this;
		}

		// Channel access
		TSHBandView<Order, false>& operator[](uint64_t Index) noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 3);
			return (&R)[Index];
		}

		const TSHBandView<Order, false>& operator[](uint64_t Index) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 3);
			return (&R)[Index];
		}

		static constexpr uint64_t GetSize() noexcept { return MaxSHBasis; }
	};

	// TSHBandViewRGB<Order, true> - Const RGB view specialization
	template<int Order>
	class TSHBandViewRGB<Order, true>
	{
	public:
		static constexpr int MaxSHOrder = Order;
		static constexpr int MaxSHBasis = 2 * Order - 1;

		TSHBandView<Order, true> R;
		TSHBandView<Order, true> G;
		TSHBandView<Order, true> B;

		// Constructors
		constexpr TSHBandViewRGB() noexcept : R(), G(), B() {}

		constexpr TSHBandViewRGB(const float* InR, const float* InG, const float* InB) noexcept
			: R(InR), G(InG), B(InB) {}

		constexpr TSHBandViewRGB(TSHBandView<Order, true> InR, TSHBandView<Order, true> InG, TSHBandView<Order, true> InB) noexcept
			: R(InR), G(InG), B(InB) {}

		// Implicit conversion from mutable RGB view
		constexpr TSHBandViewRGB(const TSHBandViewRGB<Order, false>& Other) noexcept
			: R(Other.R), G(Other.G), B(Other.B) {}

		// Assignment (shallow copy - rebind all three pointers)
		TSHBandViewRGB& operator=(const TSHBandViewRGB& Other) noexcept
		{
			R = Other.R;
			G = Other.G;
			B = Other.B;
			return *this;
		}

		// Channel access (const only)
		const TSHBandView<Order, true>& operator[](uint64_t Index) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 3);
			return (&R)[Index];
		}

		static constexpr uint64_t GetSize() noexcept { return MaxSHBasis; }
	};

	// Forward declarations for owning containers
	template<int Order>
	class TSHBandVector;

	template<int Order>
	class TSHBandVectorRGB;

	/** Owning container for spherical harmonic band coefficients. */
	template<int Order>
	class TSHBandVector
	{
	public:
		static constexpr int MaxSHOrder = Order;
		static constexpr int MaxSHBasis = 2 * Order - 1;

		// Constructors
		TSHBandVector() noexcept;
		TSHBandVector(std::initializer_list<float> InitList) noexcept;
		TSHBandVector(const TSHBandVector& Other) noexcept;
		TSHBandVector(TSHBandVector&& Other) noexcept;
		template<bool bConst>
		explicit TSHBandVector(const TSHBandView<Order, bConst>& View) noexcept;

		// Assignment operators
		TSHBandVector& operator=(const TSHBandVector& Other) noexcept;
		TSHBandVector& operator=(TSHBandVector&& Other) noexcept;

		// Element access
		float& operator[](uint64_t Index) noexcept;
		const float& operator[](uint64_t Index) const noexcept;

		// Data access
		float* GetData() noexcept { return Data; }
		const float* GetData() const noexcept { return Data; }
		static constexpr uint64_t GetSize() noexcept { return MaxSHBasis; }

		// Compound assignment operators
		TSHBandVector& operator+=(const TSHBandView<Order, true>& Other) noexcept;
		TSHBandVector& operator-=(const TSHBandView<Order, true>& Other) noexcept;
		TSHBandVector& operator*=(float Scalar) noexcept;
		TSHBandVector& operator/=(float Scalar) noexcept;

		// Dot product
		template<bool bConst>
		float Dot(const TSHBandView<Order, bConst>& Other) const noexcept;

		// Implicit conversion to view
		operator TSHBandView<Order, false>() noexcept;
		operator TSHBandView<Order, true>() const noexcept;

	private:
		float Data[MaxSHBasis];
	};

	/** Owning container for RGB spherical harmonic band coefficients. */
	template<int Order>
	class TSHBandVectorRGB
	{
	public:
		static constexpr int MaxSHOrder = Order;
		static constexpr int MaxSHBasis = 2 * Order - 1;

		TSHBandVector<Order> R;
		TSHBandVector<Order> G;
		TSHBandVector<Order> B;

		// Constructors
		TSHBandVectorRGB() noexcept;
		TSHBandVectorRGB(const TSHBandVector<Order>& InR, const TSHBandVector<Order>& InG, const TSHBandVector<Order>& InB) noexcept;
		TSHBandVectorRGB(const TSHBandVectorRGB& Other) noexcept;
		TSHBandVectorRGB(TSHBandVectorRGB&& Other) noexcept;
		template<bool bConst>
		explicit TSHBandVectorRGB(const TSHBandViewRGB<Order, bConst>& View) noexcept;

		// Assignment operators
		TSHBandVectorRGB& operator=(const TSHBandVectorRGB& Other) noexcept;
		TSHBandVectorRGB& operator=(TSHBandVectorRGB&& Other) noexcept;

		// Compound assignment operators
		TSHBandVectorRGB& operator+=(const TSHBandViewRGB<Order, true>& Other) noexcept;
		TSHBandVectorRGB& operator-=(const TSHBandViewRGB<Order, true>& Other) noexcept;
		TSHBandVectorRGB& operator*=(float Scalar) noexcept;
		TSHBandVectorRGB& operator/=(float Scalar) noexcept;

		// Implicit conversion to view
		operator TSHBandViewRGB<Order, false>() noexcept;
		operator TSHBandViewRGB<Order, true>() const noexcept;

		static constexpr uint64_t GetSize() noexcept { return MaxSHBasis; }
	};

	/** A vector of spherical harmonic coefficients. */
	template<int Order>
	class TSHVector : public TSHVectorBase<TSHVector<Order>, Order, Order* Order>
	{
	public:
		using Super = TSHVectorBase<TSHVector<Order>, Order, Order* Order>;
		using TSHVectorBase<TSHVector<Order>, Order, Order* Order>::TSHVectorBase;

		using RGBType = TSHVectorRGB<Order>;

		TSHVector(float V0, float V1, float V2, float V3) : Super{ V0,V1,V2,V3 } {}

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

		template<bool bConst>
		TSHVector(const TSHVector<Order - 1>& Other, const TSHBandView<Order, bConst>& Band)
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
	class TSHVectorAC : public TSHVectorBase<TSHVectorAC<Order>, Order, Order* Order - 1>
	{
	public:
		using Super = TSHVectorBase<TSHVectorAC<Order>, Order, Order* Order - 1>;
		using TSHVectorBase<TSHVectorAC<Order>, Order, Order* Order - 1>::TSHVectorBase;

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

	/** A vector of colored spherical harmonic coefficients. */
	template<int Order>
	class TSHVectorRGB : public TSHVectorRGBBase<TSHVectorRGB<Order>, TSHVector, Order, Order * Order>
	{
	public:
		using Super = TSHVectorRGBBase<TSHVectorRGB<Order>, TSHVector, Order, Order* Order>;
		using TSHVectorRGBBase<TSHVectorRGB<Order>, TSHVector, Order, Order* Order>::TSHVectorRGBBase;

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

		template<bool bConst>
		TSHVectorRGB(const TSHVectorRGB<Order - 1>& Other, const TSHBandViewRGB<Order, bConst>& Band)
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

	/** A vector of colored spherical harmonic coefficients without DC. */
	template<int Order>
	class TSHVectorACRGB : public TSHVectorRGBBase<TSHVectorACRGB<Order>, TSHVectorAC, Order, Order* Order>
	{
	public:
		using Super = TSHVectorRGBBase<TSHVectorACRGB<Order>, TSHVectorAC, Order, Order* Order>;
		using TSHVectorRGBBase<TSHVectorACRGB<Order>, TSHVectorAC, Order, Order* Order>::TSHVectorRGBBase;

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
	using FSHBandConstView2 = TSHBandView<2, true>;
	using FSHBandConstView3 = TSHBandView<3, true>;
	using FSHBandConstView4 = TSHBandView<4, true>;
	using FSHBandConstView5 = TSHBandView<5, true>;
	using FSHBandConstViewRGB2 = TSHBandViewRGB<2, true>;
	using FSHBandConstViewRGB3 = TSHBandViewRGB<3, true>;
	using FSHBandConstViewRGB4 = TSHBandViewRGB<4, true>;
	using FSHBandConstViewRGB5 = TSHBandViewRGB<5, true>;

	// Owning container type aliases
	using FSHBandVector2 = TSHBandVector<2>;
	using FSHBandVector3 = TSHBandVector<3>;
	using FSHBandVector4 = TSHBandVector<4>;
	using FSHBandVector5 = TSHBandVector<5>;
	using FSHBandVectorRGB2 = TSHBandVectorRGB<2>;
	using FSHBandVectorRGB3 = TSHBandVectorRGB<3>;
	using FSHBandVectorRGB4 = TSHBandVectorRGB<4>;
	using FSHBandVectorRGB5 = TSHBandVectorRGB<5>;

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
	UBPA_UCOMMON_API float HallucinateZH(const FSHVector2& SHVector2, float t, FVector4f& Buffer);

	// RotateMatrix       : (3, 3)
	// SHBand2RotateMatrix: (3, 3)
	UBPA_UCOMMON_API void ComputeSHBand2RotateMatrix(float* SHBand2RotateMatrix, const float* RotateMatrix);
	// RotateMatrix       : (3, 3)
	// SHBand2RotateMatrix: (5, 5)
	UBPA_UCOMMON_API void ComputeSHBand3RotateMatrix(float* SHBand3RotateMatrix, const float* RotateMatrix);

	// ============================================================================
	// Binary operators for TSHBandView (return TSHBandVector)
	// ============================================================================

	// Addition
	template<int Order, bool bConst1, bool bConst2>
	TSHBandVector<Order> operator+(const TSHBandView<Order, bConst1>& Lhs, const TSHBandView<Order, bConst2>& Rhs) noexcept;

	// Subtraction
	template<int Order, bool bConst1, bool bConst2>
	TSHBandVector<Order> operator-(const TSHBandView<Order, bConst1>& Lhs, const TSHBandView<Order, bConst2>& Rhs) noexcept;

	// Scalar multiplication (view * scalar)
	template<int Order, bool bConst>
	TSHBandVector<Order> operator*(const TSHBandView<Order, bConst>& View, float Scalar) noexcept;

	// Scalar multiplication (scalar * view)
	template<int Order, bool bConst>
	TSHBandVector<Order> operator*(float Scalar, const TSHBandView<Order, bConst>& View) noexcept;

	// Scalar division
	template<int Order, bool bConst>
	TSHBandVector<Order> operator/(const TSHBandView<Order, bConst>& View, float Scalar) noexcept;

	// Non-member Dot function for TSHBandVector
	template<int Order>
	float Dot(const TSHBandVector<Order>& A, const TSHBandVector<Order>& B) noexcept;

	// Binary operators for TSHBandVector (return TSHBandVector)
	template<int Order>
	TSHBandVector<Order> operator+(const TSHBandVector<Order>& Lhs, const TSHBandVector<Order>& Rhs) noexcept;

	template<int Order>
	TSHBandVector<Order> operator-(const TSHBandVector<Order>& Lhs, const TSHBandVector<Order>& Rhs) noexcept;

	template<int Order>
	TSHBandVector<Order> operator*(const TSHBandVector<Order>& Vec, float Scalar) noexcept;

	template<int Order>
	TSHBandVector<Order> operator*(float Scalar, const TSHBandVector<Order>& Vec) noexcept;

	template<int Order>
	TSHBandVector<Order> operator/(const TSHBandVector<Order>& Vec, float Scalar) noexcept;

	// ============================================================================
	// Binary operators for TSHBandViewRGB (return TSHBandVectorRGB)
	// ============================================================================

	// Addition
	template<int Order, bool bConst1, bool bConst2>
	TSHBandVectorRGB<Order> operator+(const TSHBandViewRGB<Order, bConst1>& Lhs, const TSHBandViewRGB<Order, bConst2>& Rhs) noexcept;

	// Subtraction
	template<int Order, bool bConst1, bool bConst2>
	TSHBandVectorRGB<Order> operator-(const TSHBandViewRGB<Order, bConst1>& Lhs, const TSHBandViewRGB<Order, bConst2>& Rhs) noexcept;

	// Scalar multiplication (view * scalar)
	template<int Order, bool bConst>
	TSHBandVectorRGB<Order> operator*(const TSHBandViewRGB<Order, bConst>& View, float Scalar) noexcept;

	// Scalar multiplication (scalar * view)
	template<int Order, bool bConst>
	TSHBandVectorRGB<Order> operator*(float Scalar, const TSHBandViewRGB<Order, bConst>& View) noexcept;

	// Scalar division
	template<int Order, bool bConst>
	TSHBandVectorRGB<Order> operator/(const TSHBandViewRGB<Order, bConst>& View, float Scalar) noexcept;

	// Binary operators for TSHBandVectorRGB (return TSHBandVectorRGB)
	template<int Order>
	TSHBandVectorRGB<Order> operator+(const TSHBandVectorRGB<Order>& Lhs, const TSHBandVectorRGB<Order>& Rhs) noexcept;

	template<int Order>
	TSHBandVectorRGB<Order> operator-(const TSHBandVectorRGB<Order>& Lhs, const TSHBandVectorRGB<Order>& Rhs) noexcept;

	template<int Order>
	TSHBandVectorRGB<Order> operator*(const TSHBandVectorRGB<Order>& Vec, float Scalar) noexcept;

	template<int Order>
	TSHBandVectorRGB<Order> operator*(float Scalar, const TSHBandVectorRGB<Order>& Vec) noexcept;

	template<int Order>
	TSHBandVectorRGB<Order> operator/(const TSHBandVectorRGB<Order>& Vec, float Scalar) noexcept;

} // namespace UCommon

UBPA_UCOMMON_SH_TO_NAMESPACE(UCommonTest)

#include "SH.inl"
