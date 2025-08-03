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

#include <UCommon/Guid.h>

#include <random>

UCommon::FGuid UCommon::FGuid::NewGuid()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 0xFF);
	FGuid Guid;
	for (int i = 0; i < sizeof(FGuid); i++)
	{
		((uint8_t*)&Guid)[i] = dis(rd);
	}
	return Guid;
}

uint32_t& UCommon::FGuid::operator[](uint64_t Index)
{
	UBPA_UCOMMON_ASSERT(Index < 4);
	return reinterpret_cast<uint32_t*>(this)[Index];
}

const uint32_t& UCommon::FGuid::operator[](uint64_t Index) const
{
	return const_cast<UCommon::FGuid*>(this)->operator[](Index);
}

bool UCommon::FGuid::IsValid() const
{
	return ((A | B | C | D) != 0);
}

void UCommon::FGuid::ToString(char Buffer[32]) const
{
	constexpr char Alphabat[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	for (uint64_t I = 0; I < 4; I++)
	{
		const uint32_t& Value = operator[](I);
		for (uint64_t J = 0; J < 8; J++)
		{
			Buffer[I * 8 + J] = Alphabat[(Value >> (4 * (7 - J))) & 0xF];
		}
	}
}

bool UCommon::operator==(const FGuid& Lhs, const FGuid& Rhs)
{
	return Lhs.A == Rhs.A
		&& Lhs.B == Rhs.B
		&& Lhs.C == Rhs.C
		&& Lhs.D == Rhs.D;
}

bool UCommon::operator!=(const FGuid& Lhs, const FGuid& Rhs)
{
	return Lhs.A != Rhs.A
		|| Lhs.B != Rhs.B
		|| Lhs.C != Rhs.C
		|| Lhs.D != Rhs.D;
}
