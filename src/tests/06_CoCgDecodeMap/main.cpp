#include <UCommon/UCommon.h>
#include <UCommon_ext/Tex2DIO.h>

#include <iostream>
#include <string>
#include <algorithm>

using namespace UCommon;

// Standard YCoCg to RGB conversion (non-normalized)
// Y = (R + 2*G + B) / 4
// Co = (R - B) / 2, range: [-2Y, 2Y]
// Cg = (2*G - R - B) / 4, range: [-Y, Y]
// Inverse:
// R = Y + Co - Cg
// G = Y + Cg
// B = Y - Co - Cg
static inline void StandardYCoCgToRGB(float Y, float Co, float Cg, float& R, float& G, float& B)
{
	R = Y + Co - Cg;
	G = Y + Cg;
	B = Y - Co - Cg;
}

// Triangle mapping for normalized YCoCg
// Maps UV [0,1]x[0,1] to valid (Co, Cg) triangle
// Valid region: Cg - 1 <= 2*Co <= 1 - Cg
// UV.x -> Co position within valid range for given Cg
// UV.y -> Cg in [-1, 1]
static inline void UVToNormalizedCoCg(float U, float V, float& Co, float& Cg)
{
	// Map V to Cg: [0, 1] -> [-1, 1]
	Cg = V * 2.0f - 1.0f;

	// For given Cg, Co valid range is [(Cg - 1) / 2, (1 - Cg) / 2]
	const float CoMin = (Cg - 1.0f) * 0.5f;
	const float CoMax = (1.0f - Cg) * 0.5f;

	// Map U to Co within valid range
	Co = CoMin + U * (CoMax - CoMin);
}

// Inverse: normalized CoCg to UV
// Returns true if (Co, Cg) is within valid triangle
static inline bool NormalizedCoCgToUV(float Co, float Cg, float& U, float& V)
{
	// Map Cg to V: [-1, 1] -> [0, 1]
	V = (Cg + 1.0f) * 0.5f;

	// Check if Co is within valid range for this Cg
	const float CoMin = (Cg - 1.0f) * 0.5f;
	const float CoMax = (1.0f - Cg) * 0.5f;

	if (Co < CoMin || Co > CoMax)
	{
		return false; // Outside valid triangle
	}

	// Map Co to U within valid range
	const float CoRange = CoMax - CoMin;
	if (CoRange < 1e-6f)
	{
		U = 0.5f; // Degenerate case
	}
	else
	{
		U = (Co - CoMin) / CoRange;
	}

	return true;
}

// Generate normalized CoCg decode map using square mapping (shows invalid regions)
// Co range: [-1, 1], Cg range: [-1, 1]
// Output as HDR image
void GenerateNormalizedCoCgDecodeMap_Square(const char* OutputPath, float YValue, uint64_t Resolution = 512)
{
	std::cout << "Generating Normalized CoCg decode map (Square) with Y = " << YValue << "..." << std::endl;
	std::cout << "  Resolution: " << Resolution << "x" << Resolution << std::endl;
	std::cout << "  Co range: [-1, 1]" << std::endl;
	std::cout << "  Cg range: [-1, 1]" << std::endl;
	std::cout << "  Note: Invalid regions (outside triangle) will show negative RGB values" << std::endl;

	// Create a float texture with 3 channels (RGB)
	FTex2D Tex2D(FGrid2D(Resolution, Resolution), 3, EElementType::Float);

	// Generate the decode map
	// X axis: Co from -1 to 1
	// Y axis: Cg from -1 to 1
	for (uint64_t Y = 0; Y < Resolution; Y++)
	{
		for (uint64_t X = 0; X < Resolution; X++)
		{
			// Map pixel coordinates to Co and Cg values
			const float Co = (float)X / (float)(Resolution - 1) * 2.0f - 1.0f; // [-1, 1]
			const float Cg = (float)Y / (float)(Resolution - 1) * 2.0f - 1.0f; // [-1, 1]

			// Convert YCoCg to RGB (UCommon normalized version)
			float R, G, B;
			YCoCgToRGB(YValue, Co, Cg, R, G, B);

			// Store RGB values
			const FUint64Vector2 Point(X, Y);
			Tex2D.At<float>(Point, 0) = R;
			Tex2D.At<float>(Point, 1) = G;
			Tex2D.At<float>(Point, 2) = B;
		}
	}

	// Save as HDR
	const bool bSuccess = SaveImage(OutputPath, Tex2D);
	if (bSuccess)
	{
		std::cout << "Successfully saved Normalized CoCg decode map (Square) to: " << OutputPath << std::endl;
	}
	else
	{
		std::cerr << "Failed to save Normalized CoCg decode map (Square) to: " << OutputPath << std::endl;
	}
}

// Generate normalized CoCg decode map using triangle mapping (100% space utilization)
// Maps UV [0,1]x[0,1] to valid (Co, Cg) triangle
// Output as HDR image
void GenerateNormalizedCoCgDecodeMap_Triangle(const char* OutputPath, float YValue, uint64_t Resolution = 512)
{
	std::cout << "Generating Normalized CoCg decode map (Triangle Mapping) with Y = " << YValue << "..." << std::endl;
	std::cout << "  Resolution: " << Resolution << "x" << Resolution << std::endl;
	std::cout << "  Mapping: UV [0,1]x[0,1] -> Valid CoCg triangle" << std::endl;
	std::cout << "  Valid region: Cg - 1 <= 2*Co <= 1 - Cg" << std::endl;
	std::cout << "  Space utilization: 100%" << std::endl;

	// Create a float texture with 3 channels (RGB)
	FTex2D Tex2D(FGrid2D(Resolution, Resolution), 3, EElementType::Float);

	// Generate the decode map
	// X axis: U in [0, 1]
	// Y axis: V in [0, 1]
	for (uint64_t Y = 0; Y < Resolution; Y++)
	{
		for (uint64_t X = 0; X < Resolution; X++)
		{
			// Map pixel coordinates to UV
			const float U = (float)X / (float)(Resolution - 1); // [0, 1]
			const float V = (float)Y / (float)(Resolution - 1); // [0, 1]

			// Map UV to valid (Co, Cg) in triangle
			float Co, Cg;
			UVToNormalizedCoCg(U, V, Co, Cg);

			// Convert YCoCg to RGB (UCommon normalized version)
			float R, G, B;
			YCoCgToRGB(YValue, Co, Cg, R, G, B);

			// Store RGB values (should all be valid, R,G,B >= 0)
			const FUint64Vector2 Point(X, Y);
			Tex2D.At<float>(Point, 0) = R;
			Tex2D.At<float>(Point, 1) = G;
			Tex2D.At<float>(Point, 2) = B;
		}
	}

	// Save as HDR
	const bool bSuccess = SaveImage(OutputPath, Tex2D);
	if (bSuccess)
	{
		std::cout << "Successfully saved Normalized CoCg decode map (Triangle) to: " << OutputPath << std::endl;
	}
	else
	{
		std::cerr << "Failed to save Normalized CoCg decode map (Triangle) to: " << OutputPath << std::endl;
	}
}

// Generate standard YCoCg decode map (square mapping)
// Co range: [-2Y, 2Y], Cg range: [-Y, Y]
// Output as HDR image
void GenerateStandardCoCgDecodeMap_Square(const char* OutputPath, float YValue, uint64_t Resolution = 512)
{
	std::cout << "Generating Standard YCoCg decode map (Square) with Y = " << YValue << "..." << std::endl;
	std::cout << "  Resolution: " << Resolution << "x" << Resolution << std::endl;
	std::cout << "  Co range: [-" << (2.0f * YValue) << ", " << (2.0f * YValue) << "]" << std::endl;
	std::cout << "  Cg range: [-" << YValue << ", " << YValue << "]" << std::endl;

	// Create a float texture with 3 channels (RGB)
	FTex2D Tex2D(FGrid2D(Resolution, Resolution), 3, EElementType::Float);

	// Generate the decode map
	// X axis: Co from -2Y to 2Y
	// Y axis: Cg from -Y to Y
	for (uint64_t Y = 0; Y < Resolution; Y++)
	{
		for (uint64_t X = 0; X < Resolution; X++)
		{
			// Map pixel coordinates to Co and Cg values
			const float Co = (float)X / (float)(Resolution - 1) * 4.0f * YValue - 2.0f * YValue; // [-2Y, 2Y]
			const float Cg = (float)Y / (float)(Resolution - 1) * 2.0f * YValue - YValue; // [-Y, Y]

			// Convert standard YCoCg to RGB
			float R, G, B;
			StandardYCoCgToRGB(YValue, Co, Cg, R, G, B);

			// Store RGB values
			const FUint64Vector2 Point(X, Y);
			Tex2D.At<float>(Point, 0) = R;
			Tex2D.At<float>(Point, 1) = G;
			Tex2D.At<float>(Point, 2) = B;
		}
	}

	// Save as HDR
	const bool bSuccess = SaveImage(OutputPath, Tex2D);
	if (bSuccess)
	{
		std::cout << "Successfully saved Standard YCoCg decode map (Square) to: " << OutputPath << std::endl;
	}
	else
	{
		std::cerr << "Failed to save Standard YCoCg decode map (Square) to: " << OutputPath << std::endl;
	}
}

int main(int argc, char** argv)
{
	uint64_t Resolution = 512;

	// Parse command line arguments
	if (argc > 1)
	{
		Resolution = std::stoull(argv[1]);
	}

	constexpr float YValue = 1.0f;

	std::cout << "=== Generating CoCg Decode Maps (Y = " << YValue << ") ===" << std::endl;
	std::cout << std::endl;

	// Generate standard YCoCg decode map (square)
	GenerateStandardCoCgDecodeMap_Square("CoCg_Standard_Square.hdr", YValue, Resolution);
	std::cout << std::endl;

	// Generate normalized YCoCg decode map (square)
	GenerateNormalizedCoCgDecodeMap_Square("CoCg_Normalized_Square.hdr", YValue, Resolution);
	std::cout << std::endl;

	// Generate normalized YCoCg decode map (triangle mapping - 100% utilization)
	GenerateNormalizedCoCgDecodeMap_Triangle("CoCg_Normalized_Triangle.hdr", YValue, Resolution);
	std::cout << std::endl;

	std::cout << "=== All decode maps generated successfully ===" << std::endl;

	return 0;
}

