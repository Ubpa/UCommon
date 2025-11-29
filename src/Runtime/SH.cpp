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

	const float MaxL1 = std::sqrt(3.f) * (1.f - t) * 0.975f;
	if (MaxL1 <= UBPA_UCOMMON_DELTA)
	{
		return 0.f;
	}
	float Scale = 1.f;
	if (L1 >= MaxL1)
	{
		L1 = MaxL1;
		Scale = MaxL1 / L1;
	}
	constexpr float Factor1 = 2.f / 3.f * 0.48860252f;
	const float p = L1 / SHVector2.V[0];
	const float L2 = (0.6f * p * p + 0.08f * p) * L0;

	constexpr float Factor2 = 1.f / 4.f * 0.31539157f;

	for(uint64_t i = 0; i < 3; i++)
	{
		Buffer[i] = LinearVector[i] * Scale * Factor1;
	}

	Buffer[3] -= L2 * Factor2;

	return Factor2 * L2 * 3.f / (L1 * L1 / (3.f * Pi));
}
