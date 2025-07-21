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

#include "Tex2D.h"

template<typename Element>
UCommon::FTex2D::FTex2D(FGrid2D InGrid2D, uint64_t InNumChannels, EOwnership InOwnership, Element* InStorage) noexcept
	: FTex2D(InGrid2D, InNumChannels, InOwnership, ElementTypeOf<ElementType>, InStorage) {}

template<typename Element>
UCommon::FTex2D::FTex2D(FGrid2D InGrid2D, uint64_t InNumChannels, const Element* InStorage)
	: FTex2D(InGrid2D, InNumChannels, ElementTypeOf<ElementType>, InStorage) {}

template<typename T>
T& UCommon::FTex2D::At(uint64_t Index) noexcept
{
	static_assert(IsSupported<T>, "T is not support");
	UBPA_UCOMMON_ASSERT(ElementType == ElementTypeOf<T>);
	constexpr size_t NumElementsPerTexel = sizeof(T) / sizeof(typename UCommon::TRemoveVector<T>::value_type);
	UBPA_UCOMMON_ASSERT(Index * NumElementsPerTexel < GetNumElements());
	return reinterpret_cast<T*>(Storage)[Index];
}

template<typename T>
const T& UCommon::FTex2D::At(uint64_t Index) const noexcept
{
	return const_cast<UCommon::FTex2D*>(this)->At<T>(Index);
}

template<typename T>
T& UCommon::FTex2D::At(const FUint64Vector2& Point, uint64_t C) noexcept
{
	static_assert(!UCommon::IsVector_v<T>, "T must not be a vector type");
	UBPA_UCOMMON_ASSERT(C < NumChannels);
	return At<T>(Grid2D.GetIndex(Point) * NumChannels + C);
}

template<typename T>
const T& UCommon::FTex2D::At(const FUint64Vector2& Point, uint64_t C) const noexcept
{
	return const_cast<UCommon::FTex2D*>(this)->At<T>(Point, C);
}

template<typename T>
T& UCommon::FTex2D::At(const FUint64Vector2& Point) noexcept
{
	static_assert(UCommon::IsVector_v<T>, "T must be a vector type");
	return At<T>(Grid2D.GetIndex(Point));
}

template<typename T>
const T& UCommon::FTex2D::At(const FUint64Vector2& Point) const noexcept
{
	return const_cast<UCommon::FTex2D*>(this)->At<T>(Point);
}
