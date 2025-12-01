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

	// Test that Co and Cg are in expected range [-1, 1] for normalized hue (Y=4)
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

		// Check Co and Cg are in [-1, 1]
		if (std::abs(YCoCg.Y) > 1.0f || std::abs(YCoCg.Z) > 1.0f)
		{
			bAllInRange = false;
			std::cout << "  [WARNING] RGB(" << RGB.X << ", " << RGB.Y << ", " << RGB.Z
			          << ") -> Normalized(" << NormalizedHue.X << ", " << NormalizedHue.Y << ", " << NormalizedHue.Z << ")"
			          << " -> YCoCg(" << YCoCg.X << ", " << YCoCg.Y << ", " << YCoCg.Z
			          << ") - Co or Cg out of [-1, 1] range" << std::endl;
		}
	}

	if (bAllInRange)
	{
		std::cout << "  All normalized hue values produce Y=4 and Co, Cg in [-1, 1] range" << std::endl;
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

	// Test 3: CoCg vector construction and roundtrip (using triangle mapping)
	{
		// Valid CoCg values must satisfy: Cg - 1 <= 2*Co <= 1 - Cg
		std::vector<FVector2f> CoCgTestCases = {
			FVector2f(0.0f, 0.0f),     // Center
			FVector2f(0.0f, -1.0f),    // Bottom vertex (Co=0, Cg=-1)
			FVector2f(0.5f, 0.0f),     // Right edge (Co=0.5, Cg=0)
			FVector2f(-0.5f, 0.0f),    // Left edge (Co=-0.5, Cg=0)
			FVector2f(0.0f, 1.0f),     // Top vertex (Co=0, Cg=1)
			FVector2f(0.25f, 0.5f),    // Inside triangle
			FVector2f(-0.25f, 0.5f),   // Inside triangle
			FVector2f(0.4f, -0.2f),    // Inside triangle
		};

		int PassedCount = 0;
		for (const auto& CoCg : CoCgTestCases)
		{
			// Verify the input is in valid triangle
			const bool bIsValid = (CoCg.Y - 1.0f <= 2.0f * CoCg.X) && (2.0f * CoCg.X <= 1.0f - CoCg.Y);
			if (!bIsValid)
			{
				std::cout << "  [SKIPPED] CoCg(" << CoCg.X << ", " << CoCg.Y << ") is outside valid triangle" << std::endl;
				continue;
			}

			FPackedHue Packed(CoCg);

			// Unpack and check (note: Unpack returns full RGB with Y=1)
			FVector3f UnpackedRGB = Packed.Unpack();

			// Convert unpacked RGB back to CoCg to verify roundtrip
			float Y, UnpackedCo, UnpackedCg;
			RGBToYCoCg(UnpackedRGB.X, UnpackedRGB.Y, UnpackedRGB.Z, Y, UnpackedCo, UnpackedCg);

			// Check if roundtrip is close (with quantization tolerance)
			// Triangle mapping has variable quantization depending on position
			const float CoRange = (1.0f - CoCg.Y) - (CoCg.Y - 1.0f); // Width of valid Co range at this Cg
			const float ToleranceCo = std::max(CoRange / 255.0f * 2.0f, 0.01f); // Quantization step for Co at this Cg, with minimum tolerance
			const float ToleranceCg = 2.0f / 255.0f; // Quantization step for Cg

			if (IsNearlyEqual(CoCg.X, UnpackedCo, ToleranceCo) &&
			    IsNearlyEqual(CoCg.Y, UnpackedCg, ToleranceCg))
			{
				PassedCount++;
			}
			else
			{
				std::cout << "  [FAILED] CoCg(" << CoCg.X << ", " << CoCg.Y << ")" << std::endl;
				std::cout << "    Packed: Co=" << (int)Packed.Co << ", Cg=" << (int)Packed.Cg << std::endl;
				std::cout << "    Unpacked: Co=" << UnpackedCo << ", Cg=" << UnpackedCg << std::endl;
				std::cout << "    Tolerance: Co=" << ToleranceCo << ", Cg=" << ToleranceCg << std::endl;
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

void TestVectorToHemiOctLRoundtrip()
{
	std::cout << "Testing Vector <-> HemiOctL roundtrip conversion..." << std::endl;

	// Test cases with various vectors (Z >= 0 only, upper hemisphere)
	std::vector<FVector3f> TestVectors = {
		// Unit vectors along axes (Z >= 0)
		FVector3f(1.0f, 0.0f, 0.0f),
		FVector3f(0.0f, 1.0f, 0.0f),
		FVector3f(0.0f, 0.0f, 1.0f),
		FVector3f(-1.0f, 0.0f, 0.0f),
		FVector3f(0.0f, -1.0f, 0.0f),

		// Diagonal vectors (Z >= 0)
		FVector3f(1.0f, 1.0f, 1.0f),
		FVector3f(1.0f, -1.0f, 1.0f),
		FVector3f(-1.0f, 1.0f, 1.0f),
		FVector3f(-1.0f, -1.0f, 1.0f),

		// Normalized vectors (Z >= 0)
		FVector3f(0.577f, 0.577f, 0.577f),  // Normalized (1,1,1)
		FVector3f(0.707f, 0.707f, 0.0f),    // Normalized (1,1,0)
		FVector3f(0.8f, 0.6f, 0.0f),
		FVector3f(0.267f, 0.535f, 0.802f),

		// Vectors with different lengths (Z >= 0)
		FVector3f(2.0f, 3.0f, 1.0f),
		FVector3f(10.0f, 5.0f, 2.0f),
		FVector3f(0.1f, 0.2f, 0.3f),
		FVector3f(0.01f, 0.02f, 0.03f),

		// Vectors on XY plane (Z = 0)
		FVector3f(1.0f, 0.0f, 0.0f),
		FVector3f(0.0f, 1.0f, 0.0f),
		FVector3f(1.0f, 1.0f, 0.0f),

		// Near-zero vector
		FVector3f(1e-6f, 1e-6f, 1e-6f),

		// Zero vector
		FVector3f(0.0f, 0.0f, 0.0f),
	};

	int PassedCount = 0;
	int FailedCount = 0;

	for (size_t i = 0; i < TestVectors.size(); ++i)
	{
		const FVector3f& OriginalVector = TestVectors[i];

		// Convert Vector to HemiOctL
		FVector3f HemiOctL = VectorToHemiOctL(OriginalVector);

		// Convert back to Vector
		FVector3f RecoveredVector = HemiOctLToVector(HemiOctL);

		// For zero or near-zero vectors, use absolute tolerance
		float Length = std::sqrt(OriginalVector.X * OriginalVector.X +
		                         OriginalVector.Y * OriginalVector.Y +
		                         OriginalVector.Z * OriginalVector.Z);
		float Tolerance = (Length < 1e-5f) ? 1e-5f : Length * 1e-4f;

		// Check if roundtrip is successful
		if (IsNearlyEqual(OriginalVector, RecoveredVector, Tolerance))
		{
			PassedCount++;
		}
		else
		{
			FailedCount++;
			std::cout << "  [FAILED] Test case " << i << ": Vector("
			          << OriginalVector.X << ", " << OriginalVector.Y << ", " << OriginalVector.Z << ")" << std::endl;
			std::cout << "    HemiOctL: (" << HemiOctL.X << ", " << HemiOctL.Y << ", " << HemiOctL.Z << ")" << std::endl;
			std::cout << "    Recovered Vector: (" << RecoveredVector.X << ", " << RecoveredVector.Y << ", " << RecoveredVector.Z << ")" << std::endl;
			std::cout << "    Difference: ("
			          << (RecoveredVector.X - OriginalVector.X) << ", "
			          << (RecoveredVector.Y - OriginalVector.Y) << ", "
			          << (RecoveredVector.Z - OriginalVector.Z) << ")" << std::endl;
		}
	}

	std::cout << "  Passed: " << PassedCount << "/" << TestVectors.size() << std::endl;
	std::cout << "  Failed: " << FailedCount << "/" << TestVectors.size() << std::endl;

	assert(FailedCount == 0 && "Some Vector <-> HemiOctL roundtrip tests failed!");
}

void TestHemiOctLProperties()
{
	std::cout << "Testing HemiOctL properties..." << std::endl;

	// Test that HemiOct.X and HemiOct.Y are in [-1, 1] range for normalized vectors
	std::vector<FVector3f> TestVectors = {
		FVector3f(1.0f, 0.0f, 0.0f),
		FVector3f(0.0f, 1.0f, 0.0f),
		FVector3f(0.0f, 0.0f, 1.0f),
		FVector3f(0.577f, 0.577f, 0.577f),
		FVector3f(0.707f, 0.707f, 0.0f),
		FVector3f(-0.8f, 0.6f, 0.0f),
	};

	bool bAllInRange = true;
	for (const auto& Vector : TestVectors)
	{
		FVector3f HemiOctL = VectorToHemiOctL(Vector);

		// Check HemiOct.X and HemiOct.Y are in [-1, 1]
		if (std::abs(HemiOctL.X) > 1.0f || std::abs(HemiOctL.Y) > 1.0f)
		{
			bAllInRange = false;
			std::cout << "  [WARNING] Vector(" << Vector.X << ", " << Vector.Y << ", " << Vector.Z
			          << ") -> HemiOctL(" << HemiOctL.X << ", " << HemiOctL.Y << ", " << HemiOctL.Z
			          << ") - HemiOct.X or HemiOct.Y out of [-1, 1] range" << std::endl;
		}

		// Check that |HemiOct.X| + |HemiOct.Y| <= 1 for unit vectors
		float VectorLength = std::sqrt(Vector.X * Vector.X + Vector.Y * Vector.Y + Vector.Z * Vector.Z);
		if (std::abs(VectorLength - 1.0f) < 0.01f)  // If approximately unit length
		{
			float HemiOctSum = std::abs(HemiOctL.X) + std::abs(HemiOctL.Y);
			if (HemiOctSum > 1.01f)  // Allow small tolerance
			{
				bAllInRange = false;
				std::cout << "  [WARNING] Unit vector produces |HemiOct.X| + |HemiOct.Y| = " << HemiOctSum
				          << " > 1.0" << std::endl;
			}
		}
	}

	if (bAllInRange)
	{
		std::cout << "  All vectors produce valid hemispherical octahedral coordinates" << std::endl;
	}
}

void TestFPackedHemiOct()
{
	std::cout << "Testing FPackedHemiOct..." << std::endl;

	// Test 1: Zero initialization
	{
		FPackedHemiOct ZeroHemiOct(EForceInit::Default);
		assert(ZeroHemiOct.U == 213 && ZeroHemiOct.V == 128);
		std::cout << "  Zero initialization: PASSED" << std::endl;
	}

	// Test 2: Direct uint8_t construction
	{
		FPackedHemiOct DirectHemiOct(100, 200);
		assert(DirectHemiOct.U == 100 && DirectHemiOct.V == 200);
		std::cout << "  Direct uint8_t construction: PASSED" << std::endl;
	}

	// Test 3: HemiOct vector roundtrip (HemiOct.X, HemiOct.Y in [-1, 1] with |x|+|y|<=1)
	{
		std::vector<FVector2f> HemiOctTestCases = {
			FVector2f(0.0f, 0.0f),
			FVector2f(1.0f, 0.0f),
			FVector2f(0.0f, 1.0f),
			FVector2f(-1.0f, 0.0f),
			FVector2f(0.0f, -1.0f),
			FVector2f(0.5f, 0.5f),
			FVector2f(-0.5f, -0.5f),
			FVector2f(0.7f, -0.3f),
		};

		int PassedCount = 0;
		for (const auto& HemiOct : HemiOctTestCases)
		{
			FPackedHemiOct Packed(HemiOct);

			// Unpack and check
			FVector2f Unpacked = Packed.Unpack();

			// Extract HemiOct.X and HemiOct.Y from unpacked
			float UnpackedHemiOctX = Unpacked.X;
			float UnpackedHemiOctY = Unpacked.Y;

			// Check if roundtrip is close (with quantization tolerance)
			float Tolerance = 2.0f / 255.0f; // One quantization step
			if (IsNearlyEqual(HemiOct.X, UnpackedHemiOctX, Tolerance) &&
			    IsNearlyEqual(HemiOct.Y, UnpackedHemiOctY, Tolerance))
			{
				PassedCount++;
			}
			else
			{
				std::cout << "  [FAILED] HemiOct(" << HemiOct.X << ", " << HemiOct.Y << ")" << std::endl;
				std::cout << "    Packed: U=" << (int)Packed.U << ", V=" << (int)Packed.V << std::endl;
				std::cout << "    Unpacked: (" << Unpacked.X << ", " << Unpacked.Y << ")" << std::endl;
			}
		}

		std::cout << "  HemiOct vector roundtrip: " << PassedCount << "/" << HemiOctTestCases.size() << " PASSED" << std::endl;
		assert(PassedCount == HemiOctTestCases.size());
	}

	// Test 4: Full pipeline - Vector -> HemiOctL -> FPackedHemiOct -> Unpack -> HemiOctLToVector
	{
		std::vector<FVector3f> TestVectors = {
			FVector3f(1.0f, 0.0f, 0.0f),
			FVector3f(0.0f, 1.0f, 0.0f),
			FVector3f(0.0f, 0.0f, 1.0f),
			FVector3f(0.577f, 0.577f, 0.577f),
			FVector3f(0.707f, 0.707f, 0.0f),
			FVector3f(2.0f, 3.0f, 1.0f),
		};

		int PassedCount = 0;
		for (const auto& Vector : TestVectors)
		{
			// Convert to HemiOctL
			FVector3f HemiOctL = VectorToHemiOctL(Vector);

			// Pack HemiOct coordinates
			FPackedHemiOct Packed(FVector2f(HemiOctL.X, HemiOctL.Y));

			// Unpack
			FVector2f UnpackedHemiOct = Packed.Unpack();

			// Reconstruct vector (need to restore length)
			FVector3f RecoveredVector = UCommon::HemiOctToDir(UnpackedHemiOct) * HemiOctL.Z;

			// Check roundtrip
			float Length = std::sqrt(Vector.X * Vector.X + Vector.Y * Vector.Y + Vector.Z * Vector.Z);
			float Tolerance = Length * 0.01f; // 1% tolerance due to quantization

			if (IsNearlyEqual(Vector, RecoveredVector, Tolerance))
			{
				PassedCount++;
			}
			else
			{
				std::cout << "  [FAILED] Full pipeline for Vector("
				          << Vector.X << ", " << Vector.Y << ", " << Vector.Z << ")" << std::endl;
				std::cout << "    HemiOctL: (" << HemiOctL.X << ", " << HemiOctL.Y << ", " << HemiOctL.Z << ")" << std::endl;
				std::cout << "    Packed: U=" << (int)Packed.U << ", V=" << (int)Packed.V << std::endl;
				std::cout << "    Unpacked HemiOct: (" << UnpackedHemiOct.X << ", " << UnpackedHemiOct.Y << ")" << std::endl;
				std::cout << "    Recovered Vector: (" << RecoveredVector.X << ", " << RecoveredVector.Y << ", " << RecoveredVector.Z << ")" << std::endl;
			}
		}

		std::cout << "  Full pipeline roundtrip: " << PassedCount << "/" << TestVectors.size() << " PASSED" << std::endl;
		assert(PassedCount == TestVectors.size());
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

	TestVectorToHemiOctLRoundtrip();
	std::cout << std::endl;

	TestHemiOctLProperties();
	std::cout << std::endl;

	TestFPackedHemiOct();
	std::cout << std::endl;

	std::cout << "All tests passed!" << std::endl;

	return 0;
}

