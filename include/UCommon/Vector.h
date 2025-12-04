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

#include "Cpp17.h"

#include <cmath>
#include <algorithm>

#define UBPA_UCOMMON_VECTOR_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
    template<typename T> using TVector2 = UCommon::TVector2<T>; \
    template<typename T> using TVector = UCommon::TVector<T>; \
    template<typename T> using TVector4 = UCommon::TVector4<T>; \
    using FVector2f = UCommon::FVector2f; \
    using FVector2d = UCommon::FVector2d; \
    using FInt8Vector2 = UCommon::FInt8Vector2; \
    using FInt16Vector2 = UCommon::FInt16Vector2; \
    using FInt32Vector2 = UCommon::FInt32Vector2; \
    using FInt64Vector2 = UCommon::FInt64Vector2; \
    using FUint8Vector2 = UCommon::FUint8Vector2; \
    using FUint16Vector2 = UCommon::FUint16Vector2; \
    using FUint32Vector2 = UCommon::FUint32Vector2; \
    using FUint64Vector2 = UCommon::FUint64Vector2; \
    using FVector3f = UCommon::FVector3f; \
    using FVector3d = UCommon::FVector3d; \
    using FInt8Vector = UCommon::FInt8Vector; \
    using FInt16Vector = UCommon::FInt16Vector; \
    using FInt32Vector = UCommon::FInt32Vector; \
    using FInt64Vector = UCommon::FInt64Vector; \
    using FUint8Vector = UCommon::FUint8Vector; \
    using FUint16Vector = UCommon::FUint16Vector; \
    using FUint32Vector = UCommon::FUint32Vector; \
    using FUint64Vector = UCommon::FUint64Vector; \
    using FVector4f = UCommon::FVector4f; \
    using FVector4d = UCommon::FVector4d; \
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
}

#define UBPA_UCOMMON_DEFINE_FRIEND_OP(Type, Op)    \
friend Type operator Op(const T& K, const Type& V) \
{                                                  \
    return Type(K) Op V;                           \
}

#define UBPA_UCOMMON_DEFINE_TVECTOR_OP(Op)      \
TVector operator Op(const TVector& Other) const \
{                                               \
    TVector Result;                             \
    Result.X = X Op Other.X;                    \
    Result.Y = Y Op Other.Y;                    \
    Result.Z = Z Op Other.Z;                    \
    return Result;                              \
}                                               \
TVector operator Op(const T& K) const           \
{                                               \
    TVector Result;                             \
    Result.X = X Op K;                          \
    Result.Y = Y Op K;                          \
    Result.Z = Z Op K;                          \
    return Result;                              \
}                                               \
UBPA_UCOMMON_DEFINE_FRIEND_OP(TVector, Op)

#define UBPA_UCOMMON_DEFINE_TVECTOR_OP_ASSIGN(Op) \
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

#define UBPA_UCOMMON_DEFINE_TVECTOR_OP_COMPARE(Op, BoolOp)                \
friend bool operator Op (const TVector& Lhs, const TVector& Rhs) noexcept \
{                                                                         \
    return Lhs.X Op Rhs.X BoolOp Lhs.Y Op Rhs.Y BoolOp Lhs.Z Op Rhs.Z;    \
}

#define UBPA_UCOMMON_DEFINE_TVECTOR4_OP(Op)       \
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
}                                                 \
UBPA_UCOMMON_DEFINE_FRIEND_OP(TVector4, Op)

#define UBPA_UCOMMON_DEFINE_TVECTOR4_OP_ASSIGN(Op) \
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

#define UBPA_UCOMMON_DEFINE_TVECTOR4_OP_COMPARE(Op, BoolOp)                                  \
friend bool operator Op (const TVector4& Lhs, const TVector4& Rhs) noexcept                  \
{                                                                                            \
    return Lhs.X Op Rhs.X BoolOp Lhs.Y Op Rhs.Y BoolOp Lhs.Z Op Rhs.Z BoolOp Lhs.W Op Rhs.W; \
}

#define UBPA_UCOMMON_DEFINE_TVECTOR2_OP(Op)       \
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
}                                                 \
UBPA_UCOMMON_DEFINE_FRIEND_OP(TVector2, Op)

#define UBPA_UCOMMON_DEFINE_TVECTOR2_OP_ASSIGN(Op) \
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

#define UBPA_UCOMMON_DEFINE_TVECTOR2_OP_COMPARE(Op, BoolOp)                 \
friend bool operator Op (const TVector2& Lhs, const TVector2& Rhs) noexcept \
{                                                                           \
    return Lhs.X Op Rhs.X BoolOp Lhs.Y Op Rhs.Y;                            \
}

namespace UCommon
{
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

		template<typename U>
		U& As()&
		{
			static_assert(sizeof(U) == sizeof(TVector2), "The size of U is not same with TVector2");
			return *reinterpret_cast<U*>(this);
		}

		template<typename U>
		const U& As() const&
		{
			return const_cast<TVector2*>(this)->As<U>();
		}

		template<typename U>
		const U&& As() const&&
		{
			static_assert(sizeof(U) == sizeof(TVector2), "The size of U is not same with TVector2");
			static_assert(alignof(TVector2) % alignof(U) == 0, "The alignment of U is not compatible with TVector2");
			return reinterpret_cast<const U&&>(*this);
		}

		T* GetData() noexcept { return reinterpret_cast<T*>(this); }
		const T* GetData() const noexcept { return reinterpret_cast<const T*>(this); }

		UBPA_UCOMMON_DEFINE_TVECTOR2_OP(+)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP(-)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP(*)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP(/)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP(%)

		UBPA_UCOMMON_DEFINE_TVECTOR2_OP_ASSIGN( =)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP_ASSIGN(+=)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP_ASSIGN(-=)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP_ASSIGN(*=)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP_ASSIGN(/=)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP_ASSIGN(%=)

		UBPA_UCOMMON_DEFINE_TVECTOR2_OP_COMPARE(< , &&)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP_COMPARE(<=, &&)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP_COMPARE(> , &&)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP_COMPARE(>=, &&)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP_COMPARE(==, &&)
		UBPA_UCOMMON_DEFINE_TVECTOR2_OP_COMPARE(!=, ||)

		static T Dot(const TVector2& A, const TVector2& B)
		{
			return A.X * B.X + A.Y * B.Y;
		}

		T Dot(const TVector2& Other) const
		{
			return Dot(*this, Other);
		}

		T operator|(const TVector2& Other) const
		{
			return Dot(Other);
		}

		static T Cross(const TVector2& A, const TVector2& B)
		{
			return A.X * B.Y - A.Y * B.X;
		}

		T Cross(const TVector2& Other) const
		{
			return Cross(*this, Other);
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

		TVector2 Clamp(T Min, T Max) const noexcept
		{
			return { UCommon::Clamp(X, Min, Max), UCommon::Clamp(Y, Min, Max) };
		}

		TVector2 Clamp(const TVector2& Min, const TVector2& Max) const noexcept
		{
			return { UCommon::Clamp(X, Min.X, Max.X), UCommon::Clamp(Y, Min.Y, Max.Y) };
		}

		TVector2 Saturate() const noexcept
		{
			return Clamp(static_cast<T>(0), static_cast<T>(1));
		}

		TVector2 Abs() const noexcept
		{
			return { std::abs(X), std::abs(Y) };
		}

		TVector2 Sqrt() const noexcept
		{
			return { std::sqrt(X), std::sqrt(Y) };
		}

		TVector2 Square() const noexcept
		{
			return { X * X, Y * Y };
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

		TVector2 Reciprocal() const noexcept
		{
			return { 1 / X, 1 / Y };
		}

		TVector2 Log() const noexcept
		{
			return { std::log(X), std::log(Y) };
		}

		TVector2 Log2() const noexcept
		{
			return { std::log2(X), std::log2(Y) };
		}

		TVector2 Exp() const noexcept
		{
			return { std::exp(X), std::exp(Y) };
		}

		TVector2 Exp2() const noexcept
		{
			return { std::exp2(X), std::exp2(Y) };
		}

		static TVector2 Min(const TVector2& A, T B)
		{
			return { std::min(A.X, B), std::min(A.Y, B) };
		}
		
		static TVector2 Min(const TVector2& A, const TVector2& B)
		{
			return { std::min(A.X, B.X), std::min(A.Y, B.Y) };
		}

		static TVector2 Max(const TVector2& A, T B)
		{
			return { std::max(A.X, B), std::max(A.Y, B) };
		}

		static TVector2 Max(const TVector2& A, const TVector2& B)
		{
			return { std::max(A.X, B.X), std::max(A.Y, B.Y) };
		}

		TVector2 Min(T V) const noexcept
		{
			return Min(*this, V);
		}

		TVector2 Min(const TVector2& V) const noexcept
		{
			return Min(*this, V);
		}

		TVector2 Max(T V) const noexcept
		{
			return Max(*this, V);
		}

		TVector2 Max(const TVector2& V) const noexcept
		{
			return Max(*this, V);
		}

		T MinComponent() const noexcept
		{
			return std::min(X, Y);
		}

		T MaxComponent() const noexcept
		{
			return std::max(X, Y);
		}

		T Average() const noexcept
		{
			return (X + Y) * static_cast<T>(1. / 2.);
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

	using FVector2f = TVector2<float>;
	using FVector2d = TVector2<double>;
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
		TVector(const TVector2<T>& InXY, T InZ) noexcept : X(InXY.X), Y(InXY.Y), Z(InZ) {}
		TVector(T InX, const TVector2<T>& InYZ) noexcept : X(InX), Y(InYZ.X), Z(InYZ.Y) {}
		template<typename U, typename V, typename W>
		TVector(U InX, V InY, W InZ) noexcept : X(static_cast<T>(InX)), Y(static_cast<T>(InY)), Z(static_cast<T>(InZ)) {}
		template<typename U, typename V>
		TVector(const TVector2<U>& InXY, V InZ) noexcept : X(static_cast<T>(InXY.X)), Y(static_cast<T>(InXY.Y)), Z(static_cast<T>(InZ)) {}
		template<typename U, typename V>
		TVector(U InX, TVector2<U> InYZ) noexcept : X(static_cast<T>(InX)), Y(static_cast<T>(InYZ.X)), Z(static_cast<T>(InYZ.Y)) {}
		template<typename U>
		TVector(const TVector<U>& Vec) noexcept : X(static_cast<T>(Vec.X)), Y(static_cast<T>(Vec.Y)), Z(static_cast<T>(Vec.Z)) {}
		template<typename U>
		TVector(const TVector4<U>& Vec) noexcept : X(static_cast<T>(Vec.X)), Y(static_cast<T>(Vec.Y)), Z(static_cast<T>(Vec.Z)) {}

		T X;
		T Y;
		T Z;

		template<typename U>
		U& As()&
		{
			static_assert(sizeof(U) == sizeof(TVector), "The size of U is not same with TVector");
			return *reinterpret_cast<U*>(this);
		}

		template<typename U>
		const U& As() const&
		{
			return const_cast<TVector*>(this)->As<U>();
		}

		template<typename U>
		const U&& As() const&&
		{
			static_assert(sizeof(U) == sizeof(TVector), "The size of U is not same with TVector");
			static_assert(alignof(TVector) % alignof(U) == 0, "The alignment of U is not compatible with TVector");
			return reinterpret_cast<const U&&>(*this);
		}

		T* GetData() noexcept { return reinterpret_cast<T*>(this); }
		const T* GetData() const noexcept { return reinterpret_cast<const T*>(this); }

		UBPA_UCOMMON_DEFINE_TVECTOR_OP(+)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP(-)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP(*)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP(/)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP(%)

		UBPA_UCOMMON_DEFINE_TVECTOR_OP_ASSIGN( =)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP_ASSIGN(+=)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP_ASSIGN(-=)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP_ASSIGN(*=)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP_ASSIGN(/=)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP_ASSIGN(%=)

		UBPA_UCOMMON_DEFINE_TVECTOR_OP_COMPARE(< , &&)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP_COMPARE(<=, &&)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP_COMPARE(> , &&)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP_COMPARE(>=, &&)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP_COMPARE(==, &&)
		UBPA_UCOMMON_DEFINE_TVECTOR_OP_COMPARE(!=, ||)

		static T Dot(const TVector& A, const TVector& B)
		{
			return A.X * B.X + A.Y * B.Y + A.Z * B.Z;
		}

		T Dot(const TVector& V) const
		{
			return Dot(*this, V);
		}

		T operator|(const TVector& Other) const
		{
			return Dot(Other);
		}

		static TVector Cross(const TVector& A, const TVector& B)
		{
			return TVector(
				A.Y * B.Z - A.Z * B.Y,
				A.Z * B.X - A.X * B.Z,
				A.X * B.Y - A.Y * B.X);
		}

		TVector Cross(const TVector& V) const
		{
			return Cross(*this, V);
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

		TVector Clamp(T Min, T Max) const noexcept
		{
			return { UCommon::Clamp(X, Min, Max), UCommon::Clamp(Y, Min, Max), UCommon::Clamp(Z, Min, Max) };
		}

		TVector Clamp(const TVector& Min, const TVector& Max) const noexcept
		{
			return { UCommon::Clamp(X, Min.X, Max.X), UCommon::Clamp(Y, Min.Y, Max.Y), UCommon::Clamp(Z, Min.Z, Max.Z) };
		}

		TVector Saturate() const noexcept
		{
			return Clamp(static_cast<T>(0), static_cast<T>(1));
		}

		TVector Abs() const noexcept
		{
			return { std::abs(X), std::abs(Y), std::abs(Z) };
		}

		TVector Sqrt() const noexcept
		{
			return { std::sqrt(X), std::sqrt(Y), std::sqrt(Z) };
		}

		TVector Square() const noexcept
		{
			return { X * X, Y * Y, Z * Z };
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

		TVector Reciprocal() const noexcept
		{
			return { 1 / X, 1 / Y, 1 / Z };
		}

		TVector Log() const noexcept
		{
			return { std::log(X), std::log(Y), std::log(Z) };
		}

		TVector Log2() const noexcept
		{
			return { std::log2(X), std::log2(Y), std::log2(Z) };
		}

		TVector Exp() const noexcept
		{
			return { std::exp(X), std::exp(Y), std::exp(Z) };
		}

		TVector Exp2() const noexcept
		{
			return { std::exp2(X), std::exp2(Y), std::exp2(Z) };
		}

		static TVector Min(const TVector& A, T B)
		{
			return { std::min(A.X, B), std::min(A.Y, B), std::min(A.Z, B) };
		}

		static TVector Min(const TVector& A, const TVector& B)
		{
			return { std::min(A.X, B.X), std::min(A.Y, B.Y), std::min(A.Z, B.Z) };
		}

		static TVector Max(const TVector& A, T B)
		{
			return { std::max(A.X, B), std::max(A.Y, B), std::max(A.Z, B) };
		}

		static TVector Max(const TVector& A, const TVector& B)
		{
			return { std::max(A.X, B.X), std::max(A.Y, B.Y), std::max(A.Z, B.Z) };
		}

		TVector Min(T V) const noexcept
		{
			return Min(*this, V);
		}

		TVector Min(const TVector& V) const noexcept
		{
			return Min(*this, V);
		}

		TVector Max(T V) const noexcept
		{
			return Max(*this, V);
		}

		TVector Max(const TVector& V) const noexcept
		{
			return Max(*this, V);
		}

		T MinComponent() const noexcept
		{
			return std::min(std::min(X, Y), Z);
		}

		T MaxComponent() const noexcept
		{
			return std::max(std::max(X, Y), Z);
		}

		T Average() const noexcept
		{
			return (X + Y + Z) * static_cast<T>(1. / 3.);
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
		
		/** Calculates luminance using standard RGB weights (0.3, 0.59, 0.11). */
		T GetLuminance() const
		{
			if constexpr (std::is_floating_point_v<T>)
			{
				return X * static_cast<T>(0.3) + Y * static_cast<T>(0.59) + Z * static_cast<T>(0.11);
			}
			else
			{
				using CalcType = std::conditional_t<std::is_signed_v<T>, int64_t, uint64_t>;
				return static_cast<T>((static_cast<CalcType>(X) * 30 +
				                       static_cast<CalcType>(Y) * 59 +
				                       static_cast<CalcType>(Z) * 11) / 100);
			}
		}

		/** Calculates luminance using sRGB weights (0.2126, 0.7152, 0.0722). */
		T GetSrgbLuminance() const
		{
			if constexpr (std::is_floating_point_v<T>)
			{
				return X * static_cast<T>(0.2126) + Y * static_cast<T>(0.7152) + Z * static_cast<T>(0.0722);
			}
			else
			{
				using CalcType = std::conditional_t<std::is_signed_v<T>, int64_t, uint64_t>;
				return static_cast<T>((static_cast<CalcType>(X) * 2126 +
				                       static_cast<CalcType>(Y) * 7152 +
				                       static_cast<CalcType>(Z) * 722) / 10000);
			}
		}
	};

	using FVector3f = TVector<float>;
	using FVector3d = TVector<double>;
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
		TVector4(const TVector4<U>& Vec) noexcept : X(static_cast<T>(Vec.X)), Y(static_cast<T>(Vec.Y)), Z(static_cast<T>(Vec.Z)), W(static_cast<T>(Vec.W)) {}
		template<typename U0, typename U1>
		TVector4(const TVector<U0>& Vec, U1 InW) noexcept : X(static_cast<T>(Vec.X)), Y(static_cast<T>(Vec.Y)), Z(static_cast<T>(Vec.Z)), W(static_cast<T>(InW)) {}
		template<typename U0, typename U1>
		TVector4(U0 InX, const TVector<U1>& Vec) noexcept : X(static_cast<T>(InX)), Y(static_cast<T>(Vec.X)), Z(static_cast<T>(Vec.Y)), W(static_cast<T>(Vec.Z)) {}
		template<typename U0, typename U1, typename U2>
		TVector4(const TVector2<U0>& VecXY, U1 InZ, U2 InW) noexcept : X(static_cast<T>(VecXY.X)), Y(static_cast<T>(VecXY.Y)), Z(static_cast<T>(InZ)), W(static_cast<T>(InW)) {}
		template<typename U0, typename U1, typename U2>
		TVector4(U0 InX, const TVector2<U1>& VecYZ, U2 InW) noexcept : X(static_cast<T>(InX)), Y(static_cast<T>(VecYZ.X)), Z(static_cast<T>(VecYZ.Y)), W(static_cast<T>(InW)) {}
		template<typename U0, typename U1, typename U2>
		TVector4(U0 InX, U1 InY, const TVector2<U2>& VecZW) noexcept : X(static_cast<T>(InX)), Y(static_cast<T>(InY)), Z(static_cast<T>(VecZW.X)), W(static_cast<T>(VecZW.Y)) {}
		template<typename U0, typename U1>
		TVector4(const TVector2<U0>& VecXY, const TVector2<U1>& VecZW) noexcept : X(static_cast<T>(VecXY.X)), Y(static_cast<T>(VecXY.Y)), Z(static_cast<T>(VecZW.X)), W(static_cast<T>(VecZW.Y)) {}

		T X;
		T Y;
		T Z;
		T W;

		template<typename U>
		U& As()&
		{
			static_assert(sizeof(U) == sizeof(TVector4), "The size of U is not same with TVector4");
			return *reinterpret_cast<U*>(this);
		}

		template<typename U>
		const U& As() const&
		{
			return const_cast<TVector4*>(this)->As<U>();
		}

		template<typename U>
		const U&& As() const&&
		{
			static_assert(sizeof(U) == sizeof(TVector4), "The size of U is not same with TVector4");
			static_assert(alignof(TVector4) % alignof(U) == 0, "The alignment of U is not compatible with TVector4");
			return reinterpret_cast<const U&&>(*this);
		}

		T* GetData() noexcept { return reinterpret_cast<T*>(this); }
		const T* GetData() const noexcept { return reinterpret_cast<const T*>(this); }

		UBPA_UCOMMON_DEFINE_TVECTOR4_OP(+)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP(-)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP(*)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP(/)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP(%)

		UBPA_UCOMMON_DEFINE_TVECTOR4_OP_ASSIGN( =)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP_ASSIGN(+=)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP_ASSIGN(-=)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP_ASSIGN(*=)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP_ASSIGN(/=)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP_ASSIGN(%=)

		UBPA_UCOMMON_DEFINE_TVECTOR4_OP_COMPARE(< , &&)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP_COMPARE(<=, &&)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP_COMPARE(> , &&)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP_COMPARE(>=, &&)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP_COMPARE(==, &&)
		UBPA_UCOMMON_DEFINE_TVECTOR4_OP_COMPARE(!=, ||)

		static T Dot(const TVector4& A, const TVector4& B)
		{
			return A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
		}

		T Dot(const TVector4& V) const
		{
			return Dot(*this, V);
		}

		T operator|(const TVector4& Other) const
		{
			return Dot(Other);
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

		TVector4 Clamp(T Min, T Max) const noexcept
		{
			return { UCommon::Clamp(X, Min, Max), UCommon::Clamp(Y, Min, Max), UCommon::Clamp(Z, Min, Max), UCommon::Clamp(W, Min, Max) };
		}

		TVector4 Clamp(const TVector4& Min, const TVector4& Max) const noexcept
		{
			return { UCommon::Clamp(X, Min.X, Max.X), UCommon::Clamp(Y, Min.Y, Max.Y), UCommon::Clamp(Z, Min.Z, Max.Z), UCommon::Clamp(W, Min.W, Max.W) };
		}

		TVector4 Saturate() const noexcept
		{
			return Clamp(static_cast<T>(0), static_cast<T>(1));
		}

		TVector4 Abs() const noexcept
		{
			return { std::abs(X), std::abs(Y), std::abs(Z), std::abs(W) };
		}

		TVector4 Sqrt() const noexcept
		{
			return { std::sqrt(X), std::sqrt(Y), std::sqrt(Z), std::sqrt(W) };
		}

		TVector4 Square() const noexcept
		{
			return { X * X, Y * Y, Z * Z, W * W };
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

		TVector4 Reciprocal() const noexcept
		{
			return { 1 / X, 1 / Y, 1 / Z, 1 / W };
		}

		TVector4 Log() const noexcept
		{
			return { std::log(X), std::log(Y), std::log(Z), std::log(W) };
		}

		TVector4 Log2() const noexcept
		{
			return { std::log2(X), std::log2(Y), std::log2(Z), std::log2(W) };
		}

		TVector4 Exp() const noexcept
		{
			return { std::exp(X), std::exp(Y), std::exp(Z), std::exp(W) };
		}

		TVector4 Exp2() const noexcept
		{
			return { std::exp2(X), std::exp2(Y), std::exp2(Z), std::exp2(W) };
		}

		static TVector4 Min(const TVector4& A, T B)
		{
			return { std::min(A.X, B), std::min(A.Y, B), std::min(A.Z, B), std::min(A.W, B) };
		}

		static TVector4 Min(const TVector4& A, const TVector4& B)
		{
			return { std::min(A.X, B.X), std::min(A.Y, B.Y), std::min(A.Z, B.Z), std::min(A.W, B.W) };
		}

		static TVector4 Max(const TVector4& A, T B)
		{
			return { std::max(A.X, B), std::max(A.Y, B), std::max(A.Z, B), std::max(A.W, B) };
		}

		static TVector4 Max(const TVector4& A, const TVector4& B)
		{
			return { std::max(A.X, B.X), std::max(A.Y, B.Y), std::max(A.Z, B.Z), std::max(A.W, B.W) };
		}

		TVector4 Min(T V) const noexcept
		{
			return Min(*this, V);
		}

		TVector4 Min(const TVector4& V) const noexcept
		{
			return Min(*this, V);
		}

		TVector4 Max(T V) const noexcept
		{
			return Max(*this, V);
		}

		TVector4 Max(const TVector4& V) const noexcept
		{
			return Max(*this, V);
		}

		T MaxComponent() const noexcept
		{
			return std::max(std::max(X, Y), std::max(Z, W));
		}

		T MinComponent() const noexcept
		{
			return std::min(std::min(X, Y), std::min(Z, W));
		}

		T Average() const noexcept
		{
			return (X + Y + Z + W) * static_cast<T>(1. / 4.);
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
	template<typename T> struct TRemoveVector { using value_type = T; };
	template<typename T> struct TRemoveVector<TVector2<T>> { using value_type = T; };
	template<typename T> struct TRemoveVector<TVector<T>> { using value_type = T; };
	template<typename T> struct TRemoveVector<TVector4<T>> { using value_type = T; };

	using FVector4f = TVector4<float>;
	using FVector4d = TVector4<double>;
	using FInt8Vector4 = TVector4<int8_t>;
	using FInt16Vector4 = TVector4<int16_t>;
	using FInt32Vector4 = TVector4<int32_t>;
	using FInt64Vector4 = TVector4<int64_t>;
	using FUint8Vector4 = TVector4<uint8_t>;
	using FUint16Vector4 = TVector4<uint16_t>;
	using FUint32Vector4 = TVector4<uint32_t>;
	using FUint64Vector4 = TVector4<uint64_t>;

	using FColorRGB = FUint8Vector;
	using FLinearColorRGB = FVector3f;
	using FDoubleColorRGB = FVector3d;
	using FColor = FUint8Vector4;
	using FLinearColor = FVector4f;
	using FDoubleColor = FVector4d;

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
}

UBPA_UCOMMON_VECTOR_TRAITS_DEFINE

UBPA_UCOMMON_VECTOR_TO_NAMESPACE(UCommonTest)

// Structured binding support for TVector2, TVector, TVector4
namespace std
{
	// TVector2
	template<typename T>
	struct tuple_size<UCommon::TVector2<T>> : integral_constant<size_t, 2> {};

	template<size_t I, typename T>
	struct tuple_element<I, UCommon::TVector2<T>>
	{
		using type = T;
	};

	// TVector (3D)
	template<typename T>
	struct tuple_size<UCommon::TVector<T>> : integral_constant<size_t, 3> {};

	template<size_t I, typename T>
	struct tuple_element<I, UCommon::TVector<T>>
	{
		using type = T;
	};

	// TVector4
	template<typename T>
	struct tuple_size<UCommon::TVector4<T>> : integral_constant<size_t, 4> {};

	template<size_t I, typename T>
	struct tuple_element<I, UCommon::TVector4<T>>
	{
		using type = T;
	};
}

// get functions for structured binding
namespace UCommon
{
	// TVector2 get functions
	template<size_t I, typename T>
	constexpr T& get(TVector2<T>& V) noexcept
	{
		static_assert(I < 2, "Index out of bounds for TVector2");
		if constexpr (I == 0) return V.X;
		else if constexpr (I == 1) return V.Y;
	}

	template<size_t I, typename T>
	constexpr const T& get(const TVector2<T>& V) noexcept
	{
		static_assert(I < 2, "Index out of bounds for TVector2");
		if constexpr (I == 0) return V.X;
		else if constexpr (I == 1) return V.Y;
	}

	template<size_t I, typename T>
	constexpr T&& get(TVector2<T>&& V) noexcept
	{
		static_assert(I < 2, "Index out of bounds for TVector2");
		if constexpr (I == 0) return std::move(V.X);
		else if constexpr (I == 1) return std::move(V.Y);
	}

	// TVector (3D) get functions
	template<size_t I, typename T>
	constexpr T& get(TVector<T>& V) noexcept
	{
		static_assert(I < 3, "Index out of bounds for TVector");
		if constexpr (I == 0) return V.X;
		else if constexpr (I == 1) return V.Y;
		else if constexpr (I == 2) return V.Z;
	}

	template<size_t I, typename T>
	constexpr const T& get(const TVector<T>& V) noexcept
	{
		static_assert(I < 3, "Index out of bounds for TVector");
		if constexpr (I == 0) return V.X;
		else if constexpr (I == 1) return V.Y;
		else if constexpr (I == 2) return V.Z;
	}

	template<size_t I, typename T>
	constexpr T&& get(TVector<T>&& V) noexcept
	{
		static_assert(I < 3, "Index out of bounds for TVector");
		if constexpr (I == 0) return std::move(V.X);
		else if constexpr (I == 1) return std::move(V.Y);
		else if constexpr (I == 2) return std::move(V.Z);
	}

	// TVector4 get functions
	template<size_t I, typename T>
	constexpr T& get(TVector4<T>& V) noexcept
	{
		static_assert(I < 4, "Index out of bounds for TVector4");
		if constexpr (I == 0) return V.X;
		else if constexpr (I == 1) return V.Y;
		else if constexpr (I == 2) return V.Z;
		else if constexpr (I == 3) return V.W;
	}

	template<size_t I, typename T>
	constexpr const T& get(const TVector4<T>& V) noexcept
	{
		static_assert(I < 4, "Index out of bounds for TVector4");
		if constexpr (I == 0) return V.X;
		else if constexpr (I == 1) return V.Y;
		else if constexpr (I == 2) return V.Z;
		else if constexpr (I == 3) return V.W;
	}

	template<size_t I, typename T>
	constexpr T&& get(TVector4<T>&& V) noexcept
	{
		static_assert(I < 4, "Index out of bounds for TVector4");
		if constexpr (I == 0) return std::move(V.X);
		else if constexpr (I == 1) return std::move(V.Y);
		else if constexpr (I == 2) return std::move(V.Z);
		else if constexpr (I == 3) return std::move(V.W);
	}
}
