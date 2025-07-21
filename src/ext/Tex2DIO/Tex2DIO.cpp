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

#include <UCommon_ext/Tex2DIO.h>
#define STBI_MALLOC UBPA_UCOMMON_MALLOC
#define STBI_REALLOC UBPA_UCOMMON_REALLOC
#define STBI_FREE UBPA_UCOMMON_FREE
#include <UCommon_ext/stb/stb_image.h>
#define STBIW_MALLOC UBPA_UCOMMON_MALLOC
#define STBIW_REALLOC UBPA_UCOMMON_REALLOC
#define STBIW_FREE UBPA_UCOMMON_FREE
#include <UCommon_ext/stb/stb_image_write.h>
#include <string>

void* UCommon::LoadImage(char const* FileName, EElementType& ElementType, uint64_t& Width, uint64_t& Height, uint64_t& NumChannels, uint64_t DesiredNumChannels)
{
	void* Data;
	int WidthInt, HeightInt, NumChannelsInt;
	if (stbi_is_hdr(FileName))
	{
		ElementType = EElementType::Float;
		Data = stbi_loadf(FileName, &WidthInt, &HeightInt, &NumChannelsInt, (int)DesiredNumChannels);
	}
	else
	{
		ElementType = EElementType::Uint8;
		Data = stbi_load(FileName, &WidthInt, &HeightInt, &NumChannelsInt, (int)DesiredNumChannels);
	}
	if (!Data)
	{
		return nullptr;
	}
	Width = (uint64_t)WidthInt;
	Height = (uint64_t)HeightInt;
	NumChannels = (uint64_t)NumChannelsInt;
	return Data;
}

void UCommon::FreeImage(void* Data)
{
	stbi_image_free(Data);
}

bool UCommon::SaveImage(const char* FileName, uint64_t Width, uint64_t Height, uint64_t NumChannels, const float* Data)
{
	const int error_code = stbi_write_hdr(FileName, (int)Width, (int)Height, (int)NumChannels, Data);
	return error_code == 0;
}

bool UCommon::SaveImage(const char* FileName, uint64_t Width, uint64_t Height, uint64_t NumChannels, const uint8_t* Data, uint64_t StrideInBytes)
{
	const int error_code = stbi_write_png(FileName, (int)Width, (int)Height, (int)NumChannels, Data, (int)StrideInBytes);
	return error_code == 0;
}

UCommon::FTex2D UCommon::LoadImage(char const* FileName, uint64_t DesiredNumChannels)
{
	EElementType ElementType = EElementType::Unknown;
	uint64_t Width = 0, Height = 0, NumChannels = 0;
	void* Data = LoadImage(FileName, ElementType, Width, Height, NumChannels, DesiredNumChannels);
	if (!Data)
	{
		return {};
	}
	return { { Width, Height }, NumChannels, EOwnership::TakeOwnership, ElementType, Data };
}

bool UCommon::SaveImage(char const* FileName, const FTex2D& Tex2D)
{
	int error_code = 0;
	if (Tex2D.GetElementType() == EElementType::Float)
	{
		error_code = SaveImage(FileName, (int)Tex2D.GetGrid2D().Width, (int)Tex2D.GetGrid2D().Height, (int)Tex2D.GetNumChannels(), (const float*)Tex2D.GetStorage());
	}
	else if(Tex2D.GetElementType() == EElementType::Uint8)
	{
		error_code = SaveImage(FileName, (int)Tex2D.GetGrid2D().Width, (int)Tex2D.GetGrid2D().Height, (int)Tex2D.GetNumChannels(), (const uint8_t*)Tex2D.GetStorage());
	}
	else
	{
		error_code = 1;
	}
	return error_code == 0;
}
