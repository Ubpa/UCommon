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

#include <stdint.h>
#include <cmath>
#include <type_traits>
#include <algorithm>

#define UBPA_UCOMMON_UTILS_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
    template<typename T> using TVector2 = UCommon::TVector2<T>; \
    template<typename T> using TVector = UCommon::TVector<T>; \
    template<typename T> using TVector4 = UCommon::TVector4<T>; \
    using FVector2 = UCommon::FVector2; \
    using FDoubleVector2 = UCommon::FDoubleVector2; \
    using FInt8Vector2 = UCommon::FInt8Vector2; \
    using FInt16Vector2 = UCommon::FInt16Vector2; \
    using FInt32Vector2 = UCommon::FInt32Vector2; \
    using FInt64Vector2 = UCommon::FInt64Vector2; \
    using FUint8Vector2 = UCommon::FUint8Vector2; \
    using FUint16Vector2 = UCommon::FUint16Vector2; \
    using FUint32Vector2 = UCommon::FUint32Vector2; \
    using FUint64Vector2 = UCommon::FUint64Vector2; \
    using FVector = UCommon::FVector; \
    using FDoubleVector = UCommon::FDoubleVector; \
    using FInt8Vector = UCommon::FInt8Vector; \
    using FInt16Vector = UCommon::FInt16Vector; \
    using FInt32Vector = UCommon::FInt32Vector; \
    using FInt64Vector = UCommon::FInt64Vector; \
    using FUint8Vector = UCommon::FUint8Vector; \
    using FUint16Vector = UCommon::FUint16Vector; \
    using FUint32Vector = UCommon::FUint32Vector; \
    using FUint64Vector = UCommon::FUint64Vector; \
    using FVector4 = UCommon::FVector4; \
    using FDoubleVector4 = UCommon::FDoubleVector4; \
    using FInt8Vector4 = UCommon::FInt8Vector4; \
    using FInt16Vector4 = UCommon::FInt16Vector4; \
    using FInt32Vector4 = UCommon::FInt32Vector4; \
    using FInt64Vector4 = UCommon::FInt64Vector4; \
    using FUint8Vector4 = UCommon::FUint8Vector4; \
    using FUint16Vector4 = UCommon::FUint16Vector4; \
    using FUint32Vector4 = UCommon::FUint32Vector4; \
    using FUint64Vector4 = UCommon::FUint64Vector4; \
    using FColorRGB = UCommon::FColorRGB; \
    using FLinearColorRGB = UCommon::FLinearColorRGB; \
    using FDoubleColorRGB = UCommon::FDoubleColorRGB; \
    using FColor = UCommon::FColor; \
    using FLinearColor = UCommon::FLinearColor; \
    using FDoubleColor = UCommon::FDoubleColor; \
    template<typename T> using TBox2 = UCommon::FBox2; \
    template<typename T> using TBox = UCommon::FBox; \
    template<typename T> using TBox4 = UCommon::FBox4; \
    using FRange = UCommon::FRange; \
    using FBox2 = UCommon::FBox2; \
    using FBox = UCommon::FBox; \
    using FBox4 = UCommon::FBox4; \
    template<typename T> using TSpan = UCommon::TSpan<T>; \
    constexpr float Pi = UCommon::Pi; \
}

#define UBPA_UCOMMON_DEFINE_FVECTOR_OP(Op)        \
TVector operator Op(const TVector& Other) const   \
{                                                 \
    TVector Result;                               \
    Result.X = X Op Other.X;                      \
    Result.Y = Y Op Other.Y;                      \
    Result.Z = Z Op Other.Z;                      \
    return Result;                                \
}                                                 \
TVector operator Op(const T& K) const             \
{                                                 \
    TVector Result;                               \
    Result.X = X Op K;                            \
    Result.Y = Y Op K;                            \
    Result.Z = Z Op K;                            \
    return Result;                                \
}

#define UBPA_UCOMMON_DEFINE_FVECTOR_OP_ASSIGN(Op) \
TVector& operator Op(const TVector& Other)        \
{                                                 \
    X Op Other.X;                                 \
    Y Op Other.Y;                                 \
    Z Op Other.Z;                                 \
    return *this;                                 \
}                                                 \
TVector& operator Op(const T& K)                  \
{                                                 \
    X Op K;                                       \
    Y Op K;                                       \
    Z Op K;                                       \
    return *this;                                 \
}

#define UBPA_UCOMMON_DEFINE_FVECTOR_OP_COMPARE(Op, BoolOp)                \
friend bool operator Op (const TVector& Lhs, const TVector& Rhs) noexcept \
{                                                                         \
    return Lhs.X Op Rhs.X BoolOp Lhs.Y Op Rhs.Y BoolOp Lhs.Z Op Rhs.Z;    \
}

#define UBPA_UCOMMON_DEFINE_FVECTOR4_OP(Op)       \
TVector4 operator Op(const TVector4& Other) const \
{                                                 \
    TVector4 Result;                              \
    Result.X = X Op Other.X;                      \
    Result.Y = Y Op Other.Y;                      \
    Result.Z = Z Op Other.Z;                      \
    Result.W = W Op Other.W;                      \
    return Result;                                \
}                                                 \
TVector4 operator Op(const T& K) const            \
{                                                 \
    TVector4 Result;                              \
    Result.X = X Op K;                            \
    Result.Y = Y Op K;                            \
    Result.Z = Z Op K;                            \
    Result.W = W Op K;                            \
    return Result;                                \
}

#define UBPA_UCOMMON_DEFINE_FVECTOR4_OP_ASSIGN(Op) \
TVector4& operator Op(const TVector4& Other)       \
{                                                  \
    X Op Other.X;                                  \
    Y Op Other.Y;                                  \
    Z Op Other.Z;                                  \
    W Op Other.W;                                  \
    return *this;                                  \
}                                                  \
TVector4& operator Op(const T& K)                  \
{                                                  \
    X Op K;                                        \
    Y Op K;                                        \
    Z Op K;                                        \
    W Op K;                                        \
    return *this;                                  \
}

#define UBPA_UCOMMON_DEFINE_FVECTOR4_OP_COMPARE(Op, BoolOp)                                  \
friend bool operator Op (const TVector4& Lhs, const TVector4& Rhs) noexcept                  \
{                                                                                            \
    return Lhs.X Op Rhs.X BoolOp Lhs.Y Op Rhs.Y BoolOp Lhs.Z Op Rhs.Z BoolOp Lhs.W Op Rhs.W; \
}

#define UBPA_UCOMMON_DEFINE_FVECTOR2_OP(Op)       \
TVector2 operator Op(const TVector2& Other) const \
{                                                 \
    TVector2 Result;                              \
    Result.X = X Op Other.X;                      \
    Result.Y = Y Op Other.Y;                      \
    return Result;                                \
}                                                 \
TVector2 operator Op(const T& K) const            \
{                                                 \
    TVector2 Result;                              \
    Result.X = X Op K;                            \
    Result.Y = Y Op K;                            \
    return Result;                                \
}

#define UBPA_UCOMMON_DEFINE_FVECTOR2_OP_ASSIGN(Op) \
TVector2& operator Op(const TVector2& Other)       \
{                                                  \
    X Op Other.X;                                  \
    Y Op Other.Y;                                  \
    return *this;                                  \
}                                                  \
TVector2& operator Op(const T& K)                  \
{                                                  \
    X Op K;                                        \
    Y Op K;                                        \
    return *this;                                  \
}

#define UBPA_UCOMMON_DEFINE_FVECTOR2_OP_COMPARE(Op, BoolOp)                 \
friend bool operator Op (const TVector2& Lhs, const TVector2& Rhs) noexcept \
{                                                                           \
    return Lhs.X Op Rhs.X BoolOp Lhs.Y Op Rhs.Y;                            \
}

namespace UCommon
{
	template<typename T>
	T Clamp(const T& V, const T& VMin, const T& VMax) noexcept
	{
		UBPA_UCOMMON_ASSERT(VMin <= VMax);
		return V <= VMin ? VMin : (V >= VMax ? VMax : V);
	}

	template<typename T> struct TVector2;
	template<typename T> struct TVector;
	template<typename T> struct TVector4;

	template<typename T>
	struct TVector2
	{
		using value_type = T;

		UBPA_UCOMMON_TVECTOR2_DEFINE

		TVector2() noexcept {} //uninitilaized
		explicit TVector2(T V) noexcept : X(V), Y(V) {}
		TVector2(T InX, T InY) noexcept : X(InX), Y(InY) {}
		template<typename U, typename V>
		TVector2(U InX, V InY) noexcept : X(static_cast<T>(InX)), Y(static_cast<T>(InY)) {}
		template<typename U>
		TVector2(TVector2<U> Vec)  noexcept : X(static_cast<T>(Vec.X)), Y(static_cast<T>(Vec.Y)) {}
		template<typename U>
		TVector2(TVector<U> Vec)  noexcept : X(static_cast<T>(Vec.X)), Y(static_cast<T>(Vec.Y)) {}
		template<typename U>
		TVector2(TVector4<U> Vec)  noexcept : X(static_cast<T>(Vec.X)), Y(static_cast<T>(Vec.Y)) {}

		T X;
		T Y;

		T* GetData() noexcept { return reinterpret_cast<T*>(this); }
		const T* GetData() const noexcept { return reinterpret_cast<const T*>(this); }

		UBPA_UCOMMON_DEFINE_FVECTOR2_OP(+)
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP(-)
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP(*)
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP(/)
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP(%)

		UBPA_UCOMMON_DEFINE_FVECTOR2_OP_ASSIGN(= )
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP_ASSIGN(+=)
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP_ASSIGN(-=)
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP_ASSIGN(*=)
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP_ASSIGN(/=)
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP_ASSIGN(%=)

		UBPA_UCOMMON_DEFINE_FVECTOR2_OP_COMPARE(< , &&)
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP_COMPARE(<=, &&)
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP_COMPARE(> , &&)
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP_COMPARE(>=, &&)
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP_COMPARE(==, &&)
		UBPA_UCOMMON_DEFINE_FVECTOR2_OP_COMPARE(!=, ||)

		static T Dot(const TVector2& A, const TVector2& B)
		{
			return A.X * B.X + A.Y * B.Y;
		}

		static T Cross(const TVector2& A, const TVector2& B)
		{
			return A.X * B.Y - A.Y * B.X;
		}

		static T SquaredDistance(const TVector2& A, const TVector2& B)
		{
			const TVector2 Diff = A - B;
			return Dot(Diff, Diff);
		}

		static T Distance(const TVector2& A, const TVector2& B)
		{
			return std::sqrt(SquaredDistance(A, B));
		}

		T GetSquaredLength() const
		{
			return Dot(*this, *this);
		}

		T GetLength() const
		{
			return std::sqrt(GetSquaredLength());
		}

		bool IsZero() const
		{
			return X == static_cast<T>(0) && Y == static_cast<T>(0);
		}

		bool IsAlmostZero(T Delta = UBPA_UCOMMON_DELTA) const
		{
			return std::abs(X) < Delta
				&& std::abs(Y) < Delta;
		}

		bool IsUnit() const
		{
			return GetSquaredLength() == static_cast<T>(1);
		}

		bool IsAlmostUnit(T SquaredDelta = UBPA_UCOMMON_DELTA) const
		{
			return std::abs(GetSquaredLength() - static_cast<T>(1)) < SquaredDelta;
		}

		TVector2 SafeNormalize(T SquaredDelta = UBPA_UCOMMON_DELTA) const
		{
			const T SquaredLength = GetSquaredLength();
			if (SquaredLength < SquaredDelta)
			{
				return TVector2(0, 0);
			}
			const T Length = std::sqrt(SquaredLength);

			const TVector2 Normed = *this * TVector2(static_cast<T>(1) / Length);
			UBPA_UCOMMON_ASSERT(Normed.IsAlmostUnit(SquaredDelta));
			return Normed;
		}

		TVector2 Project(const TVector2& N) const
		{
			UBPA_UCOMMON_ASSERT(N.IsAlmostUnit());
			return TVector2(Dot(*this, N)) * N;
		}

		TVector2 Perpendicular(const TVector2& N) const
		{
			UBPA_UCOMMON_ASSERT(N.IsAlmostUnit());
			return *this - TVector2(Dot(*this, N)) * N;
		}

		template<typename U>
		operator TVector2<U>() const noexcept
		{
			return{ static_cast<U>(X), static_cast<U>(Y) };
		}

		TVector2 Clmap(T Min, T Max) const noexcept
		{
			return { Clamp(X, Min, Max), Clamp(Y, Min, Max) };
		}

		TVector2 Abs() const noexcept
		{
			return { std::abs(X), std::abs(Y) };
		}

		TVector2 Round() const noexcept
		{
			return { std::round(X), std::round(Y) };
		}

		TVector2 Ceil() const noexcept
		{
			return { std::ceil(X), std::ceil(Y) };
		}

		TVector2 Floor() const noexcept
		{
			return { std::floor(X), std::floor(Y) };
		}

		TVector2 Min(T V) const noexcept
		{
			return { std::min(X, V), std::min(Y, V) };
		}

		TVector2 Max(T V) const noexcept
		{
			return { std::max(X, V), std::max(Y, V) };
		}

		TVector2 Min(const TVector2& V) const noexcept
		{
			return { std::min(X, V.X), std::min(Y, V.Y) };
		}

		TVector2 Max(const TVector2& V) const noexcept
		{
			return { std::max(X, V.X), std::max(Y, V.Y) };
		}

		T Area() const noexcept
		{
			return X * Y;
		}

		static TVector2 Rand01()
		{
			return { rand() / static_cast<T>(RAND_MAX),rand() / static_cast<T>(RAND_MAX) };
		}

		T& operator[](uint64_t Index) noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 2);
			return reinterpret_cast<T*>(this)[Index];
		}

		const T& operator[](uint64_t Index) const noexcept
		{
			return const_cast<TVector2*>(this)->operator[](Index);
		}

		TVector2 operator-() const noexcept
		{
			return { -X,-Y };
		}
	};

	using FVector2 = TVector2<float>;
	using FDoubleVector2 = TVector2<double>;
	using FInt8Vector2 = TVector2<int8_t>;
	using FInt16Vector2 = TVector2<int16_t>;
	using FInt32Vector2 = TVector2<int32_t>;
	using FInt64Vector2 = TVector2<int64_t>;
	using FUint8Vector2 = TVector2<uint8_t>;
	using FUint16Vector2 = TVector2<uint16_t>;
	using FUint32Vector2 = TVector2<uint32_t>;
	using FUint64Vector2 = TVector2<uint64_t>;

	template<typename T>
	struct TVector
	{
		using value_type = T;

		UBPA_UCOMMON_TVECTOR_DEFINE

		TVector() noexcept {} //uninitilaized
		explicit TVector(T V) noexcept : X(V), Y(V), Z(V) {}
		TVector(T InX, T InY, T InZ) noexcept : X(InX), Y(InY), Z(InZ) {}
		TVector(TVector2<T> InXY, T InZ) noexcept : X(InXY.X), Y(InXY.Y), Z(InZ) {}
		TVector(T InX, TVector2<T> InYZ) noexcept : X(InX), Y(InYZ.X), Z(InYZ.Z) {}
		template<typename U, typename V, typename W>
		TVector(U InX, V InY, W InZ) noexcept : X(static_cast<T>(InX)), Y(static_cast<T>(InY)), Z(static_cast<T>(InZ)) {}
		template<typename U, typename V>
		TVector(TVector2<U> InXY, V InZ) noexcept : X(static_cast<T>(InXY.X)), Y(static_cast<T>(InXY.Y)), Z(static_cast<T>(InZ)) {}
		template<typename U, typename V>
		TVector(U InX, TVector2<U> InYZ) noexcept : X(static_cast<T>(InX)), Y(static_cast<T>(InYZ.X)), Z(static_cast<T>(InYZ.Y)) {}
		template<typename U>
		TVector(const TVector<U>& Vec) noexcept : X(static_cast<T>(Vec.X)), Y(static_cast<T>(Vec.Y)), Z(static_cast<T>(Vec.Z)) {}
		template<typename U>
		TVector(const TVector4<U>& Vec) noexcept : X(static_cast<T>(Vec.X)), Y(static_cast<T>(Vec.Y)), Z(static_cast<T>(Vec.Z)) {}

		T X;
		T Y;
		T Z;

		T* GetData() noexcept { return reinterpret_cast<T*>(this); }
		const T* GetData() const noexcept { return reinterpret_cast<const T*>(this); }

		UBPA_UCOMMON_DEFINE_FVECTOR_OP(+)
		UBPA_UCOMMON_DEFINE_FVECTOR_OP(-)
		UBPA_UCOMMON_DEFINE_FVECTOR_OP(*)
		UBPA_UCOMMON_DEFINE_FVECTOR_OP(/)
		UBPA_UCOMMON_DEFINE_FVECTOR_OP(%)

		UBPA_UCOMMON_DEFINE_FVECTOR_OP_ASSIGN(= )
		UBPA_UCOMMON_DEFINE_FVECTOR_OP_ASSIGN(+=)
		UBPA_UCOMMON_DEFINE_FVECTOR_OP_ASSIGN(-=)
		UBPA_UCOMMON_DEFINE_FVECTOR_OP_ASSIGN(*=)
		UBPA_UCOMMON_DEFINE_FVECTOR_OP_ASSIGN(/=)
		UBPA_UCOMMON_DEFINE_FVECTOR_OP_ASSIGN(%=)

		UBPA_UCOMMON_DEFINE_FVECTOR_OP_COMPARE(< , &&)
		UBPA_UCOMMON_DEFINE_FVECTOR_OP_COMPARE(<=, &&)
		UBPA_UCOMMON_DEFINE_FVECTOR_OP_COMPARE(> , &&)
		UBPA_UCOMMON_DEFINE_FVECTOR_OP_COMPARE(>=, &&)
		UBPA_UCOMMON_DEFINE_FVECTOR_OP_COMPARE(==, &&)
		UBPA_UCOMMON_DEFINE_FVECTOR_OP_COMPARE(!=, ||)

		static T Dot(const TVector& A, const TVector& B)
		{
			return A.X * B.X + A.Y * B.Y + A.Z * B.Z;
		}

		static TVector Cross(const TVector& A, const TVector& B)
		{
			return TVector(
				A.Y * B.Z - A.Z * B.Y,
				A.Z * B.X - A.X * B.Z,
				A.X * B.Y - A.Y * B.X);
		}

		static T SquaredDistance(const TVector& A, const TVector& B)
		{
			const TVector Diff = A - B;
			return Dot(Diff, Diff);
		}

		static T Distance(const TVector& A, const TVector& B)
		{
			return std::sqrt(SquaredDistance(A, B));
		}

		T GetSquaredLength() const
		{
			return Dot(*this, *this);
		}

		T GetLength() const
		{
			return std::sqrt(GetSquaredLength());
		}

		bool IsZero() const
		{
			return X == static_cast<T>(0) && Y == static_cast<T>(0) && Z == static_cast<T>(0);
		}

		bool IsAlmostZero(T Delta = UBPA_UCOMMON_DELTA) const
		{
			return std::abs(X) < Delta
				&& std::abs(Y) < Delta
				&& std::abs(Z) < Delta;
		}

		bool IsUnit() const
		{
			return GetSquaredLength() == static_cast<T>(1);
		}

		bool IsAlmostUnit(T SquaredDelta = UBPA_UCOMMON_DELTA) const
		{
			return std::abs(GetSquaredLength() - static_cast<T>(1)) < SquaredDelta;
		}

		TVector SafeNormalize(T SquaredDelta = UBPA_UCOMMON_DELTA) const
		{
			const T SquaredLength = GetSquaredLength();
			if (SquaredLength < SquaredDelta)
			{
				return TVector(0, 0, 0);
			}
			const T Length = std::sqrt(SquaredLength);

			const TVector Normed = *this * TVector(static_cast<T>(1) / Length);
			UBPA_UCOMMON_ASSERT(Normed.IsAlmostUnit(SquaredDelta));
			return Normed;
		}

		TVector Project(const TVector& N) const
		{
			UBPA_UCOMMON_ASSERT(N.IsAlmostUnit());
			return TVector(Dot(*this, N)) * N;
		}

		TVector Perpendicular(const TVector& N) const
		{
			UBPA_UCOMMON_ASSERT(N.IsAlmostUnit());
			return *this - TVector(Dot(*this, N)) * N;
		}

		template<typename U>
		operator TVector<U>() const noexcept
		{
			return { static_cast<U>(X), static_cast<U>(Y), static_cast<U>(Z) };
		}

		TVector Clmap(T Min, T Max) const noexcept
		{
			return { Clamp(X, Min, Max), Clamp(Y, Min, Max), Clamp(Z, Min, Max) };
		}

		TVector Clmap(TVector Min, TVector Max) const noexcept
		{
			return { Clamp(X, Min.X, Max.X), Clamp(Y, Min.Y, Max.Y), Clamp(Z, Min.Z, Max.Z) };
		}

		TVector Abs() const noexcept
		{
			return { std::abs(X), std::abs(Y), std::abs(Z) };
		}

		TVector Round() const noexcept
		{
			return { std::round(X), std::round(Y), std::round(Z) };
		}

		TVector Ceil() const noexcept
		{
			return { std::ceil(X), std::ceil(Y), std::ceil(Z) };
		}

		TVector Floor() const noexcept
		{
			return { std::floor(X), std::floor(Y), std::floor(Z) };
		}

		TVector Min(T V) const noexcept
		{
			return { std::min(X, V), std::min(Y, V), std::min(Z, V) };
		}

		TVector Max(T V) const noexcept
		{
			return { std::max(X, V), std::max(Y, V), std::max(Z, V) };
		}

		TVector Min(const TVector& V) const noexcept
		{
			return { std::min(X, V.X), std::min(Y, V.Y), std::min(Z, V.Z) };
		}

		TVector Max(const TVector& V) const noexcept
		{
			return { std::max(X, V.X), std::max(Y, V.Y), std::max(Z, V.Z) };
		}

		T Volume() const noexcept
		{
			return X * Y * Z;
		}

		static TVector Rand01()
		{
			return TVector{ rand() / static_cast<T>(RAND_MAX),rand() / static_cast<T>(RAND_MAX) ,rand() / static_cast<T>(RAND_MAX) };
		}

		T& operator[](uint64_t Index) noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 3);
			return reinterpret_cast<T*>(this)[Index];
		}

		const T& operator[](uint64_t Index) const noexcept
		{
			return const_cast<TVector*>(this)->operator[](Index);
		}

		TVector operator-() const noexcept
		{
			return { -X,-Y,-Z };
		}
	};

	using FVector = TVector<float>;
	using FDoubleVector = TVector<double>;
	using FInt8Vector = TVector<int8_t>;
	using FInt16Vector = TVector<int16_t>;
	using FInt32Vector = TVector<int32_t>;
	using FInt64Vector = TVector<int64_t>;
	using FUint8Vector = TVector<uint8_t>;
	using FUint16Vector = TVector<uint16_t>;
	using FUint32Vector = TVector<uint32_t>;
	using FUint64Vector = TVector<uint64_t>;

	template<typename T>
	struct TVector4
	{
		using value_type = T;

		UBPA_UCOMMON_TVECTOR4_DEFINE

		TVector4() noexcept {} //uninitilaized
		explicit TVector4(T V) noexcept : X(V), Y(V), Z(V), W(V) {}
		TVector4(T InX, T InY, T InZ, T InW) noexcept : X(InX), Y(InY), Z(InZ), W(InW) {}
		template<typename U0, typename U1, typename U2, typename U3>
		TVector4(U0 InX, U1 InY, U2 InZ, U3 InW) noexcept : X(static_cast<T>(InX)), Y(static_cast<T>(InY)), Z(static_cast<T>(InZ)), W(static_cast<T>(InW)) {}
		template<typename U>
		TVector4(TVector4<U> Vec) noexcept : X(static_cast<T>(Vec.X)), Y(static_cast<T>(Vec.Y)), Z(static_cast<T>(Vec.Z)), W(static_cast<T>(Vec.W)) {}
		template<typename U0, typename U1>
		TVector4(TVector<U0> Vec, U1 InW) noexcept : X(static_cast<T>(Vec.X)), Y(static_cast<T>(Vec.Y)), Z(static_cast<T>(Vec.Z)), W(static_cast<T>(InW)) {}
		template<typename U0, typename U1>
		TVector4(U0 InX, TVector<U1> Vec) noexcept : X(static_cast<T>(InX)), Y(static_cast<T>(Vec.X)), Z(static_cast<T>(Vec.Y)), W(static_cast<T>(Vec.Z)) {}
		template<typename U0, typename U1>
		TVector4(TVector2<U0> VecXY, TVector2<U1> VecZW) noexcept : X(static_cast<T>(VecXY.X)), Y(static_cast<T>(VecXY.Y)), Z(static_cast<T>(VecZW.X)), W(static_cast<T>(VecZW.Y)) {}

		T X;
		T Y;
		T Z;
		T W;

		T* GetData() noexcept { return reinterpret_cast<T*>(this); }
		const T* GetData() const noexcept { return reinterpret_cast<const T*>(this); }

		UBPA_UCOMMON_DEFINE_FVECTOR4_OP(+)
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP(-)
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP(*)
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP(/)
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP(%)

		UBPA_UCOMMON_DEFINE_FVECTOR4_OP_ASSIGN(= )
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP_ASSIGN(+=)
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP_ASSIGN(-=)
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP_ASSIGN(*=)
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP_ASSIGN(/=)
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP_ASSIGN(%=)

		UBPA_UCOMMON_DEFINE_FVECTOR4_OP_COMPARE(< , &&)
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP_COMPARE(<=, &&)
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP_COMPARE(> , &&)
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP_COMPARE(>=, &&)
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP_COMPARE(==, &&)
		UBPA_UCOMMON_DEFINE_FVECTOR4_OP_COMPARE(!=, ||)

		static T Dot(const TVector4& A, const TVector4& B)
		{
			return A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
		}

		static T SquaredDistance(const TVector4& A, const TVector4& B)
		{
			const TVector4 Diff = A - B;
			return Dot(Diff, Diff);
		}

		static T Distance(const TVector4& A, const TVector4& B)
		{
			return std::sqrt(SquaredDistance(A, B));
		}

		T GetSquaredLength() const
		{
			return Dot(*this, *this);
		}

		T GetLength() const
		{
			return std::sqrt(GetSquaredLength());
		}

		bool IsZero() const
		{
			return X == static_cast<T>(0) && Y == static_cast<T>(0) && Z == static_cast<T>(0) && W == static_cast<T>(0);
		}

		bool IsAlmostZero(T Delta = UBPA_UCOMMON_DELTA) const
		{
			return std::abs(X) < Delta
				&& std::abs(Y) < Delta
				&& std::abs(Z) < Delta
				&& std::abs(W) < Delta;
		}

		bool IsUnit() const
		{
			return GetSquaredLength() == static_cast<T>(1);
		}

		bool IsAlmostUnit(T SquaredDelta = UBPA_UCOMMON_DELTA) const
		{
			return std::abs(GetSquaredLength() - static_cast<T>(1)) < SquaredDelta;
		}

		TVector4 SafeNormalize(T SquaredDelta = UBPA_UCOMMON_DELTA) const
		{
			const T SquaredLength = GetSquaredLength();
			if (SquaredLength < SquaredDelta)
			{
				return TVector4(0);
			}
			const T Length = std::sqrt(SquaredLength);

			const TVector4 Normed = *this * TVector4(static_cast<T>(1) / Length);
			UBPA_UCOMMON_ASSERT(Normed.IsAlmostUnit(SquaredDelta));
			return Normed;
		}

		TVector4 Project(const TVector4& N) const
		{
			UBPA_UCOMMON_ASSERT(N.IsAlmostUnit());
			return TVector4(Dot(*this, N)) * N;
		}

		TVector4 Perpendicular(const TVector4& N) const
		{
			UBPA_UCOMMON_ASSERT(N.IsAlmostUnit());
			return *this - TVector4(Dot(*this, N)) * N;
		}

		template<typename U>
		operator TVector4<U>() const noexcept
		{
			return { static_cast<U>(X), static_cast<U>(Y), static_cast<U>(Z), static_cast<U>(W) };
		}

		TVector4 Clmap(T Min, T Max) const noexcept
		{
			return { Clamp(X, Min, Max), Clamp(Y, Min, Max), Clamp(Z, Min, Max), Clamp(W, Min, Max) };
		}

		TVector4 Abs() const noexcept
		{
			return { std::abs(X), std::abs(Y), std::abs(Z), std::abs(W) };
		}

		TVector4 Round() const noexcept
		{
			return { std::round(X), std::round(Y), std::round(Z), std::round(W) };
		}

		TVector4 Ceil() const noexcept
		{
			return { std::ceil(X), std::ceil(Y), std::ceil(Z), std::ceil(W) };
		}

		TVector4 Floor() const noexcept
		{
			return { std::floor(X), std::floor(Y), std::floor(Z), std::floor(W) };
		}

		TVector4 Min(T V) const noexcept
		{
			return { std::min(X, V), std::min(Y, V), std::min(Z, V), std::min(W, V) };
		}

		TVector4 Max(T V) const noexcept
		{
			return { std::max(X, V), std::max(Y, V), std::max(Z, V), std::max(W, V) };
		}

		TVector4 Min(const TVector4& V) const noexcept
		{
			return { std::min(X, V.X), std::min(Y, V.Y), std::min(Z, V.Z), std::min(W, V.W) };
		}

		TVector4 Max(const TVector4& V) const noexcept
		{
			return { std::max(X, V.X), std::max(Y, V.Y), std::max(Z, V.Z), std::max(W, V.W) };
		}

		T Volume() const noexcept
		{
			return X * Y * Z * W;
		}

		static TVector4 Rand01()
		{
			return TVector4{ rand() / static_cast<T>(RAND_MAX),rand() / static_cast<T>(RAND_MAX) ,rand() / static_cast<T>(RAND_MAX),rand() / static_cast<T>(RAND_MAX) };
		}

		T& operator[](uint64_t Index) noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 4);
			return reinterpret_cast<T*>(this)[Index];
		}

		const T& operator[](uint64_t Index) const noexcept
		{
			return const_cast<TVector4*>(this)->operator[](Index);
		}

		TVector4 operator-() const noexcept
		{
			return { -X,-Y,-Z,-W };
		}
	};

	template<typename T> struct TIsVector { static constexpr bool value = false; };
	template<typename T> struct TIsVector<TVector2<T>> { static constexpr bool value = true; };
	template<typename T> struct TIsVector<TVector<T>> { static constexpr bool value = true; };
	template<typename T> struct TIsVector<TVector4<T>> { static constexpr bool value = true; };
	template<typename T>
	constexpr bool IsVector_v = TIsVector<T>::value;

	using FVector4 = TVector4<float>;
	using FDoubleVector4 = TVector4<double>;
	using FInt8Vector4 = TVector4<int8_t>;
	using FInt16Vector4 = TVector4<int16_t>;
	using FInt32Vector4 = TVector4<int32_t>;
	using FInt64Vector4 = TVector4<int64_t>;
	using FUint8Vector4 = TVector4<uint8_t>;
	using FUint16Vector4 = TVector4<uint16_t>;
	using FUint32Vector4 = TVector4<uint32_t>;
	using FUint64Vector4 = TVector4<uint64_t>;

	using FColorRGB = FUint8Vector;
	using FLinearColorRGB = FVector;
	using FDoubleColorRGB = FDoubleVector;
	using FColor = FUint8Vector4;
	using FLinearColor = FVector4;
	using FDoubleColor = FDoubleVector4;

	template<typename T>
	struct TRange
	{
		using value_type = T;

		T Min;
		T Max;
	};

	template<typename T>
	struct TBox2
	{
		using value_type = T;

		TVector2<T> Min;
		TVector2<T> Max;
	};

	template<typename T>
	struct TBox
	{
		using value_type = T;

		TVector<T> Min;
		TVector<T> Max;
	};

	template<typename T>
	struct TBox4
	{
		using value_type = T;

		TVector4<T> Min;
		TVector4<T> Max;
	};

	using FRange = TRange<float>;
	using FBox2 = TBox2<float>;
	using FBox = TBox<float>;
	using FBox4 = TBox4<float>;

	template<typename T>
	constexpr T Pow2(const T& V) noexcept { return V * V; }

	template<typename T, typename U, uint64_t Stride = 1>
	inline T LinearInterpolate(const T val1[2], const U& Texcoord) noexcept
	{
		T val;

		const U OneMinusTexcoord = static_cast<U>(1) - Texcoord;

		val = val1[Stride * 0b1] * Texcoord + val1[Stride * 0b0] * OneMinusTexcoord;

		return val;
	}

	template<typename T>
	class TSpan
	{
	public:
		using value_type = T;

		inline TSpan(T* InPointer, uint64_t InSize) noexcept
			: Pointer(InPointer)
			, Size(InSize)
		{
			UBPA_UCOMMON_ASSERT(InPointer || InSize == 0);
		}

		template<uint64_t N>
		inline TSpan(T(&InPointer)[N]) noexcept : TSpan(InPointer, N) {}

		inline T& operator[](uint64_t Index) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < Size);
			return Pointer[Index];
		}

		inline uint64_t Num() const noexcept { return Size; }
		inline bool Empty() const noexcept { return Size == 0; }
		inline T* GetData() const noexcept { return Pointer; }

		inline T* begin() const noexcept { return Pointer; }
		inline T* end() const noexcept { return Pointer + Size; }

		operator TSpan<typename std::add_const<T>::type>() const noexcept { return { Pointer, Size }; }

	private:
		T* Pointer;
		uint64_t Size;
	};

	static inline void RGBToYCoCg(float R, float G, float B, float& Y, float& Co, float& Cg)
	{
		const float RB = R + B;
		Y = (2 * G + RB) / 4;
		Co = (R - B) / 2;
		Cg = (2 * G - RB) / 4;
	}

	static inline FVector RGBToYCoCg(const FVector& RGB)
	{
		FVector YCoCg;
		RGBToYCoCg(RGB.X, RGB.Y, RGB.Z, YCoCg.X, YCoCg.Y, YCoCg.Z);
		return YCoCg;
	}

	static inline void YCoCgToRGB(float Y, float Co, float Cg, float& R, float& G, float& B)
	{
		const float Tmp = Y - Cg;
		R = Tmp + Co;
		G = Y + Cg;
		B = Tmp - Co;
	}

	static inline FVector YCoCgToRGB(const FVector& YCoCg)
	{
		FVector RGB;
		YCoCgToRGB(YCoCg.X, YCoCg.Y, YCoCg.Z, RGB.X, RGB.Y, RGB.Z);
		return RGB;
	}

	static inline FVector ClampRGBwithYCoCg(const FVector& RGB, const FVector& MinYCoCg, const FVector& MaxYCoCg)
	{
		const FVector YCoCg = RGBToYCoCg(RGB);
		const FVector ClampYCoCg = YCoCg.Clmap(MinYCoCg, MaxYCoCg);
		return YCoCgToRGB(ClampYCoCg);
	}

	static inline FVector ClampRGBwithYCoCg(const FVector& RGB, const FBox& YCoCgBox)
	{
		return ClampRGBwithYCoCg(RGB, YCoCgBox.Min, YCoCgBox.Max);
	}

	static constexpr float Pi = 3.1415926f;

	static inline FVector4 UniformSampleSphere(FVector2 E)
	{
		float Phi = 2 * Pi * E.X;
		float CosTheta = 1 - 2 * E.Y;
		float SinTheta = std::sqrt(1 - CosTheta * CosTheta);

		FVector H;
		H.X = SinTheta * cos(Phi);
		H.Y = SinTheta * sin(Phi);
		H.Z = CosTheta;

		constexpr float PDF = 1.0f / (4 * Pi);

		return FVector4(H, PDF);
	}

	constexpr float GlodenRatio = 1.618034f;

	//Reference: https://zhuanlan.zhihu.com/p/25988652?group_id=828963677192491008
	static inline FVector4 FibonacciSpherePoint(int N, int n)
	{
		constexpr float Phi = 2.f * Pi / GlodenRatio;
		const float Z = (2.f * n + 1.f) / N - 1.f;
		const float r = std::sqrt(1.f - Z * Z);
		const float X = r * std::cos(Phi * n);
		const float Y = r * std::sin(Phi * n);

		constexpr float PDF = 1.0f / (4.f * Pi);

		return FVector4(X, Y, Z, PDF);
	}
}

UBPA_UCOMMON_UTILS_TO_NAMESPACE(UCommonTest)
