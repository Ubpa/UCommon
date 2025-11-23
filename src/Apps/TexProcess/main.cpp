#include <UCommon/UCommon.h>
#include <UCommon_ext/Tex2DIO.h>

#include <iostream>
#include <string>

using namespace UCommon;

void PrintUsage(const char* ProgramName)
{
	std::cout << "Usage: " << ProgramName << " <command> <input> <output> [options]" << std::endl;
	std::cout << std::endl;
	std::cout << "Commands:" << std::endl;
	std::cout << "  threshold <input> <output> <threshold_value>" << std::endl;
	std::cout << "    Apply threshold to texture. Pixels below threshold are set to 0." << std::endl;
	std::cout << "    Example: " << ProgramName << " threshold input.hdr output.hdr 0.5" << std::endl;
	std::cout << std::endl;
	std::cout << "  inpainting <input> <output>" << std::endl;
	std::cout << "    Apply image inpainting to fill regions where all channels are 0." << std::endl;
	std::cout << "    Coverage is automatically determined: pixels with all channels = 0 are uncovered." << std::endl;
	std::cout << "    Example: " << ProgramName << " inpainting input.hdr output.hdr" << std::endl;
}

int main(int Argc, char** Argv)
{
	if (Argc < 4)
	{
		PrintUsage(Argv[0]);
		return 1;
	}

	const std::string Command = Argv[1];
	const char* InputPath = Argv[2];
	const char* OutputPath = Argv[3];

	// Load input texture
	std::cout << "Loading texture from: " << InputPath << std::endl;
	FTex2D Tex2D = LoadImage(InputPath);
	if (!Tex2D.IsValid())
	{
		std::cerr << "Error: Failed to load texture from: " << InputPath << std::endl;
		return 1;
	}

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

	// Process based on command
	if (Command == "threshold")
	{
		if (Argc != 5)
		{
			std::cerr << "Error: threshold command requires threshold value" << std::endl;
			PrintUsage(Argv[0]);
			return 1;
		}

		const float ThresholdValue = std::stof(Argv[4]);
		std::cout << "Applying threshold: " << ThresholdValue << std::endl;
		
		Tex2D.Threshold(ThresholdValue);
		
		std::cout << "Threshold applied successfully" << std::endl;
	}
	else if (Command == "inpainting")
	{
		std::cout << "Preparing inpainting..." << std::endl;
		
		// Convert to Float if needed (ImageInpainting requires non-Uint8 coverage)
		FTex2D ProcessTex = (ElementType == EElementType::Uint8) ? Tex2D.ToFloat() : Tex2D;
		
		// Create coverage texture based on whether pixels are all zero
		FTex2D CoverageTex(Grid2D, ProcessTex.GetNumChannels(), ProcessTex.GetElementType());
		
		std::cout << "Generating coverage map (pixels with all channels = 0 are uncovered)..." << std::endl;
		
		uint64_t NumUncoveredPixels = 0;
		for (const FUint64Vector2& Point : Grid2D)
		{
			bool bIsZero = true;
			for (uint64_t C = 0; C < ProcessTex.GetNumChannels(); C++)
			{
				const float Value = ProcessTex.GetFloat(Point, C);
				if (Value != 0.f)
				{
					bIsZero = false;
					break;
				}
			}
			
			// Set coverage: 0 for uncovered (all channels zero), 1 for covered
			const float CoverageValue = bIsZero ? 0.f : 1.f;
			if (bIsZero)
			{
				NumUncoveredPixels++;
			}
			
			for (uint64_t C = 0; C < CoverageTex.GetNumChannels(); C++)
			{
				CoverageTex.SetFloat(Point, C, CoverageValue);
			}
		}
		
		const uint64_t TotalPixels = Grid2D.Width * Grid2D.Height;
		const float UncoveredPercentage = (NumUncoveredPixels * 100.f) / TotalPixels;
		std::cout << "  Uncovered pixels: " << NumUncoveredPixels << " / " << TotalPixels 
		          << " (" << UncoveredPercentage << "%)" << std::endl;
		
		if (NumUncoveredPixels == 0)
		{
			std::cout << "Warning: No uncovered pixels found. Output will be same as input." << std::endl;
		}
		else if (NumUncoveredPixels == TotalPixels)
		{
			std::cerr << "Error: All pixels are uncovered. Cannot perform inpainting." << std::endl;
			return 1;
		}
		else
		{
			std::cout << "Performing inpainting..." << std::endl;
			ProcessTex.ImageInpainting(CoverageTex);
			std::cout << "Inpainting completed successfully" << std::endl;
		}
		
		// Convert back to original type if needed
		Tex2D = (ElementType == EElementType::Uint8) ? ProcessTex.ToUint8() : ProcessTex;
	}
	else
	{
		std::cerr << "Error: Unknown command: " << Command << std::endl;
		PrintUsage(Argv[0]);
		return 1;
	}

	// Save output texture
	std::cout << "Saving texture to: " << OutputPath << std::endl;
	const bool bSuccess = SaveImage(OutputPath, Tex2D);

	if (bSuccess)
	{
		std::cout << "Successfully saved to: " << OutputPath << std::endl;
		return 0;
	}
	else
	{
		std::cerr << "Error: Failed to save texture to: " << OutputPath << std::endl;
		return 1;
	}
}

