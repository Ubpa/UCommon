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
    using EASTCProfile = UCommon::EASTCProfile; \
    using FASTCBlock = UCommon::FASTCBlock; \
    using FASTCConfig = UCommon::FASTCConfig; \
}

#include <UCommon/Vector.h>

namespace UCommon
{
	class FThreadPool;
	class FTex2D;
	enum class EElementType : std::uint64_t;

	enum class EASTCProfile
	{
		/** @brief The LDR sRGB color profile. */
		LDR_SRGB = 0,
		/** @brief The LDR linear color profile. */
		LDR,
		/** @brief The HDR RGB with LDR alpha color profile. */
		HDR_RGB_LDR_A,
		/** @brief The HDR RGBA color profile. */
		HDR
	};

	/**
	 * 128 bits = 16 x 8 bits
	 * 2D
	 */
	struct UBPA_UCOMMON_API FASTCBlock
	{
		uint8_t Data[16];

		void GetPixels(EASTCProfile Profile, const FUint64Vector2& BlockSize, TSpan<const uint64_t> PointIndices, FVector4f* Pixels) const;
		FVector4f GetPixel(EASTCProfile Profile, const FUint64Vector2& BlockSize, uint64_t PointIndex) const;
		void GetAllPixels(EASTCProfile Profile, const FUint64Vector2& BlockSize, FVector4f* Pixels) const;
	};

	struct UBPA_UCOMMON_API FASTCConfig
	{
		enum class EFormat
		{
			None,
			RGBM,
			RGBD,
			RGBV,
		};
		EFormat Format = EFormat::None;
		float MaxValue = 0.f;
		/** RGBV s parameter for encoding/decoding: k = -s, b = s + 1/M. Default is 1. */
		float RGBV_S = 1.f;
		float Quality = 100.f;
		FThreadPool* ThreadPool = nullptr;
		const FVector4f* Weights = nullptr;
		const char* Swizzel = nullptr;
		const float* Cw = nullptr;
	};

	UBPA_UCOMMON_API void InitBlockSizeDescriptorMngr(TSpan<const uint64_t> Sizes);
	UBPA_UCOMMON_API void ReleaseBlockSizeDescriptorMngr();
	UBPA_UCOMMON_API uint64_t GetBlockSizeDescriptorMngrAllocatedBytes();
	
	UBPA_UCOMMON_API void CompressImageToASTC(FASTCBlock* Blocks, EASTCProfile Profile, EElementType ElementType, const void* Image, const FUint64Vector2& ImageSize, const FUint64Vector2& BlockSize, FASTCConfig ASTCConfig);
	UBPA_UCOMMON_API void DecompressASTCImage(FVector4f* Image, EASTCProfile Profile, const FASTCBlock* Blocks, const FUint64Vector2& ImageSize, const FUint64Vector2& BlockSize);

	/**
	 * @param Tex float4
	 * @param This this float/half/uint8
	 * @param BlockBuffer = new FASTCBlock[((GetGrid().GetExtent() + BlockSize - 1) / BlockSize).Area()];
	 */
	UBPA_UCOMMON_API void ToASTC(FTex2D& Tex, EASTCProfile Profile, const FTex2D& This, uint64_t BlockSize, FASTCConfig ASTCConfig, FASTCBlock* BlockBuffer = nullptr);

	UBPA_UCOMMON_API FTex2D ToASTC(EASTCProfile Profile, const FTex2D& This, uint64_t BlockSize, FASTCConfig ASTCConfig, FASTCBlock* BlockBuffer = nullptr);
}

UBPA_UCOMMON_ASTCUTILS_TO_NAMESPACE(UCommonTest)
