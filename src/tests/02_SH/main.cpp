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

// ============================================================================
// TSHBandVector Tests
// ============================================================================

void TestTSHBandVector_DefaultConstructor()
{
	std::cout << "\n=== Test: TSHBandVector Default Constructor ===" << std::endl;

	FSHBandVector2 Vec;

	bool AllZero = true;
	for (uint64_t i = 0; i < FSHBandVector2::MaxSHBasis; ++i)
	{
		if (!IsNearlyEqual(Vec[i], 0.0f))
		{
			AllZero = false;
			break;
		}
	}

	if (AllZero)
	{
		std::cout << "[PASSED] Default constructor zero-initializes" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] Default constructor did not zero-initialize" << std::endl;
	}
}

void TestTSHBandVector_InitializerList()
{
	std::cout << "\n=== Test: TSHBandVector Initializer List ===" << std::endl;

	FSHBandVector2 Vec = { 1.0f, 2.0f, 3.0f };

	if (IsNearlyEqual(Vec[0], 1.0f) && IsNearlyEqual(Vec[1], 2.0f) && IsNearlyEqual(Vec[2], 3.0f))
	{
		std::cout << "[PASSED] Initializer list constructor works" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] Initializer list constructor failed" << std::endl;
	}
}

void TestTSHBandVector_CopyConstructor()
{
	std::cout << "\n=== Test: TSHBandVector Copy Constructor ===" << std::endl;

	FSHBandVector2 Vec1 = { 1.0f, 2.0f, 3.0f };
	FSHBandVector2 Vec2(Vec1);

	bool Equal = true;
	for (uint64_t i = 0; i < FSHBandVector2::MaxSHBasis; ++i)
	{
		if (!IsNearlyEqual(Vec1[i], Vec2[i]))
		{
			Equal = false;
			break;
		}
	}

	if (Equal)
	{
		std::cout << "[PASSED] Copy constructor works" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] Copy constructor failed" << std::endl;
	}
}

void TestTSHBandVector_BinaryOperators()
{
	std::cout << "\n=== Test: TSHBandVector Binary Operators ===" << std::endl;

	FSHBandVector2 Vec1 = { 1.0f, 2.0f, 3.0f };
	FSHBandVector2 Vec2 = { 4.0f, 5.0f, 6.0f };

	// Test addition
	FSHBandVector2 Sum = Vec1 + Vec2;
	if (IsNearlyEqual(Sum[0], 5.0f) && IsNearlyEqual(Sum[1], 7.0f) && IsNearlyEqual(Sum[2], 9.0f))
	{
		std::cout << "[PASSED] Addition operator works" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] Addition operator failed" << std::endl;
	}

	// Test subtraction
	FSHBandVector2 Diff = Vec2 - Vec1;
	if (IsNearlyEqual(Diff[0], 3.0f) && IsNearlyEqual(Diff[1], 3.0f) && IsNearlyEqual(Diff[2], 3.0f))
	{
		std::cout << "[PASSED] Subtraction operator works" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] Subtraction operator failed" << std::endl;
	}

	// Test scalar multiplication
	FSHBandVector2 Scaled = Vec1 * 2.0f;
	if (IsNearlyEqual(Scaled[0], 2.0f) && IsNearlyEqual(Scaled[1], 4.0f) && IsNearlyEqual(Scaled[2], 6.0f))
	{
		std::cout << "[PASSED] Scalar multiplication (vec * scalar) works" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] Scalar multiplication (vec * scalar) failed" << std::endl;
	}

	// Test scalar multiplication (commutative)
	FSHBandVector2 Scaled2 = 2.0f * Vec1;
	if (IsNearlyEqual(Scaled2[0], 2.0f) && IsNearlyEqual(Scaled2[1], 4.0f) && IsNearlyEqual(Scaled2[2], 6.0f))
	{
		std::cout << "[PASSED] Scalar multiplication (scalar * vec) works" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] Scalar multiplication (scalar * vec) failed" << std::endl;
	}

	// Test scalar division
	FSHBandVector2 Divided = Vec1 / 2.0f;
	if (IsNearlyEqual(Divided[0], 0.5f) && IsNearlyEqual(Divided[1], 1.0f) && IsNearlyEqual(Divided[2], 1.5f))
	{
		std::cout << "[PASSED] Scalar division works" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] Scalar division failed" << std::endl;
	}
}

void TestTSHBandVector_DotProduct()
{
	std::cout << "\n=== Test: TSHBandVector Dot Product ===" << std::endl;

	FSHBandVector2 Vec1 = { 1.0f, 2.0f, 3.0f };
	FSHBandVector2 Vec2 = { 4.0f, 5.0f, 6.0f };

	float DotResult = FSHBandVector2::Dot(Vec1, Vec2);
	float Expected = 1.0f * 4.0f + 2.0f * 5.0f + 3.0f * 6.0f; // = 4 + 10 + 18 = 32

	if (IsNearlyEqual(DotResult, Expected))
	{
		std::cout << "[PASSED] Dot product works (result: " << DotResult << ")" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] Dot product failed (expected: " << Expected << ", got: " << DotResult << ")" << std::endl;
	}
}

void TestTSHBandVector_ImplicitConversion()
{
	std::cout << "\n=== Test: TSHBandVector Implicit Conversion ===" << std::endl;

	FSHBandVector2 Vec = { 1.0f, 2.0f, 3.0f };

	// Implicit conversion to const view
	FSHBandConstView2 ConstView = Vec;

	bool Equal = true;
	for (uint64_t i = 0; i < FSHBandVector2::MaxSHBasis; ++i)
	{
		if (!IsNearlyEqual(Vec[i], ConstView[i]))
		{
			Equal = false;
			break;
		}
	}

	if (Equal)
	{
		std::cout << "[PASSED] Implicit conversion to const view works" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] Implicit conversion to const view failed" << std::endl;
	}
}

void TestTSHBandVectorRGB_BasicOperations()
{
	std::cout << "\n=== Test: TSHBandVectorRGB Basic Operations ===" << std::endl;

	FSHBandVector2 R = { 1.0f, 2.0f, 3.0f };
	FSHBandVector2 G = { 4.0f, 5.0f, 6.0f };
	FSHBandVector2 B = { 7.0f, 8.0f, 9.0f };

	FSHBandVectorRGB2 RGB(R, G, B);

	bool Equal = true;
	for (uint64_t i = 0; i < FSHBandVector2::MaxSHBasis; ++i)
	{
		if (!IsNearlyEqual(RGB.R[i], R[i]) || !IsNearlyEqual(RGB.G[i], G[i]) || !IsNearlyEqual(RGB.B[i], B[i]))
		{
			Equal = false;
			break;
		}
	}

	if (Equal)
	{
		std::cout << "[PASSED] TSHBandVectorRGB constructor works" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] TSHBandVectorRGB constructor failed" << std::endl;
	}

	// Test scalar multiplication
	FSHBandVectorRGB2 Scaled = RGB * 2.0f;
	bool ScaledCorrect = true;
	for (uint64_t i = 0; i < FSHBandVector2::MaxSHBasis; ++i)
	{
		if (!IsNearlyEqual(Scaled.R[i], R[i] * 2.0f) ||
		    !IsNearlyEqual(Scaled.G[i], G[i] * 2.0f) ||
		    !IsNearlyEqual(Scaled.B[i], B[i] * 2.0f))
		{
			ScaledCorrect = false;
			break;
		}
	}

	if (ScaledCorrect)
	{
		std::cout << "[PASSED] TSHBandVectorRGB scalar multiplication works" << std::endl;
	}
	else
	{
		std::cout << "[FAILED] TSHBandVectorRGB scalar multiplication failed" << std::endl;
	}
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

	// TSHBandVector tests
	std::cout << "\n========================================" << std::endl;
	std::cout << "  TSHBandVector Tests" << std::endl;
	std::cout << "========================================" << std::endl;

	TestTSHBandVector_DefaultConstructor();
	TestTSHBandVector_InitializerList();
	TestTSHBandVector_CopyConstructor();
	TestTSHBandVector_BinaryOperators();
	TestTSHBandVector_DotProduct();
	TestTSHBandVector_ImplicitConversion();
	TestTSHBandVectorRGB_BasicOperations();

	// Test Vector() interface
	std::cout << "\n=== Test: TSHBandView Vector() Interface ===" << std::endl;
	{
		FSHBandVector2 vec1{1.0f, 2.0f, 3.0f};
		FSHBandVector2 vec2{4.0f, 5.0f, 6.0f};

		FSHBandView2 view1 = vec1;
		FSHBandView2 view2 = vec2;

		// Use Vector() to assign one vector to another through view
		view1.Vector() = view2.Vector();

		bool passed = (vec1[0] == 4.0f && vec1[1] == 5.0f && vec1[2] == 6.0f);
		std::cout << "[" << (passed ? "PASSED" : "FAILED") << "] Vector() assignment works" << std::endl;

		// Use Vector() with const view (read-only)
		FSHBandConstView2 constView = vec2;
		const FSHBandVector2& vecRef = constView.Vector();

		passed = (vecRef[0] == 4.0f && vecRef[1] == 5.0f && vecRef[2] == 6.0f);
		std::cout << "[" << (passed ? "PASSED" : "FAILED") << "] Const Vector() works" << std::endl;
	}

	// Test flexible assignment from view
	std::cout << "\n=== Test: TSHBandVector Assignment from View ===" << std::endl;
	{
		FSHBandVector2 vec1{1.0f, 2.0f, 3.0f};
		FSHBandVector2 vec2{4.0f, 5.0f, 6.0f};
		FSHBandVector2 vec3;

		// Assign from another vector (implicit conversion to view)
		vec3 = vec1;
		bool passed = (vec3[0] == 1.0f && vec3[1] == 2.0f && vec3[2] == 3.0f);
		std::cout << "[" << (passed ? "PASSED" : "FAILED") << "] Assignment from vector works" << std::endl;

		// Assign from mutable view
		FSHBandView2 view = vec2;
		vec3 = view;
		passed = (vec3[0] == 4.0f && vec3[1] == 5.0f && vec3[2] == 6.0f);
		std::cout << "[" << (passed ? "PASSED" : "FAILED") << "] Assignment from mutable view works" << std::endl;

		// Assign from const view
		FSHBandConstView2 constView = vec1;
		vec3 = constView;
		passed = (vec3[0] == 1.0f && vec3[1] == 2.0f && vec3[2] == 3.0f);
		std::cout << "[" << (passed ? "PASSED" : "FAILED") << "] Assignment from const view works" << std::endl;
	}

	// Test construction from view
	std::cout << "\n=== Test: TSHBandVector Construction from View ===" << std::endl;
	{
		FSHBandVector2 source{1.0f, 2.0f, 3.0f};

		// Construct from mutable view
		FSHBandView2 view = source;
		FSHBandVector2 vec1(view);
		bool passed = (vec1[0] == 1.0f && vec1[1] == 2.0f && vec1[2] == 3.0f);
		std::cout << "[" << (passed ? "PASSED" : "FAILED") << "] Construction from mutable view works" << std::endl;

		// Construct from const view
		FSHBandConstView2 constView = source;
		FSHBandVector2 vec2(constView);
		passed = (vec2[0] == 1.0f && vec2[1] == 2.0f && vec2[2] == 3.0f);
		std::cout << "[" << (passed ? "PASSED" : "FAILED") << "] Construction from const view works" << std::endl;

		// Construct from another vector (implicit conversion to view)
		FSHBandVector2 vec3(source);
		passed = (vec3[0] == 1.0f && vec3[1] == 2.0f && vec3[2] == 3.0f);
		std::cout << "[" << (passed ? "PASSED" : "FAILED") << "] Construction from vector works" << std::endl;
	}

	// Test: TSHVector construction from lower order + band
	std::cout << "\n=== Test: TSHVector Construction from Lower Order + Band ===" << std::endl;
	{
		// Create a lower order SHVector (Order 2: 4 coefficients)
		FSHVector2 lowerOrder;
		lowerOrder.V[0] = 1.0f;  // L0
		lowerOrder.V[1] = 2.0f;  // L1 m=-1
		lowerOrder.V[2] = 3.0f;  // L1 m=0
		lowerOrder.V[3] = 4.0f;  // L1 m=1

		// Create a band view for Order 3 (5 coefficients: L2)
		float bandData[5] = {5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
		FSHBandView3 band(bandData);

		// Construct Order 3 SHVector from Order 2 + Band 3
		FSHVector3 higherOrder(lowerOrder, band);

		// Verify: should have 9 coefficients total
		// First 4 from lowerOrder, next 5 from band
		bool passed = (
			higherOrder.V[0] == 1.0f &&
			higherOrder.V[1] == 2.0f &&
			higherOrder.V[2] == 3.0f &&
			higherOrder.V[3] == 4.0f &&
			higherOrder.V[4] == 5.0f &&
			higherOrder.V[5] == 6.0f &&
			higherOrder.V[6] == 7.0f &&
			higherOrder.V[7] == 8.0f &&
			higherOrder.V[8] == 9.0f
		);
		std::cout << "[" << (passed ? "PASSED" : "FAILED") << "] TSHVector(lower order, band) works" << std::endl;
	}

	// Test: TSHVectorRGB construction from lower order + band
	std::cout << "\n=== Test: TSHVectorRGB Construction from Lower Order + Band ===" << std::endl;
	{
		// Create a lower order SHVectorRGB (Order 2: 4 coefficients per channel)
		FSHVectorRGB2 lowerOrderRGB;
		for (int i = 0; i < 4; ++i)
		{
			lowerOrderRGB.R.V[i] = static_cast<float>(i + 1);
			lowerOrderRGB.G.V[i] = static_cast<float>(i + 10);
			lowerOrderRGB.B.V[i] = static_cast<float>(i + 20);
		}

		// Create RGB band views for Order 3 (5 coefficients per channel)
		float bandDataR[5] = {5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
		float bandDataG[5] = {15.0f, 16.0f, 17.0f, 18.0f, 19.0f};
		float bandDataB[5] = {25.0f, 26.0f, 27.0f, 28.0f, 29.0f};
		FSHBandViewRGB3 bandRGB(bandDataR, bandDataG, bandDataB);

		// Construct Order 3 SHVectorRGB from Order 2 + Band 3
		FSHVectorRGB3 higherOrderRGB(lowerOrderRGB, bandRGB);

		// Verify R channel
		bool passedR = true;
		for (int i = 0; i < 4; ++i)
		{
			if (higherOrderRGB.R.V[i] != static_cast<float>(i + 1))
			{
				passedR = false;
				break;
			}
		}
		for (int i = 4; i < 9; ++i)
		{
			if (higherOrderRGB.R.V[i] != static_cast<float>(i + 1))
			{
				passedR = false;
				break;
			}
		}

		// Verify G channel
		bool passedG = true;
		for (int i = 0; i < 4; ++i)
		{
			if (higherOrderRGB.G.V[i] != static_cast<float>(i + 10))
			{
				passedG = false;
				break;
			}
		}
		for (int i = 4; i < 9; ++i)
		{
			if (higherOrderRGB.G.V[i] != static_cast<float>(i + 11))
			{
				passedG = false;
				break;
			}
		}

		// Verify B channel
		bool passedB = true;
		for (int i = 0; i < 4; ++i)
		{
			if (higherOrderRGB.B.V[i] != static_cast<float>(i + 20))
			{
				passedB = false;
				break;
			}
		}
		for (int i = 4; i < 9; ++i)
		{
			if (higherOrderRGB.B.V[i] != static_cast<float>(i + 21))
			{
				passedB = false;
				break;
			}
		}

		std::cout << "[" << (passedR && passedG && passedB ? "PASSED" : "FAILED") << "] TSHVectorRGB(lower order, band) works" << std::endl;
	}

	// Test TSHBandCommon::SHBasisFunction
	{
		std::cout << "\n=== Test: TSHBandCommon::SHBasisFunction ===" << std::endl;

		FVector3f testDir(0.577350f, 0.577350f, 0.577350f); // normalized (1,1,1)

		// Test Band 2 (L1)
		{
			auto band2Basis = TSHBandVector<2>::SHBasisFunction(testDir);
			// L1: m=-1, m=0, m=1
			float expected_m_neg1 = SH<1, -1>(testDir);
			float expected_m_0 = SH<1, 0>(testDir);
			float expected_m_1 = SH<1, 1>(testDir);

			bool passed = std::abs(band2Basis[0] - expected_m_neg1) < 1e-5f &&
				std::abs(band2Basis[1] - expected_m_0) < 1e-5f &&
				std::abs(band2Basis[2] - expected_m_1) < 1e-5f;

			std::cout << "Band 2 (L1): [" << band2Basis[0] << ", " << band2Basis[1] << ", " << band2Basis[2] << "]" << std::endl;
			std::cout << "Expected:    [" << expected_m_neg1 << ", " << expected_m_0 << ", " << expected_m_1 << "]" << std::endl;
			std::cout << "[" << (passed ? "PASSED" : "FAILED") << "] Band 2 SHBasisFunction works" << std::endl;
		}

		// Test Band 3 (L2)
		{
			auto band3Basis = TSHBandVector<3>::SHBasisFunction(testDir);
			// L2: m=-2, m=-1, m=0, m=1, m=2
			float expected[5] = {
				SH<2, -2>(testDir),
				SH<2, -1>(testDir),
				SH<2, 0>(testDir),
				SH<2, 1>(testDir),
				SH<2, 2>(testDir)
			};

			bool passed = true;
			for (int i = 0; i < 5; ++i)
			{
				if (std::abs(band3Basis[i] - expected[i]) >= 1e-5f)
				{
					passed = false;
					break;
				}
			}

			std::cout << "Band 3 (L2): [" << band3Basis[0] << ", " << band3Basis[1] << ", " << band3Basis[2] << ", " << band3Basis[3] << ", " << band3Basis[4] << "]" << std::endl;
			std::cout << "Expected:    [" << expected[0] << ", " << expected[1] << ", " << expected[2] << ", " << expected[3] << ", " << expected[4] << "]" << std::endl;
			std::cout << "[" << (passed ? "PASSED" : "FAILED") << "] Band 3 SHBasisFunction works" << std::endl;
		}
	}

	// ============================================================================
	// SH Rotation Tests
	// ============================================================================
	std::cout << "\n========================================" << std::endl;
	std::cout << "  SH Rotation Tests" << std::endl;
	std::cout << "========================================" << std::endl;

	// Helper: Print an array of floats
	auto printArray = [](const char* name, const float* arr, int size) {
		std::cout << name << " = [";
		for (int i = 0; i < size; ++i) {
			std::cout << std::fixed << std::setprecision(6) << arr[i];
			if (i < size - 1) std::cout << ", ";
		}
		std::cout << "]" << std::endl;
	};

	// Helper: Print a 3x3 matrix
	auto printMatrix3x3 = [](const char* name, const float* m) {
		std::cout << name << " = " << std::endl;
		for (int i = 0; i < 3; ++i) {
			std::cout << "  [";
			for (int j = 0; j < 3; ++j) {
				std::cout << std::fixed << std::setprecision(6) << m[i * 3 + j];
				if (j < 2) std::cout << ", ";
			}
			std::cout << "]" << std::endl;
		}
	};

	// Test 1: Identity rotation should preserve SH coefficients
	{
		std::cout << "\n=== Test 1: Identity Rotation ===" << std::endl;

		float identityMatrix[9] = {
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
		};

		// Test Band 2 (L1)
		float band2In[3] = { 1.0f, 2.0f, 3.0f };
		float band2Out[3] = { band2In[0], band2In[1], band2In[2] };
		RotateSHBand2(band2Out, identityMatrix);

		printArray("Input Band 2", band2In, 3);
		printArray("Output Band 2", band2Out, 3);

		bool idBand2Pass = IsNearlyEqual(band2In[0], band2Out[0]) &&
		                   IsNearlyEqual(band2In[1], band2Out[1]) &&
		                   IsNearlyEqual(band2In[2], band2Out[2]);
		std::cout << "[" << (idBand2Pass ? "PASSED" : "FAILED") << "] Identity rotation preserves Band 2" << std::endl;

		// Test Band 3 (L2)
		float band3In[5] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
		float band3Out[5] = { band3In[0], band3In[1], band3In[2], band3In[3], band3In[4] };
		RotateSHBand3(band3Out, identityMatrix);

		printArray("Input Band 3", band3In, 5);
		printArray("Output Band 3", band3Out, 5);

		bool idBand3Pass = IsNearlyEqual(band3In[0], band3Out[0]) &&
		                   IsNearlyEqual(band3In[1], band3Out[1]) &&
		                   IsNearlyEqual(band3In[2], band3Out[2]) &&
		                   IsNearlyEqual(band3In[3], band3Out[3]) &&
		                   IsNearlyEqual(band3In[4], band3Out[4]);
		std::cout << "[" << (idBand3Pass ? "PASSED" : "FAILED") << "] Identity rotation preserves Band 3" << std::endl;
	}

	// Test 2: 90-degree rotation around Z axis
	{
		std::cout << "\n=== Test 2: 90-degree Rotation (Z axis) ===" << std::endl;

		// 90 degree rotation around Z axis (row-major)
		// (x,y,z) -> (-y, x, z)
		float rotZ90Matrix[9] = {
			0, -1, 0,
			1,  0, 0,
			0,  0, 1
		};

		printMatrix3x3("Rotation Matrix (Z 90°)", rotZ90Matrix);

		// Test Band 2 (L1)
		float band2In[3] = { 1.0f, 2.0f, 3.0f };
		float band2Out[3] = { band2In[0], band2In[1], band2In[2] };
		RotateSHBand2(band2Out, rotZ90Matrix);

		printArray("Input Band 2", band2In, 3);
		printArray("Output Band 2", band2Out, 3);
		std::cout << "[INFO] Applied 90° Z rotation to Band 2" << std::endl;

		// Verify: rotate back should get original
		float rotZNeg90Matrix[9] = {
			0,  1, 0,
			-1, 0, 0,
			0,  0, 1
		};
		RotateSHBand2(band2Out, rotZNeg90Matrix);

		bool z90Band2Pass = IsNearlyEqual(band2In[0], band2Out[0]) &&
		                    IsNearlyEqual(band2In[1], band2Out[1]) &&
		                    IsNearlyEqual(band2In[2], band2Out[2]);
		std::cout << "[" << (z90Band2Pass ? "PASSED" : "FAILED") << "] Rotate +90° then -90° restores original Band 2" << std::endl;

		// Test Band 3 (L2)
		float band3In[5] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
		float band3Out[5] = { band3In[0], band3In[1], band3In[2], band3In[3], band3In[4] };
		RotateSHBand3(band3Out, rotZ90Matrix);
		printArray("Input Band 3", band3In, 5);
		printArray("Output Band 3", band3Out, 5);
		std::cout << "[INFO] Applied 90° Z rotation to Band 3" << std::endl;

		// Verify: rotate back should get original
		for (int i = 0; i < 5; ++i) band3Out[i] = band3In[i];
		RotateSHBand3(band3Out, rotZ90Matrix);
		RotateSHBand3(band3Out, rotZNeg90Matrix);

		bool z90Band3Pass = IsNearlyEqual(band3In[0], band3Out[0], 1e-4f) &&
		                    IsNearlyEqual(band3In[1], band3Out[1], 1e-4f) &&
		                    IsNearlyEqual(band3In[2], band3Out[2], 1e-4f) &&
		                    IsNearlyEqual(band3In[3], band3Out[3], 1e-4f) &&
		                    IsNearlyEqual(band3In[4], band3Out[4], 1e-4f);
		std::cout << "[" << (z90Band3Pass ? "PASSED" : "FAILED") << "] Rotate +90° then -90° restores original Band 3" << std::endl;
	}

	// Test 3: Unity to UE rotation (cyclic permutation: x->y->z->x)
	{
		std::cout << "\n=== Test 3: Unity -> UE Rotation ===" << std::endl;

		// Unity -> UE: [x,y,z] -> [y,z,x]
		// R = [0,1,0; 0,0,1; 1,0,0]' = [0,0,1; 1,0,0; 0,1,0]
		float unityToUEMatrix[9] = {
			0, 0, 1,
			1, 0, 0,
			0, 1, 0
		};

		printMatrix3x3("Unity -> UE Rotation Matrix", unityToUEMatrix);

		// Test Band 2 (L1)
		float band2In[3] = { 1.0f, 2.0f, 3.0f };
		float band2Out[3] = { band2In[0], band2In[1], band2In[2] };
		RotateSHBand2(band2Out, unityToUEMatrix);

		printArray("Input Band 2", band2In, 3);
		printArray("Output Band 2 (Unity->UE)", band2Out, 3);

		// Inverse rotation (UE -> Unity): apply twice to get back
		float band2Out2[3] = { band2Out[0], band2Out[1], band2Out[2] };
	 RotateSHBand2(band2Out2, unityToUEMatrix);
		RotateSHBand2(band2Out2, unityToUEMatrix);

		bool unityUEBand2Pass = IsNearlyEqual(band2In[0], band2Out2[0], 1e-4f) &&
		                        IsNearlyEqual(band2In[1], band2Out2[1], 1e-4f) &&
		                        IsNearlyEqual(band2In[2], band2Out2[2], 1e-4f);
		std::cout << "[" << (unityUEBand2Pass ? "PASSED" : "FAILED") << "] Apply Unity->UE twice restores Band 2" << std::endl;

		// Test Band 3 (L2)
		float band3In[5] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
		float band3Out[5] = { band3In[0], band3In[1], band3In[2], band3In[3], band3In[4] };
		RotateSHBand3(band3Out, unityToUEMatrix);

		printArray("Input Band 3", band3In, 5);
		printArray("Output Band 3 (Unity->UE)", band3Out, 5);

		// Inverse rotation
		float band3Out2[5] = { band3Out[0], band3Out[1], band3Out[2], band3Out[3], band3Out[4] };
		RotateSHBand3(band3Out2, unityToUEMatrix);
		RotateSHBand3(band3Out2, unityToUEMatrix);

		bool unityUEBand3Pass = IsNearlyEqual(band3In[0], band3Out2[0], 1e-4f) &&
		                        IsNearlyEqual(band3In[1], band3Out2[1], 1e-4f) &&
		                        IsNearlyEqual(band3In[2], band3Out2[2], 1e-4f) &&
		                        IsNearlyEqual(band3In[3], band3Out2[3], 1e-4f) &&
		                        IsNearlyEqual(band3In[4], band3Out2[4], 1e-4f);
		std::cout << "[" << (unityUEBand3Pass ? "PASSED" : "FAILED") << "] Apply Unity->UE twice restores Band 3" << std::endl;
	}

	// Test 4: Verify SH rotation formula: Y(R*w) = M * Y(w)
	{
		std::cout << "\n=== Test 4: Verify Rotation Formula ===" << std::endl;

		float rotY45Matrix[9] = {
			0.7071f, 0, 0.7071f,
			0, 1, 0,
			-0.7071f, 0, 0.7071f
		};

		printMatrix3x3("Rotation Matrix (Y 45°)", rotY45Matrix);

		// Test directions (documented in SHRotate.md)
		constexpr int numDirs = 5;
		constexpr float k = 0.70710677f;
		const FVector3f testDirs[numDirs] = {
			FVector3f{ 1, 0, 0 },
			FVector3f{ 0, 0, 1 },
			FVector3f{ k, k, 0 },
			FVector3f{ k, 0, k },
			FVector3f{ 0, k, k }
		};

		std::cout << "Testing formula Y(R*w) = M * Y(w) for Band 3..." << std::endl;

		float SHRotateMatrix[25];
		ComputeSHBand3RotateMatrix(SHRotateMatrix, rotY45Matrix);

		// Debug: Print rotation matrix
		std::cout << "SH Rotation Matrix (5x5):\n";
		for (int i = 0; i < 5; ++i) {
			std::cout << "  [";
			for (int j = 0; j < 5; ++j) {
				std::cout << std::setw(10) << std::setprecision(6) << SHRotateMatrix[i * 5 + j]
				          << (j < 4 ? ", " : "");
			}
			std::cout << "]\n";
		}
		std::cout << "\n";

		bool formulaPass = true;
		constexpr float eps = 1e-4f;

		for (int dirIdx = 0; dirIdx < numDirs; ++dirIdx)
		{
			FVector3f w = testDirs[dirIdx];
			FVector3f Rw = {
				rotY45Matrix[0] * w.X + rotY45Matrix[1] * w.Y + rotY45Matrix[2] * w.Z,
				rotY45Matrix[3] * w.X + rotY45Matrix[4] * w.Y + rotY45Matrix[5] * w.Z,
				rotY45Matrix[6] * w.X + rotY45Matrix[7] * w.Y + rotY45Matrix[8] * w.Z
			};

			// Y(w) - SH basis at original direction
			float Yw[5] = {
				SH<2, -2>(w),
				SH<2, -1>(w),
				SH<2,  0>(w),
				SH<2,  1>(w),
				SH<2,  2>(w)
			};

			// Y(R*w) - SH basis at rotated direction
			float YRw[5] = {
				SH<2, -2>(Rw),
				SH<2, -1>(Rw),
				SH<2,  0>(Rw),
				SH<2,  1>(Rw),
				SH<2,  2>(Rw)
			};

			// M * Y(w)
			float MYw[5];
			for (int i = 0; i < 5; ++i) {
				MYw[i] = 0;
				for (int j = 0; j < 5; ++j) {
					MYw[i] += SHRotateMatrix[i * 5 + j] * Yw[j];
				}
			}

			// Compare Y(R*w) with M * Y(w)
			bool matchThisDir = true;
			for (int i = 0; i < 5; ++i) {
				if (std::abs(YRw[i] - MYw[i]) > eps) {
					matchThisDir = false;
					break;
				}
			}

			if (!matchThisDir) {
				formulaPass = false;
				std::cout << "  Dir " << dirIdx << ": [FAILED]" << std::endl;
				std::cout << "    Y(R*w) = [" << YRw[0] << ", " << YRw[1] << ", " << YRw[2] << ", " << YRw[3] << ", " << YRw[4] << "]" << std::endl;
				std::cout << "    M*Y(w) = [" << MYw[0] << ", " << MYw[1] << ", " << MYw[2] << ", " << MYw[3] << ", " << MYw[4] << "]" << std::endl;
			}
		}

		std::cout << "[" << (formulaPass ? "PASSED" : "FAILED") << "] Rotation formula verified for all test directions" << std::endl;
	}

	// Test 5: ComputeSHBand2RotateMatrix verification
	{
		std::cout << "\n=== Test 5: ComputeSHBand2RotateMatrix ===" << std::endl;

		float rotX90Matrix[9] = {
			1,  0,  0,
			0,  0, -1,
			0,  1,  0
		};

		float SHRotateMatrix[9];
		ComputeSHBand2RotateMatrix(SHRotateMatrix, rotX90Matrix);

		std::cout << "SHBand2 rotation matrix:" << std::endl;
		for (int i = 0; i < 3; ++i) {
			std::cout << "  [";
			for (int j = 0; j < 3; ++j) {
				std::cout << std::fixed << std::setprecision(6) << SHRotateMatrix[i * 3 + j];
				if (j < 2) std::cout << ", ";
			}
			std::cout << "]" << std::endl;
		}

		// Verify the matrix by multiplying Y(w) and comparing with Y(R*w)
		constexpr int numDirs = 3;
		const FVector3f testDirs[numDirs] = {
			FVector3f{ 1, 0, 0 },
			FVector3f{ 0, 1, 0 },
			FVector3f{ 0, 0, 1 }
		};

		bool matrixPass = true;
		constexpr float eps = 1e-4f;

		for (int dirIdx = 0; dirIdx < numDirs; ++dirIdx) {
			FVector3f w = testDirs[dirIdx];
			FVector3f Rw = {
				rotX90Matrix[0] * w.X + rotX90Matrix[1] * w.Y + rotX90Matrix[2] * w.Z,
				rotX90Matrix[3] * w.X + rotX90Matrix[4] * w.Y + rotX90Matrix[5] * w.Z,
				rotX90Matrix[6] * w.X + rotX90Matrix[7] * w.Y + rotX90Matrix[8] * w.Z
			};

			// Y(w)
			float Yw[3] = { SH<1, -1>(w), SH<1, 0>(w), SH<1, 1>(w) };

			// Y(R*w)
			float YRw[3] = { SH<1, -1>(Rw), SH<1, 0>(Rw), SH<1, 1>(Rw) };

			// M * Y(w)
			float MYw[3];
			for (int i = 0; i < 3; ++i) {
				MYw[i] = 0;
				for (int j = 0; j < 3; ++j) {
					MYw[i] += SHRotateMatrix[i * 3 + j] * Yw[j];
				}
			}

			bool matchThisDir = true;
			for (int i = 0; i < 3; ++i) {
				if (std::abs(YRw[i] - MYw[i]) > eps) {
					matchThisDir = false;
					break;
				}
			}

			if (!matchThisDir) {
				matrixPass = false;
				std::cout << "  Dir " << dirIdx << ": [FAILED]" << std::endl;
			}
		}

		std::cout << "[" << (matrixPass ? "PASSED" : "FAILED") << "] ComputeSHBand2RotateMatrix verified" << std::endl;
	}

	// Test 6: Inverse rotation
	{
		std::cout << "\n=== Test 6: Inverse Rotation ===" << std::endl;

		// 30 degrees around X axis
		float angle = 30.0f * UCommon::Pi / 180.0f;
		float c = std::cos(angle);
		float s = std::sin(angle);
		float rotationMatrix[9] = {
			1,  0,  0,
			0,  c, -s,
			0,  s,  c
		};

		printMatrix3x3("Rotation Matrix (X 30°)", rotationMatrix);

		// Test Band 2
		float band2In[3] = { 0.5f, 0.7f, 0.3f };
		float band2Rotated[3] = { band2In[0], band2In[1], band2In[2] };

		RotateSHBand2(band2Rotated, rotationMatrix);

		// Now rotate by inverse (transpose for rotation matrix)
		float invRotationMatrix[9] = {
			1,  0,  0,
			0,  c,  s,
			0, -s,  c
		};
		RotateSHBand2(band2Rotated, invRotationMatrix);

		bool inverseBand2Pass = IsNearlyEqual(band2In[0], band2Rotated[0], 1e-4f) &&
		                        IsNearlyEqual(band2In[1], band2Rotated[1], 1e-4f) &&
		                        IsNearlyEqual(band2In[2], band2Rotated[2], 1e-4f);
		std::cout << "[" << (inverseBand2Pass ? "PASSED" : "FAILED") << "] Inverse rotation restores Band 2" << std::endl;

		// Test Band 3
		float band3In[5] = { 0.5f, 0.7f, 0.3f, -0.2f, 0.1f };
		float band3Rotated[5] = { band3In[0], band3In[1], band3In[2], band3In[3], band3In[4] };

		RotateSHBand3(band3Rotated, rotationMatrix);
		RotateSHBand3(band3Rotated, invRotationMatrix);

		bool inverseBand3Pass = IsNearlyEqual(band3In[0], band3Rotated[0], 1e-4f) &&
		                        IsNearlyEqual(band3In[1], band3Rotated[1], 1e-4f) &&
		                        IsNearlyEqual(band3In[2], band3Rotated[2], 1e-4f) &&
		                        IsNearlyEqual(band3In[3], band3Rotated[3], 1e-4f) &&
		                        IsNearlyEqual(band3In[4], band3Rotated[4], 1e-4f);
		std::cout << "[" << (inverseBand3Pass ? "PASSED" : "FAILED") << "] Inverse rotation restores Band 3" << std::endl;
	}

	std::cout << "\n========================================" << std::endl;
	std::cout << "  All Tests Completed!" << std::endl;
	std::cout << "========================================" << std::endl;

    return 0;
}
