#include <UCommon/Tex2D.h>
#include <UCommon/Half.h>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cmath>

using namespace UCommon;

// Helper function to compare floats with tolerance
bool FloatEqual(float A, float B, float Tolerance = 0.001f)
{
	return std::abs(A - B) < Tolerance;
}

void TestClampUint8()
{
	std::cout << "Testing Clamp with Uint8..." << std::endl;

	// Create a 2x2 texture with Uint8 type
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Uint8);

	// Set values: 0.0, 0.5, 0.75, 1.0 (as uint8: 0, 128, 191, 255)
	Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) = 0;
	Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) = 128;
	Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) = 191;
	Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) = 255;

	// Clamp to [0.25, 0.75]
	Tex.Clamp(0.25f, 0.75f);

	// Check results (0.25 = 64, 0.75 = 191)
	assert(Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) == 64);  // 0 -> 64
	assert(Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) == 128); // 128 stays
	assert(Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) == 191); // 191 stays
	assert(Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) == 191); // 255 -> 191

	std::cout << "  Clamp Uint8: PASSED" << std::endl;
}

void TestMinUint8()
{
	std::cout << "Testing Min with Uint8..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Uint8);

	// Set values: 0.0, 0.3, 0.6, 1.0
	Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) = 0;
	Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) = 77;  // ~0.3
	Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) = 153; // ~0.6
	Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) = 255;

	// Min to 0.5 (128)
	Tex.Min(0.5f);

	assert(Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) == 128); // 0 -> 128
	assert(Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) == 128); // 77 -> 128
	assert(Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) == 153); // 153 stays
	assert(Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) == 255); // 255 stays

	std::cout << "  Min Uint8: PASSED" << std::endl;
}

void TestMaxUint8()
{
	std::cout << "Testing Max with Uint8..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Uint8);

	// Set values
	Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) = 0;
	Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) = 128;
	Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) = 191;
	Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) = 255;

	// Max to 0.5 (128)
	Tex.Max(0.5f);

	assert(Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) == 0);   // 0 stays
	assert(Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) == 128); // 128 stays
	assert(Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) == 128); // 191 -> 128
	assert(Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) == 128); // 255 -> 128

	std::cout << "  Max Uint8: PASSED" << std::endl;
}

void TestClampFloat()
{
	std::cout << "Testing Clamp with Float..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Float);

	// Set values
	Tex.At<float>(FUint64Vector2(0, 0), 0) = -1.0f;
	Tex.At<float>(FUint64Vector2(1, 0), 0) = 0.5f;
	Tex.At<float>(FUint64Vector2(0, 1), 0) = 1.5f;
	Tex.At<float>(FUint64Vector2(1, 1), 0) = 2.0f;

	// Clamp to [0.0, 1.0]
	Tex.Clamp(0.0f, 1.0f);

	assert(FloatEqual(Tex.At<float>(FUint64Vector2(0, 0), 0), 0.0f));
	assert(FloatEqual(Tex.At<float>(FUint64Vector2(1, 0), 0), 0.5f));
	assert(FloatEqual(Tex.At<float>(FUint64Vector2(0, 1), 0), 1.0f));
	assert(FloatEqual(Tex.At<float>(FUint64Vector2(1, 1), 0), 1.0f));

	std::cout << "  Clamp Float: PASSED" << std::endl;
}

void TestMinFloat()
{
	std::cout << "Testing Min with Float..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Float);

	Tex.At<float>(FUint64Vector2(0, 0), 0) = -1.0f;
	Tex.At<float>(FUint64Vector2(1, 0), 0) = 0.5f;
	Tex.At<float>(FUint64Vector2(0, 1), 0) = 1.5f;
	Tex.At<float>(FUint64Vector2(1, 1), 0) = 2.0f;

	Tex.Min(0.0f);

	assert(FloatEqual(Tex.At<float>(FUint64Vector2(0, 0), 0), 0.0f));
	assert(FloatEqual(Tex.At<float>(FUint64Vector2(1, 0), 0), 0.5f));
	assert(FloatEqual(Tex.At<float>(FUint64Vector2(0, 1), 0), 1.5f));
	assert(FloatEqual(Tex.At<float>(FUint64Vector2(1, 1), 0), 2.0f));

	std::cout << "  Min Float: PASSED" << std::endl;
}

void TestMaxFloat()
{
	std::cout << "Testing Max with Float..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Float);

	Tex.At<float>(FUint64Vector2(0, 0), 0) = -1.0f;
	Tex.At<float>(FUint64Vector2(1, 0), 0) = 0.5f;
	Tex.At<float>(FUint64Vector2(0, 1), 0) = 1.5f;
	Tex.At<float>(FUint64Vector2(1, 1), 0) = 2.0f;

	Tex.Max(1.0f);

	assert(FloatEqual(Tex.At<float>(FUint64Vector2(0, 0), 0), -1.0f));
	assert(FloatEqual(Tex.At<float>(FUint64Vector2(1, 0), 0), 0.5f));
	assert(FloatEqual(Tex.At<float>(FUint64Vector2(0, 1), 0), 1.0f));
	assert(FloatEqual(Tex.At<float>(FUint64Vector2(1, 1), 0), 1.0f));

	std::cout << "  Max Float: PASSED" << std::endl;
}

void TestClampHalf()
{
	std::cout << "Testing Clamp with Half..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Half);

	// Set values
	Tex.At<FHalf>(FUint64Vector2(0, 0), 0) = static_cast<FHalf>(-1.0f);
	Tex.At<FHalf>(FUint64Vector2(1, 0), 0) = static_cast<FHalf>(0.5f);
	Tex.At<FHalf>(FUint64Vector2(0, 1), 0) = static_cast<FHalf>(1.5f);
	Tex.At<FHalf>(FUint64Vector2(1, 1), 0) = static_cast<FHalf>(2.0f);

	// Clamp to [0.0, 1.0]
	Tex.Clamp(0.0f, 1.0f);

	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 0), 0)), 0.0f));
	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 0), 0)), 0.5f));
	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 1), 0)), 1.0f));
	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 1), 0)), 1.0f));

	std::cout << "  Clamp Half: PASSED" << std::endl;
}

void TestMinHalf()
{
	std::cout << "Testing Min with Half..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Half);

	Tex.At<FHalf>(FUint64Vector2(0, 0), 0) = static_cast<FHalf>(-1.0f);
	Tex.At<FHalf>(FUint64Vector2(1, 0), 0) = static_cast<FHalf>(0.5f);
	Tex.At<FHalf>(FUint64Vector2(0, 1), 0) = static_cast<FHalf>(1.5f);
	Tex.At<FHalf>(FUint64Vector2(1, 1), 0) = static_cast<FHalf>(2.0f);

	Tex.Min(0.0f);

	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 0), 0)), 0.0f));
	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 0), 0)), 0.5f));
	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 1), 0)), 1.5f));
	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 1), 0)), 2.0f));

	std::cout << "  Min Half: PASSED" << std::endl;
}

void TestMaxHalf()
{
	std::cout << "Testing Max with Half..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Half);

	Tex.At<FHalf>(FUint64Vector2(0, 0), 0) = static_cast<FHalf>(-1.0f);
	Tex.At<FHalf>(FUint64Vector2(1, 0), 0) = static_cast<FHalf>(0.5f);
	Tex.At<FHalf>(FUint64Vector2(0, 1), 0) = static_cast<FHalf>(1.5f);
	Tex.At<FHalf>(FUint64Vector2(1, 1), 0) = static_cast<FHalf>(2.0f);

	Tex.Max(1.0f);

	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 0), 0)), -1.0f));
	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 0), 0)), 0.5f));
	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 1), 0)), 1.0f));
	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 1), 0)), 1.0f));

	std::cout << "  Max Half: PASSED" << std::endl;
}

void TestClampDouble()
{
	std::cout << "Testing Clamp with Double..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Double);

	// Set values
	Tex.At<double>(FUint64Vector2(0, 0), 0) = -1.0;
	Tex.At<double>(FUint64Vector2(1, 0), 0) = 0.5;
	Tex.At<double>(FUint64Vector2(0, 1), 0) = 1.5;
	Tex.At<double>(FUint64Vector2(1, 1), 0) = 2.0;

	// Clamp to [0.0, 1.0]
	Tex.Clamp(0.0f, 1.0f);

	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 0), 0)), 0.0f));
	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 0), 0)), 0.5f));
	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 1), 0)), 1.0f));
	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 1), 0)), 1.0f));

	std::cout << "  Clamp Double: PASSED" << std::endl;
}

void TestMinDouble()
{
	std::cout << "Testing Min with Double..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Double);

	Tex.At<double>(FUint64Vector2(0, 0), 0) = -1.0;
	Tex.At<double>(FUint64Vector2(1, 0), 0) = 0.5;
	Tex.At<double>(FUint64Vector2(0, 1), 0) = 1.5;
	Tex.At<double>(FUint64Vector2(1, 1), 0) = 2.0;

	Tex.Min(0.0f);

	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 0), 0)), 0.0f));
	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 0), 0)), 0.5f));
	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 1), 0)), 1.5f));
	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 1), 0)), 2.0f));

	std::cout << "  Min Double: PASSED" << std::endl;
}

void TestMaxDouble()
{
	std::cout << "Testing Max with Double..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Double);

	Tex.At<double>(FUint64Vector2(0, 0), 0) = -1.0;
	Tex.At<double>(FUint64Vector2(1, 0), 0) = 0.5;
	Tex.At<double>(FUint64Vector2(0, 1), 0) = 1.5;
	Tex.At<double>(FUint64Vector2(1, 1), 0) = 2.0;

	Tex.Max(1.0f);

	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 0), 0)), -1.0f));
	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 0), 0)), 0.5f));
	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 1), 0)), 1.0f));
	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 1), 0)), 1.0f));

	std::cout << "  Max Double: PASSED" << std::endl;
}

void TestThresholdUint8()
{
	std::cout << "Testing Threshold with Uint8..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Uint8);

	// Set values: 0.0, 0.3, 0.6, 1.0
	Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) = 0;
	Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) = 77;  // ~0.3
	Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) = 153; // ~0.6
	Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) = 255; // 1.0

	// Threshold at 0.5
	Tex.Threshold(0.5f);

	assert(Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) == 0);   // 0 -> 0
	assert(Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) == 0);   // 77 -> 0 (< 128)
	assert(Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) == 153); // 153 stays (>= 128)
	assert(Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) == 255); // 255 stays

	std::cout << "  Threshold Uint8: PASSED" << std::endl;
}

void TestThresholdFloat()
{
	std::cout << "Testing Threshold with Float..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Float);

	// Set values
	Tex.At<float>(FUint64Vector2(0, 0), 0) = -0.5f;
	Tex.At<float>(FUint64Vector2(1, 0), 0) = 0.3f;
	Tex.At<float>(FUint64Vector2(0, 1), 0) = 0.7f;
	Tex.At<float>(FUint64Vector2(1, 1), 0) = 1.5f;

	// Threshold at 0.5
	Tex.Threshold(0.5f);

	assert(FloatEqual(Tex.At<float>(FUint64Vector2(0, 0), 0), 0.f)); // -0.5 -> 0
	assert(FloatEqual(Tex.At<float>(FUint64Vector2(1, 0), 0), 0.f)); // 0.3 -> 0
	assert(FloatEqual(Tex.At<float>(FUint64Vector2(0, 1), 0), 0.7f)); // 0.7 stays
	assert(FloatEqual(Tex.At<float>(FUint64Vector2(1, 1), 0), 1.5f)); // 1.5 stays

	std::cout << "  Threshold Float: PASSED" << std::endl;
}

void TestThresholdHalf()
{
	std::cout << "Testing Threshold with Half..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Half);

	// Set values
	Tex.At<FHalf>(FUint64Vector2(0, 0), 0) = static_cast<FHalf>(0.1f);
	Tex.At<FHalf>(FUint64Vector2(1, 0), 0) = static_cast<FHalf>(0.4f);
	Tex.At<FHalf>(FUint64Vector2(0, 1), 0) = static_cast<FHalf>(0.6f);
	Tex.At<FHalf>(FUint64Vector2(1, 1), 0) = static_cast<FHalf>(0.9f);

	// Threshold at 0.5
	Tex.Threshold(0.5f);

	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 0), 0)), 0.f)); // 0.1 -> 0
	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 0), 0)), 0.f)); // 0.4 -> 0
	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 1), 0)), 0.6f)); // 0.6 stays
	assert(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 1), 0)), 0.9f)); // 0.9 stays

	std::cout << "  Threshold Half: PASSED" << std::endl;
}

void TestThresholdDouble()
{
	std::cout << "Testing Threshold with Double..." << std::endl;

	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Double);

	// Set values
	Tex.At<double>(FUint64Vector2(0, 0), 0) = 0.2;
	Tex.At<double>(FUint64Vector2(1, 0), 0) = 0.45;
	Tex.At<double>(FUint64Vector2(0, 1), 0) = 0.55;
	Tex.At<double>(FUint64Vector2(1, 1), 0) = 0.8;

	// Threshold at 0.5
	Tex.Threshold(0.5f);

	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 0), 0)), 0.f)); // 0.2 -> 0
	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 0), 0)), 0.f)); // 0.45 -> 0
	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 1), 0)), 0.55f)); // 0.55 stays
	assert(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 1), 0)), 0.8f)); // 0.8 stays

	std::cout << "  Threshold Double: PASSED" << std::endl;
}

void TestImageInpainting()
{
	std::cout << "Testing ImageInpainting..." << std::endl;

	// Create a 16x16 texture with Float type
	const uint64_t Size = 16;
	FTex2D Tex(FGrid2D(Size, Size), 3, EElementType::Float);
	FTex2D Coverage(FGrid2D(Size, Size), 3, EElementType::Float);

	// Initialize all to zero
	for (uint64_t Y = 0; Y < Size; Y++)
	{
		for (uint64_t X = 0; X < Size; X++)
		{
			for (uint64_t C = 0; C < 3; C++)
			{
				Tex.SetFloat(FUint64Vector2(X, Y), C, 0.f);
				Coverage.SetFloat(FUint64Vector2(X, Y), C, 0.f);
			}
		}
	}

	// Fill center region (4x4 to 12x12) with valid data
	for (uint64_t Y = 4; Y < 12; Y++)
	{
		for (uint64_t X = 4; X < 12; X++)
		{
			const float U = static_cast<float>(X) / static_cast<float>(Size - 1);
			const float V = static_cast<float>(Y) / static_cast<float>(Size - 1);

			// Set RGB values based on position
			Tex.SetFloat(FUint64Vector2(X, Y), 0, U);           // R
			Tex.SetFloat(FUint64Vector2(X, Y), 1, V * V);       // G
			Tex.SetFloat(FUint64Vector2(X, Y), 2, std::sqrt((U + V * V) / 2.f)); // B

			// Mark as covered
			for (uint64_t C = 0; C < 3; C++)
			{
				Coverage.SetFloat(FUint64Vector2(X, Y), C, 1.f);
			}
		}
	}

	// Perform inpainting
	Tex.ImageInpainting(Coverage);

	// Verify that previously uncovered regions now have non-zero values
	bool bHasFilledValues = false;
	for (uint64_t Y = 0; Y < Size; Y++)
	{
		for (uint64_t X = 0; X < Size; X++)
		{
			// Check border regions (should be filled now)
			if (X < 4 || X >= 12 || Y < 4 || Y >= 12)
			{
				for (uint64_t C = 0; C < 3; C++)
				{
					const float Value = Tex.GetFloat(FUint64Vector2(X, Y), C);
					if (Value > 0.f)
					{
						bHasFilledValues = true;
						break;
					}
				}
			}
		}
	}

	assert(bHasFilledValues && "ImageInpainting should fill uncovered regions");

	// Verify that covered regions remain unchanged (approximately)
	for (uint64_t Y = 6; Y < 10; Y++)
	{
		for (uint64_t X = 6; X < 10; X++)
		{
			const float U = static_cast<float>(X) / static_cast<float>(Size - 1);
			const float V = static_cast<float>(Y) / static_cast<float>(Size - 1);

			const float ExpectedR = U;
			const float ExpectedG = V * V;
			const float ExpectedB = std::sqrt((U + V * V) / 2.f);

			const float ActualR = Tex.GetFloat(FUint64Vector2(X, Y), 0);
			const float ActualG = Tex.GetFloat(FUint64Vector2(X, Y), 1);
			const float ActualB = Tex.GetFloat(FUint64Vector2(X, Y), 2);

			assert(FloatEqual(ActualR, ExpectedR, 0.01f) && "Original covered region R should remain unchanged");
			assert(FloatEqual(ActualG, ExpectedG, 0.01f) && "Original covered region G should remain unchanged");
			assert(FloatEqual(ActualB, ExpectedB, 0.01f) && "Original covered region B should remain unchanged");
		}
	}

	// Test edge case: texture with partial coverage in one channel
	FTex2D Tex2(FGrid2D(8, 8), 2, EElementType::Float);
	FTex2D Coverage2(FGrid2D(8, 8), 2, EElementType::Float);

	// Initialize
	for (uint64_t Y = 0; Y < 8; Y++)
	{
		for (uint64_t X = 0; X < 8; X++)
		{
			Tex2.SetFloat(FUint64Vector2(X, Y), 0, 0.f);
			Tex2.SetFloat(FUint64Vector2(X, Y), 1, 0.f);
			Coverage2.SetFloat(FUint64Vector2(X, Y), 0, 0.f);
			Coverage2.SetFloat(FUint64Vector2(X, Y), 1, 0.f);
		}
	}

	// Set center pixel with coverage
	Tex2.SetFloat(FUint64Vector2(4, 4), 0, 1.f);
	Tex2.SetFloat(FUint64Vector2(4, 4), 1, 0.5f);
	Coverage2.SetFloat(FUint64Vector2(4, 4), 0, 1.f);
	Coverage2.SetFloat(FUint64Vector2(4, 4), 1, 1.f);

	Tex2.ImageInpainting(Coverage2);

	// Check that neighbors got filled
	const float Neighbor = Tex2.GetFloat(FUint64Vector2(3, 4), 0);
	assert(Neighbor > 0.f && "Neighbor should be filled from center");

	std::cout << "  ImageInpainting: PASSED" << std::endl;
}

int main(int argc, char** argv)
{
	std::cout << "=== Testing FTex2D Min/Max/Clamp Operations ===" << std::endl << std::endl;

	// Test Uint8
	TestClampUint8();
	TestMinUint8();
	TestMaxUint8();

	std::cout << std::endl;

	// Test Float
	TestClampFloat();
	TestMinFloat();
	TestMaxFloat();

	std::cout << std::endl;

	// Test Half
	TestClampHalf();
	TestMinHalf();
	TestMaxHalf();

	std::cout << std::endl;

	// Test Double
	TestClampDouble();
	TestMinDouble();
	TestMaxDouble();

	std::cout << std::endl;

	// Test Threshold
	TestThresholdUint8();
	TestThresholdFloat();
	TestThresholdHalf();
	TestThresholdDouble();

	std::cout << std::endl;

	// Test ImageInpainting
	TestImageInpainting();

	std::cout << std::endl;
	std::cout << "=== All tests passed! ===" << std::endl;

	return 0;
}

