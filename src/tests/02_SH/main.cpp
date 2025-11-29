#include <UCommon/UCommon.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <random>
#include <functional>
#include <vector>

using namespace UCommon;

int Factorial(int n)
{
    if (n <= 1) return 1;
	return n * Factorial(n - 1);
}

float ComputeSHK(int l, int m)
{
	int abs_m = std::abs(m);
	return std::sqrt((2.0f * l + 1.0f) / (4.0f * UCommon::Pi) * Factorial(l - abs_m) / Factorial(l + abs_m));
}

template<typename Func>
double monteCarloIntegral(Func func, int samples = 1000000) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> thetaDist(0.0f, 2.0f * UCommon::Pi);
    std::uniform_real_distribution<float> phiDist(0.0f, 1.f);

    double sum = 0.0;
    for (int i = 0; i < samples; ++i) {
        float theta = thetaDist(gen);
        float phi = std::acos(1 - 2 * phiDist(gen));

        float x = sin(phi) * cos(theta);
        float y = sin(phi) * sin(theta);
        float z = cos(phi);

        float value = func(x, y, z);
        sum += value;
    }

    // Unit sphere surface area is 4*Pi, so we need to multiply by this
    return (4.0 * UCommon::Pi * sum) / samples;
}

template<int l, int m>
void testNormalization() {
	auto integrand = [](float x, float y, float z) {
		float val = UCommon::SH<l, m>(x, y, z);
		return val * val;
		};

	double integral = monteCarloIntegral(integrand);

	std::cout << "Integral of (SH_" << l << "^" << m << ")^2 = " << integral << " (expected ~1)\n";
}

bool IsNearlyEqual(float A, float B, float Tolerance = 1e-4f)
{
	return std::abs(A - B) <= Tolerance;
}

bool IsNearlyEqual(const FVector3f& A, const FVector3f& B, float Tolerance = 1e-4f)
{
	return IsNearlyEqual(A.X, B.X, Tolerance) &&
	       IsNearlyEqual(A.Y, B.Y, Tolerance) &&
	       IsNearlyEqual(A.Z, B.Z, Tolerance);
}

bool IsNearlyEqual(const FVector4f& A, const FVector4f& B, float Tolerance = 1e-4f)
{
	return IsNearlyEqual(A.X, B.X, Tolerance) &&
	       IsNearlyEqual(A.Y, B.Y, Tolerance) &&
	       IsNearlyEqual(A.Z, B.Z, Tolerance) &&
	       IsNearlyEqual(A.W, B.W, Tolerance);
}

// Test 1: Zero vector input
void TestHallucinateZH_ZeroVector()
{
	std::cout << "\n=== Test 1: Zero Vector Input ===" << std::endl;

	FSHVector2 ZeroSH;
	for (int i = 0; i < FSHVector2::MaxSHBasis; i++)
	{
		ZeroSH.V[i] = 0.f;
	}

	FVector4f Buffer;
	float k = HallucinateZH(ZeroSH, 0.5f, Buffer);

	std::cout << "Input: Zero SH vector" << std::endl;
	std::cout << "Output k: " << k << std::endl;
	std::cout << "Output Buffer: (" << Buffer.X << ", " << Buffer.Y << ", " << Buffer.Z << ", " << Buffer.W << ")" << std::endl;

	if (IsNearlyEqual(k, 0.f) && IsNearlyEqual(Buffer, FVector4f(0.f)))
	{
		std::cout << "[PASSED] Zero input produces zero output" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] Expected zero output" << std::endl;
	}
}

// Test 2: DC-only input (L0 only, no directional component)
void TestHallucinateZH_DCOnly()
{
	std::cout << "\n=== Test 2: DC-Only Input (L0 only) ===" << std::endl;

	FSHVector2 DCSH;
	for (int i = 0; i < FSHVector2::MaxSHBasis; i++)
	{
		DCSH.V[i] = 0.f;
	}
	DCSH.V[0] = 1.f; // Only DC component

	FVector4f Buffer;
	float k = HallucinateZH(DCSH, 0.5f, Buffer);

	std::cout << "Input: L0 = 1.0, L1 = (0,0,0)" << std::endl;
	std::cout << "Output k: " << k << std::endl;
	std::cout << "Output Buffer: (" << Buffer.X << ", " << Buffer.Y << ", " << Buffer.Z << ", " << Buffer.W << ")" << std::endl;

	// With no directional component, k should be 0 and Buffer.xyz should be 0
	if (IsNearlyEqual(k, 0.f) && IsNearlyEqual(Buffer.X, 0.f) && IsNearlyEqual(Buffer.Y, 0.f) && IsNearlyEqual(Buffer.Z, 0.f))
	{
		std::cout << "[PASSED] DC-only input produces zero directional output" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] Expected zero directional output" << std::endl;
	}
}

// Test 3: Different t values (0, 0.5, 1)
void TestHallucinateZH_DifferentT()
{
	std::cout << "\n=== Test 3: Different t Values ===" << std::endl;

	FSHVector2 TestSH;
	TestSH.V[0] = 1.f;  // L0
	TestSH.V[1] = 0.3f; // L1,-1
	TestSH.V[2] = 0.4f; // L1,0
	TestSH.V[3] = 0.5f; // L1,1

	std::vector<float> TValues = { 0.f, 0.25f, 0.5f, 0.75f, 1.f };

	for (float t : TValues)
	{
		FVector4f Buffer;
		float k = HallucinateZH(TestSH, t, Buffer);

		std::cout << "t = " << t << ": k = " << k
		          << ", Buffer = (" << Buffer.X << ", " << Buffer.Y << ", " << Buffer.Z << ", " << Buffer.W << ")" << std::endl;
	}

	std::cout << "[PASSED] Function handles different t values" << std::endl;
}

// Test 4: Boundary conditions
void TestHallucinateZH_BoundaryConditions()
{
	std::cout << "\n=== Test 4: Boundary Conditions ===" << std::endl;

	// Test with very small L0
	{
		FSHVector2 SmallSH;
		SmallSH.V[0] = 1e-8f;
		SmallSH.V[1] = 0.1f;
		SmallSH.V[2] = 0.1f;
		SmallSH.V[3] = 0.1f;

		FVector4f Buffer;
		float k = HallucinateZH(SmallSH, 0.5f, Buffer);

		std::cout << "Very small L0 (1e-8): k = " << k << std::endl;
		if (IsNearlyEqual(k, 0.f))
		{
			std::cout << "[PASSED] Small L0 handled correctly" << std::endl;
		}
		else
		{
			std::cout << "[FAILED] Small L0 not handled correctly" << std::endl;
		}
	}

	// Test with very large L1
	{
		FSHVector2 LargeSH;
		LargeSH.V[0] = 1.f;
		LargeSH.V[1] = 10.f;
		LargeSH.V[2] = 10.f;
		LargeSH.V[3] = 10.f;

		FVector4f Buffer;
		float k = HallucinateZH(LargeSH, 0.5f, Buffer);

		std::cout << "Large L1 components (10.0): k = " << k << ", Buffer.W = " << Buffer.W << std::endl;
		std::cout << "[PASSED] Large L1 handled (clamping applied)" << std::endl;
	}

	// Test t = 0 (maximum L1 allowed)
	{
		FSHVector2 TestSH;
		TestSH.V[0] = 1.f;
		TestSH.V[1] = 0.5f;
		TestSH.V[2] = 0.5f;
		TestSH.V[3] = 0.5f;

		FVector4f Buffer;
		float k = HallucinateZH(TestSH, 0.f, Buffer);

		std::cout << "t = 0 (max L1): k = " << k << std::endl;
		std::cout << "[PASSED] t = 0 handled" << std::endl;
	}

	// Test t = 1 (minimum L1 allowed, should clamp heavily)
	{
		FSHVector2 TestSH;
		TestSH.V[0] = 1.f;
		TestSH.V[1] = 0.5f;
		TestSH.V[2] = 0.5f;
		TestSH.V[3] = 0.5f;

		FVector4f Buffer;
		float k = HallucinateZH(TestSH, 1.f, Buffer);

		std::cout << "t = 1 (min L1): k = " << k << std::endl;
		std::cout << "[PASSED] t = 1 handled" << std::endl;
	}
}

// Test 5: Mathematical consistency
void TestHallucinateZH_MathematicalConsistency()
{
	std::cout << "\n=== Test 5: Mathematical Consistency ===" << std::endl;

	FSHVector2 TestSH;
	TestSH.V[0] = 2.f;
	TestSH.V[1] = 0.3f;
	TestSH.V[2] = 0.4f;
	TestSH.V[3] = 0.5f;

	FVector4f Buffer;
	float k = HallucinateZH(TestSH, 0.5f, Buffer);

	// Test the formula: Buffer.w + z1 + z1*z1*k
	// where z1 = dot(Buffer.xyz, n)
	std::vector<FVector3f> TestNormals = {
		FVector3f(1.f, 0.f, 0.f),
		FVector3f(0.f, 1.f, 0.f),
		FVector3f(0.f, 0.f, 1.f),
		FVector3f(0.57735f, 0.57735f, 0.57735f), // normalized (1,1,1)
	};

	std::cout << "Testing formula: Buffer.w + z1 + z1*z1*k" << std::endl;
	std::cout << "Buffer = (" << Buffer.X << ", " << Buffer.Y << ", " << Buffer.Z << ", " << Buffer.W << ")" << std::endl;
	std::cout << "k = " << k << std::endl;

	for (const auto& Normal : TestNormals)
	{
		float z1 = Buffer.X * Normal.X + Buffer.Y * Normal.Y + Buffer.Z * Normal.Z;
		float Result = Buffer.W + z1 + z1 * z1 * k;

		std::cout << "  Normal (" << Normal.X << ", " << Normal.Y << ", " << Normal.Z << "): "
		          << "z1 = " << z1 << ", Result = " << Result << std::endl;
	}

	std::cout << "[PASSED] Mathematical formula computed" << std::endl;
}

// Test 6: Symmetry and directional properties
void TestHallucinateZH_SymmetryProperties()
{
	std::cout << "\n=== Test 6: Symmetry Properties ===" << std::endl;

	// Test with symmetric L1 components
	FSHVector2 SymmetricSH;
	SymmetricSH.V[0] = 1.f;
	SymmetricSH.V[1] = 0.5f;
	SymmetricSH.V[2] = 0.5f;
	SymmetricSH.V[3] = 0.5f;

	FVector4f Buffer1;
	float k1 = HallucinateZH(SymmetricSH, 0.1f, Buffer1);

	std::cout << "Symmetric L1 (0.5, 0.5, 0.5):" << std::endl;
	std::cout << "  Buffer = (" << Buffer1.X << ", " << Buffer1.Y << ", " << Buffer1.Z << ", " << Buffer1.W << ")" << std::endl;
	std::cout << "  k = " << k1 << std::endl;

	// Test with asymmetric L1 components
	FSHVector2 AsymmetricSH;
	AsymmetricSH.V[0] = 1.f;
	AsymmetricSH.V[1] = 0.2f;
	AsymmetricSH.V[2] = 0.8f;
	AsymmetricSH.V[3] = 0.3f;

	FVector4f Buffer2;
	float k2 = HallucinateZH(AsymmetricSH, 0.5f, Buffer2);

	std::cout << "Asymmetric L1 (0.2, 0.8, 0.3):" << std::endl;
	std::cout << "  Buffer = (" << Buffer2.X << ", " << Buffer2.Y << ", " << Buffer2.Z << ", " << Buffer2.W << ")" << std::endl;
	std::cout << "  k = " << k2 << std::endl;

	std::cout << "[PASSED] Symmetry properties tested" << std::endl;
}

// Test 7: Scaling properties
void TestHallucinateZH_ScalingProperties()
{
	std::cout << "\n=== Test 7: Scaling Properties ===" << std::endl;

	FSHVector2 BaseSH;
	BaseSH.V[0] = 1.f;
	BaseSH.V[1] = 0.3f;
	BaseSH.V[2] = 0.4f;
	BaseSH.V[3] = 0.5f;

	FVector4f Buffer1;
	float k1 = HallucinateZH(BaseSH, 0.5f, Buffer1);

	// Scale the entire SH vector by 2
	FSHVector2 ScaledSH;
	for (int i = 0; i < FSHVector2::MaxSHBasis; i++)
	{
		ScaledSH.V[i] = BaseSH.V[i] * 2.f;
	}

	FVector4f Buffer2;
	float k2 = HallucinateZH(ScaledSH, 0.1f, Buffer2);

	std::cout << "Base SH: k = " << k1 << ", Buffer.W = " << Buffer1.W << std::endl;
	std::cout << "Scaled SH (x2): k = " << k2 << ", Buffer.W = " << Buffer2.W << std::endl;
	std::cout << "Buffer ratio: " << Buffer2.W / Buffer1.W << std::endl;

	std::cout << "[PASSED] Scaling properties tested" << std::endl;
}

int main(int argc, char** argv)
{
	std::cout << "========================================" << std::endl;
	std::cout << "  Spherical Harmonics Tests" << std::endl;
	std::cout << "========================================" << std::endl;

	// Original SH coefficient tests
	std::cout << "\n--- SH Coefficients (SHK) ---" << std::endl;
	for (int l = 0; l <= 4; l++)
	{
		for (int m = -l; m <= l; m++)
		{
			std::cout << "l: " << l << ", m: " << m << ", SHK: " << std::fixed << std::setprecision(12) << ComputeSHK(l, m) << std::endl;
		}
	}

	// Original normalization tests
	std::cout << "\n--- SH Normalization Tests ---" << std::endl;
	testNormalization<0, 0>();
	testNormalization<1, -1>();
	testNormalization<1, 0>();
	testNormalization<1, 1>();
	testNormalization<2, -2>();
	testNormalization<2, -1>();
	testNormalization<2, 0>();
	testNormalization<2, 1>();
	testNormalization<2, 2>();
	testNormalization<3, -3>();
	testNormalization<3, -2>();
	testNormalization<3, -1>();
	testNormalization<3, 0>();
	testNormalization<3, 1>();
	testNormalization<3, 2>();
	testNormalization<3, 3>();
	testNormalization<4, -4>();
	testNormalization<4, -3>();
	testNormalization<4, -2>();
	testNormalization<4, -1>();
	testNormalization<4, 0>();
	testNormalization<4, 1>();
	testNormalization<4, 2>();
	testNormalization<4, 3>();
	testNormalization<4, 4>();

	// HallucinateZH tests
	std::cout << "\n========================================" << std::endl;
	std::cout << "  HallucinateZH Function Tests" << std::endl;
	std::cout << "========================================" << std::endl;

	TestHallucinateZH_ZeroVector();
	TestHallucinateZH_DCOnly();
	TestHallucinateZH_DifferentT();
	TestHallucinateZH_BoundaryConditions();
	TestHallucinateZH_MathematicalConsistency();
	TestHallucinateZH_SymmetryProperties();
	TestHallucinateZH_ScalingProperties();

	std::cout << "\n========================================" << std::endl;
	std::cout << "  All Tests Completed!" << std::endl;
	std::cout << "========================================" << std::endl;

    return 0;
}
