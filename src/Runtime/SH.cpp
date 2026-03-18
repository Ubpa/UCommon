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

#include <UCommon/SH.h>

float UCommon::HallucinateZH(const FSHVector2& SHVector2, float t, FVector4f& Buffer, float Delta)
{
	UBPA_UCOMMON_ASSERT(t >= 0.f && t <= 1.f);

	Buffer = FVector4f(0.f);

	const float L0 = SHVector2.V[0];

	if (L0 <= Delta)
	{
		return 0.f;
	}

	Buffer[3] = L0 * 0.28209479f;

	const FVector3f LinearVector = SHVector2.GetLinearVector();
	float L1 = LinearVector.GetLength();
	if (L1 <= Delta)
	{
		return 0.f;
	}

	const float MaxL1 = 3.f / 2.f * (1.f - t) * 0.975f * L0;
	float Scale = 1.f;
	if (L1 >= MaxL1)
	{
		Scale = MaxL1 / L1;
		L1 = MaxL1;
	}
	constexpr float Factor1 = 2.f / 3.f * 0.48860252f;
	const float p = L1 / L0;
	const float L2 = (0.6f * p * p + 0.08f * p) * L0;

	constexpr float Factor2 = 1.f / 4.f * 0.31539157f;

	for(uint64_t i = 0; i < 3; i++)
	{
		Buffer[i] = LinearVector[i] * Scale * Factor1;
	}

	Buffer[3] -= L2 * Factor2;

	return Factor2 * L2 * 3.f / (L1 * L1 / (3.f * Pi));
}

void UCommon::ComputeSHBand2RotateMatrix(float* SHBand2RotateMatrix, const FMatrix3x3f& RotateMatrix)
{
	UBPA_UCOMMON_ASSERT(SHBand2RotateMatrix);
	// Reference: http://filmicworlds.com/blog/simple-and-fast-spherical-harmonic-rotation/
	//
	// Algorithm overview:
	//   Choose 3 linearly independent sample directions w0..w2 (axis-aligned for simplicity).
	//   Build matrix Y  : Y[i, j]   = SH_i(w_j)       -- SH basis values at sample points
	//   Build matrix Y_R: Y_R[i, j] = SH_i(R * w_j)   -- SH basis values at rotated sample points
	//   The SH rotation matrix M satisfies: Y_R = M * Y  =>  M = Y_R * Y^{-1}
	//
	//   Element form: M[row, col] = sum_k  Y_R[row, k] * invY[k, col]
	//                             = sum_k  Yrws[k][row] * invY[k, col]
	//   where Yrws[k][row] = SH_row(R*w_k)  (Y_R transposed in first index)
	//
	// RotateMatrix is row-major 3x3: RotateMatrix.Rows[i][j] = R_{i,j}
	// Applying R to a column vector: (R*w)[i] = sum_j R_{i,j} * w[j]
	// The j-th column of R is R * e_j, so:
	//   Rws[j] = R * w_j  (extract j-th column)
	//
	// Sample points (axis-aligned):
	//   w0 = (1,0,0),  w1 = (0,1,0),  w2 = (0,0,1)
	//
	// SH<1,m>(x,y,z):  m=-1: -K*y,  m=0: K*z,  m=1: -K*x   (K = 0.48860252)
	//
	// Y matrix (rows = SH index, cols = sample point):
	//   Y = [  0  -K   0 ]    (m=-1)
	//       [  0   0   K ]    (m= 0)
	//       [ -K   0   0 ]    (m=+1)
	//
	// invY = Y^{-1}, k = 1/K = 2.0466533:
	//   invY = [  0  0 -k ]    (row 0)
	//          [ -k  0  0 ]    (row 1)
	//          [  0  k  0 ]    (row 2)
	//
	// Expanding M[row, col] = sum_k Yrws[k][row] * invY[k, col]
	// using invY columns (only nonzero entries per column of invY):
	//   invY col 0: k=1 -> -k        =>  M[row, 0] = Yrws[1][row] * (-k)
	//   invY col 1: k=2 -> +k        =>  M[row, 1] = Yrws[2][row] * (+k)
	//   invY col 2: k=0 -> -k        =>  M[row, 2] = Yrws[0][row] * (-k)
	constexpr float k = 2.0466533f; // k = 1 / SHK<1,±1> = 1 / 0.48860252
	const FVector3f Rws[3] =
	{
		// R * w0 = R * (1,0,0) = first column of R
		{ RotateMatrix.Rows[0][0], RotateMatrix.Rows[1][0], RotateMatrix.Rows[2][0] },
		// R * w1 = R * (0,1,0) = second column of R
		{ RotateMatrix.Rows[0][1], RotateMatrix.Rows[1][1], RotateMatrix.Rows[2][1] },
		// R * w2 = R * (0,0,1) = third column of R
		{ RotateMatrix.Rows[0][2], RotateMatrix.Rows[1][2], RotateMatrix.Rows[2][2] },
	};
	// Yrws[sample][sh] = SH_sh(R * w_sample)
	float Yrws[3][3];
	for (uint64_t Index = 0; Index < 3; Index++)
	{
		Yrws[Index][0] = SH<1, -1>(Rws[Index]);
		Yrws[Index][1] = SH<1,  0>(Rws[Index]);
		Yrws[Index][2] = SH<1,  1>(Rws[Index]);
	}
	// M = Y_R * invY, iterate over output column (SH input index)
	// col 0: invY col0 nonzero at k=1 -> -k  =>  M[row, 0] = -k * Yrws[1][row]
	// col 1: invY col1 nonzero at k=2 -> +k  =>  M[row, 1] = +k * Yrws[2][row]
	// col 2: invY col2 nonzero at k=0 -> -k  =>  M[row, 2] = -k * Yrws[0][row]
	for (uint64_t RowIndex = 0; RowIndex < 3; RowIndex++)
	{
		SHBand2RotateMatrix[RowIndex * 3 + 0] = -k * Yrws[1][RowIndex];
		SHBand2RotateMatrix[RowIndex * 3 + 1] =  k * Yrws[2][RowIndex];
		SHBand2RotateMatrix[RowIndex * 3 + 2] = -k * Yrws[0][RowIndex];
	}
}

void UCommon::ComputeSHBand3RotateMatrix(float* SHBand3RotateMatrix, const FMatrix3x3f& RotateMatrix)
{
	UBPA_UCOMMON_ASSERT(SHBand3RotateMatrix);
	// Reference: http://filmicworlds.com/blog/simple-and-fast-spherical-harmonic-rotation/
	//
	// Algorithm: same as Band2 (M = Y_R * invY), but for l=2 (5-dimensional).
	// 5 linearly independent sample directions are required:
	//   w0 = (1, 0, 0)
	//   w1 = (0, 0, 1)
	//   w2 = (k, k, 0)  where k = 1/sqrt(2)
	//   w3 = (k, 0, k)
	//   w4 = (0, k, k)
	//
	// RotateMatrix is row-major 3x3: RotateMatrix.Rows[i][j] = R_{i,j}
	// Applying R to a column vector: (R*w)[i] = sum_j R_{i,j} * w[j]
	// The j-th column of R gives R * e_j, so rotated sample directions are:
	//   Rws[0] = R * w0  (first column of R)
	//   Rws[1] = R * w1  (third column of R)
	//   Rws[2] = R * (k, k, 0) = k * (R[:,0] + R[:,1])
	//   Rws[3] = R * (k, 0, k) = k * (R[:,0] + R[:,2])
	//   Rws[4] = R * (0, k, k) = k * (R[:,1] + R[:,2])
	//
	// SH<2,m>(x,y,z):
	//   m=-2: a * x*y    (a = 1.0925485)
	//   m=-1: -a * y*z
	//   m= 0: c2 * (3z²-1)  (c2 = 0.31539157)
	//   m= 1: -a * x*z
	//   m= 2: b * (x²-y²)   (b = 0.54627424)
	//
	// invY (5x5) expressed in terms of k0, k1, k2  (rows of invY):
	//   invY row 0: [  0  -k0   0   k0   k1 ]
	//   invY row 1: [ k0    0  k2   k0   k0 ]
	//   invY row 2: [ k1    0   0    0    0 ]
	//   invY row 3: [  0    0   0  -k1    0 ]
	//   invY row 4: [  0  -k1   0    0    0 ]
	//
	// M = Y_R * invY, element form: M[row, col] = sum_k Yrws[k][row] * invY[k, col]
	// Expanding by invY columns (nonzero entries per column):
	//   col 0: k=1 -> +k0, k=2 -> +k1   =>  M[row,0] = k0*Yrws[1][row] + k1*Yrws[2][row]
	//   col 1: k=0 -> -k0, k=4 -> -k1   =>  M[row,1] = -k0*Yrws[0][row] - k1*Yrws[4][row]
	//   col 2: k=1 -> +k2               =>  M[row,2] = k2*Yrws[1][row]
	//   col 3: k=0 -> +k0, k=1 -> +k0, k=3 -> -k1  =>  M[row,3] = k0*(Yrws[0][row]+Yrws[1][row]) - k1*Yrws[3][row]
	//   col 4: k=0 -> +k1, k=1 -> +k0   =>  M[row,4] = k1*Yrws[0][row] + k0*Yrws[1][row]
	constexpr float k = 0.70710677f; // 1/sqrt(2)
	const FVector3f Rws[5] =
	{
		// R * w0 = first column of R
		{ RotateMatrix.Rows[0][0], RotateMatrix.Rows[1][0], RotateMatrix.Rows[2][0] },
		// R * w1 = third column of R
		{ RotateMatrix.Rows[0][2], RotateMatrix.Rows[1][2], RotateMatrix.Rows[2][2] },
		// R * w2 = k * (col0 + col1)
		FVector3f{ (RotateMatrix.Rows[0][0] + RotateMatrix.Rows[0][1]), (RotateMatrix.Rows[1][0] + RotateMatrix.Rows[1][1]), (RotateMatrix.Rows[2][0] + RotateMatrix.Rows[2][1]) } *k,
		// R * w3 = k * (col0 + col2)
		FVector3f{ (RotateMatrix.Rows[0][0] + RotateMatrix.Rows[0][2]), (RotateMatrix.Rows[1][0] + RotateMatrix.Rows[1][2]), (RotateMatrix.Rows[2][0] + RotateMatrix.Rows[2][2]) } *k,
		// R * w4 = k * (col1 + col2)
		FVector3f{ (RotateMatrix.Rows[0][1] + RotateMatrix.Rows[0][2]), (RotateMatrix.Rows[1][1] + RotateMatrix.Rows[1][2]), (RotateMatrix.Rows[2][1] + RotateMatrix.Rows[2][2]) } *k,
	};
	// Yrws[sample][sh] = SH_sh(R * w_sample),  sh index: 0=m-2, 1=m-1, 2=m0, 3=m1, 4=m2
	float Yrws[5][5];
	for (uint64_t Index = 0; Index < 5; Index++)
	{
		Yrws[Index][0] = SH<2, -2>(Rws[Index]);
		Yrws[Index][1] = SH<2, -1>(Rws[Index]);
		Yrws[Index][2] = SH<2,  0>(Rws[Index]);
		Yrws[Index][3] = SH<2,  1>(Rws[Index]);
		Yrws[Index][4] = SH<2,  2>(Rws[Index]);
	}
	// k0, k1, k2 are entries of invY (derived by inverting the Y matrix analytically):
	//   k0 = 0.9152912,  k1 = 1.8305824 = 2*k0,  k2 = 1.5853308
	constexpr float k0 = 0.9152912f;
	constexpr float k1 = 1.8305824f; // = 2 * k0
	constexpr float k2 = 1.5853308f;
	for (uint64_t RowIndex = 0; RowIndex < 5; RowIndex++)
	{
		SHBand3RotateMatrix[RowIndex * 5 + 0] =  k0 * Yrws[1][RowIndex] + k1 * Yrws[2][RowIndex];
		SHBand3RotateMatrix[RowIndex * 5 + 1] = -k0 * Yrws[0][RowIndex] - k1 * Yrws[4][RowIndex];
		SHBand3RotateMatrix[RowIndex * 5 + 2] =  k2 * Yrws[1][RowIndex];
		SHBand3RotateMatrix[RowIndex * 5 + 3] =  k0 * (Yrws[0][RowIndex] + Yrws[1][RowIndex]) - k1 * Yrws[3][RowIndex];
		SHBand3RotateMatrix[RowIndex * 5 + 4] =  k1 * Yrws[0][RowIndex] + k0 * Yrws[1][RowIndex];
	}
}
