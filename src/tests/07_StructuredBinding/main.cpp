#include <UCommon/Vector.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <UCommon_ext/doctest/doctest.h>

using namespace UCommon;

TEST_CASE("TVector2 Structured Binding - Basic Reading")
{
	// Test with float
	{
		FVector2f Vec(1.0f, 2.0f);
		const auto& [X, Y] = Vec;

		CHECK(X == 1.0f);
		CHECK(Y == 2.0f);
	}

	// Test with int
	{
		TVector2<int> Vec(10, 20);
		const auto& [X, Y] = Vec;

		CHECK(X == 10);
		CHECK(Y == 20);
	}
}

TEST_CASE("TVector2 Structured Binding - Modification")
{
	FVector2f Vec(3.0f, 4.0f);
	auto& [X, Y] = Vec;

	X = 5.0f;
	Y = 6.0f;

	CHECK(Vec.X == 5.0f);
	CHECK(Vec.Y == 6.0f);
}

TEST_CASE("TVector Structured Binding - Basic Reading")
{
	// Test with float
	{
		FVector3f Vec(1.0f, 2.0f, 3.0f);
		const auto& [X, Y, Z] = Vec;

		CHECK(X == 1.0f);
		CHECK(Y == 2.0f);
		CHECK(Z == 3.0f);
	}

	// Test with double
	{
		FVector3d Vec(10.5, 20.5, 30.5);
		const auto& [X, Y, Z] = Vec;

		CHECK(X == 10.5);
		CHECK(Y == 20.5);
		CHECK(Z == 30.5);
	}
}

TEST_CASE("TVector Structured Binding - Modification")
{
	FVector3f Vec(1.0f, 2.0f, 3.0f);
	auto& [X, Y, Z] = Vec;

	X = 4.0f;
	Y = 5.0f;
	Z = 6.0f;

	CHECK(Vec.X == 4.0f);
	CHECK(Vec.Y == 5.0f);
	CHECK(Vec.Z == 6.0f);
}

TEST_CASE("TVector4 Structured Binding - Basic Reading")
{
	// Test with float
	{
		FVector4f Vec(1.0f, 2.0f, 3.0f, 4.0f);
		const auto& [X, Y, Z, W] = Vec;

		CHECK(X == 1.0f);
		CHECK(Y == 2.0f);
		CHECK(Z == 3.0f);
		CHECK(W == 4.0f);
	}

	// Test with int
	{
		TVector4<int> Vec(10, 20, 30, 40);
		const auto& [X, Y, Z, W] = Vec;

		CHECK(X == 10);
		CHECK(Y == 20);
		CHECK(Z == 30);
		CHECK(W == 40);
	}
}

TEST_CASE("TVector4 Structured Binding - Modification")
{
	FVector4f Vec(1.0f, 2.0f, 3.0f, 4.0f);
	auto& [X, Y, Z, W] = Vec;

	X = 5.0f;
	Y = 6.0f;
	Z = 7.0f;
	W = 8.0f;

	CHECK(Vec.X == 5.0f);
	CHECK(Vec.Y == 6.0f);
	CHECK(Vec.Z == 7.0f);
	CHECK(Vec.W == 8.0f);
}

TEST_CASE("Const Structured Binding")
{
	// Test const TVector2
	{
		const FVector2f Vec(1.0f, 2.0f);
		const auto& [X, Y] = Vec;

		CHECK(X == 1.0f);
		CHECK(Y == 2.0f);
	}

	// Test const TVector
	{
		const FVector3f Vec(1.0f, 2.0f, 3.0f);
		const auto& [X, Y, Z] = Vec;

		CHECK(X == 1.0f);
		CHECK(Y == 2.0f);
		CHECK(Z == 3.0f);
	}

	// Test const TVector4
	{
		const FVector4f Vec(1.0f, 2.0f, 3.0f, 4.0f);
		const auto& [X, Y, Z, W] = Vec;

		CHECK(X == 1.0f);
		CHECK(Y == 2.0f);
		CHECK(Z == 3.0f);
		CHECK(W == 4.0f);
	}
}

TEST_CASE("Structured Binding in Loop")
{
	// Test with array of TVector2
	{
		FVector2f Vectors[] = { FVector2f(1.0f, 2.0f), FVector2f(3.0f, 4.0f), FVector2f(5.0f, 6.0f) };
		int Index = 0;
		for (const auto& [X, Y] : Vectors)
		{
			CHECK(X == 1.0f + Index * 2.0f);
			CHECK(Y == 2.0f + Index * 2.0f);
			Index++;
		}
	}

	// Test with array of TVector
	{
		FVector3f Vectors[] = { FVector3f(1.0f, 2.0f, 3.0f), FVector3f(4.0f, 5.0f, 6.0f) };
		int Index = 0;
		for (const auto& [X, Y, Z] : Vectors)
		{
			CHECK(X == 1.0f + Index * 3.0f);
			CHECK(Y == 2.0f + Index * 3.0f);
			CHECK(Z == 3.0f + Index * 3.0f);
			Index++;
		}
	}

	// Test with array of TVector4
	{
		FVector4f Vectors[] = { FVector4f(1.0f, 2.0f, 3.0f, 4.0f), FVector4f(5.0f, 6.0f, 7.0f, 8.0f) };
		int Index = 0;
		for (const auto& [X, Y, Z, W] : Vectors)
		{
			CHECK(X == 1.0f + Index * 4.0f);
			CHECK(Y == 2.0f + Index * 4.0f);
			CHECK(Z == 3.0f + Index * 4.0f);
			CHECK(W == 4.0f + Index * 4.0f);
			Index++;
		}
	}
}

TEST_CASE("Structured Binding with Vector Operations")
{
	// Test TVector2 operations
	{
		FVector2f Vec1(1.0f, 2.0f);
		FVector2f Vec2(3.0f, 4.0f);
		auto Result = Vec1 + Vec2;
		const auto& [X, Y] = Result;

		CHECK(X == 4.0f);
		CHECK(Y == 6.0f);
	}

	// Test TVector operations
	{
		FVector3f Vec1(1.0f, 2.0f, 3.0f);
		FVector3f Vec2(4.0f, 5.0f, 6.0f);
		auto Result = Vec1 * 2.0f;
		const auto& [X, Y, Z] = Result;

		CHECK(X == 2.0f);
		CHECK(Y == 4.0f);
		CHECK(Z == 6.0f);
	}

	// Test TVector4 operations
	{
		FVector4f Vec(1.0f, 2.0f, 3.0f, 4.0f);
		auto Result = Vec.Saturate();
		const auto& [X, Y, Z, W] = Result;

		CHECK(X == 1.0f);
		CHECK(Y == 1.0f);
		CHECK(Z == 1.0f);
		CHECK(W == 1.0f);
	}
}
