#include <UCommon/Vector.h>

#include <iostream>
#include <cassert>

using namespace UCommon;

void TestTVector2StructuredBinding()
{
	std::cout << "Testing TVector2 structured binding..." << std::endl;

	// Test with float
	{
		FVector2f Vec(1.0f, 2.0f);
		const auto& [X, Y] = Vec;
		
		assert(X == 1.0f);
		assert(Y == 2.0f);
		std::cout << "  FVector2f: X=" << X << ", Y=" << Y << " [PASSED]" << std::endl;
	}

	// Test with int
	{
		TVector2<int> Vec(10, 20);
		const auto& [X, Y] = Vec;
		
		assert(X == 10);
		assert(Y == 20);
		std::cout << "  TVector2<int>: X=" << X << ", Y=" << Y << " [PASSED]" << std::endl;
	}

	// Test modification through structured binding
	{
		FVector2f Vec(3.0f, 4.0f);
		auto& [X, Y] = Vec;
		
		X = 5.0f;
		Y = 6.0f;
		
		assert(Vec.X == 5.0f);
		assert(Vec.Y == 6.0f);
		std::cout << "  TVector2 modification: Vec.X=" << Vec.X << ", Vec.Y=" << Vec.Y << " [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestTVectorStructuredBinding()
{
	std::cout << "Testing TVector (3D) structured binding..." << std::endl;

	// Test with float
	{
		FVector3f Vec(1.0f, 2.0f, 3.0f);
		const auto& [X, Y, Z] = Vec;
		
		assert(X == 1.0f);
		assert(Y == 2.0f);
		assert(Z == 3.0f);
		std::cout << "  FVector3f: X=" << X << ", Y=" << Y << ", Z=" << Z << " [PASSED]" << std::endl;
	}

	// Test with double
	{
		FVector3d Vec(10.5, 20.5, 30.5);
		const auto& [X, Y, Z] = Vec;
		
		assert(X == 10.5);
		assert(Y == 20.5);
		assert(Z == 30.5);
		std::cout << "  FVector3d: X=" << X << ", Y=" << Y << ", Z=" << Z << " [PASSED]" << std::endl;
	}

	// Test modification through structured binding
	{
		FVector3f Vec(1.0f, 2.0f, 3.0f);
		auto& [X, Y, Z] = Vec;
		
		X = 4.0f;
		Y = 5.0f;
		Z = 6.0f;
		
		assert(Vec.X == 4.0f);
		assert(Vec.Y == 5.0f);
		assert(Vec.Z == 6.0f);
		std::cout << "  TVector modification: Vec.X=" << Vec.X << ", Vec.Y=" << Vec.Y << ", Vec.Z=" << Vec.Z << " [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestTVector4StructuredBinding()
{
	std::cout << "Testing TVector4 structured binding..." << std::endl;

	// Test with float
	{
		FVector4f Vec(1.0f, 2.0f, 3.0f, 4.0f);
		const auto& [X, Y, Z, W] = Vec;
		
		assert(X == 1.0f);
		assert(Y == 2.0f);
		assert(Z == 3.0f);
		assert(W == 4.0f);
		std::cout << "  FVector4f: X=" << X << ", Y=" << Y << ", Z=" << Z << ", W=" << W << " [PASSED]" << std::endl;
	}

	// Test with int
	{
		TVector4<int> Vec(10, 20, 30, 40);
		const auto& [X, Y, Z, W] = Vec;
		
		assert(X == 10);
		assert(Y == 20);
		assert(Z == 30);
		assert(W == 40);
		std::cout << "  TVector4<int>: X=" << X << ", Y=" << Y << ", Z=" << Z << ", W=" << W << " [PASSED]" << std::endl;
	}

	// Test modification through structured binding
	{
		FVector4f Vec(1.0f, 2.0f, 3.0f, 4.0f);
		auto& [X, Y, Z, W] = Vec;
		
		X = 5.0f;
		Y = 6.0f;
		Z = 7.0f;
		W = 8.0f;
		
		assert(Vec.X == 5.0f);
		assert(Vec.Y == 6.0f);
		assert(Vec.Z == 7.0f);
		assert(Vec.W == 8.0f);
		std::cout << "  TVector4 modification: Vec.X=" << Vec.X << ", Vec.Y=" << Vec.Y << ", Vec.Z=" << Vec.Z << ", Vec.W=" << Vec.W << " [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestConstStructuredBinding()
{
	std::cout << "Testing const structured binding..." << std::endl;

	// Test const TVector2
	{
		const FVector2f Vec(1.0f, 2.0f);
		const auto& [X, Y] = Vec;

		assert(X == 1.0f);
		assert(Y == 2.0f);
		std::cout << "  const FVector2f: X=" << X << ", Y=" << Y << " [PASSED]" << std::endl;
	}

	// Test const TVector
	{
		const FVector3f Vec(1.0f, 2.0f, 3.0f);
		const auto& [X, Y, Z] = Vec;

		assert(X == 1.0f);
		assert(Y == 2.0f);
		assert(Z == 3.0f);
		std::cout << "  const FVector3f: X=" << X << ", Y=" << Y << ", Z=" << Z << " [PASSED]" << std::endl;
	}

	// Test const TVector4
	{
		const FVector4f Vec(1.0f, 2.0f, 3.0f, 4.0f);
		const auto& [X, Y, Z, W] = Vec;

		assert(X == 1.0f);
		assert(Y == 2.0f);
		assert(Z == 3.0f);
		assert(W == 4.0f);
		std::cout << "  const FVector4f: X=" << X << ", Y=" << Y << ", Z=" << Z << ", W=" << W << " [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestStructuredBindingInLoop()
{
	std::cout << "Testing structured binding in loop..." << std::endl;

	// Test with array of TVector2
	{
		FVector2f Vectors[] = { FVector2f(1.0f, 2.0f), FVector2f(3.0f, 4.0f), FVector2f(5.0f, 6.0f) };
		int Index = 0;
		for (const auto& [X, Y] : Vectors)
		{
			assert(X == 1.0f + Index * 2.0f);
			assert(Y == 2.0f + Index * 2.0f);
			Index++;
		}
		std::cout << "  TVector2 array loop: [PASSED]" << std::endl;
	}

	// Test with array of TVector
	{
		FVector3f Vectors[] = { FVector3f(1.0f, 2.0f, 3.0f), FVector3f(4.0f, 5.0f, 6.0f) };
		int Index = 0;
		for (const auto& [X, Y, Z] : Vectors)
		{
			assert(X == 1.0f + Index * 3.0f);
			assert(Y == 2.0f + Index * 3.0f);
			assert(Z == 3.0f + Index * 3.0f);
			Index++;
		}
		std::cout << "  TVector array loop: [PASSED]" << std::endl;
	}

	// Test with array of TVector4
	{
		FVector4f Vectors[] = { FVector4f(1.0f, 2.0f, 3.0f, 4.0f), FVector4f(5.0f, 6.0f, 7.0f, 8.0f) };
		int Index = 0;
		for (const auto& [X, Y, Z, W] : Vectors)
		{
			assert(X == 1.0f + Index * 4.0f);
			assert(Y == 2.0f + Index * 4.0f);
			assert(Z == 3.0f + Index * 4.0f);
			assert(W == 4.0f + Index * 4.0f);
			Index++;
		}
		std::cout << "  TVector4 array loop: [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestStructuredBindingWithOperations()
{
	std::cout << "Testing structured binding with vector operations..." << std::endl;

	// Test TVector2 operations
	{
		FVector2f Vec1(1.0f, 2.0f);
		FVector2f Vec2(3.0f, 4.0f);
		auto Result = Vec1 + Vec2;
		const auto& [X, Y] = Result;

		assert(X == 4.0f);
		assert(Y == 6.0f);
		std::cout << "  TVector2 addition: X=" << X << ", Y=" << Y << " [PASSED]" << std::endl;
	}

	// Test TVector operations
	{
		FVector3f Vec1(1.0f, 2.0f, 3.0f);
		FVector3f Vec2(4.0f, 5.0f, 6.0f);
		auto Result = Vec1 * 2.0f;
		const auto& [X, Y, Z] = Result;

		assert(X == 2.0f);
		assert(Y == 4.0f);
		assert(Z == 6.0f);
		std::cout << "  TVector multiplication: X=" << X << ", Y=" << Y << ", Z=" << Z << " [PASSED]" << std::endl;
	}

	// Test TVector4 operations
	{
		FVector4f Vec(1.0f, 2.0f, 3.0f, 4.0f);
		auto Result = Vec.Saturate();
		const auto& [X, Y, Z, W] = Result;

		assert(X == 1.0f);
		assert(Y == 1.0f);
		assert(Z == 1.0f);
		assert(W == 1.0f);
		std::cout << "  TVector4 saturate: X=" << X << ", Y=" << Y << ", Z=" << Z << ", W=" << W << " [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

int main(int argc, char** argv)
{
	std::cout << "=== Testing Structured Binding for TVector234 ===" << std::endl << std::endl;

	TestTVector2StructuredBinding();
	TestTVectorStructuredBinding();
	TestTVector4StructuredBinding();
	TestConstStructuredBinding();
	TestStructuredBindingInLoop();
	TestStructuredBindingWithOperations();

	std::cout << "=== All tests passed! ===" << std::endl;

	return 0;
}

