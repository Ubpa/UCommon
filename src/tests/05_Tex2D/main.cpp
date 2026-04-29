#include <UCommon/Tex2D.h>
#include <UCommon/Half.h>
#include <cmath>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <UCommon_ext/doctest/doctest.h>

using namespace UCommon;

// Helper function to compare floats with tolerance
bool FloatEqual(float A, float B, float Tolerance = 0.001f)
{
	return std::abs(A - B) < Tolerance;
}

TEST_CASE("Tex2D - Clamp Uint8")
{
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
	CHECK(Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) == 64);  // 0 -> 64
	CHECK(Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) == 128); // 128 stays
	CHECK(Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) == 191); // 191 stays
	CHECK(Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) == 191); // 255 -> 191
}

TEST_CASE("Tex2D - Min Uint8")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Uint8);

	// Set values: 0.0, 0.3, 0.6, 1.0
	Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) = 0;
	Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) = 77;  // ~0.3
	Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) = 153; // ~0.6
	Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) = 255;

	// Min to 0.5 (128)
	Tex.Min(0.5f);

	CHECK(Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) == 128); // 0 -> 128
	CHECK(Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) == 128); // 77 -> 128
	CHECK(Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) == 153); // 153 stays
	CHECK(Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) == 255); // 255 stays
}

TEST_CASE("Tex2D - Max Uint8")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Uint8);

	// Set values
	Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) = 0;
	Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) = 128;
	Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) = 191;
	Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) = 255;

	// Max to 0.5 (128)
	Tex.Max(0.5f);

	CHECK(Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) == 0);   // 0 stays
	CHECK(Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) == 128); // 128 stays
	CHECK(Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) == 128); // 191 -> 128
	CHECK(Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) == 128); // 255 -> 128
}

TEST_CASE("Tex2D - Clamp Float")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Float);

	// Set values
	Tex.At<float>(FUint64Vector2(0, 0), 0) = -1.0f;
	Tex.At<float>(FUint64Vector2(1, 0), 0) = 0.5f;
	Tex.At<float>(FUint64Vector2(0, 1), 0) = 1.5f;
	Tex.At<float>(FUint64Vector2(1, 1), 0) = 2.0f;

	// Clamp to [0.0, 1.0]
	Tex.Clamp(0.0f, 1.0f);

	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(0, 0), 0), 0.0f));
	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(1, 0), 0), 0.5f));
	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(0, 1), 0), 1.0f));
	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(1, 1), 0), 1.0f));
}

TEST_CASE("Tex2D - Min Float")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Float);

	Tex.At<float>(FUint64Vector2(0, 0), 0) = -1.0f;
	Tex.At<float>(FUint64Vector2(1, 0), 0) = 0.5f;
	Tex.At<float>(FUint64Vector2(0, 1), 0) = 1.5f;
	Tex.At<float>(FUint64Vector2(1, 1), 0) = 2.0f;

	Tex.Min(0.0f);

	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(0, 0), 0), 0.0f));
	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(1, 0), 0), 0.5f));
	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(0, 1), 0), 1.5f));
	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(1, 1), 0), 2.0f));
}

TEST_CASE("Tex2D - Max Float")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Float);

	Tex.At<float>(FUint64Vector2(0, 0), 0) = -1.0f;
	Tex.At<float>(FUint64Vector2(1, 0), 0) = 0.5f;
	Tex.At<float>(FUint64Vector2(0, 1), 0) = 1.5f;
	Tex.At<float>(FUint64Vector2(1, 1), 0) = 2.0f;

	Tex.Max(1.0f);

	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(0, 0), 0), -1.0f));
	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(1, 0), 0), 0.5f));
	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(0, 1), 0), 1.0f));
	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(1, 1), 0), 1.0f));
}

TEST_CASE("Tex2D - Clamp Half")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Half);

	// Set values
	Tex.At<FHalf>(FUint64Vector2(0, 0), 0) = static_cast<FHalf>(-1.0f);
	Tex.At<FHalf>(FUint64Vector2(1, 0), 0) = static_cast<FHalf>(0.5f);
	Tex.At<FHalf>(FUint64Vector2(0, 1), 0) = static_cast<FHalf>(1.5f);
	Tex.At<FHalf>(FUint64Vector2(1, 1), 0) = static_cast<FHalf>(2.0f);

	// Clamp to [0.0, 1.0]
	Tex.Clamp(0.0f, 1.0f);

	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 0), 0)), 0.0f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 0), 0)), 0.5f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 1), 0)), 1.0f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 1), 0)), 1.0f));
}

TEST_CASE("Tex2D - Min Half")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Half);

	Tex.At<FHalf>(FUint64Vector2(0, 0), 0) = static_cast<FHalf>(-1.0f);
	Tex.At<FHalf>(FUint64Vector2(1, 0), 0) = static_cast<FHalf>(0.5f);
	Tex.At<FHalf>(FUint64Vector2(0, 1), 0) = static_cast<FHalf>(1.5f);
	Tex.At<FHalf>(FUint64Vector2(1, 1), 0) = static_cast<FHalf>(2.0f);

	Tex.Min(0.0f);

	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 0), 0)), 0.0f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 0), 0)), 0.5f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 1), 0)), 1.5f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 1), 0)), 2.0f));
}

TEST_CASE("Tex2D - Max Half")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Half);

	Tex.At<FHalf>(FUint64Vector2(0, 0), 0) = static_cast<FHalf>(-1.0f);
	Tex.At<FHalf>(FUint64Vector2(1, 0), 0) = static_cast<FHalf>(0.5f);
	Tex.At<FHalf>(FUint64Vector2(0, 1), 0) = static_cast<FHalf>(1.5f);
	Tex.At<FHalf>(FUint64Vector2(1, 1), 0) = static_cast<FHalf>(2.0f);

	Tex.Max(1.0f);

	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 0), 0)), -1.0f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 0), 0)), 0.5f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 1), 0)), 1.0f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 1), 0)), 1.0f));
}

TEST_CASE("Tex2D - Clamp Double")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Double);

	// Set values
	Tex.At<double>(FUint64Vector2(0, 0), 0) = -1.0;
	Tex.At<double>(FUint64Vector2(1, 0), 0) = 0.5;
	Tex.At<double>(FUint64Vector2(0, 1), 0) = 1.5;
	Tex.At<double>(FUint64Vector2(1, 1), 0) = 2.0;

	// Clamp to [0.0, 1.0]
	Tex.Clamp(0.0f, 1.0f);

	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 0), 0)), 0.0f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 0), 0)), 0.5f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 1), 0)), 1.0f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 1), 0)), 1.0f));
}

TEST_CASE("Tex2D - Min Double")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Double);

	Tex.At<double>(FUint64Vector2(0, 0), 0) = -1.0;
	Tex.At<double>(FUint64Vector2(1, 0), 0) = 0.5;
	Tex.At<double>(FUint64Vector2(0, 1), 0) = 1.5;
	Tex.At<double>(FUint64Vector2(1, 1), 0) = 2.0;

	Tex.Min(0.0f);

	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 0), 0)), 0.0f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 0), 0)), 0.5f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 1), 0)), 1.5f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 1), 0)), 2.0f));
}

TEST_CASE("Tex2D - Max Double")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Double);

	Tex.At<double>(FUint64Vector2(0, 0), 0) = -1.0;
	Tex.At<double>(FUint64Vector2(1, 0), 0) = 0.5;
	Tex.At<double>(FUint64Vector2(0, 1), 0) = 1.5;
	Tex.At<double>(FUint64Vector2(1, 1), 0) = 2.0;

	Tex.Max(1.0f);

	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 0), 0)), -1.0f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 0), 0)), 0.5f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 1), 0)), 1.0f));
	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 1), 0)), 1.0f));
}

TEST_CASE("Tex2D - Threshold Uint8")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Uint8);

	// Set values: 0.0, 0.3, 0.6, 1.0
	Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) = 0;
	Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) = 77;  // ~0.3
	Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) = 153; // ~0.6
	Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) = 255; // 1.0

	// Threshold at 0.5
	Tex.Threshold(0.5f);

	CHECK(Tex.At<uint8_t>(FUint64Vector2(0, 0), 0) == 0);   // 0 -> 0
	CHECK(Tex.At<uint8_t>(FUint64Vector2(1, 0), 0) == 0);   // 77 -> 0 (< 128)
	CHECK(Tex.At<uint8_t>(FUint64Vector2(0, 1), 0) == 153); // 153 stays (>= 128)
	CHECK(Tex.At<uint8_t>(FUint64Vector2(1, 1), 0) == 255); // 255 stays
}

TEST_CASE("Tex2D - Threshold Float")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Float);

	// Set values
	Tex.At<float>(FUint64Vector2(0, 0), 0) = -0.5f;
	Tex.At<float>(FUint64Vector2(1, 0), 0) = 0.3f;
	Tex.At<float>(FUint64Vector2(0, 1), 0) = 0.7f;
	Tex.At<float>(FUint64Vector2(1, 1), 0) = 1.5f;

	// Threshold at 0.5
	Tex.Threshold(0.5f);

	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(0, 0), 0), 0.f)); // -0.5 -> 0
	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(1, 0), 0), 0.f)); // 0.3 -> 0
	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(0, 1), 0), 0.7f)); // 0.7 stays
	CHECK(FloatEqual(Tex.At<float>(FUint64Vector2(1, 1), 0), 1.5f)); // 1.5 stays
}

TEST_CASE("Tex2D - Threshold Half")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Half);

	// Set values
	Tex.At<FHalf>(FUint64Vector2(0, 0), 0) = static_cast<FHalf>(0.1f);
	Tex.At<FHalf>(FUint64Vector2(1, 0), 0) = static_cast<FHalf>(0.4f);
	Tex.At<FHalf>(FUint64Vector2(0, 1), 0) = static_cast<FHalf>(0.6f);
	Tex.At<FHalf>(FUint64Vector2(1, 1), 0) = static_cast<FHalf>(0.9f);

	// Threshold at 0.5
	Tex.Threshold(0.5f);

	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 0), 0)), 0.f)); // 0.1 -> 0
	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 0), 0)), 0.f)); // 0.4 -> 0
	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(0, 1), 0)), 0.6f)); // 0.6 stays
	CHECK(FloatEqual(static_cast<float>(Tex.At<FHalf>(FUint64Vector2(1, 1), 0)), 0.9f)); // 0.9 stays
}

TEST_CASE("Tex2D - Threshold Double")
{
	FTex2D Tex(FGrid2D(2, 2), 1, EElementType::Double);

	// Set values
	Tex.At<double>(FUint64Vector2(0, 0), 0) = 0.2;
	Tex.At<double>(FUint64Vector2(1, 0), 0) = 0.45;
	Tex.At<double>(FUint64Vector2(0, 1), 0) = 0.55;
	Tex.At<double>(FUint64Vector2(1, 1), 0) = 0.8;

	// Threshold at 0.5
	Tex.Threshold(0.5f);

	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 0), 0)), 0.f)); // 0.2 -> 0
	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 0), 0)), 0.f)); // 0.45 -> 0
	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(0, 1), 0)), 0.55f)); // 0.55 stays
	CHECK(FloatEqual(static_cast<float>(Tex.At<double>(FUint64Vector2(1, 1), 0)), 0.8f)); // 0.8 stays
}

TEST_CASE("Tex2D - ImageInpainting")
{
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

	REQUIRE(bHasFilledValues);


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

			bool RMatch = FloatEqual(ActualR, ExpectedR, 0.01f);
			bool GMatch = FloatEqual(ActualG, ExpectedG, 0.01f);
			bool BMatch = FloatEqual(ActualB, ExpectedB, 0.01f);

			CHECK(RMatch);
			CHECK(GMatch);
			CHECK(BMatch);
		}
	}
}

TEST_CASE("Tex2D - ImageInpainting Partial Coverage")
{
	// Test edge case: texture with partial coverage in one channel
	FTex2D Tex(FGrid2D(8, 8), 2, EElementType::Float);
	FTex2D Coverage(FGrid2D(8, 8), 2, EElementType::Float);

	// Initialize
	for (uint64_t Y = 0; Y < 8; Y++)
	{
		for (uint64_t X = 0; X < 8; X++)
		{
			Tex.SetFloat(FUint64Vector2(X, Y), 0, 0.f);
			Tex.SetFloat(FUint64Vector2(X, Y), 1, 0.f);
			Coverage.SetFloat(FUint64Vector2(X, Y), 0, 0.f);
			Coverage.SetFloat(FUint64Vector2(X, Y), 1, 0.f);
		}
	}

	// Set center pixel with coverage
	Tex.SetFloat(FUint64Vector2(4, 4), 0, 1.f);
	Tex.SetFloat(FUint64Vector2(4, 4), 1, 0.5f);
	Coverage.SetFloat(FUint64Vector2(4, 4), 0, 1.f);
	Coverage.SetFloat(FUint64Vector2(4, 4), 1, 1.f);

	Tex.ImageInpainting(Coverage);

	// Check that neighbors got filled
	const float Neighbor = Tex.GetFloat(FUint64Vector2(3, 4), 0);
	bool NeighborFilled = Neighbor > 0.f;
	CHECK_MESSAGE(NeighborFilled, "Neighbor should be filled from center");
}

TEST_CASE("Tex2D - ImageInpainting SingleChannel Coverage")
{
	// Test: CoverageData.NumChannels == 1 should work for multi-channel textures.
	// The single coverage channel is broadcast to all color channels.
	const uint64_t Size = 8;
	const uint64_t NumColorChannels = 3;

	FTex2D Tex(FGrid2D(Size, Size), NumColorChannels, EElementType::Float);
	// Single-channel coverage
	FTex2D Coverage(FGrid2D(Size, Size), 1, EElementType::Float);

	// Initialize all to zero
	for (uint64_t Y = 0; Y < Size; Y++)
	{
		for (uint64_t X = 0; X < Size; X++)
		{
			for (uint64_t C = 0; C < NumColorChannels; C++)
			{
				Tex.SetFloat(FUint64Vector2(X, Y), C, 0.f);
			}
			Coverage.SetFloat(FUint64Vector2(X, Y), 0, 0.f);
		}
	}

	// Fill center 4x4 region with known values; coverage = 1 (shared for all channels)
	for (uint64_t Y = 2; Y < 6; Y++)
	{
		for (uint64_t X = 2; X < 6; X++)
		{
			Tex.SetFloat(FUint64Vector2(X, Y), 0, 1.f);   // R = 1
			Tex.SetFloat(FUint64Vector2(X, Y), 1, 0.5f);  // G = 0.5
			Tex.SetFloat(FUint64Vector2(X, Y), 2, 0.25f); // B = 0.25
			Coverage.SetFloat(FUint64Vector2(X, Y), 0, 1.f);
		}
	}

	// Should not assert or crash
	Tex.ImageInpainting(Coverage);

	// Border pixels should be filled (non-zero) from the covered region
	bool bBorderFilled = false;
	for (uint64_t Y = 0; Y < Size; Y++)
	{
		for (uint64_t X = 0; X < Size; X++)
		{
			if (X < 2 || X >= 6 || Y < 2 || Y >= 6)
			{
				for (uint64_t C = 0; C < NumColorChannels; C++)
				{
					if (Tex.GetFloat(FUint64Vector2(X, Y), C) > 0.f)
					{
						bBorderFilled = true;
					}
				}
			}
		}
	}
	CHECK_MESSAGE(bBorderFilled, "Border pixels should be filled after inpainting with single-channel coverage");

	// Center pixels should retain their values
	CHECK(FloatEqual(Tex.GetFloat(FUint64Vector2(3, 3), 0), 1.f));
	CHECK(FloatEqual(Tex.GetFloat(FUint64Vector2(3, 3), 1), 0.5f));
	CHECK(FloatEqual(Tex.GetFloat(FUint64Vector2(3, 3), 2), 0.25f));

	// All 3 color channels of a border pixel should be filled (not just channel 0)
	const FUint64Vector2 BorderPt(1, 3); // adjacent to covered region
	const float BorderR = Tex.GetFloat(BorderPt, 0);
	const float BorderG = Tex.GetFloat(BorderPt, 1);
	const float BorderB = Tex.GetFloat(BorderPt, 2);
	CHECK_MESSAGE(BorderR > 0.f, "R channel of border pixel should be filled");
	CHECK_MESSAGE(BorderG > 0.f, "G channel of border pixel should be filled");
	CHECK_MESSAGE(BorderB > 0.f, "B channel of border pixel should be filled");
}
