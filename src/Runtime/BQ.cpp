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
	Data[0] = 0;
	Data[1] = 0;

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
	Components.Center = FFP8_E4M3((Min + Max) / 2);
	const float Centerf = Components.Center;
	Components.Scale = FUFP8_E4M4(std::max(std::abs(Max - Centerf), std::abs(Min - Centerf)), ERound::Up);
	const float Scalef = Components.Scale;
	for (uint64_t i = 0; i < 16; ++i)
	{
		UBPA_UCOMMON_ASSERT(Values[i] >= Min && Values[i] <= Max);

		uint8_t Value = Scalef > 0.f ? ElementFloatClampToUint7(((Values[i] - Centerf) / Scalef + 1.f) / 2.f) : 64;
		Data[i / 8] |= (uint64_t)Value << (8 + (i % 8) * 7);
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
	static constexpr int MoveBits[16] =
	{
		8, 15, 22, 29, 36, 43, 50, 57,
		8, 15, 22, 29, 36, 43, 50, 57,
	};
	const auto ValueUint7 = (Data[Index >> 3] >> MoveBits[Index]) & 0x7F;
	static constexpr uint32_t Uint7SNormTable[128]
	{
0xBF800000,0xBF7BF7F0,0xBF77EFE0,0xBF73E7D0,0xBF6FDFBF,0xBF6BD7AF,0xBF67CF9F,0xBF63C78F,
0xBF5FBF7F,0xBF5BB76F,0xBF57AF5F,0xBF53A74F,0xBF4F9F3E,0xBF4B972E,0xBF478F1E,0xBF43870E,
0xBF3F7EFE,0xBF3B76EE,0xBF376EDE,0xBF3366CE,0xBF2F5EBD,0xBF2B56AD,0xBF274E9D,0xBF23468D,
0xBF1F3E7D,0xBF1B366D,0xBF172E5D,0xBF13264D,0xBF0F1E3C,0xBF0B162C,0xBF070E1C,0xBF03060C,
0xBEFDFBF8,0xBEF5EBD8,0xBEEDDBB7,0xBEE5CB97,0xBEDDBB77,0xBED5AB57,0xBECD9B36,0xBEC58B16,
0xBEBD7AF6,0xBEB56AD6,0xBEAD5AB5,0xBEA54A95,0xBE9D3A75,0xBE952A55,0xBE8D1A34,0xBE850A14,
0xBE79F3E8,0xBE69D3A7,0xBE59B367,0xBE499326,0xBE3972E6,0xBE2952A5,0xBE193265,0xBE091224,
0xBDF1E3C8,0xBDD1A347,0xBDB162C6,0xBD912245,0xBD61C387,0xBD214285,0xBCC18306,0xBC010204,
0x3C010204,0x3CC18306,0x3D214285,0x3D61C387,0x3D912245,0x3DB162C6,0x3DD1A347,0x3DF1E3C8,
0x3E091224,0x3E193265,0x3E2952A5,0x3E3972E6,0x3E499326,0x3E59B367,0x3E69D3A7,0x3E79F3E8,
0x3E850A14,0x3E8D1A34,0x3E952A55,0x3E9D3A75,0x3EA54A95,0x3EAD5AB5,0x3EB56AD6,0x3EBD7AF6,
0x3EC58B16,0x3ECD9B36,0x3ED5AB57,0x3EDDBB77,0x3EE5CB97,0x3EEDDBB7,0x3EF5EBD8,0x3EFDFBF8,
0x3F03060C,0x3F070E1C,0x3F0B162C,0x3F0F1E3C,0x3F13264D,0x3F172E5D,0x3F1B366D,0x3F1F3E7D,
0x3F23468D,0x3F274E9D,0x3F2B56AD,0x3F2F5EBD,0x3F3366CE,0x3F376EDE,0x3F3B76EE,0x3F3F7EFE,
0x3F43870E,0x3F478F1E,0x3F4B972E,0x3F4F9F3E,0x3F53A74F,0x3F57AF5F,0x3F5BB76F,0x3F5FBF7F,
0x3F63C78F,0x3F67CF9F,0x3F6BD7AF,0x3F6FDFBF,0x3F73E7D0,0x3F77EFE0,0x3F7BF7F0,0x3F800000,
	};
	return reinterpret_cast<const float&>(Uint7SNormTable[ValueUint7]) * float(Components.Scale) + float(Components.Center);
}
