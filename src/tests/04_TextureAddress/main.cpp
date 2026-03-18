#include <UCommon/Tex2D.h>
#include <UCommon/Utils.h>
#include <iostream>
#include <iomanip>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <UCommon_ext/doctest/doctest.h>

using namespace UCommon;

FTex2D CreateTestTexture()
{
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
	return Tex;
}

TEST_CASE("Texture Address - Basic Wrap Behavior")
{
	FTex2D Tex = CreateTestTexture();

	// Wrap: UV(1.5, 0.5) should be same as UV(0.5, 0.5)
	float Result1, Result2;
	Tex.BilinearSample(&Result1, FVector2f(1.5f, 0.5f), ETextureAddress::Wrap, ETextureAddress::Wrap);
	Tex.BilinearSample(&Result2, FVector2f(0.5f, 0.5f), ETextureAddress::Wrap, ETextureAddress::Wrap);
	bool Match = std::abs(Result1 - Result2) < 0.001f;
	CHECK(Match);
}

TEST_CASE("Texture Address - Basic Clamp Behavior")
{
	FTex2D Tex = CreateTestTexture();

	// Clamp: UV(1.5, 0.5) should clamp to UV(1.0, 0.5)
	float Result1, Result2;
	Tex.BilinearSample(&Result1, FVector2f(1.5f, 0.5f), ETextureAddress::Clamp, ETextureAddress::Clamp);
	Tex.BilinearSample(&Result2, FVector2f(1.0f, 0.5f), ETextureAddress::Clamp, ETextureAddress::Clamp);
	bool Match = std::abs(Result1 - Result2) < 0.001f;
	CHECK(Match);
}

TEST_CASE("Texture Address - Basic Mirror Behavior")
{
	FTex2D Tex = CreateTestTexture();

	// Mirror: UV(1.5, 0.5) should mirror to UV(0.5, 0.5)
	float Result1, Result2;
	Tex.BilinearSample(&Result1, FVector2f(1.5f, 0.5f), ETextureAddress::Mirror, ETextureAddress::Mirror);
	Tex.BilinearSample(&Result2, FVector2f(0.5f, 0.5f), ETextureAddress::Mirror, ETextureAddress::Mirror);
	bool Match = std::abs(Result1 - Result2) < 0.001f;
	CHECK(Match);
}

TEST_CASE("Texture Address - Wrap Boundary Bilinear Interpolation")
{
	FTex2D Tex = CreateTestTexture();

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
	Tex.BilinearSample(&Result, FVector2f(0.0f, 0.5f), ETextureAddress::Wrap, ETextureAddress::Wrap);
	CHECK(std::abs(Result - 7.5f) < 0.001f);
}

TEST_CASE("Texture Address - Wrap At UV(1.0, 0.5)")
{
	FTex2D Tex = CreateTestTexture();

	// At UV(1.0, 0.5), should wrap to UV(0.0, 0.5) in Wrap mode
	float Result1, Result2;
	Tex.BilinearSample(&Result1, FVector2f(1.0f, 0.5f), ETextureAddress::Wrap, ETextureAddress::Wrap);
	Tex.BilinearSample(&Result2, FVector2f(0.0f, 0.5f), ETextureAddress::Wrap, ETextureAddress::Wrap);
	bool Match = std::abs(Result1 - Result2) < 0.001f;
	CHECK(Match);
}

TEST_CASE("Texture Address - Clamp At UV(0.0, 0.5)")
{
	FTex2D Tex = CreateTestTexture();

	// At UV(0.0, 0.5), should clamp to edge
	// IntPoint0 = floor(-0.5, 1.5) = (-1, 1)
	// IntPoint1 = (-1, 1) + (1, 1) = (0, 2)
	// With Clamp: (-1, 1) clamps to (0, 1), (0, 2) stays (0, 2)
	// Should interpolate between pixels: (0,1)=4, (0,2)=8, (0,1)=4, (0,2)=8
	// LocalTexcoord = (0.5, 0.5)
	// Result = 0.25 * (4 + 8 + 4 + 8) = 0.25 * 24 = 6.0
	float Result;
	Tex.BilinearSample(&Result, FVector2f(0.0f, 0.5f), ETextureAddress::Clamp, ETextureAddress::Clamp);
	CHECK(std::abs(Result - 6.0f) < 0.001f);
}

TEST_CASE("Texture Address - Clamp At UV(1.0, 0.5)")
{
	FTex2D Tex = CreateTestTexture();

	// At UV(1.0, 0.5), should clamp to edge
	// IntPoint0 = (3, 1), IntPoint1 = (4, 2)
	// With Clamp: (3, 1) stays (3, 1), (4, 2) clamps to (3, 2)
	// Should interpolate between pixels: (3,1)=7, (3,2)=11, (3,1)=7, (3,2)=11
	// Result = 0.25 * (7 + 11 + 7 + 11) = 0.25 * 36 = 9.0
	float Result;
	Tex.BilinearSample(&Result, FVector2f(1.0f, 0.5f), ETextureAddress::Clamp, ETextureAddress::Clamp);
	CHECK(std::abs(Result - 9.0f) < 0.001f);
}

TEST_CASE("Texture Address - Mirror At UV(0.0, 0.5)")
{
	FTex2D Tex = CreateTestTexture();

	// At UV(0.0, 0.5), should mirror
	// IntPoint0 = floor(-0.5, 1.5) = (-1, 1)
	// IntPoint1 = (-1, 1) + (1, 1) = (0, 2)
	// With Mirror: (-1, 1) mirrors to (0, 1), (0, 2) stays (0, 2)
	// Should interpolate between pixels: (0,1)=4, (0,2)=8, (0,1)=4, (0,2)=8
	// LocalTexcoord = (0.5, 0.5)
	// Result = 0.25 * (4 + 8 + 4 + 8) = 0.25 * 24 = 6.0
	float Result;
	Tex.BilinearSample(&Result, FVector2f(0.0f, 0.5f), ETextureAddress::Mirror, ETextureAddress::Mirror);
	CHECK(std::abs(Result - 6.0f) < 0.001f);
}

TEST_CASE("Texture Address - Mirror At UV(1.0, 0.5)")
{
	FTex2D Tex = CreateTestTexture();

	// At UV(1.0, 0.5), should be at the edge (no mirroring at exactly 1.0)
	// IntPoint0 = (3, 1), IntPoint1 = (4, 2)
	// With Mirror: (3, 1) stays (3, 1), (4, 2) mirrors to (3, 2)
	// Should interpolate between pixels: (3,1)=7, (3,2)=11, (3,1)=7, (3,2)=11
	// Result = 0.25 * (7 + 11 + 7 + 11) = 0.25 * 36 = 9.0
	float Result;
	Tex.BilinearSample(&Result, FVector2f(1.0f, 0.5f), ETextureAddress::Mirror, ETextureAddress::Mirror);
	CHECK(std::abs(Result - 9.0f) < 0.001f);
}

TEST_CASE("Texture Address - Wrap At UV(0.0, 0.0)")
{
	FTex2D Tex = CreateTestTexture();

	// Test corner case: At UV(0.0, 0.0) with Wrap, should wrap around all corners
	float Result;
	Tex.BilinearSample(&Result, FVector2f(0.0f, 0.0f), ETextureAddress::Wrap, ETextureAddress::Wrap);
	// IntPoint0 = floor(-0.5, -0.5) = (-1, -1)
	// IntPoint1 = (-1, -1) + (1, 1) = (0, 0)
	// With Wrap: (-1, -1) wraps to (3, 3), (0, 0) stays (0, 0)
	// Pixels: (3,3)=15, (3,0)=3, (0,3)=12, (0,0)=0
	// LocalTexcoord = (0.5, 0.5)
	// Result = 0.25 * (15 + 3 + 12 + 0) = 0.25 * 30 = 7.5
	CHECK(std::abs(Result - 7.5f) < 0.001f);
}

TEST_CASE("Texture Address - Clamp At UV(0.0, 0.0)")
{
	FTex2D Tex = CreateTestTexture();

	// At UV(0.0, 0.0) with Clamp, should clamp to edge
	float Result;
	Tex.BilinearSample(&Result, FVector2f(0.0f, 0.0f), ETextureAddress::Clamp, ETextureAddress::Clamp);
	CHECK(std::abs(Result - 0.0f) < 0.001f);
}

TEST_CASE("Texture Address - Clamp At UV(1.0, 1.0)")
{
	FTex2D Tex = CreateTestTexture();

	// At UV(1.0, 1.0) with Clamp, should clamp to edge
	float Result;
	Tex.BilinearSample(&Result, FVector2f(1.0f, 1.0f), ETextureAddress::Clamp, ETextureAddress::Clamp);
	CHECK(std::abs(Result - 15.0f) < 0.001f);
}

TEST_CASE("Texture Address - Mixed Wrap Clamp")
{
	FTex2D Tex = CreateTestTexture();

	// Wrap X, Clamp Y at UV(1.5, 1.5)
	// PointT = (6.0, 6.0), IntPoint0 = (5, 5), IntPoint1 = (6, 6)
	// Wrap X: PointsX = [1, 2]
	// Clamp Y: PointsY = [3, 3]
	// Four points: (1,3)=13, (1,3)=13, (2,3)=14, (2,3)=14
	// Result = 0.25 * (13 + 13 + 14 + 14) = 13.5
	float Result;
	Tex.BilinearSample(&Result, FVector2f(1.5f, 1.5f), ETextureAddress::Wrap, ETextureAddress::Clamp);
	CHECK(std::abs(Result - 13.5f) < 0.001f);
}

TEST_CASE("Texture Address - Mixed Clamp Wrap")
{
	FTex2D Tex = CreateTestTexture();

	// Clamp X, Wrap Y at UV(-0.5, -0.5)
	// PointT = (-2.0, -2.0), IntPoint0 = (-3, -3), IntPoint1 = (-2, -2)
	// Clamp X: PointsX = [0, 0]
	// Wrap Y: PointsY = [1, 2]
	// Four points: (0,1)=4, (0,2)=8, (0,1)=4, (0,2)=8
	// Result = 0.25 * (4 + 8 + 4 + 8) = 6.0
	float Result;
	Tex.BilinearSample(&Result, FVector2f(-0.5f, -0.5f), ETextureAddress::Clamp, ETextureAddress::Wrap);
	CHECK(std::abs(Result - 6.0f) < 0.001f);
}

TEST_CASE("Texture Address - Mixed Mirror Wrap")
{
	FTex2D Tex = CreateTestTexture();

	// Mirror X, Wrap Y at UV(1.5, 1.5)
	// PointT = (6.0, 6.0), IntPoint0 = (5, 5), IntPoint1 = (6, 6)
	// Mirror X: PointsX = [2, 1] (5 mirrors to 2, 6 mirrors to 1)
	// Wrap Y: PointsY = [1, 2]
	// Four points: (2,1)=6, (2,2)=10, (1,1)=5, (1,2)=9
	// Result = 0.25 * (6 + 10 + 5 + 9) = 7.5
	float Result;
	Tex.BilinearSample(&Result, FVector2f(1.5f, 1.5f), ETextureAddress::Mirror, ETextureAddress::Wrap);
	CHECK(std::abs(Result - 7.5f) < 0.001f);
}

TEST_CASE("Texture Address - Mixed Wrap Mirror")
{
	FTex2D Tex = CreateTestTexture();

	// Wrap X, Mirror Y at UV(2.5, 2.5)
	// PointT = (10.0, 10.0), IntPoint0 = (9, 9), IntPoint1 = (10, 10)
	// Wrap X: PointsX = [1, 2]
	// Mirror Y: PointsY = [2, 1] (9 mirrors to 2, 10 mirrors to 1)
	// Four points: (1,2)=9, (1,1)=5, (2,2)=10, (2,1)=6
	// Result = 0.25 * (9 + 5 + 10 + 6) = 7.5
	float Result;
	Tex.BilinearSample(&Result, FVector2f(2.5f, 2.5f), ETextureAddress::Wrap, ETextureAddress::Mirror);
	CHECK(std::abs(Result - 7.5f) < 0.001f);
}
