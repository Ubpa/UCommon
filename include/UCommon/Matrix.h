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
	// Row 0: Rows[0]
	// Row 1: Rows[1]
	// Row 2: Rows[2]
	template<typename T>
	struct TMatrix3x3
	{
		using value_type = T;

		// Constructors
		TMatrix3x3() noexcept {} // uninitialized
		TMatrix3x3(T Value) noexcept
			: Rows{TVector<T>(Value), TVector<T>(Value), TVector<T>(Value)} {}
		TMatrix3x3(
			TVector<T> Row0,
			TVector<T> Row1,
			TVector<T> Row2) noexcept
			: Rows{Row0, Row1, Row2} {}
		TMatrix3x3(
			T In00, T In01, T In02,
			T In10, T In11, T In12,
			T In20, T In21, T In22) noexcept
			: Rows{
				TVector<T>(In00, In01, In02),
				TVector<T>(In10, In11, In12),
				TVector<T>(In20, In21, In22)
			} {}

		// Row-major storage using TVector
		TVector<T> Rows[3]; // Row 0, Row 1, Row 2

		// Access operators
		TVector<T>& operator[](uint32_t Index) noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 3);
			return Rows[Index];
		}

		const TVector<T>& operator[](uint32_t Index) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 3);
			return Rows[Index];
		}

		T& operator()(uint32_t Row, uint32_t Col) noexcept
		{
			UBPA_UCOMMON_ASSERT(Row < 3 && Col < 3);
			return Rows[Row][Col];
		}

		const T& operator()(uint32_t Row, uint32_t Col) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Row < 3 && Col < 3);
			return Rows[Row][Col];
		}

		T* GetData() noexcept { return Rows[0].GetData(); }
		const T* GetData() const noexcept { return Rows[0].GetData(); }

		// Matrix-Vector multiplication (M * V)
		// Matrix is left, vector is right (column vector)
		TVector<T> operator*(const TVector<T>& V) const noexcept
		{
			return TVector<T>(
				Rows[0] | V, // dot product with each row
				Rows[1] | V,
				Rows[2] | V
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
				TVector<T>(Rows[0] | OtherT.Rows[0], Rows[0] | OtherT.Rows[1], Rows[0] | OtherT.Rows[2]),
				TVector<T>(Rows[1] | OtherT.Rows[0], Rows[1] | OtherT.Rows[1], Rows[1] | OtherT.Rows[2]),
				TVector<T>(Rows[2] | OtherT.Rows[0], Rows[2] | OtherT.Rows[1], Rows[2] | OtherT.Rows[2])
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
				TVector<T>(Rows[0].X, Rows[1].X, Rows[2].X),
				TVector<T>(Rows[0].Y, Rows[1].Y, Rows[2].Y),
				TVector<T>(Rows[0].Z, Rows[1].Z, Rows[2].Z)
			);
		}

		// Determinant
		T Determinant() const noexcept
		{
			return Rows[0].X * (Rows[1].Y * Rows[2].Z - Rows[1].Z * Rows[2].Y)
			     - Rows[0].Y * (Rows[1].X * Rows[2].Z - Rows[1].Z * Rows[2].X)
			     + Rows[0].Z * (Rows[1].X * Rows[2].Y - Rows[1].Y * Rows[2].X);
		}

		// Trace (sum of diagonal elements)
		T Trace() const noexcept
		{
			return Rows[0].X + Rows[1].Y + Rows[2].Z;
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
					(Rows[1].Y * Rows[2].Z - Rows[1].Z * Rows[2].Y) * invDet,
					(Rows[0].Z * Rows[2].Y - Rows[0].Y * Rows[2].Z) * invDet,
					(Rows[0].Y * Rows[1].Z - Rows[0].Z * Rows[1].Y) * invDet
				),
				TVector<T>(
					(Rows[1].Z * Rows[2].X - Rows[1].X * Rows[2].Z) * invDet,
					(Rows[0].X * Rows[2].Z - Rows[0].Z * Rows[2].X) * invDet,
					(Rows[0].Z * Rows[1].X - Rows[0].X * Rows[1].Z) * invDet
				),
				TVector<T>(
					(Rows[1].X * Rows[2].Y - Rows[1].Y * Rows[2].X) * invDet,
					(Rows[0].Y * Rows[2].X - Rows[0].X * Rows[2].Y) * invDet,
					(Rows[0].X * Rows[1].Y - Rows[0].Y * Rows[1].X) * invDet
				)
			);
		}
	};

	// 4x4 Matrix (Row-Major Order)
	// Row 0: Rows[0]
	// Row 1: Rows[1]
	// Row 2: Rows[2]
	// Row 3: Rows[3]
	template<typename T>
	struct TMatrix4x4
	{
		using value_type = T;

		// Constructors
		TMatrix4x4() noexcept {} // uninitialized
		TMatrix4x4(T Value) noexcept
			: Rows{TVector4<T>(Value), TVector4<T>(Value), TVector4<T>(Value), TVector4<T>(Value)} {}
		TMatrix4x4(
			TVector4<T> Row0,
			TVector4<T> Row1,
			TVector4<T> Row2,
			TVector4<T> Row3) noexcept
			: Rows{Row0, Row1, Row2, Row3} {}
		TMatrix4x4(
			T In00, T In01, T In02, T In03,
			T In10, T In11, T In12, T In13,
			T In20, T In21, T In22, T In23,
			T In30, T In31, T In32, T In33) noexcept
			: Rows{
				TVector4<T>(In00, In01, In02, In03),
				TVector4<T>(In10, In11, In12, In13),
				TVector4<T>(In20, In21, In22, In23),
				TVector4<T>(In30, In31, In32, In33)
			} {}

		// Row-major storage using TVector4
		TVector4<T> Rows[4]; // Row 0, Row 1, Row 2, Row 3

		// Access operators
		TVector4<T>& operator[](uint32_t Index) noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 4);
			return Rows[Index];
		}

		const TVector4<T>& operator[](uint32_t Index) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < 4);
			return Rows[Index];
		}

		T& operator()(uint32_t Row, uint32_t Col) noexcept
		{
			UBPA_UCOMMON_ASSERT(Row < 4 && Col < 4);
			return Rows[Row][Col];
		}

		const T& operator()(uint32_t Row, uint32_t Col) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Row < 4 && Col < 4);
			return Rows[Row][Col];
		}

		T* GetData() noexcept { return Rows[0].GetData(); }
		const T* GetData() const noexcept { return Rows[0].GetData(); }

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
				Rows[0] | V,
				Rows[1] | V,
				Rows[2] | V,
				Rows[3] | V
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
				TVector4<T>(Rows[0] | OtherT.Rows[0], Rows[0] | OtherT.Rows[1], Rows[0] | OtherT.Rows[2], Rows[0] | OtherT.Rows[3]),
				TVector4<T>(Rows[1] | OtherT.Rows[0], Rows[1] | OtherT.Rows[1], Rows[1] | OtherT.Rows[2], Rows[1] | OtherT.Rows[3]),
				TVector4<T>(Rows[2] | OtherT.Rows[0], Rows[2] | OtherT.Rows[1], Rows[2] | OtherT.Rows[2], Rows[2] | OtherT.Rows[3]),
				TVector4<T>(Rows[3] | OtherT.Rows[0], Rows[3] | OtherT.Rows[1], Rows[3] | OtherT.Rows[2], Rows[3] | OtherT.Rows[3])
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

			Rows[0] = TVector4<T>(
				c + x * x * oneMinusC,
				x * y * oneMinusC - z * s,
				x * z * oneMinusC + y * s,
				static_cast<T>(0)
			);

			Rows[1] = TVector4<T>(
				y * x * oneMinusC + z * s,
				c + y * y * oneMinusC,
				y * z * oneMinusC - x * s,
				static_cast<T>(0)
			);

			Rows[2] = TVector4<T>(
				z * x * oneMinusC - y * s,
				z * y * oneMinusC + x * s,
				c + z * z * oneMinusC,
				static_cast<T>(0)
			);

			Rows[3] = TVector4<T>(0, 0, 0, 1);
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
			Rows[0] = TVector4<T>(
				(c + x * x * oneMinusC) * Scale.X,
				(x * y * oneMinusC - z * s) * Scale.Y,
				(x * z * oneMinusC + y * s) * Scale.Z,
				Trans.X
			);

			Rows[1] = TVector4<T>(
				(y * x * oneMinusC + z * s) * Scale.X,
				(c + y * y * oneMinusC) * Scale.Y,
				(y * z * oneMinusC - x * s) * Scale.Z,
				Trans.Y
			);

			Rows[2] = TVector4<T>(
				(z * x * oneMinusC - y * s) * Scale.X,
				(z * y * oneMinusC + x * s) * Scale.Y,
				(c + z * z * oneMinusC) * Scale.Z,
				Trans.Z
			);

			Rows[3] = TVector4<T>(0, 0, 0, 1);
		}

		// Transpose
		TMatrix4x4 Transpose() const noexcept
		{
			return TMatrix4x4(
				TVector4<T>(Rows[0].X, Rows[1].X, Rows[2].X, Rows[3].X),
				TVector4<T>(Rows[0].Y, Rows[1].Y, Rows[2].Y, Rows[3].Y),
				TVector4<T>(Rows[0].Z, Rows[1].Z, Rows[2].Z, Rows[3].Z),
				TVector4<T>(Rows[0].W, Rows[1].W, Rows[2].W, Rows[3].W)
			);
		}

		// Determinant
		T Determinant() const noexcept
		{
			// Compute using cofactor expansion along last row
			T det3x3 = TMatrix3x3<T>(
				TVector<T>(Rows[0].X, Rows[0].Y, Rows[0].Z),
				TVector<T>(Rows[1].X, Rows[1].Y, Rows[1].Z),
				TVector<T>(Rows[2].X, Rows[2].Y, Rows[2].Z)
			).Determinant();

			return Rows[3].W * det3x3;
		}

		// Trace (sum of diagonal elements)
		T Trace() const noexcept
		{
			return Rows[0].X + Rows[1].Y + Rows[2].Z + Rows[3].W;
		}

		// Inverse (optimized for affine transforms)
		TMatrix4x4 Inverse() const noexcept
		{
			TMatrix3x3<T> Linear(
				TVector<T>(Rows[0].X, Rows[0].Y, Rows[0].Z),
				TVector<T>(Rows[1].X, Rows[1].Y, Rows[1].Z),
				TVector<T>(Rows[2].X, Rows[2].Y, Rows[2].Z)
			);

			TMatrix3x3<T> LinearInv = Linear.Inverse();

			if (LinearInv == TMatrix3x3<T>::Zero())
			{
				return Zero();
			}

			// Translate inverse: need to compute -LinearInv * t where t = [Rows[0].W, Rows[1].W, Rows[2].W]
			// result translation components:
			// -trans_inv.x = LinearInv.Rows[0] ⋅ t
			// -trans_inv.y = LinearInv.Rows[1] ⋅ t
			// -trans_inv.z = LinearInv.Rows[2] ⋅ t

			TVector<T> t(Rows[0].W, Rows[1].W, Rows[2].W);
			T tX = -(LinearInv.Rows[0] | t);
			T tY = -(LinearInv.Rows[1] | t);
			T tZ = -(LinearInv.Rows[2] | t);

			return TMatrix4x4(
				TVector4<T>(LinearInv.Rows[0].X, LinearInv.Rows[0].Y, LinearInv.Rows[0].Z, tX),
				TVector4<T>(LinearInv.Rows[1].X, LinearInv.Rows[1].Y, LinearInv.Rows[1].Z, tY),
				TVector4<T>(LinearInv.Rows[2].X, LinearInv.Rows[2].Y, LinearInv.Rows[2].Z, tZ),
				TVector4<T>(0, 0, 0, Rows[3].W)
			);
		}

		// Extract rotation as 3x3 matrix
		TMatrix3x3<T> GetRotation3x3() const noexcept
		{
			return TMatrix3x3<T>(
				TVector<T>(Rows[0].X, Rows[0].Y, Rows[0].Z),
				TVector<T>(Rows[1].X, Rows[1].Y, Rows[1].Z),
				TVector<T>(Rows[2].X, Rows[2].Y, Rows[2].Z)
			);
		}

		// Extract translation
		TVector<T> GetTranslation() const noexcept
		{
			return TVector<T>(Rows[0].W, Rows[1].W, Rows[2].W);
		}

		// Extract scale (diagonal elements after removing rotation)
		TVector<T> GetScale() const noexcept
		{
			return TVector<T>(
				TVector<T>(Rows[0].X, Rows[0].Y, Rows[0].Z).GetLength(),
				TVector<T>(Rows[1].X, Rows[1].Y, Rows[1].Z).GetLength(),
				TVector<T>(Rows[2].X, Rows[2].Y, Rows[2].Z).GetLength()
			);
		}
	};

	// Matrix-Vector comparison for zero matrix
	template<typename T>
	bool operator==(const TMatrix3x3<T>& A, const TMatrix3x3<T>& B) noexcept
	{
		return A.Rows[0] == B.Rows[0] && A.Rows[1] == B.Rows[1] && A.Rows[2] == B.Rows[2];
	}

	template<typename T>
	bool operator!=(const TMatrix3x3<T>& A, const TMatrix3x3<T>& B) noexcept
	{
		return !(A == B);
	}

	template<typename T>
	bool operator==(const TMatrix4x4<T>& A, const TMatrix4x4<T>& B) noexcept
	{
		return A.Rows[0] == B.Rows[0] && A.Rows[1] == B.Rows[1] && A.Rows[2] == B.Rows[2] && A.Rows[3] == B.Rows[3];
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
