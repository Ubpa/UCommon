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

float UCommon::HallucinateZH(const FSHVector2& SHVector2, float t, FVector4f& Buffer)
{
	UBPA_UCOMMON_ASSERT(t >= 0.f && t <= 1.f);

	Buffer = FVector4f(0.f);

	if (SHVector2.V[0] <= UBPA_UCOMMON_DELTA)
	{
		return 0.f;
	}

	const float L0 = SHVector2.V[0];

	Buffer[3] = L0 * 0.28209479f;

	const FVector3f LinearVector = SHVector2.GetLinearVector();
	float L1 = LinearVector.GetLength();
	if (L1 <= UBPA_UCOMMON_DELTA)
	{
		return 0.f;
	}

	const float MaxL1 = 3.f / 2.f * (1.f - t) * 0.975f * L0;
	if (MaxL1 <= UBPA_UCOMMON_DELTA)
	{
		return 0.f;
	}
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

void UCommon::ComputeSHBand2RotateMatrix(float* SHBand2RotateMatrix, const float* RotateMatrix)
{
	UBPA_UCOMMON_ASSERT(SHBand2RotateMatrix && RotateMatrix);
	//http://filmicworlds.com/blog/simple-and-fast-spherical-harmonic-rotation/
	// w0 = (1,0,0)
	// w1 = (0,1,0)
	// w2 = (0,0,1)
	constexpr float k = 2.0466533f;
	// invY = [ 0  0 -k;
	//         -k  0  0;
	//          0  k  0]
	const FVector3f Rws[3] =
	{
		{ RotateMatrix[0 * 3 + 0], RotateMatrix[1 * 3 + 0], RotateMatrix[2 * 3 + 0] },
		{ RotateMatrix[0 * 3 + 1], RotateMatrix[1 * 3 + 1], RotateMatrix[2 * 3 + 1] },
		{ RotateMatrix[0 * 3 + 2], RotateMatrix[1 * 3 + 2], RotateMatrix[2 * 3 + 2] },
	};
	float Yrws[3][3];
	for (uint64_t Index = 0; Index < 3; Index++)
	{
		Yrws[Index][0] = SH<1, -1>(Rws[Index]);
		Yrws[Index][1] = SH<1,  0>(Rws[Index]);
		Yrws[Index][2] = SH<1,  1>(Rws[Index]);
	}
	for (uint64_t ColIndex = 0; ColIndex < 3; ColIndex++)
	{
		SHBand2RotateMatrix[0 * 3 + ColIndex] = -k * Yrws[ColIndex][2];
		SHBand2RotateMatrix[1 * 3 + ColIndex] = -k * Yrws[ColIndex][0];
		SHBand2RotateMatrix[2 * 3 + ColIndex] =  k * Yrws[ColIndex][1];
	}
}

void UCommon::ComputeSHBand3RotateMatrix(float* SHBand3RotateMatrix, const float* RotateMatrix)
{
	UBPA_UCOMMON_ASSERT(SHBand3RotateMatrix && RotateMatrix);
	//http://filmicworlds.com/blog/simple-and-fast-spherical-harmonic-rotation/
	constexpr float k = 0.70710677f;
	// w0 = (1,0,0)
	// w1 = (0,0,1)
	// w2 = (k,k,0)
	// w2 = (k,0,k)
	// w2 = (0,k,k)
	const FVector3f Rws[5] =
	{
		{ RotateMatrix[0 * 3 + 0], RotateMatrix[1 * 3 + 0], RotateMatrix[2 * 3 + 0] },
		{ RotateMatrix[0 * 3 + 2], RotateMatrix[1 * 3 + 2], RotateMatrix[2 * 3 + 2] },
		FVector3f{ (RotateMatrix[0 * 3 + 0] + RotateMatrix[0 * 3 + 1]), (RotateMatrix[1 * 3 + 0] + RotateMatrix[1 * 3 + 1]), (RotateMatrix[2 * 3 + 0] + RotateMatrix[2 * 3 + 1]) } *k,
		FVector3f{ (RotateMatrix[0 * 3 + 0] + RotateMatrix[0 * 3 + 2]), (RotateMatrix[1 * 3 + 0] + RotateMatrix[1 * 3 + 2]), (RotateMatrix[2 * 3 + 0] + RotateMatrix[2 * 3 + 2]) } *k,
		FVector3f{ (RotateMatrix[0 * 3 + 1] + RotateMatrix[0 * 3 + 2]), (RotateMatrix[1 * 3 + 1] + RotateMatrix[1 * 3 + 2]), (RotateMatrix[2 * 3 + 1] + RotateMatrix[2 * 3 + 2]) } *k,
	};
	float Yrws[5][5];
	for (uint64_t Index = 0; Index < 5; Index++)
	{
		Yrws[Index][0] = SH<2, -2>(Rws[Index]);
		Yrws[Index][1] = SH<2, -1>(Rws[Index]);
		Yrws[Index][2] = SH<2,  0>(Rws[Index]);
		Yrws[Index][3] = SH<2,  1>(Rws[Index]);
		Yrws[Index][4] = SH<2,  2>(Rws[Index]);
	}
	// invY = [  0 -k0   0  k0  k1;
	//          k0   0  k2  k0  k0;
	//          k1   0   0   0   0;
	//           0   0   0 -k1   0;
	//           0 -k1   0   0   0]
	constexpr float k0 = 0.9152912f;
	constexpr float k1 = 1.8305824f;
	constexpr float k2 = 1.5853308f;
	for (uint64_t ColIndex = 0; ColIndex < 3; ColIndex++)
	{
		SHBand3RotateMatrix[0 * 5 + ColIndex] = k0 * (-Yrws[ColIndex][1] + Yrws[ColIndex][3]) + k1 * Yrws[ColIndex][4];
		SHBand3RotateMatrix[1 * 5 + ColIndex] = k0 * (Yrws[ColIndex][0] + Yrws[ColIndex][3] + Yrws[ColIndex][4]) + k2 * Yrws[ColIndex][2];
		SHBand3RotateMatrix[2 * 5 + ColIndex] = k1 * Yrws[ColIndex][0];
		SHBand3RotateMatrix[3 * 5 + ColIndex] = -k1 * Yrws[ColIndex][3];
		SHBand3RotateMatrix[4 * 5 + ColIndex] = -k1 * Yrws[ColIndex][1];
	}
}
