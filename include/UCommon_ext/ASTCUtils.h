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

#define UBPA_UCOMMON_ASTCUTILS_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
    using FASTCBlock = UCommon::FASTCBlock; \
    using FASTCConfig = UCommon::FASTCConfig; \
}

#include <UCommon/Utils.h>
#include <UCommon/ThreadPool.h>

namespace UCommon
{
	/**
	 * 128 bits = 16 x 8 bits
	 * 2D
	 */
	struct UBPA_UCOMMON_API FASTCBlock
	{
		uint8_t Data[16];

		FVector4f GetPixel(const FUint64Vector2& BlockSize, uint64_t PointIndex) const;
		void GetPixels(const FUint64Vector2& BlockSize, TSpan<const uint64_t> PointIndices, FVector4f* Pixels) const;
		void GetAllPixels(const FUint64Vector2& BlockSize, FVector4f* Pixels) const;
	};

	struct UBPA_UCOMMON_API FASTCConfig
	{
		float Quality = 100.f;
		FThreadPool* ThreadPool = nullptr;
		const FVector4f* Weights = nullptr;
	};

	UBPA_UCOMMON_API void InitBlockSizeDescriptorMngr(TSpan<const uint64_t> Sizes);
	UBPA_UCOMMON_API void ReleaseBlockSizeDescriptorMngr();
	UBPA_UCOMMON_API uint64_t GetBlockSizeDescriptorMngrAllocatedBytes();
	
	void CompressImageToASTC(FASTCBlock* Blocks, const uint8_t* Image, const FUint64Vector2& ImageSize, const FUint64Vector2& BlockSize, FASTCConfig ASTCConfig);
	void DecompressASTCImage(uint8_t* Image, const FASTCBlock* Blocks, const FUint64Vector2& ImageSize, const FUint64Vector2& BlockSize);
}

UBPA_UCOMMON_ASTCUTILS_TO_NAMESPACE(UCommonTest)
