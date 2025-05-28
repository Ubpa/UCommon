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
#include <UCommon_ext/stb/stb_image.h>
#include <UCommon_ext/stb/stb_image_write.h>
#include <string>

UCommon::FTex2D UCommon::LoadImage(char const* FileName)
{
	EElementType ElementType = EElementType::Unknown;
	void* Data;
	int Width, Height, NumChannels;
	if (stbi_is_hdr(FileName))
	{
		ElementType = EElementType::Float;
		Data = stbi_loadf(FileName, &Width, &Height, &NumChannels, 0);
	}
	else
	{
		ElementType = EElementType::Uint8;
		Data = stbi_load(FileName, &Width, &Height, &NumChannels, 0);
	}
	if (!Data)
	{
		return {};
	}
	FTex2D Tex2D(FGrid2D((uint64_t)Width, (uint64_t)Height), (uint64_t)NumChannels, ElementType);
	std::memcpy(Tex2D.GetStorage(), Data, Tex2D.GetStorageSizeInBytes());
	stbi_image_free(Data);
	return Tex2D;
}

bool UCommon::SaveImage(const FTex2D& Tex2D, char const* FileName)
{
	int error_code = 0;
	if (Tex2D.GetElementType() == EElementType::Float)
	{
		error_code = stbi_write_hdr(FileName, (int)Tex2D.GetGrid2D().Width, (int)Tex2D.GetGrid2D().Height, (int)Tex2D.GetNumChannel(), (float*)Tex2D.GetStorage());
	}
	else
	{
		UBPA_UCOMMON_ASSERT(Tex2D.GetElementType() == EElementType::Uint8);
		error_code = stbi_write_png(FileName, (int)Tex2D.GetGrid2D().Width, (int)Tex2D.GetGrid2D().Height, (int)Tex2D.GetNumChannel(), Tex2D.GetStorage(), 0);
	}
	return error_code == 0;
}
