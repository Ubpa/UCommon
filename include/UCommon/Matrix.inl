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

#include "Matrix.h"

// ============================================================================
// TMatrix3x3 out-of-class implementations
// ============================================================================

template<typename T>
inline UCommon::TMatrix3x3<T> UCommon::TMatrix3x3<T>::operator*(const TMatrix3x3& Other) const noexcept
{
	TMatrix3x3 OtherT = Other.Transpose();
	return TMatrix3x3(
		TVector<T>(Rows[0] | OtherT.Rows[0], Rows[0] | OtherT.Rows[1], Rows[0] | OtherT.Rows[2]),
		TVector<T>(Rows[1] | OtherT.Rows[0], Rows[1] | OtherT.Rows[1], Rows[1] | OtherT.Rows[2]),
		TVector<T>(Rows[2] | OtherT.Rows[0], Rows[2] | OtherT.Rows[1], Rows[2] | OtherT.Rows[2])
	);
}

template<typename T>
inline UCommon::TMatrix3x3<T> UCommon::TMatrix3x3<T>::Rotation(const TVector<T>& Axis, T Angle) noexcept
{
	T c = std::cos(Angle);
	T s = std::sin(Angle);
	T oneMinusC = static_cast<T>(1) - c;

	TVector<T> AxisSq = Axis * Axis;

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

template<typename T>
inline UCommon::TMatrix3x3<T> UCommon::TMatrix3x3<T>::RotationX(T Angle) noexcept
{
	T c = std::cos(Angle);
	T s = std::sin(Angle);
	return TMatrix3x3(
		TVector<T>(1, 0,  0),
		TVector<T>(0, c, -s),
		TVector<T>(0, s,  c)
	);
}

template<typename T>
inline UCommon::TMatrix3x3<T> UCommon::TMatrix3x3<T>::RotationY(T Angle) noexcept
{
	T c = std::cos(Angle);
	T s = std::sin(Angle);
	return TMatrix3x3(
		TVector<T>( c, 0, s),
		TVector<T>( 0, 1, 0),
		TVector<T>(-s, 0, c)
	);
}

template<typename T>
inline UCommon::TMatrix3x3<T> UCommon::TMatrix3x3<T>::RotationZ(T Angle) noexcept
{
	T c = std::cos(Angle);
	T s = std::sin(Angle);
	return TMatrix3x3(
		TVector<T>(c, -s, 0),
		TVector<T>(s,  c, 0),
		TVector<T>(0,  0, 1)
	);
}

template<typename T>
inline UCommon::TMatrix3x3<T> UCommon::TMatrix3x3<T>::Inverse() const noexcept
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

// ============================================================================
// TMatrix4x4 out-of-class implementations
// ============================================================================

template<typename T>
inline UCommon::TMatrix4x4<T> UCommon::TMatrix4x4<T>::operator*(const TMatrix4x4& Other) const noexcept
{
	TMatrix4x4 OtherT = Other.Transpose();
	return TMatrix4x4(
		TVector4<T>(Rows[0] | OtherT.Rows[0], Rows[0] | OtherT.Rows[1], Rows[0] | OtherT.Rows[2], Rows[0] | OtherT.Rows[3]),
		TVector4<T>(Rows[1] | OtherT.Rows[0], Rows[1] | OtherT.Rows[1], Rows[1] | OtherT.Rows[2], Rows[1] | OtherT.Rows[3]),
		TVector4<T>(Rows[2] | OtherT.Rows[0], Rows[2] | OtherT.Rows[1], Rows[2] | OtherT.Rows[2], Rows[2] | OtherT.Rows[3]),
		TVector4<T>(Rows[3] | OtherT.Rows[0], Rows[3] | OtherT.Rows[1], Rows[3] | OtherT.Rows[2], Rows[3] | OtherT.Rows[3])
	);
}

template<typename T>
inline UCommon::TMatrix4x4<T> UCommon::TMatrix4x4<T>::RotationX(T Angle) noexcept
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

template<typename T>
inline UCommon::TMatrix4x4<T> UCommon::TMatrix4x4<T>::RotationY(T Angle) noexcept
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

template<typename T>
inline UCommon::TMatrix4x4<T> UCommon::TMatrix4x4<T>::RotationZ(T Angle) noexcept
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

template<typename T>
inline void UCommon::TMatrix4x4<T>::SetRotation(const TVector<T>& Axis, T Angle) noexcept
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

template<typename T>
inline void UCommon::TMatrix4x4<T>::SetTRS(const TVector<T>& Trans,
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

template<typename T>
inline T UCommon::TMatrix4x4<T>::Determinant() const noexcept
{
	// Optimized for affine transforms (bottom row = [0,0,0,1]):
	// det(M4x4) = Rows[3].W * det(upper-left 3x3)
	T det3x3 = TMatrix3x3<T>(
		TVector<T>(Rows[0].X, Rows[0].Y, Rows[0].Z),
		TVector<T>(Rows[1].X, Rows[1].Y, Rows[1].Z),
		TVector<T>(Rows[2].X, Rows[2].Y, Rows[2].Z)
	).Determinant();

	return Rows[3].W * det3x3;
}

template<typename T>
inline UCommon::TMatrix4x4<T> UCommon::TMatrix4x4<T>::Inverse() const noexcept
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

	// Translate inverse: -LinearInv * t, where t = [Rows[0].W, Rows[1].W, Rows[2].W]
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
