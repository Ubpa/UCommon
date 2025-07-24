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
#include "Utils.h"
#include "Half.h"
#include "FP8.h"

#define UBPA_UCOMMON_BQ_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
    using FBQBlock = UCommon::FBQBlock; \
}

namespace UCommon
{
	struct UBPA_UCOMMON_API FBQBlock
	{
		union
		{
			struct
			{
				FUFP8_E4M4 Scale;
				uint8_t Buffer0[7];
				FFP8_E4M3 Center;
				uint8_t Buffer1[7];
			} Components;
			uint64_t Data[2];
		};
		FBQBlock(const float(&Values)[16]) noexcept;
		FBQBlock(TSpan<const float> Values) noexcept;
		float GetValue(uint64_t Index) const noexcept;
	};
	static_assert(sizeof(FBQBlock) == 16, "FBQBlock size mismatch");
}

UBPA_UCOMMON_BQ_TO_NAMESPACE(UCommonTest)
