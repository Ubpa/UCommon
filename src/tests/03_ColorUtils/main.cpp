#include <UCommon/UCommon.h>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <UCommon_ext/doctest/doctest.h>

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

TEST_CASE("RGB <-> YCoCg Roundtrip")
{
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

	int FailCount = 0;
	for (size_t i = 0; i < TestCases.size(); ++i)
	{
		const FVector3f& OriginalRGB = TestCases[i];

		FLinearColorRGB YCoCg = RGBToYCoCg(OriginalRGB);
		FLinearColorRGB RecoveredHue = YCoCgToRGB(YCoCg);

		if (!IsNearlyEqual(OriginalRGB, RecoveredHue, 1e-3f))
		{
			FAIL_CHECK("RGB<->YCoCg roundtrip failed for test case " << i);
			FailCount++;
		}
	}
}

TEST_CASE("YCoCg Properties")
{
	std::vector<FVector3f> TestRGBs = {
		FVector3f(1.0f, 0.0f, 0.0f),
		FVector3f(0.0f, 1.0f, 0.0f),
		FVector3f(0.0f, 0.0f, 1.0f),
		FVector3f(1.0f, 1.0f, 1.0f),
		FVector3f(0.5f, 0.5f, 0.5f),
		FVector3f(0.8f, 0.3f, 0.2f),
		FVector3f(0.2f, 0.6f, 0.9f),
	};

	int FailCount = 0;
	for (const auto& RGB : TestRGBs)
	{
		float Y = (RGB.X + 2.0f * RGB.Y + RGB.Z) / 4.0f;
		FVector3f NormalizedHue = RGB / Y * 4.0f;
		FLinearColorRGB YCoCg = RGBToYCoCg(NormalizedHue);

		bool YInRange = IsNearlyEqual(YCoCg.X, 4.0f, 0.01f);
		if (!YInRange)
		{
			FAIL_CHECK("Y is not 4.0: " << YCoCg.X);
			FailCount++;
		}

		bool CoCgInRange = (std::abs(YCoCg.Y) <= 1.0f && std::abs(YCoCg.Z) <= 1.0f);
		if (!CoCgInRange)
		{
			FAIL_CHECK("Co or Cg out of [-1, 1] range: (" << YCoCg.Y << ", " << YCoCg.Z << ")");
			FailCount++;
		}
	}
}

TEST_CASE("FPackedHue - Zero Initialization")
{
	FPackedHue ZeroHue(EForceInit::Default);
	CHECK(ZeroHue.U == 128);
	CHECK(ZeroHue.V == 128);
}

TEST_CASE("FPackedHue - Direct uint8_t Construction")
{
	FPackedHue DirectHue(100, 200);
	CHECK(DirectHue.U == 100);
	CHECK(DirectHue.V == 200);
}

TEST_CASE("FPackedHue - CoCg Vector Roundtrip")
{
	std::vector<FVector2f> CoCgTestCases = {
		FVector2f(0.0f, 0.0f),
		FVector2f(0.0f, -1.0f),
		FVector2f(0.5f, 0.0f),
		FVector2f(-0.5f, 0.0f),
		FVector2f(0.0f, 1.0f),
		FVector2f(0.25f, 0.5f),
		FVector2f(-0.25f, 0.5f),
		FVector2f(0.4f, -0.2f),
	};

	int PassedCount = 0;
	for (const auto& CoCg : CoCgTestCases)
	{
		const bool bIsValid = (CoCg.Y - 1.0f <= 2.0f * CoCg.X) && (2.0f * CoCg.X <= 1.0f - CoCg.Y);
		REQUIRE(bIsValid);

		FPackedHue Packed(CoCg);
		FVector2f UnpackedCoCg = Packed.Unpack();

		const float CoRange = (1.0f - CoCg.Y) - (CoCg.Y - 1.0f);
		const float ToleranceCo = std::max(CoRange / 255.0f * 2.0f, 0.01f);
		const float ToleranceCg = 2.0f / 255.0f;

		bool CoMatch = IsNearlyEqual(CoCg.X, UnpackedCoCg.X, ToleranceCo);
		bool CgMatch = IsNearlyEqual(CoCg.Y, UnpackedCoCg.Y, ToleranceCg);
		CHECK(CoMatch);
		CHECK(CgMatch);
		PassedCount += (CoMatch && CgMatch) ? 1 : 0;
	}
	CHECK(PassedCount == CoCgTestCases.size());
}

TEST_CASE("Vector <-> HemiOctL Roundtrip")
{
	std::vector<FVector3f> TestVectors = {
		FVector3f(1.0f, 0.0f, 0.0f),
		FVector3f(0.0f, 1.0f, 0.0f),
		FVector3f(0.0f, 0.0f, 1.0f),
		FVector3f(-1.0f, 0.0f, 0.0f),
		FVector3f(0.0f, -1.0f, 0.0f),
		FVector3f(1.0f, 1.0f, 1.0f),
		FVector3f(1.0f, -1.0f, 1.0f),
		FVector3f(-1.0f, 1.0f, 1.0f),
		FVector3f(-1.0f, -1.0f, 1.0f),
		FVector3f(0.577f, 0.577f, 0.577f),
		FVector3f(0.707f, 0.707f, 0.0f),
		FVector3f(0.8f, 0.6f, 0.0f),
		FVector3f(0.267f, 0.535f, 0.802f),
		FVector3f(2.0f, 3.0f, 1.0f),
		FVector3f(10.0f, 5.0f, 2.0f),
		FVector3f(0.1f, 0.2f, 0.3f),
		FVector3f(0.01f, 0.02f, 0.03f),
		FVector3f(1e-6f, 1e-6f, 1e-6f),
		FVector3f(0.0f, 0.0f, 0.0f),
	};

	int FailCount = 0;
	for (size_t i = 0; i < TestVectors.size(); ++i)
	{
		const FVector3f& OriginalVector = TestVectors[i];
		FVector3f HemiOctL = VectorToHemiOctL(OriginalVector);
		FVector3f RecoveredVector = HemiOctLToVector(HemiOctL);

		float Length = std::sqrt(OriginalVector.X * OriginalVector.X +
		                         OriginalVector.Y * OriginalVector.Y +
		                         OriginalVector.Z * OriginalVector.Z);
		float Tolerance = (Length < 1e-5f) ? 1e-5f : Length * 1e-4f;

		if (!IsNearlyEqual(OriginalVector, RecoveredVector, Tolerance))
		{
			FAIL_CHECK("Vector<->HemiOctL roundtrip failed for test case " << i);
			FailCount++;
		}
	}
}

TEST_CASE("HemiOctL Properties")
{
	std::vector<FVector3f> TestVectors = {
		FVector3f(1.0f, 0.0f, 0.0f),
		FVector3f(0.0f, 1.0f, 0.0f),
		FVector3f(0.0f, 0.0f, 1.0f),
		FVector3f(0.577f, 0.577f, 0.577f),
		FVector3f(0.707f, 0.707f, 0.0f),
		FVector3f(-0.8f, 0.6f, 0.0f),
	};

	int FailCount = 0;
	for (const auto& Vector : TestVectors)
	{
		FVector3f HemiOctL = VectorToHemiOctL(Vector);

		bool XYInRange = (std::abs(HemiOctL.X) <= 1.0f && std::abs(HemiOctL.Y) <= 1.0f);
		if (!XYInRange)
		{
			FAIL_CHECK("HemiOct.X or HemiOct.Y out of [-1, 1] range");
			FailCount++;
		}

		float VectorLength = std::sqrt(Vector.X * Vector.X + Vector.Y * Vector.Y + Vector.Z * Vector.Z);
		if (std::abs(VectorLength - 1.0f) < 0.01f)
		{
			float HemiOctSum = std::abs(HemiOctL.X) + std::abs(HemiOctL.Y);
			bool SumLeOne = (HemiOctSum <= 1.01f);
			if (!SumLeOne)
			{
				FAIL_CHECK("Unit vector produces |HemiOct.X| + |HemiOct.Y| = " << HemiOctSum << " > 1.0");
				FailCount++;
			}
		}
	}
}

TEST_CASE("FPackedHemiOct - Zero Initialization")
{
	FPackedHemiOct ZeroHemiOct(EForceInit::Default);
	CHECK(ZeroHemiOct.U == 213);
	CHECK(ZeroHemiOct.V == 128);
}

TEST_CASE("FPackedHemiOct - Direct uint8_t Construction")
{
	FPackedHemiOct DirectHemiOct(100, 200);
	CHECK(DirectHemiOct.U == 100);
	CHECK(DirectHemiOct.V == 200);
}

TEST_CASE("FPackedHemiOct - HemiOct Vector Roundtrip")
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
		FVector2f Unpacked = Packed.Unpack();
		float UnpackedHemiOctX = Unpacked.X;
		float UnpackedHemiOctY = Unpacked.Y;

		float Tolerance = 2.0f / 255.0f;
		bool XMatch = IsNearlyEqual(HemiOct.X, UnpackedHemiOctX, Tolerance);
		bool YMatch = IsNearlyEqual(HemiOct.Y, UnpackedHemiOctY, Tolerance);
		CHECK(XMatch);
		CHECK(YMatch);
		PassedCount += (XMatch && YMatch) ? 1 : 0;
	}
	CHECK(PassedCount == HemiOctTestCases.size());
}

TEST_CASE("FPackedHemiOct - Full Pipeline")
{
	std::vector<FVector3f> TestVectors = {
		FVector3f(1.0f, 0.0f, 0.0f),
		FVector3f(0.0f, 1.0f, 0.0f),
		FVector3f(0.0f, 0.0f, 1.0f),
		FVector3f(0.577f, 0.577f, 0.577f),
		FVector3f(0.707f, 0.707f, 0.0f),
		FVector3f(2.0f, 3.0f, 1.0f),
	};

	int FailCount = 0;
	for (const auto& Vector : TestVectors)
	{
		FVector3f HemiOctL = VectorToHemiOctL(Vector);
		FPackedHemiOct Packed(FVector2f(HemiOctL.X, HemiOctL.Y));
		FVector2f UnpackedHemiOct = Packed.Unpack();
		FVector3f RecoveredVector = UCommon::HemiOctToDir(UnpackedHemiOct) * HemiOctL.Z;

		float Length = std::sqrt(Vector.X * Vector.X + Vector.Y * Vector.Y + Vector.Z * Vector.Z);
		float Tolerance = Length * 0.01f;

		if (!IsNearlyEqual(Vector, RecoveredVector, Tolerance))
		{
			FAIL_CHECK("Full pipeline failed for Vector(" << Vector.X << ", " << Vector.Y << ", " << Vector.Z << ")");
			FailCount++;
		}
	}
}
