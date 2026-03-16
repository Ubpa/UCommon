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

#include "Vector.h"

#include <cmath>

#define UBPA_UCOMMON_MATRIX_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
    template<typename T> using TMatrix3x3 = UCommon::TMatrix3x3<T>; \
    template<typename T> using TMatrix4x4 = UCommon::TMatrix4x4<T>; \
    using FMatrix3x3f = UCommon::FMatrix3x3f; \
    using FMatrix3x3d = UCommon::FMatrix3x3d; \
    using FMatrix4x4f = UCommon::FMatrix4x4f; \
    using FMatrix4x4d = UCommon::FMatrix4x4d; \
}

namespace UCommon
{
	template<typename T> struct TMatrix3x3;
	template<typename T> struct TMatrix4x4;

	// 3x3 Matrix (Row-Major Order)
	// Row 0: r0
	// Row 1: r1
	// Row 2: r2
	template<typename T>
	struct TMatrix3x3
	{
		using value_type = T;

		// Constructors
		TMatrix3x3() noexcept {} // uninitialized
		TMatrix3x3(T Value) noexcept
			: r0(Value), r1(Value), r2(Value) {}
		TMatrix3x3(
			TVector<T> Row0,
			TVector<T> Row1,
			TVector<T> Row2) noexcept
			: r0(Row0), r1(Row1), r2(Row2) {}
		TMatrix3x3(
			T In00, T In01, T In02,
			T In10, T In11, T In12,
			T In20, T In21, T In22) noexcept
			: r0(In00, In01, In02)
			, r1(In10, In11, In12)
			, r2(In20, In21, In22) {}

		// Row-major storage using TVector
		TVector<T> r0; // Row 0
		TVector<T> r1; // Row 1
		TVector<T> r2; // Row 2

		// Access operators
		TVector<T>& operator[](uint32_t Index) noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 3);
			return (&r0)[Index];
		}

		const TVector<T>& operator[](uint32_t Index) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 3);
			return (&r0)[Index];
		}

		T& operator()(uint32_t Row, uint32_t Col) noexcept
		{
			UBPA_UCOMMON_ASSERT(Row < 3 && Col < 3);
			return (&r0)[Row][Col];
		}

		const T& operator()(uint32_t Row, uint32_t Col) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Row < 3 && Col < 3);
			return (*this)[Row][Col];
		}

		T* GetData() noexcept { return r0.GetData(); }
		const T* GetData() const noexcept { return r0.GetData(); }

		// Matrix-Vector multiplication (M * V)
		// Matrix is left, vector is right (column vector)
		TVector<T> operator*(const TVector<T>& V) const noexcept
		{
			return TVector<T>(
				r0 | V, // dot product with each row
				r1 | V,
				r2 | V
			);
		}

		// Matrix-Vector multiplication for 2D points (w=1)
		TVector2<T> operator*(const TVector2<T>& V) const noexcept
		{
			TVector<T> V3D(V.X, V.Y, static_cast<T>(1));
			TVector<T> Result = (*this) * V3D;
			return TVector2<T>(Result.X, Result.Y);
		}

		// Matrix-Vector multiplication for 2D vectors (w=0)
		TVector2<T> TransformVector(const TVector2<T>& V) const noexcept
		{
			TVector<T> V3D(V.X, V.Y, static_cast<T>(0));
			TVector<T> Result = (*this) * V3D;
			return TVector2<T>(Result.X, Result.Y);
		}

		// Matrix-Matrix multiplication
		TMatrix3x3 operator*(const TMatrix3x3& Other) const noexcept
		{
			TMatrix3x3 OtherT = Other.Transpose();
			return TMatrix3x3(
				TVector<T>(r0 | OtherT.r0, r0 | OtherT.r1, r0 | OtherT.r2),
				TVector<T>(r1 | OtherT.r0, r1 | OtherT.r1, r1 | OtherT.r2),
				TVector<T>(r2 | OtherT.r0, r2 | OtherT.r1, r2 | OtherT.r2)
			);
		}

		// Static factory methods

		// Identity matrix
		static TMatrix3x3 Identity() noexcept
		{
			return TMatrix3x3(
				TVector<T>(1, 0, 0),
				TVector<T>(0, 1, 0),
				TVector<T>(0, 0, 1)
			);
		}

		// Zero matrix
		static TMatrix3x3 Zero() noexcept
		{
			return TMatrix3x3(static_cast<T>(0));
		}

		// Rotation matrix from axis and angle (radians)
		static TMatrix3x3 Rotation(const TVector<T>& Axis, T Angle) noexcept
		{
			T c = std::cos(Angle);
			T s = std::sin(Angle);
			T oneMinusC = static_cast<T>(1) - c;

			TVector<T> AxisSq = Axis * Axis;
			TVector<T> AxisScaled = Axis * oneMinusC;

			return TMatrix3x3(
				TVector<T>(
					c + AxisSq.X * oneMinusC,
					Axis.X * Axis.Y * oneMinusC - Axis.Z * s,
					Axis.X * Axis.Z * oneMinusC + Axis.Y * s
				),
				TVector<T>(
					Axis.Y * Axis.X * oneMinusC + Axis.Z * s,
					c + AxisSq.Y * oneMinusC,
					Axis.Y * Axis.Z * oneMinusC - Axis.X * s
				),
				TVector<T>(
					Axis.Z * Axis.X * oneMinusC - Axis.Y * s,
					Axis.Z * Axis.Y * oneMinusC + Axis.X * s,
					c + AxisSq.Z * oneMinusC
				)
			);
		}

		// Rotation around X axis
		static TMatrix3x3 RotationX(T Angle) noexcept
		{
			T c = std::cos(Angle);
			T s = std::sin(Angle);
			return TMatrix3x3(
				TVector<T>(1, 0,  0),
				TVector<T>(0, c, -s),
				TVector<T>(0, s,  c)
			);
		}

		// Rotation around Y axis
		static TMatrix3x3 RotationY(T Angle) noexcept
		{
			T c = std::cos(Angle);
			T s = std::sin(Angle);
			return TMatrix3x3(
				TVector<T>( c, 0, s),
				TVector<T>( 0, 1, 0),
				TVector<T>(-s, 0, c)
			);
		}

		// Rotation around Z axis
		static TMatrix3x3 RotationZ(T Angle) noexcept
		{
			T c = std::cos(Angle);
			T s = std::sin(Angle);
			return TMatrix3x3(
				TVector<T>(c, -s, 0),
				TVector<T>(s,  c, 0),
				TVector<T>(0,  0, 1)
			);
		}

		// Scaling matrix
		static TMatrix3x3 Scaling(const TVector<T>& Scale) noexcept
		{
			return TMatrix3x3(
				TVector<T>(Scale.X, 0,       0),
				TVector<T>(0,       Scale.Y, 0),
				TVector<T>(0,       0,       Scale.Z)
			);
		}

		// Uniform scaling (all components equal)
		static TMatrix3x3 Scaling(T Scale) noexcept
		{
			return Scaling(TVector<T>(Scale));
		}

		// Transpose
		TMatrix3x3 Transpose() const noexcept
		{
			return TMatrix3x3(
				TVector<T>(r0.X, r1.X, r2.X),
				TVector<T>(r0.Y, r1.Y, r2.Y),
				TVector<T>(r0.Z, r1.Z, r2.Z)
			);
		}

		// Determinant
		T Determinant() const noexcept
		{
			return r0.X * (r1.Y * r2.Z - r1.Z * r2.Y)
			     - r0.Y * (r1.X * r2.Z - r1.Z * r2.X)
			     + r0.Z * (r1.X * r2.Y - r1.Y * r2.X);
		}

		// Trace (sum of diagonal elements)
		T Trace() const noexcept
		{
			return r0.X + r1.Y + r2.Z;
		}

		// Inverse
		TMatrix3x3 Inverse() const noexcept
		{
			T det = Determinant();
			if (std::abs(det) < UBPA_UCOMMON_DELTA)
			{
				return Zero();
			}

			T invDet = static_cast<T>(1) / det;

			return TMatrix3x3(
				TVector<T>(
					(r1.Y * r2.Z - r1.Z * r2.Y) * invDet,
					(r0.Z * r2.Y - r0.Y * r2.Z) * invDet,
					(r0.Y * r1.Z - r0.Z * r1.Y) * invDet
				),
				TVector<T>(
					(r1.Z * r2.X - r1.X * r2.Z) * invDet,
					(r0.X * r2.Z - r0.Z * r2.X) * invDet,
					(r0.Z * r1.X - r0.X * r1.Z) * invDet
				),
				TVector<T>(
					(r1.X * r2.Y - r1.Y * r2.X) * invDet,
					(r0.Y * r2.X - r0.X * r2.Y) * invDet,
					(r0.X * r1.Y - r0.Y * r1.X) * invDet
				)
			);
		}
	};

	// 4x4 Matrix (Row-Major Order)
	// Row 0: r0
	// Row 1: r1
	// Row 2: r2
	// Row 3: r3
	template<typename T>
	struct TMatrix4x4
	{
		using value_type = T;

		// Constructors
		TMatrix4x4() noexcept {} // uninitialized
		TMatrix4x4(T Value) noexcept
			: r0(Value), r1(Value), r2(Value), r3(Value) {}
		TMatrix4x4(
			TVector4<T> Row0,
			TVector4<T> Row1,
			TVector4<T> Row2,
			TVector4<T> Row3) noexcept
			: r0(Row0), r1(Row1), r2(Row2), r3(Row3) {}
		TMatrix4x4(
			T In00, T In01, T In02, T In03,
			T In10, T In11, T In12, T In13,
			T In20, T In21, T In22, T In23,
			T In30, T In31, T In32, T In33) noexcept
			: r0(In00, In01, In02, In03)
			, r1(In10, In11, In12, In13)
			, r2(In20, In21, In22, In23)
			, r3(In30, In31, In32, In33) {}

		// Row-major storage using TVector4
		TVector4<T> r0; // Row 0
		TVector4<T> r1; // Row 1
		TVector4<T> r2; // Row 2
		TVector4<T> r3; // Row 3

		// Access operators
		TVector4<T>& operator[](uint32_t Index) noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 4);
			return (&r0)[Index];
		}

		const TVector4<T>& operator[](uint32_t Index) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 4);
			return (&r0)[Index];
		}

		T& operator()(uint32_t Row, uint32_t Col) noexcept
		{
			UBPA_UCOMMON_ASSERT(Row < 4 && Col < 4);
			return (&r0)[Row][Col];
		}

		const T& operator()(uint32_t Row, uint32_t Col) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Row < 4 && Col < 4);
			return (*this)[Row][Col];
		}

		T* GetData() noexcept { return r0.GetData(); }
		const T* GetData() const noexcept { return r0.GetData(); }

		// Matrix-Vector multiplication (M * V)
		// Treats V as a point (w=1)
		TVector<T> operator*(const TVector<T>& V) const noexcept
		{
			TVector4<T> V4(V.X, V.Y, V.Z, static_cast<T>(1));
			TVector4<T> Result = (*this) * V4;
			return TVector<T>(Result.X, Result.Y, Result.Z);
		}

		// Matrix-Vector multiplication (M * V)
		TVector4<T> operator*(const TVector4<T>& V) const noexcept
		{
			return TVector4<T>(
				r0 | V,
				r1 | V,
				r2 | V,
				r3 | V
			);
		}

		// Matrix-Vector multiplication (M * V for vectors, w=0)
		TVector<T> TransformVector(const TVector<T>& V) const noexcept
		{
			TVector4<T> V4(V.X, V.Y, V.Z, static_cast<T>(0));
			TVector4<T> Result = (*this) * V4;
			return TVector<T>(Result.X, Result.Y, Result.Z);
		}

		// Matrix-Matrix multiplication
		TMatrix4x4 operator*(const TMatrix4x4& Other) const noexcept
		{
			TMatrix4x4 OtherT = Other.Transpose();
			return TMatrix4x4(
				TVector4<T>(r0 | OtherT.r0, r0 | OtherT.r1, r0 | OtherT.r2, r0 | OtherT.r3),
				TVector4<T>(r1 | OtherT.r0, r1 | OtherT.r1, r1 | OtherT.r2, r1 | OtherT.r3),
				TVector4<T>(r2 | OtherT.r0, r2 | OtherT.r1, r2 | OtherT.r2, r2 | OtherT.r3),
				TVector4<T>(r3 | OtherT.r0, r3 | OtherT.r1, r3 | OtherT.r2, r3 | OtherT.r3)
			);
		}

		// Static factory methods

		// Identity matrix
		static TMatrix4x4 Identity() noexcept
		{
			return TMatrix4x4(
				TVector4<T>(1, 0, 0, 0),
				TVector4<T>(0, 1, 0, 0),
				TVector4<T>(0, 0, 1, 0),
				TVector4<T>(0, 0, 0, 1)
			);
		}

		// Zero matrix
		static TMatrix4x4 Zero() noexcept
		{
			return TMatrix4x4(static_cast<T>(0));
		}

		// Rotation matrix from axis and angle (radians)
		static TMatrix4x4 Rotation(const TVector<T>& Axis, T Angle) noexcept
		{
			TMatrix4x4 Result = Identity();
			Result.SetRotation(Axis, Angle);
			return Result;
		}

		// Rotation around X axis
		static TMatrix4x4 RotationX(T Angle) noexcept
		{
			T c = std::cos(Angle);
			T s = std::sin(Angle);
			return TMatrix4x4(
				TVector4<T>(1, 0,  0, 0),
				TVector4<T>(0, c, -s, 0),
				TVector4<T>(0, s,  c, 0),
				TVector4<T>(0, 0,  0, 1)
			);
		}

		// Rotation around Y axis
		static TMatrix4x4 RotationY(T Angle) noexcept
		{
			T c = std::cos(Angle);
			T s = std::sin(Angle);
			return TMatrix4x4(
				TVector4<T>( c, 0, s, 0),
				TVector4<T>( 0, 1, 0, 0),
				TVector4<T>(-s, 0, c, 0),
				TVector4<T>( 0, 0, 0, 1)
			);
		}

		// Rotation around Z axis
		static TMatrix4x4 RotationZ(T Angle) noexcept
		{
			T c = std::cos(Angle);
			T s = std::sin(Angle);
			return TMatrix4x4(
				TVector4<T>(c, -s, 0, 0),
				TVector4<T>(s,  c, 0, 0),
				TVector4<T>(0,  0, 1, 0),
				TVector4<T>(0,  0, 0, 1)
			);
		}

		// Set rotation from axis and angle (radians)
		void SetRotation(const TVector<T>& Axis, T Angle) noexcept
		{
			T c = std::cos(Angle);
			T s = std::sin(Angle);
			T oneMinusC = static_cast<T>(1) - c;

			T x = Axis.X;
			T y = Axis.Y;
			T z = Axis.Z;

			r0 = TVector4<T>(
				c + x * x * oneMinusC,
				x * y * oneMinusC - z * s,
				x * z * oneMinusC + y * s,
				static_cast<T>(0)
			);

			r1 = TVector4<T>(
				y * x * oneMinusC + z * s,
				c + y * y * oneMinusC,
				y * z * oneMinusC - x * s,
				static_cast<T>(0)
			);

			r2 = TVector4<T>(
				z * x * oneMinusC - y * s,
				z * y * oneMinusC + x * s,
				c + z * z * oneMinusC,
				static_cast<T>(0)
			);

			r3 = TVector4<T>(0, 0, 0, 1);
		}

		// Translation matrix
		static TMatrix4x4 Translation(const TVector<T>& Val) noexcept
		{
			return TMatrix4x4(
				TVector4<T>(1, 0, 0, Val.X),
				TVector4<T>(0, 1, 0, Val.Y),
				TVector4<T>(0, 0, 1, Val.Z),
				TVector4<T>(0, 0, 0, 1)
			);
		}

		// Scaling matrix
		static TMatrix4x4 Scaling(const TVector<T>& Scale) noexcept
		{
			return TMatrix4x4(
				TVector4<T>(Scale.X, 0,       0,       0),
				TVector4<T>(0,       Scale.Y, 0,       0),
				TVector4<T>(0,       0,       Scale.Z, 0),
				TVector4<T>(0,       0,       0,       1)
			);
		}

		// Uniform scaling (all components equal)
		static TMatrix4x4 Scaling(T Scale) noexcept
		{
			return Scaling(TVector<T>(Scale));
		}

		// TRS (Translate, Rotate, Scale) composition
		static TMatrix4x4 TRS(const TVector<T>& Translation,
		                      const TVector<T>& Axis, T Angle,
		                      const TVector<T>& Scale) noexcept
		{
			TMatrix4x4 Result;
			Result.SetTRS(Translation, Axis, Angle, Scale);
			return Result;
		}

		void SetTRS(const TVector<T>& Trans,
		            const TVector<T>& Axis, T Angle,
		            const TVector<T>& Scale) noexcept
		{
			T c = std::cos(Angle);
			T s = std::sin(Angle);
			T oneMinusC = static_cast<T>(1) - c;

			T x = Axis.X;
			T y = Axis.Y;
			T z = Axis.Z;

			// Apply scale to rotation matrix
			r0 = TVector4<T>(
				(c + x * x * oneMinusC) * Scale.X,
				(x * y * oneMinusC - z * s) * Scale.Y,
				(x * z * oneMinusC + y * s) * Scale.Z,
				Trans.X
			);

			r1 = TVector4<T>(
				(y * x * oneMinusC + z * s) * Scale.X,
				(c + y * y * oneMinusC) * Scale.Y,
				(y * z * oneMinusC - x * s) * Scale.Z,
				Trans.Y
			);

			r2 = TVector4<T>(
				(z * x * oneMinusC - y * s) * Scale.X,
				(z * y * oneMinusC + x * s) * Scale.Y,
				(c + z * z * oneMinusC) * Scale.Z,
				Trans.Z
			);

			r3 = TVector4<T>(0, 0, 0, 1);
		}

		// Transpose
		TMatrix4x4 Transpose() const noexcept
		{
			return TMatrix4x4(
				TVector4<T>(r0.X, r1.X, r2.X, r3.X),
				TVector4<T>(r0.Y, r1.Y, r2.Y, r3.Y),
				TVector4<T>(r0.Z, r1.Z, r2.Z, r3.Z),
				TVector4<T>(r0.W, r1.W, r2.W, r3.W)
			);
		}

		// Determinant
		T Determinant() const noexcept
		{
			// Compute using cofactor expansion along last row
			T det3x3 = TMatrix3x3<T>(
				TVector<T>(r0.X, r0.Y, r0.Z),
				TVector<T>(r1.X, r1.Y, r1.Z),
				TVector<T>(r2.X, r2.Y, r2.Z)
			).Determinant();

			return r3.W * det3x3;
		}

		// Trace (sum of diagonal elements)
		T Trace() const noexcept
		{
			return r0.X + r1.Y + r2.Z + r3.W;
		}

		// Inverse (optimized for affine transforms)
		TMatrix4x4 Inverse() const noexcept
		{
			TMatrix3x3<T> Linear(
				TVector<T>(r0.X, r0.Y, r0.Z),
				TVector<T>(r1.X, r1.Y, r1.Z),
				TVector<T>(r2.X, r2.Y, r2.Z)
			);

			TMatrix3x3<T> LinearInv = Linear.Inverse();

			if (LinearInv == TMatrix3x3<T>::Zero())
			{
				return Zero();
			}

			// Translate inverse: need to compute -LinearInv * t where t = [r0.W, r1.W, r2.W]
			// result translation components:
			// -trans_inv.x = LinearInv.r0 ⋅ t
			// -trans_inv.y = LinearInv.r1 ⋅ t
			// -trans_inv.z = LinearInv.r2 ⋅ t

			TVector<T> t(r0.W, r1.W, r2.W);
			T tX = -(LinearInv.r0 | t);
			T tY = -(LinearInv.r1 | t);
			T tZ = -(LinearInv.r2 | t);

			return TMatrix4x4(
				TVector4<T>(LinearInv.r0.X, LinearInv.r0.Y, LinearInv.r0.Z, tX),
				TVector4<T>(LinearInv.r1.X, LinearInv.r1.Y, LinearInv.r1.Z, tY),
				TVector4<T>(LinearInv.r2.X, LinearInv.r2.Y, LinearInv.r2.Z, tZ),
				TVector4<T>(0, 0, 0, r3.W)
			);
		}

		// Extract rotation as 3x3 matrix
		TMatrix3x3<T> GetRotation3x3() const noexcept
		{
			return TMatrix3x3<T>(
				TVector<T>(r0.X, r0.Y, r0.Z),
				TVector<T>(r1.X, r1.Y, r1.Z),
				TVector<T>(r2.X, r2.Y, r2.Z)
			);
		}

		// Extract translation
		TVector<T> GetTranslation() const noexcept
		{
			return TVector<T>(r0.W, r1.W, r2.W);
		}

		// Extract scale (diagonal elements after removing rotation)
		TVector<T> GetScale() const noexcept
		{
			return TVector<T>(
				TVector<T>(r0.X, r0.Y, r0.Z).GetLength(),
				TVector<T>(r1.X, r1.Y, r1.Z).GetLength(),
				TVector<T>(r2.X, r2.Y, r2.Z).GetLength()
			);
		}
	};

	// Matrix-Vector comparison for zero matrix
	template<typename T>
	bool operator==(const TMatrix3x3<T>& A, const TMatrix3x3<T>& B) noexcept
	{
		return A.r0 == B.r0 && A.r1 == B.r1 && A.r2 == B.r2;
	}

	template<typename T>
	bool operator!=(const TMatrix3x3<T>& A, const TMatrix3x3<T>& B) noexcept
	{
		return !(A == B);
	}

	template<typename T>
	bool operator==(const TMatrix4x4<T>& A, const TMatrix4x4<T>& B) noexcept
	{
		return A.r0 == B.r0 && A.r1 == B.r1 && A.r2 == B.r2 && A.r3 == B.r3;
	}

	template<typename T>
	bool operator!=(const TMatrix4x4<T>& A, const TMatrix4x4<T>& B) noexcept
	{
		return !(A == B);
	}

	// Type aliases
	using FMatrix3x3f = TMatrix3x3<float>;
	using FMatrix3x3d = TMatrix3x3<double>;
	using FMatrix4x4f = TMatrix4x4<float>;
	using FMatrix4x4d = TMatrix4x4<double>;
}

UBPA_UCOMMON_MATRIX_TO_NAMESPACE(UCommonTest)
