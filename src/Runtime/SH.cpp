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
	// The j-th column of R is R * e_j, which equals (R^T).Rows[j], so:
	//   Rws[j] = R * w_j = (R^T).Rows[j]  (extract j-th column = j-th row of transpose)
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
	// invY = Y^{-1}, k0 = 1/K = 2.0466533:
	//   invY = [  0   0  -k0 ]    (row 0)
	//          [ -k0  0   0  ]    (row 1)
	//          [  0  k0   0  ]    (row 2)
	//
	// Expanding M[row, col] = sum_k Yrws[k][row] * invY[k, col]
	// using invY columns (only nonzero entries per column of invY):
	//   invY col 0: k=1 -> -k0       =>  M[row, 0] = Yrws[1][row] * (-k0)
	//   invY col 1: k=2 -> +k0       =>  M[row, 1] = Yrws[2][row] * (+k0)
	//   invY col 2: k=0 -> -k0       =>  M[row, 2] = Yrws[0][row] * (-k0)
	constexpr float k0 = 2.0466533f; // k0 = 1 / SHK<1,+/-1> = 1 / 0.48860252
	const FMatrix3x3f RT = RotateMatrix.Transpose();
	const FVector3f Rws[3] =
	{
		RT.Rows[0],  // R * w0 = R * (1,0,0) = first column of R
		RT.Rows[1],  // R * w1 = R * (0,1,0) = second column of R
		RT.Rows[2],  // R * w2 = R * (0,0,1) = third column of R
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
	// col 0: invY col0 nonzero at k=1 -> -k0  =>  M[row, 0] = -k0 * Yrws[1][row]
	// col 1: invY col1 nonzero at k=2 -> +k0  =>  M[row, 1] = +k0 * Yrws[2][row]
	// col 2: invY col2 nonzero at k=0 -> -k0  =>  M[row, 2] = -k0 * Yrws[0][row]
	for (uint64_t RowIndex = 0; RowIndex < 3; RowIndex++)
	{
		SHBand2RotateMatrix[RowIndex * 3 + 0] = -k0 * Yrws[1][RowIndex];
		SHBand2RotateMatrix[RowIndex * 3 + 1] =  k0 * Yrws[2][RowIndex];
		SHBand2RotateMatrix[RowIndex * 3 + 2] = -k0 * Yrws[0][RowIndex];
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
	//   w2 = c1*(1, 1, 0)  where c1 = 1/sqrt(2)
	//   w3 = c1*(1, 0, 1)
	//   w4 = c1*(0, 1, 1)
	//
	// RotateMatrix is row-major 3x3: RotateMatrix.Rows[i][j] = R_{i,j}
	// Applying R to a column vector: (R*w)[i] = sum_j R_{i,j} * w[j]
	// The j-th column of R is R * e_j, which equals (R^T).Rows[j], so:
	//   Rws[j] = R * w_j = (R^T).Rows[j]  (extract j-th column = j-th row of transpose)
	//
	// 5 linearly independent sample directions:
	//   w0 = (1, 0, 0)
	//   w1 = (0, 0, 1)
	//   w2 = c1*(1, 1, 0)  where c1 = 1/sqrt(2)
	//   w3 = c1*(1, 0, 1)
	//   w4 = c1*(0, 1, 1)
	//
	// By linearity:
	//   Rws[0] = R * w0  = (R^T).Rows[0]
	//   Rws[1] = R * w1  = (R^T).Rows[2]
	//   Rws[2] = R * w2  = c1 * ((R^T).Rows[0] + (R^T).Rows[1])
	//   Rws[3] = R * w3  = c1 * ((R^T).Rows[0] + (R^T).Rows[2])
	//   Rws[4] = R * w4  = c1 * ((R^T).Rows[1] + (R^T).Rows[2])
	//
	// SH<2,m>(x,y,z):
	//   m=-2: a * x*y    (a = 1.0925485)
	//   m=-1: -a * y*z
	//   m= 0: c2 * (3z^2-1)  (c2 = 0.31539157)
	//   m= 1: -a * x*z
	//   m= 2: b * (x^2-y^2)   (b = 0.54627424)
	//
	// invY (5x5) expressed in terms of k0, k1, k2:
	//   invY = [  0  -k0   0   k0   k1 ]
	//          [ k0    0  k2   k0   k0 ]
	//          [ k1    0   0    0    0 ]
	//          [  0    0   0  -k1    0 ]
	//          [  0  -k1   0    0    0 ]
	//
	// M = Y_R * invY, element form: M[row, col] = sum_k Yrws[k][row] * invY[k, col]
	// Expanding by invY columns (nonzero entries per column):
	//   col 0: k=1 -> +k0, k=2 -> +k1   =>  M[row,0] = k0*Yrws[1][row] + k1*Yrws[2][row]
	//   col 1: k=0 -> -k0, k=4 -> -k1   =>  M[row,1] = -k0*Yrws[0][row] - k1*Yrws[4][row]
	//   col 2: k=1 -> +k2               =>  M[row,2] = k2*Yrws[1][row]
	//   col 3: k=0 -> +k0, k=1 -> +k0, k=3 -> -k1  =>  M[row,3] = k0*(Yrws[0][row]+Yrws[1][row]) - k1*Yrws[3][row]
	//   col 4: k=0 -> +k1, k=1 -> +k0   =>  M[row,4] = k1*Yrws[0][row] + k0*Yrws[1][row]
	constexpr float c1 = 0.70710677f; // 1/sqrt(2)
	const FMatrix3x3f RT = RotateMatrix.Transpose();
	const FVector3f Rws[5] =
	{
		RT.Rows[0],                             // R * w0 = first column of R
		RT.Rows[2],                             // R * w1 = third column of R
		(RT.Rows[0] + RT.Rows[1]) * c1,         // R * w2 = c1 * (col0 + col1)
		(RT.Rows[0] + RT.Rows[2]) * c1,         // R * w3 = c1 * (col0 + col2)
		(RT.Rows[1] + RT.Rows[2]) * c1,         // R * w4 = c1 * (col1 + col2)
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

void UCommon::ComputeSHBand4RotateMatrix(float* SHBand4RotateMatrix, const FMatrix3x3f& RotateMatrix)
{
	UBPA_UCOMMON_ASSERT(SHBand4RotateMatrix);
	// Reference: http://filmicworlds.com/blog/simple-and-fast-spherical-harmonic-rotation/
	//
	// Algorithm: same as Band2/Band3 (M = Y_R * invY), but for l=3 (7-dimensional).
	// 7 linearly independent sample directions are required:
	//   w0 = (1,  0,  0)         = e0
	//   w1 = (0,  1,  0)         = e1
	//   w2 = (0,  0,  1)         = e2
	//   w3 = c1*(1, -1,  0)      = c1*(e0 - e1)   // breaks x-y symmetry
	//   w4 = c1*(1,  0,  1)      = c1*(e0 + e2)
	//   w5 = c1*(0,  1,  1)      = c1*(e1 + e2)
	//   w6 = c2*(1,  1,  1)      = c2*(e0 + e1 + e2)
	// where c1 = 1/sqrt(2), c2 = 1/sqrt(3)
	//
	// RotateMatrix is row-major 3x3: RotateMatrix.Rows[i][j] = R_{i,j}
	// Applying R to a column vector: (R*w)[i] = sum_j R_{i,j} * w[j]
	// The j-th column of R is R * e_j, which equals (R^T).Rows[j], so:
	//   Rws[j] = R * w_j = (R^T).Rows[j]  (extract j-th column = j-th row of transpose)
	//
	// By linearity:
	//   Rws[0] = R * w0  = (R^T).Rows[0]
	//   Rws[1] = R * w1  = (R^T).Rows[1]
	//   Rws[2] = R * w2  = (R^T).Rows[2]
	//   Rws[3] = R * w3  = c1 * ((R^T).Rows[0] - (R^T).Rows[1])
	//   Rws[4] = R * w4  = c1 * ((R^T).Rows[0] + (R^T).Rows[2])
	//   Rws[5] = R * w5  = c1 * ((R^T).Rows[1] + (R^T).Rows[2])
	//   Rws[6] = R * w6  = c2 * ((R^T).Rows[0] + (R^T).Rows[1] + (R^T).Rows[2])
	//
	// SH<3,m>(x,y,z):
	//   m=-3: c * x*(5x^2-3(y^2+z^2))     (c = 0.5900435)
	//   m=-2: a * x*y*(5z^2-1)           (a = 2.8906115)
	//   m=-1: b * y*(5z^2-1)             (b = 0.45704579)
	//   m= 0: c2 * (5z^3-3z)             (c2 = 0.3156899)
	//   m=+1: b * x*(5z^2-1)
	//   m=+2: a * (x^2-y^2)*(5z^2-1)/2
	//   m=+3: c * z*(5z^2-3(x^2+y^2))
	//
	// invY (7x7) with numerical values (error < 1e-15):
	//   col 0 (m=-3): k0*Y[0] + k8*Y[1] + k0*Y[2] + k3*Y[3] + k3*Y[4] + k3*Y[5]
	//   col 1 (m=-2): k4*Y[0] + k4*Y[1] + k4*Y[2]                       + k10*Y[6]
	//   col 2 (m=-1): -k1*Y[0] + k6*Y[1] - k1*Y[2] - k5*Y[3] - k5*Y[4] - k5*Y[5]
	//   col 3 (m= 0): k9*Y[2]
	//   col 4 (m=+1): k6*Y[0] - k1*Y[1] - k1*Y[2] + k5*Y[3] - k5*Y[4] - k5*Y[5]
	//   col 5 (m=+2): k2*Y[0] - k2*Y[1]            + k7*Y[3] + k7*Y[4] - k7*Y[5]
	//   col 6 (m=+3): -k8*Y[0] - k0*Y[1] - k0*Y[2]  + k3*Y[3] - k3*Y[4] - k3*Y[5]
	constexpr float c1 = 0.70710677f;  // 1/sqrt(2)
	constexpr float c2 = 0.57735027f;  // 1/sqrt(3)
	const FMatrix3x3f RT = RotateMatrix.Transpose();
	const FVector3f Rws[7] =
	{
		RT.Rows[0],                                        // R * w0 = e0 column
		RT.Rows[1],                                        // R * w1 = e1 column
		RT.Rows[2],                                        // R * w2 = e2 column
		(RT.Rows[0] - RT.Rows[1]) * c1,                   // R * w3 = c1*(e0-e1)
		(RT.Rows[0] + RT.Rows[2]) * c1,                   // R * w4 = c1*(e0+e2)
		(RT.Rows[1] + RT.Rows[2]) * c1,                   // R * w5 = c1*(e1+e2)
		(RT.Rows[0] + RT.Rows[1] + RT.Rows[2]) * c2,      // R * w6 = c2*(e0+e1+e2)
	};
	// Yrws[sample][sh] = SH_sh(R * w_sample), sh index: 0=m-3, 1=m-2, 2=m-1, 3=m0, 4=m+1, 5=m+2, 6=m+3
	float Yrws[7][7];
	for (uint64_t Index = 0; Index < 7; Index++)
	{
		Yrws[Index][0] = SH<3, -3>(Rws[Index]);
		Yrws[Index][1] = SH<3, -2>(Rws[Index]);
		Yrws[Index][2] = SH<3, -1>(Rws[Index]);
		Yrws[Index][3] = SH<3,  0>(Rws[Index]);
		Yrws[Index][4] = SH<3,  1>(Rws[Index]);
		Yrws[Index][5] = SH<3,  2>(Rws[Index]);
		Yrws[Index][6] = SH<3,  3>(Rws[Index]);
	}
	// invY coefficients (numerical, error < 1e-15):
	constexpr float k0  = 0.2118488f;
	constexpr float k1  = 0.2734956f;
	constexpr float k2  = 0.3459476f;
	constexpr float k3  = 0.5991988f;
	constexpr float k4  = 0.6918951f;
	constexpr float k5  = 0.7735623f;
	constexpr float k6  = 0.8204867f;
	constexpr float k7  = 0.9784875f;
	constexpr float k8  = 1.0592438f;
	constexpr float k9  = 1.3398491f;
	constexpr float k10 = 1.7975963f;
	// M = Y_R * invY, expand by invY columns
	for (uint64_t RowIndex = 0; RowIndex < 7; RowIndex++)
	{
		// col 0 (m=-3)
		SHBand4RotateMatrix[RowIndex * 7 + 0] = k0 * Yrws[0][RowIndex] + k8 * Yrws[1][RowIndex] + k0 * Yrws[2][RowIndex]
		                                       + k3 * Yrws[3][RowIndex] + k3 * Yrws[4][RowIndex] + k3 * Yrws[5][RowIndex];
		// col 1 (m=-2)
		SHBand4RotateMatrix[RowIndex * 7 + 1] = k4 * Yrws[0][RowIndex] + k4 * Yrws[1][RowIndex] + k4 * Yrws[2][RowIndex]
		                                       + k10 * Yrws[6][RowIndex];
		// col 2 (m=-1)
		SHBand4RotateMatrix[RowIndex * 7 + 2] = -k1 * Yrws[0][RowIndex] + k6 * Yrws[1][RowIndex] - k1 * Yrws[2][RowIndex]
		                                        - k5 * Yrws[3][RowIndex] - k5 * Yrws[4][RowIndex] - k5 * Yrws[5][RowIndex];
		// col 3 (m= 0)
		SHBand4RotateMatrix[RowIndex * 7 + 3] = k9 * Yrws[2][RowIndex];
		// col 4 (m=+1)
		SHBand4RotateMatrix[RowIndex * 7 + 4] = k6 * Yrws[0][RowIndex] - k1 * Yrws[1][RowIndex] - k1 * Yrws[2][RowIndex]
		                                        + k5 * Yrws[3][RowIndex] - k5 * Yrws[4][RowIndex] - k5 * Yrws[5][RowIndex];
		// col 5 (m=+2)
		SHBand4RotateMatrix[RowIndex * 7 + 5] = k2 * Yrws[0][RowIndex] - k2 * Yrws[1][RowIndex]
		                                        + k7 * Yrws[3][RowIndex] + k7 * Yrws[4][RowIndex] - k7 * Yrws[5][RowIndex];
		// col 6 (m=+3)
		SHBand4RotateMatrix[RowIndex * 7 + 6] = -k8 * Yrws[0][RowIndex] - k0 * Yrws[1][RowIndex] - k0 * Yrws[2][RowIndex]
		                                        + k3 * Yrws[3][RowIndex] - k3 * Yrws[4][RowIndex] - k3 * Yrws[5][RowIndex];
	}
}
