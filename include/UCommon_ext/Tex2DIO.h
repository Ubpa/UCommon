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

#include <UCommon/Tex2D.h>

namespace UCommon
{
	UBPA_UCOMMON_API void* LoadImage(const char* FileName, EElementType& ElementType, uint64_t& Width, uint64_t& Height, uint64_t& NumChannels, uint64_t DesiredNumChannels = 0);

	UBPA_UCOMMON_API void FreeImage(void* Data);

	UBPA_UCOMMON_API bool SaveImage(const char* FileName, uint64_t Width, uint64_t Height, uint64_t NumChannels, const float* Data);

	UBPA_UCOMMON_API bool SaveImage(const char* FileName, uint64_t Width, uint64_t Height, uint64_t NumChannels, const uint8_t* Data, uint64_t StrideInBytes = 0);

	UBPA_UCOMMON_API FTex2D LoadImage(const char* FileName, uint64_t DesiredNumChannels = 0);

	UBPA_UCOMMON_API bool SaveImage(char const* FileName, const FTex2D& Tex2D);
}
