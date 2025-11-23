#include <UCommon/UCommon.h>
#include <UCommon_ext/Tex2DIO.h>

#include <iostream>
#include <string>

using namespace UCommon;

void PrintUsage(const char* ProgramName)
{
	std::cout << "Usage: " << ProgramName << " <input.bin> <output.png/hdr> [--force-png]" << std::endl;
	std::cout << "  Reads a FTex2D serialized binary file and exports it to PNG (uint8) or HDR (float) format." << std::endl;
	std::cout << "  The output format is determined by the ElementType in the binary file:" << std::endl;
	std::cout << "    - Uint8 -> PNG" << std::endl;
	std::cout << "    - Float -> HDR" << std::endl;
	std::cout << "  Options:" << std::endl;
	std::cout << "    --force-png  Convert to Uint8 and export as PNG regardless of input type" << std::endl;
}

int main(int Argc, char** Argv)
{
	if (Argc < 3 || Argc > 4)
	{
		PrintUsage(Argv[0]);
		return 1;
	}

	const char* InputPath = Argv[1];
	const char* OutputPath = Argv[2];
	bool bForcePng = false;

	if (Argc == 4)
	{
		std::string Option = Argv[3];
		if (Option == "--force-png")
		{
			bForcePng = true;
		}
		else
		{
			std::cerr << "Error: Unknown option: " << Option << std::endl;
			PrintUsage(Argv[0]);
			return 1;
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
	std::cout << "Texture Info:" << std::endl;
	std::cout << "  Size: " << Grid2D.Width << " x " << Grid2D.Height << std::endl;
	std::cout << "  Channels: " << Tex2D.GetNumChannels() << std::endl;
	std::cout << "  ElementType: ";
	
	const EElementType ElementType = Tex2D.GetElementType();
	switch (ElementType)
	{
	case EElementType::Uint8:
		std::cout << "Uint8" << std::endl;
		break;
	case EElementType::Float:
		std::cout << "Float" << std::endl;
		break;
	case EElementType::Double:
		std::cout << "Double" << std::endl;
		break;
	default:
		std::cout << "Unknown" << std::endl;
		break;
	}

	// Save to output file
	bool bSuccess = false;
	if (bForcePng)
	{
		std::cout << "Converting to Uint8 and exporting as PNG..." << std::endl;
		const FTex2D Uint8Tex2D = Tex2D.ToUint8();
		bSuccess = SaveImage(OutputPath, Uint8Tex2D);
	}
	else if (ElementType == EElementType::Uint8)
	{
		std::cout << "Exporting as PNG..." << std::endl;
		bSuccess = SaveImage(OutputPath, Tex2D);
	}
	else if (ElementType == EElementType::Float)
	{
		std::cout << "Exporting as HDR..." << std::endl;
		bSuccess = SaveImage(OutputPath, Tex2D);
	}
	else if (ElementType == EElementType::Double)
	{
		std::cout << "Converting Double to Float and exporting as HDR..." << std::endl;
		const FTex2D FloatTex2D = Tex2D.ToFloat();
		bSuccess = SaveImage(OutputPath, FloatTex2D);
	}
	else
	{
		std::cerr << "Error: Unsupported element type" << std::endl;
		return 1;
	}

	if (bSuccess)
	{
		std::cout << "Successfully exported to: " << OutputPath << std::endl;
		return 0;
	}
	else
	{
		std::cerr << "Error: Failed to save image to: " << OutputPath << std::endl;
		return 1;
	}
}

