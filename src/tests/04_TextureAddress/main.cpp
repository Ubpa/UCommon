#include <UCommon/Tex2D.h>
#include <UCommon/Utils.h>
#include <iostream>
#include <iomanip>
#include <cassert>

using namespace UCommon;

void TestTextureAddressModes()
{
	std::cout << "Testing Texture Address Modes..." << std::endl;

	// Create a simple 4x4 texture with distinct values
	FTex2D Tex(FGrid2D(4, 4), 1, EElementType::Float);
	for (uint64_t Y = 0; Y < 4; ++Y)
	{
		for (uint64_t X = 0; X < 4; ++X)
		{
			float Value = static_cast<float>(Y * 4 + X);
			Tex.At<float>(FUint64Vector2(X, Y), 0) = Value;
		}
	}

	// Test coordinates (some outside [0, 1])
	std::vector<FVector2f> TestCoords = {
		FVector2f(0.5f, 0.5f),   // Center
		FVector2f(0.0f, 0.0f),   // Corner
		FVector2f(1.0f, 1.0f),   // Opposite corner
		FVector2f(1.5f, 0.5f),   // Outside X (positive)
		FVector2f(-0.5f, 0.5f),  // Outside X (negative)
		FVector2f(0.5f, 1.5f),   // Outside Y (positive)
		FVector2f(0.5f, -0.5f),  // Outside Y (negative)
		FVector2f(2.5f, 2.5f),   // Far outside
		FVector2f(-1.5f, -1.5f), // Far outside (negative)
	};

	std::cout << std::fixed << std::setprecision(3);

	// Test Wrap mode
	std::cout << "\n=== Wrap Mode ===" << std::endl;
	for (const auto& Coord : TestCoords)
	{
		float Result;
		Tex.BilinearSample(&Result, Coord, ETextureAddress::Wrap);
		std::cout << "  UV(" << std::setw(6) << Coord.X << ", " << std::setw(6) << Coord.Y 
		          << ") -> " << std::setw(8) << Result << std::endl;
	}

	// Test Clamp mode
	std::cout << "\n=== Clamp Mode ===" << std::endl;
	for (const auto& Coord : TestCoords)
	{
		float Result;
		Tex.BilinearSample(&Result, Coord, ETextureAddress::Clamp);
		std::cout << "  UV(" << std::setw(6) << Coord.X << ", " << std::setw(6) << Coord.Y 
		          << ") -> " << std::setw(8) << Result << std::endl;
	}

	// Test Mirror mode
	std::cout << "\n=== Mirror Mode ===" << std::endl;
	for (const auto& Coord : TestCoords)
	{
		float Result;
		Tex.BilinearSample(&Result, Coord, ETextureAddress::Mirror);
		std::cout << "  UV(" << std::setw(6) << Coord.X << ", " << std::setw(6) << Coord.Y 
		          << ") -> " << std::setw(8) << Result << std::endl;
	}

	// Verify some specific behaviors
	std::cout << "\n=== Verification ===" << std::endl;

	// Wrap: UV(1.5, 0.5) should be same as UV(0.5, 0.5)
	{
		float Result1, Result2;
		Tex.BilinearSample(&Result1, FVector2f(1.5f, 0.5f), ETextureAddress::Wrap);
		Tex.BilinearSample(&Result2, FVector2f(0.5f, 0.5f), ETextureAddress::Wrap);
		assert(std::abs(Result1 - Result2) < 0.001f);
		std::cout << "  Wrap: UV(1.5, 0.5) == UV(0.5, 0.5): PASSED" << std::endl;
	}

	// Clamp: UV(1.5, 0.5) should clamp to UV(1.0, 0.5)
	{
		float Result1, Result2;
		Tex.BilinearSample(&Result1, FVector2f(1.5f, 0.5f), ETextureAddress::Clamp);
		Tex.BilinearSample(&Result2, FVector2f(1.0f, 0.5f), ETextureAddress::Clamp);
		assert(std::abs(Result1 - Result2) < 0.001f);
		std::cout << "  Clamp: UV(1.5, 0.5) == UV(1.0, 0.5): PASSED" << std::endl;
	}

	// Mirror: UV(1.5, 0.5) should mirror to UV(0.5, 0.5)
	{
		float Result1, Result2;
		Tex.BilinearSample(&Result1, FVector2f(1.5f, 0.5f), ETextureAddress::Mirror);
		Tex.BilinearSample(&Result2, FVector2f(0.5f, 0.5f), ETextureAddress::Mirror);
		assert(std::abs(Result1 - Result2) < 0.001f);
		std::cout << "  Mirror: UV(1.5, 0.5) == UV(0.5, 0.5): PASSED" << std::endl;
	}

	std::cout << "\n=== Boundary Bilinear Interpolation Tests ===" << std::endl;

	// Test Wrap mode at boundaries
	{
		// At UV(0.0, 0.5), should interpolate wrapping around the left edge
		// Texture layout (4x4):
		// 0  1  2  3
		// 4  5  6  7
		// 8  9  10 11
		// 12 13 14 15

		// UV(0.0, 0.5) in pixel space is (0.0, 2.0)
		// Sample point is at (-0.5, 1.5) after -0.5 offset
		// IntPoint0 = floor(-0.5, 1.5) = (-1, 1)
		// IntPoint1 = (-1, 1) + (1, 1) = (0, 2)
		// With Wrap: (-1, 1) wraps to (3, 1), (0, 2) stays (0, 2)
		// Should interpolate between pixels: (3,1)=7, (3,2)=11, (0,1)=4, (0,2)=8
		float Result;
		Tex.BilinearSample(&Result, FVector2f(0.0f, 0.5f), ETextureAddress::Wrap);
		// LocalTexcoord = ((0.0, 2.0) - ((-1, 1) + (0.5, 0.5))).Clamp(0, 1) = (0.5, 0.5)
		// Result = 0.25 * (7 + 11 + 4 + 8) = 0.25 * 30 = 7.5
		assert(std::abs(Result - 7.5f) < 0.001f);
		std::cout << "  Wrap at UV(0.0, 0.5): " << Result << " (expected 7.5): PASSED" << std::endl;
	}

	{
		// At UV(1.0, 0.5), should wrap to UV(0.0, 0.5) in Wrap mode
		float Result1, Result2;
		Tex.BilinearSample(&Result1, FVector2f(1.0f, 0.5f), ETextureAddress::Wrap);
		Tex.BilinearSample(&Result2, FVector2f(0.0f, 0.5f), ETextureAddress::Wrap);
		assert(std::abs(Result1 - Result2) < 0.001f);
		std::cout << "  Wrap at UV(1.0, 0.5) == UV(0.0, 0.5): PASSED" << std::endl;
	}

	// Test Clamp mode at boundaries
	{
		// At UV(0.0, 0.5), should clamp to edge
		// IntPoint0 = floor(-0.5, 1.5) = (-1, 1)
		// IntPoint1 = (-1, 1) + (1, 1) = (0, 2)
		// With Clamp: (-1, 1) clamps to (0, 1), (0, 2) stays (0, 2)
		// Should interpolate between pixels: (0,1)=4, (0,2)=8, (0,1)=4, (0,2)=8
		// LocalTexcoord = (0.5, 0.5)
		// Result = 0.25 * (4 + 8 + 4 + 8) = 0.25 * 24 = 6.0
		float Result;
		Tex.BilinearSample(&Result, FVector2f(0.0f, 0.5f), ETextureAddress::Clamp);
		assert(std::abs(Result - 6.0f) < 0.001f);
		std::cout << "  Clamp at UV(0.0, 0.5): " << Result << " (expected 6.0): PASSED" << std::endl;
	}

	{
		// At UV(1.0, 0.5), should clamp to edge
		// IntPoint0 = (3, 1), IntPoint1 = (4, 2)
		// With Clamp: (3, 1) stays (3, 1), (4, 2) clamps to (3, 2)
		// Should interpolate between pixels: (3,1)=7, (3,2)=11, (3,1)=7, (3,2)=11
		// Result = 0.25 * (7 + 11 + 7 + 11) = 0.25 * 36 = 9.0
		float Result;
		Tex.BilinearSample(&Result, FVector2f(1.0f, 0.5f), ETextureAddress::Clamp);
		assert(std::abs(Result - 9.0f) < 0.001f);
		std::cout << "  Clamp at UV(1.0, 0.5): " << Result << " (expected 9.0): PASSED" << std::endl;
	}

	// Test Mirror mode at boundaries
	{
		// At UV(0.0, 0.5), should mirror
		// IntPoint0 = floor(-0.5, 1.5) = (-1, 1)
		// IntPoint1 = (-1, 1) + (1, 1) = (0, 2)
		// With Mirror: (-1, 1) mirrors to (0, 1), (0, 2) stays (0, 2)
		// Should interpolate between pixels: (0,1)=4, (0,2)=8, (0,1)=4, (0,2)=8
		// LocalTexcoord = (0.5, 0.5)
		// Result = 0.25 * (4 + 8 + 4 + 8) = 0.25 * 24 = 6.0
		float Result;
		Tex.BilinearSample(&Result, FVector2f(0.0f, 0.5f), ETextureAddress::Mirror);
		assert(std::abs(Result - 6.0f) < 0.001f);
		std::cout << "  Mirror at UV(0.0, 0.5): " << Result << " (expected 6.0): PASSED" << std::endl;
	}

	{
		// At UV(1.0, 0.5), should be at the edge (no mirroring at exactly 1.0)
		// IntPoint0 = (3, 1), IntPoint1 = (4, 2)
		// With Mirror: (3, 1) stays (3, 1), (4, 2) mirrors to (3, 2)
		// Should interpolate between pixels: (3,1)=7, (3,2)=11, (3,1)=7, (3,2)=11
		// Result = 0.25 * (7 + 11 + 7 + 11) = 0.25 * 36 = 9.0
		float Result;
		Tex.BilinearSample(&Result, FVector2f(1.0f, 0.5f), ETextureAddress::Mirror);
		assert(std::abs(Result - 9.0f) < 0.001f);
		std::cout << "  Mirror at UV(1.0, 0.5): " << Result << " (expected 9.0): PASSED" << std::endl;
	}

	// Test corner cases
	{
		// At UV(0.0, 0.0) with Wrap, should wrap around all corners
		float Result;
		Tex.BilinearSample(&Result, FVector2f(0.0f, 0.0f), ETextureAddress::Wrap);
		// IntPoint0 = floor(-0.5, -0.5) = (-1, -1)
		// IntPoint1 = (-1, -1) + (1, 1) = (0, 0)
		// With Wrap: (-1, -1) wraps to (3, 3), (0, 0) stays (0, 0)
		// Pixels: (3,3)=15, (3,0)=3, (0,3)=12, (0,0)=0
		// LocalTexcoord = (0.5, 0.5)
		// Result = 0.25 * (15 + 3 + 12 + 0) = 0.25 * 30 = 7.5
		assert(std::abs(Result - 7.5f) < 0.001f);
		std::cout << "  Wrap at UV(0.0, 0.0): " << Result << " (expected 7.5): PASSED" << std::endl;
	}

	{
		// At UV(0.0, 0.0) with Clamp, should clamp to (0, 0)
		float Result;
		Tex.BilinearSample(&Result, FVector2f(0.0f, 0.0f), ETextureAddress::Clamp);
		// All pixels clamp to (0, 0) = 0
		assert(std::abs(Result - 0.0f) < 0.001f);
		std::cout << "  Clamp at UV(0.0, 0.0): " << Result << " (expected 0.0): PASSED" << std::endl;
	}

	{
		// At UV(1.0, 1.0) with Clamp, should clamp to (3, 3)
		float Result;
		Tex.BilinearSample(&Result, FVector2f(1.0f, 1.0f), ETextureAddress::Clamp);
		// All pixels clamp to (3, 3) = 15
		assert(std::abs(Result - 15.0f) < 0.001f);
		std::cout << "  Clamp at UV(1.0, 1.0): " << Result << " (expected 15.0): PASSED" << std::endl;
	}

	std::cout << "\nAll texture address mode tests passed!" << std::endl;
}

int main(int argc, char** argv)
{
	TestTextureAddressModes();
	return 0;
}

