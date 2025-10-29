#include <UCommon/UCommon.h>

#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace UCommon;

constexpr float Epsilon = 1e-5f;

bool IsNearlyEqual(float A, float B, float Tolerance = Epsilon)
{
	return std::abs(A - B) < Tolerance;
}

bool IsNearlyEqual(const FVector3f& A, const FVector3f& B, float Tolerance = Epsilon)
{
	return IsNearlyEqual(A.X, B.X, Tolerance) &&
	       IsNearlyEqual(A.Y, B.Y, Tolerance) &&
	       IsNearlyEqual(A.Z, B.Z, Tolerance);
}

void TestRGBToYCoCgRoundtrip()
{
	std::cout << "Testing RGB <-> YCoCg roundtrip conversion..." << std::endl;

	// Test cases with various positive RGB values
	std::vector<FVector3f> TestCases = {
		// Primary colors
		FVector3f(1.0f, 0.0f, 0.0f),    // Red
		FVector3f(0.0f, 1.0f, 0.0f),    // Green
		FVector3f(0.0f, 0.0f, 1.0f),    // Blue

		// Grayscale
		FVector3f(1.0f, 1.0f, 1.0f),    // White
		FVector3f(0.5f, 0.5f, 0.5f),    // Gray
		FVector3f(0.25f, 0.25f, 0.25f), // Dark gray

		// Mixed colors
		FVector3f(0.8f, 0.3f, 0.2f),    // Orange-ish
		FVector3f(0.2f, 0.6f, 0.9f),    // Sky blue
		FVector3f(0.9f, 0.9f, 0.1f),    // Yellow
		FVector3f(0.5f, 0.0f, 0.5f),    // Purple
		FVector3f(0.0f, 0.8f, 0.4f),    // Cyan-green

		// Large values (HDR)
		FVector3f(10.0f, 5.0f, 2.0f),
		FVector3f(2.5f, 3.0f, 1.5f),

		// Small values
		FVector3f(0.01f, 0.02f, 0.03f),
		FVector3f(0.001f, 0.005f, 0.002f),
	};

	int PassedCount = 0;
	int FailedCount = 0;

	for (size_t i = 0; i < TestCases.size(); ++i)
	{
		const FVector3f& OriginalRGB = TestCases[i];

		// Normalize RGB to get hue (Y = 4)
		//float Y = (OriginalRGB.X + 2.0f * OriginalRGB.Y + OriginalRGB.Z) / 4.0f;
		//FVector3f NormalizedHue = OriginalRGB / Y;

		// Convert normalized hue to YCoCg
		FLinearColorRGB YCoCg = RGBToYCoCg(OriginalRGB);

		// Convert back to normalized hue
		FLinearColorRGB RecoveredHue = YCoCgToRGB(YCoCg);

		// Check if roundtrip is successful for normalized hue
		if (IsNearlyEqual(OriginalRGB, RecoveredHue, 1e-3f))
		{
			PassedCount++;
		}
		else
		{
			FailedCount++;
			std::cout << "  [FAILED] Test case " << i << ": RGB("
			          << OriginalRGB.X << ", " << OriginalRGB.Y << ", " << OriginalRGB.Z << ")" << std::endl;
			//std::cout << "    Normalized Hue: (" << NormalizedHue.X << ", " << NormalizedHue.Y << ", " << NormalizedHue.Z << ")" << std::endl;
			std::cout << "    YCoCg: (" << YCoCg.X << ", " << YCoCg.Y << ", " << YCoCg.Z << ")" << std::endl;
			std::cout << "    Recovered Hue: (" << RecoveredHue.X << ", " << RecoveredHue.Y << ", " << RecoveredHue.Z << ")" << std::endl;
			std::cout << "    Difference: ("
			          << (RecoveredHue.X - OriginalRGB.X) << ", "
			          << (RecoveredHue.Y - OriginalRGB.Y) << ", "
			          << (RecoveredHue.Z - OriginalRGB.Z) << ")" << std::endl;
		}
	}

	std::cout << "  Passed: " << PassedCount << "/" << TestCases.size() << std::endl;
	std::cout << "  Failed: " << FailedCount << "/" << TestCases.size() << std::endl;

	assert(FailedCount == 0 && "Some RGB <-> YCoCg roundtrip tests failed!");
}

void TestYCoCgProperties()
{
	std::cout << "Testing YCoCg properties..." << std::endl;

	// Test that Co and Cg are in expected range [-2, 2] for normalized hue (Y=4)
	std::vector<FVector3f> TestRGBs = {
		FVector3f(1.0f, 0.0f, 0.0f),
		FVector3f(0.0f, 1.0f, 0.0f),
		FVector3f(0.0f, 0.0f, 1.0f),
		FVector3f(1.0f, 1.0f, 1.0f),
		FVector3f(0.5f, 0.5f, 0.5f),
		FVector3f(0.8f, 0.3f, 0.2f),
		FVector3f(0.2f, 0.6f, 0.9f),
	};

	bool bAllInRange = true;
	for (const auto& RGB : TestRGBs)
	{
		// Normalize to Y=4
		float Y = (RGB.X + 2.0f * RGB.Y + RGB.Z) / 4.0f;
		FVector3f NormalizedHue = RGB / Y * 4.0f;

		FLinearColorRGB YCoCg = RGBToYCoCg(NormalizedHue);

		// Check Y should be 4
		if (!IsNearlyEqual(YCoCg.X, 4.0f, 0.01f))
		{
			std::cout << "  [WARNING] Y is not 4.0: " << YCoCg.X << std::endl;
			bAllInRange = false;
		}

		// Check Co and Cg are in [-2, 2]
		if (std::abs(YCoCg.Y) > 2.0f || std::abs(YCoCg.Z) > 2.0f)
		{
			bAllInRange = false;
			std::cout << "  [WARNING] RGB(" << RGB.X << ", " << RGB.Y << ", " << RGB.Z
			          << ") -> Normalized(" << NormalizedHue.X << ", " << NormalizedHue.Y << ", " << NormalizedHue.Z << ")"
			          << " -> YCoCg(" << YCoCg.X << ", " << YCoCg.Y << ", " << YCoCg.Z
			          << ") - Co or Cg out of [-2, 2] range" << std::endl;
		}
	}

	if (bAllInRange)
	{
		std::cout << "  All normalized hue values produce Y=4 and Co, Cg in [-2, 2] range" << std::endl;
	}
}

void TestFPackedHue()
{
	std::cout << "Testing FPackedHue..." << std::endl;

	// Test 1: Zero hue initialization
	{
		FPackedHue ZeroHue(EForceInit::Default);
		assert(ZeroHue.Co == 128 && ZeroHue.Cg == 128);
		std::cout << "  Zero hue initialization: PASSED" << std::endl;
	}

	// Test 2: Direct uint8_t construction
	{
		FPackedHue DirectHue(100, 200);
		assert(DirectHue.Co == 100 && DirectHue.Cg == 200);
		std::cout << "  Direct uint8_t construction: PASSED" << std::endl;
	}

	// Test 3: CoCg vector construction and roundtrip
	{
		std::vector<FVector2f> CoCgTestCases = {
			FVector2f(0.0f, 0.0f),    // Center
			FVector2f(-2.0f, -2.0f),  // Min corner
			FVector2f(2.0f, 2.0f),    // Max corner
			FVector2f(-1.0f, 1.0f),   // Mixed
			FVector2f(1.5f, -0.5f),   // Mixed
		};

		int PassedCount = 0;
		for (const auto& CoCg : CoCgTestCases)
		{
			FPackedHue Packed(CoCg);
			
			// Unpack and check (note: Unpack returns full RGB with Y=4)
			FVector3f UnpackedRGB = Packed.Unpack();
			
			// Extract Co and Cg from unpacked RGB
			// From the Unpack implementation:
			// Cof = Co / 255 * 4 - 2
			// Cgf = Cg / 255 * 4 - 2
			float UnpackedCo = ElementUint8ToFloat(Packed.Co) * 4.0f - 2.0f;
			float UnpackedCg = ElementUint8ToFloat(Packed.Cg) * 4.0f - 2.0f;
			
			// Check if roundtrip is close (with quantization tolerance)
			float Tolerance = 4.0f / 255.0f; // One quantization step
			if (IsNearlyEqual(CoCg.X, UnpackedCo, Tolerance) &&
			    IsNearlyEqual(CoCg.Y, UnpackedCg, Tolerance))
			{
				PassedCount++;
			}
			else
			{
				std::cout << "  [FAILED] CoCg(" << CoCg.X << ", " << CoCg.Y << ")" << std::endl;
				std::cout << "    Packed: Co=" << (int)Packed.Co << ", Cg=" << (int)Packed.Cg << std::endl;
				std::cout << "    Unpacked: Co=" << UnpackedCo << ", Cg=" << UnpackedCg << std::endl;
			}
		}
		
		std::cout << "  CoCg vector roundtrip: " << PassedCount << "/" << CoCgTestCases.size() << " PASSED" << std::endl;
		assert(PassedCount == CoCgTestCases.size());
	}

	// Test 4: RGB hue construction (normalized to Y=4)
	{
		// Test cases: start with arbitrary positive RGB, then normalize to R + 2G + B = 4
		std::vector<FVector3f> TestRGBs = {
			FVector3f(1.0f, 0.0f, 0.0f),    // Red
			FVector3f(0.0f, 1.0f, 0.0f),    // Green
			FVector3f(0.0f, 0.0f, 1.0f),    // Blue
			FVector3f(1.0f, 1.0f, 1.0f),    // White
			FVector3f(0.8f, 0.3f, 0.2f),    // Orange
			FVector3f(0.2f, 0.6f, 0.9f),    // Sky blue
		};

		std::vector<FVector3f> HueTestCases;
		for (const auto& RGB : TestRGBs)
		{
			// Normalize to R + 2G + B = 4
			float Sum = RGB.X + 2.0f * RGB.Y + RGB.Z;
			FVector3f NormalizedHue = RGB / Sum * 4.0f;
			HueTestCases.push_back(NormalizedHue);
		}

		int PassedCount = 0;
		for (size_t i = 0; i < HueTestCases.size(); ++i)
		{
			const FVector3f& Hue = HueTestCases[i];

			// Verify constraint
			float Sum = Hue.X + 2.0f * Hue.Y + Hue.Z;
			assert(IsNearlyEqual(Sum, 4.0f, 0.001f) && "Hue should satisfy R + 2G + B = 4");

			FPackedHue Packed(Hue);
			FVector3f Unpacked = Packed.Unpack();

			// Check roundtrip with quantization tolerance
			float Tolerance = 8.0f / 255.0f; // Quantization tolerance
			if (IsNearlyEqual(Hue, Unpacked, Tolerance))
			{
				PassedCount++;
			}
			else
			{
				std::cout << "  [FAILED] Hue(" << Hue.X << ", " << Hue.Y << ", " << Hue.Z << ")" << std::endl;
				std::cout << "    Packed: Co=" << (int)Packed.Co << ", Cg=" << (int)Packed.Cg << std::endl;
				std::cout << "    Unpacked: (" << Unpacked.X << ", " << Unpacked.Y << ", " << Unpacked.Z << ")" << std::endl;
				std::cout << "    Difference: ("
				          << (Unpacked.X - Hue.X) << ", "
				          << (Unpacked.Y - Hue.Y) << ", "
				          << (Unpacked.Z - Hue.Z) << ")" << std::endl;
			}
		}

		std::cout << "  RGB hue roundtrip: " << PassedCount << "/" << HueTestCases.size() << " PASSED" << std::endl;
		assert(PassedCount == HueTestCases.size());
	}
}

int main(int argc, char** argv)
{
	std::cout << std::fixed << std::setprecision(6);
	
	TestRGBToYCoCgRoundtrip();
	std::cout << std::endl;
	
	TestYCoCgProperties();
	std::cout << std::endl;
	
	TestFPackedHue();
	std::cout << std::endl;
	
	std::cout << "All tests passed!" << std::endl;
	
	return 0;
}

