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

#include "Config.h"

#include <stdint.h>
#include <type_traits>

#define UBPA_UCOMMON_CPP17_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
    template<typename T> using TSpan = UCommon::TSpan<T>; \
}

namespace UCommon
{
	template<typename T>
	T Clamp(const T& V, const T& VMin, const T& VMax) noexcept
	{
		UBPA_UCOMMON_ASSERT(VMin <= VMax);
		return V <= VMin ? VMin : (V >= VMax ? VMax : V);
	}

	template<typename T>
	class TSpan
	{
	public:
		using value_type = T;

		inline TSpan(T* InPointer, uint64_t InSize) noexcept
			: Pointer(InPointer)
			, Size(InSize)
		{
			UBPA_UCOMMON_ASSERT(InPointer || InSize == 0);
		}

		inline TSpan() noexcept : TSpan(nullptr, 0) {}

		template<uint64_t N>
		inline TSpan(T(&InPointer)[N]) noexcept : TSpan(InPointer, N) {}

		inline T& operator[](uint64_t Index) const noexcept
		{
			UBPA_UCOMMON_ASSERT(Index < Size);
			return Pointer[Index];
		}

		inline uint64_t Num() const noexcept { return Size; }
		inline bool Empty() const noexcept { return Size == 0; }
		inline T* GetData() const noexcept { return Pointer; }

		inline T* begin() const noexcept { return Pointer; }
		inline T* end() const noexcept { return Pointer + Size; }

		operator TSpan<typename std::add_const<T>::type>() const noexcept { return { Pointer, Size }; }

	private:
		T* Pointer;
		uint64_t Size;
	};

	template<typename F, typename... ArgTypes>
	using InvokeResult =
#ifdef UCOMMON_COMPATIBLE_CPP14
		std::result_of<F(ArgTypes...)>;
#else
		std::invoke_result<F, ArgTypes...>;
#endif
	template<typename F, typename... ArgTypes>
	using InvokeResult_t = typename InvokeResult<F, ArgTypes...>::type;
}

UBPA_UCOMMON_CPP17_TO_NAMESPACE(UCommonTest)
