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

#include <UCommon/BQ.h>

UCommon::FBQBlock::FBQBlock(const float(&Values)[16]) noexcept
{
	// find min max
	float Min = Values[0];
	float Max = Values[0];
	for (uint64_t i = 1; i < 16; ++i)
	{
		if (Values[i] < Min)
		{
			Min = Values[i];
		}
		if (Values[i] > Max)
		{
			Max = Values[i];
		}
	}

	// compute indices
	Bias = FHalf(Min);
	const float Biasf = Bias;
	Scale = FHalf(Max - Biasf);
	const float Scalef = Scale;
	for (uint64_t i = 0; i < 16; ++i)
	{
		UBPA_UCOMMON_ASSERT(Values[i] >= Min && Values[i] <= Max);
		Indices[i] = Scalef > 0.f ? ElementFloatClampToUint8((Values[i] - Biasf) / Scalef) : 0;
	}
}

UCommon::FBQBlock::FBQBlock(TSpan<const float> Values) noexcept
	: FBQBlock(*reinterpret_cast<const float(*)[16]>(Values.GetData()))
{
	UBPA_UCOMMON_ASSERT(Values.Num() == 16);
}

float UCommon::FBQBlock::GetValue(uint64_t Index) const noexcept
{
	UBPA_UCOMMON_ASSERT(Index < 16);
	return ElementUint8ToFloat(Indices[Index]) * Scale + Bias;
}
