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

#include <UCommon/Utils.h>
#include <UCommon/Config.h>
#include <cmath>
#include <algorithm>

float UCommon::ApplyAddressMode(float Coord, ETextureAddress AddressMode)
{
	switch (AddressMode)
	{
	case ETextureAddress::Wrap:
	{
		// Wrap: repeat the texture
		float Frac = std::fmod(Coord, 1.0f);
		if (Frac < 0.0f)
			Frac += 1.0f;
		return Frac;
	}
	case ETextureAddress::Clamp:
	{
		// Clamp: clamp to [0, 1]
		return std::clamp(Coord, 0.0f, 1.0f);
	}
	case ETextureAddress::Mirror:
	{
		// Mirror: reflect at boundaries
		float Frac = std::fmod(Coord, 2.0f);
		if (Frac < 0.0f)
			Frac += 2.0f;
		if (Frac > 1.0f)
			Frac = 2.0f - Frac;
		return Frac;
	}
	default:
		UBPA_UCOMMON_NO_ENTRY();
		return Coord;
	}
}

uint64_t UCommon::ApplyAddressMode(int64_t Coord, uint64_t Size, ETextureAddress AddressMode)
{
	switch (AddressMode)
	{
	case ETextureAddress::Wrap:
	{
		// Wrap: repeat the texture
		int64_t Wrapped = Coord % static_cast<int64_t>(Size);
		if (Wrapped < 0)
			Wrapped += static_cast<int64_t>(Size);
		return static_cast<uint64_t>(Wrapped);
	}
	case ETextureAddress::Clamp:
	{
		// Clamp: clamp to [0, Size-1]
		return static_cast<uint64_t>(std::clamp(Coord, static_cast<int64_t>(0), static_cast<int64_t>(Size - 1)));
	}
	case ETextureAddress::Mirror:
	{
		// Mirror: reflect at boundaries
		// First handle negative coordinates
		int64_t AbsCoord = Coord;
		if (AbsCoord < 0)
		{
			AbsCoord = -AbsCoord - 1;
		}

		int64_t Period = static_cast<int64_t>(Size) * 2;
		int64_t Wrapped = AbsCoord % Period;
		if (Wrapped >= static_cast<int64_t>(Size))
			Wrapped = Period - Wrapped - 1;
		return static_cast<uint64_t>(Wrapped);
	}
	default:
		UBPA_UCOMMON_NO_ENTRY();
		return 0;
	}
}

UCommon::FVector2f UCommon::ApplyAddressMode(const FVector2f& Coord, ETextureAddress AddressMode)
{
	return FVector2f(
		ApplyAddressMode(Coord.X, AddressMode),
		ApplyAddressMode(Coord.Y, AddressMode)
	);
}

UCommon::FUint64Vector2 UCommon::ApplyAddressMode(const FInt64Vector2& Coord, const FUint64Vector2& Size, ETextureAddress AddressMode)
{
	return FUint64Vector2(
		ApplyAddressMode(Coord.X, Size.X, AddressMode),
		ApplyAddressMode(Coord.Y, Size.Y, AddressMode)
	);
}

