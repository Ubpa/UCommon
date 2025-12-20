#include <UCommon/UCommon.h>

#include <iostream>
#include <string>
#include <cstdlib>

using namespace UCommon;

void PrintUsage(const char* ProgramName)
{
	std::cout << "Usage: " << ProgramName << " <input.bin> <x> <y> <w> <h> [output.bin]" << std::endl;
	std::cout << "  Crops a region [x, x+w) x [y, y+h) from a FTex2D serialized binary file." << std::endl;
	std::cout << "  Arguments:" << std::endl;
	std::cout << "    <input.bin>   Input binary file (FTex2D serialized format)" << std::endl;
	std::cout << "    <x>           Start X coordinate" << std::endl;
	std::cout << "    <y>           Start Y coordinate" << std::endl;
	std::cout << "    <w>           Width of the cropped region" << std::endl;
	std::cout << "    <h>           Height of the cropped region" << std::endl;
	std::cout << "    [output.bin]  Output binary file (optional, defaults to <input>_crop.bin)" << std::endl;
}

int main(int Argc, char** Argv)
{
	if (Argc < 6 || Argc > 7)
	{
		PrintUsage(Argv[0]);
		return 1;
	}

	const char* InputPath = Argv[1];
	const uint64_t X = std::strtoull(Argv[2], nullptr, 10);
	const uint64_t Y = std::strtoull(Argv[3], nullptr, 10);
	const uint64_t W = std::strtoull(Argv[4], nullptr, 10);
	const uint64_t H = std::strtoull(Argv[5], nullptr, 10);

	std::string OutputPath;
	if (Argc == 7)
	{
		OutputPath = Argv[6];
	}
	else
	{
		// Generate default output path: <input>_crop.bin
		OutputPath = InputPath;
		const size_t DotPos = OutputPath.rfind('.');
		if (DotPos != std::string::npos)
		{
			OutputPath.insert(DotPos, "_crop");
		}
		else
		{
			OutputPath += "_crop.bin";
		}
	}

	// Load FTex2D from binary file
	FTex2D Tex2D;
	{
		FFileArchive Reader(IArchive::EState::Loading, InputPath);
		if (!Reader.IsValid())
		{
			std::cerr << "Error: Failed to open input file: " << InputPath << std::endl;
			return 1;
		}
		Tex2D.Serialize(Reader);
	}

	// Check if texture is valid
	if (!Tex2D.IsValid())
	{
		std::cerr << "Error: Invalid texture data in file: " << InputPath << std::endl;
		return 1;
	}

	// Print texture information
	const FGrid2D Grid2D = Tex2D.GetGrid2D();
	std::cout << "Input Texture Info:" << std::endl;
	std::cout << "  Size: " << Grid2D.Width << " x " << Grid2D.Height << std::endl;
	std::cout << "  Channels: " << Tex2D.GetNumChannels() << std::endl;

	// Validate crop region
	if (X + W > Grid2D.Width || Y + H > Grid2D.Height)
	{
		std::cerr << "Error: Crop region [" << X << ", " << (X + W) << ") x [" << Y << ", " << (Y + H) << ") "
		          << "exceeds texture bounds [0, " << Grid2D.Width << ") x [0, " << Grid2D.Height << ")" << std::endl;
		return 1;
	}

	if (W == 0 || H == 0)
	{
		std::cerr << "Error: Crop region width and height must be greater than 0" << std::endl;
		return 1;
	}

	std::cout << "Cropping region: [" << X << ", " << (X + W) << ") x [" << Y << ", " << (Y + H) << ")" << std::endl;

	// Create cropped texture
	const FGrid2D CroppedGrid2D(W, H);
	const uint64_t NumChannels = Tex2D.GetNumChannels();
	const EElementType ElementType = Tex2D.GetElementType();
	FTex2D CroppedTex2D(CroppedGrid2D, NumChannels, ElementType);

	// Copy pixel data
	const uint64_t ElementSize = ElementGetSize(ElementType);
	const uint64_t RowSizeInBytes = W * NumChannels * ElementSize;

	for (uint64_t DstY = 0; DstY < H; ++DstY)
	{
		const uint64_t SrcY = Y + DstY;
		const uint64_t SrcOffset = (SrcY * Grid2D.Width + X) * NumChannels * ElementSize;
		const uint64_t DstOffset = DstY * W * NumChannels * ElementSize;

		const uint8_t* SrcPtr = static_cast<const uint8_t*>(Tex2D.GetStorage()) + SrcOffset;
		uint8_t* DstPtr = static_cast<uint8_t*>(CroppedTex2D.GetStorage()) + DstOffset;

		std::memcpy(DstPtr, SrcPtr, RowSizeInBytes);
	}

	// Save cropped texture
	{
		FFileArchive Writer(IArchive::EState::Saving, OutputPath.c_str());
		if (!Writer.IsValid())
		{
			std::cerr << "Error: Failed to create output file: " << OutputPath << std::endl;
			return 1;
		}
		CroppedTex2D.Serialize(Writer);
	}

	std::cout << "Output Texture Info:" << std::endl;
	std::cout << "  Size: " << W << " x " << H << std::endl;
	std::cout << "Successfully saved cropped texture to: " << OutputPath << std::endl;

	return 0;
}

