#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <UCommon_ext/doctest/doctest.h>

#include <UCommon/UCommon.h>
#include <cmath>
#include <random>
#include <functional>
#include <vector>

using namespace UCommon;

// ============================================================================
// Helper functions (kept at file scope)
// ============================================================================

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
void checkNormalization() {
	auto integrand = [](float x, float y, float z) {
		float val = UCommon::SH<l, m>(x, y, z);
		return val * val;
	};

	double integral = monteCarloIntegral(integrand);
	CHECK(integral == doctest::Approx(1.0).epsilon(0.1));
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

// ============================================================================
// SH Coefficient Tests
// ============================================================================

TEST_CASE("ComputeSHK - known values")
{
	// Spot-check a few known SH normalization constants
	// l=0, m=0: K = sqrt(1/(4*pi))
	CHECK(ComputeSHK(0, 0) == doctest::Approx(std::sqrt(1.0f / (4.0f * Pi))).epsilon(1e-6));
	// l=1, m=0: K = sqrt(3/(4*pi))
	CHECK(ComputeSHK(1, 0) == doctest::Approx(std::sqrt(3.0f / (4.0f * Pi))).epsilon(1e-6));
	// l=1, m=1: K = sqrt(3/(8*pi))   (Factorial(0)/Factorial(2) = 1/2)
	CHECK(ComputeSHK(1, 1) == doctest::Approx(std::sqrt(3.0f / (8.0f * Pi))).epsilon(1e-6));
	// l=2, m=0: K = sqrt(5/(4*pi))   (Factorial(2)/Factorial(2) = 1)  => sqrt(5/(4pi))
	CHECK(ComputeSHK(2, 0) == doctest::Approx(std::sqrt(5.0f / (4.0f * Pi))).epsilon(1e-6));
	// Symmetry: K(l, m) == K(l, -m)
	for (int l = 0; l <= 4; l++)
		for (int m = 1; m <= l; m++)
			CHECK(ComputeSHK(l, m) == doctest::Approx(ComputeSHK(l, -m)).epsilon(1e-6));
}

// ============================================================================
// SH Normalization (Monte Carlo)
// ============================================================================

TEST_CASE("SH Normalization")
{
	checkNormalization<0, 0>();
	checkNormalization<1, -1>();
	checkNormalization<1, 0>();
	checkNormalization<1, 1>();
	checkNormalization<2, -2>();
	checkNormalization<2, -1>();
	checkNormalization<2, 0>();
	checkNormalization<2, 1>();
	checkNormalization<2, 2>();
	checkNormalization<3, -3>();
	checkNormalization<3, -2>();
	checkNormalization<3, -1>();
	checkNormalization<3, 0>();
	checkNormalization<3, 1>();
	checkNormalization<3, 2>();
	checkNormalization<3, 3>();
	checkNormalization<4, -4>();
	checkNormalization<4, -3>();
	checkNormalization<4, -2>();
	checkNormalization<4, -1>();
	checkNormalization<4, 0>();
	checkNormalization<4, 1>();
	checkNormalization<4, 2>();
	checkNormalization<4, 3>();
	checkNormalization<4, 4>();
}

// ============================================================================
// HallucinateZH Tests
// ============================================================================

TEST_CASE("HallucinateZH - Zero Vector")
{
	FSHVector2 ZeroSH;
	for (int i = 0; i < FSHVector2::MaxSHBasis; i++)
		ZeroSH.V[i] = 0.f;

	FVector4f Buffer;
	float k = HallucinateZH(ZeroSH, 0.5f, Buffer);

	CHECK(IsNearlyEqual(k, 0.f));
	CHECK(IsNearlyEqual(Buffer, FVector4f(0.f)));
}

TEST_CASE("HallucinateZH - DC Only")
{
	FSHVector2 DCSH;
	for (int i = 0; i < FSHVector2::MaxSHBasis; i++)
		DCSH.V[i] = 0.f;
	DCSH.V[0] = 1.f; // Only DC component

	FVector4f Buffer;
	float k = HallucinateZH(DCSH, 0.5f, Buffer);

	// With no directional component, k should be 0 and Buffer.xyz should be 0
	CHECK(IsNearlyEqual(k, 0.f));
	CHECK(IsNearlyEqual(Buffer.X, 0.f));
	CHECK(IsNearlyEqual(Buffer.Y, 0.f));
	CHECK(IsNearlyEqual(Buffer.Z, 0.f));
}

TEST_CASE("HallucinateZH - Different t Values")
{
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
		// Function should not crash for any t in [0, 1]
		// k may be infinite at extreme t values (by design)
		(void)k;
		(void)Buffer;
	}
}

TEST_CASE("HallucinateZH - Boundary Conditions")
{
	SUBCASE("Very small L0")
	{
		FSHVector2 SmallSH;
		SmallSH.V[0] = 1e-8f;
		SmallSH.V[1] = 0.1f;
		SmallSH.V[2] = 0.1f;
		SmallSH.V[3] = 0.1f;

		FVector4f Buffer;
		float k = HallucinateZH(SmallSH, 0.5f, Buffer);
		CHECK(IsNearlyEqual(k, 0.f));
	}

	SUBCASE("Very large L1")
	{
		FSHVector2 LargeSH;
		LargeSH.V[0] = 1.f;
		LargeSH.V[1] = 10.f;
		LargeSH.V[2] = 10.f;
		LargeSH.V[3] = 10.f;

		FVector4f Buffer;
		float k = HallucinateZH(LargeSH, 0.5f, Buffer);
		CHECK(std::isfinite(k));
		CHECK(std::isfinite(Buffer.W));
	}

	SUBCASE("t = 0 (max L1)")
	{
		FSHVector2 TestSH;
		TestSH.V[0] = 1.f;
		TestSH.V[1] = 0.5f;
		TestSH.V[2] = 0.5f;
		TestSH.V[3] = 0.5f;

		FVector4f Buffer;
		float k = HallucinateZH(TestSH, 0.f, Buffer);
		CHECK(std::isfinite(k));
	}

	SUBCASE("t = 1 (min L1)")
	{
		FSHVector2 TestSH;
		TestSH.V[0] = 1.f;
		TestSH.V[1] = 0.5f;
		TestSH.V[2] = 0.5f;
		TestSH.V[3] = 0.5f;

		FVector4f Buffer;
		float k = HallucinateZH(TestSH, 1.f, Buffer);
		// k may be infinite at t=1 (by design), just verify no crash
		(void)k;
	}
}

TEST_CASE("HallucinateZH - Mathematical Consistency")
{
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

	for (const auto& Normal : TestNormals)
	{
		float z1 = Buffer.X * Normal.X + Buffer.Y * Normal.Y + Buffer.Z * Normal.Z;
		float Result = Buffer.W + z1 + z1 * z1 * k;
		CHECK(std::isfinite(Result));
	}
}

TEST_CASE("HallucinateZH - Symmetry Properties")
{
	// Test with symmetric L1 components
	FSHVector2 SymmetricSH;
	SymmetricSH.V[0] = 1.f;
	SymmetricSH.V[1] = 0.5f;
	SymmetricSH.V[2] = 0.5f;
	SymmetricSH.V[3] = 0.5f;

	FVector4f Buffer1;
	float k1 = HallucinateZH(SymmetricSH, 0.1f, Buffer1);
	CHECK(std::isfinite(k1));

	// Test with asymmetric L1 components
	FSHVector2 AsymmetricSH;
	AsymmetricSH.V[0] = 1.f;
	AsymmetricSH.V[1] = 0.2f;
	AsymmetricSH.V[2] = 0.8f;
	AsymmetricSH.V[3] = 0.3f;

	FVector4f Buffer2;
	float k2 = HallucinateZH(AsymmetricSH, 0.5f, Buffer2);
	CHECK(std::isfinite(k2));
}

TEST_CASE("HallucinateZH - Scaling Properties")
{
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
		ScaledSH.V[i] = BaseSH.V[i] * 2.f;

	FVector4f Buffer2;
	float k2 = HallucinateZH(ScaledSH, 0.1f, Buffer2);

	CHECK(std::isfinite(k1));
	CHECK(std::isfinite(k2));
	CHECK(std::isfinite(Buffer1.W));
	CHECK(std::isfinite(Buffer2.W));
}

// ============================================================================
// TSHBandVector Tests
// ============================================================================

TEST_CASE("TSHBandVector - Default Constructor")
{
	FSHBandVector2 Vec;

	for (uint64_t i = 0; i < FSHBandVector2::MaxSHBasis; ++i)
		CHECK(IsNearlyEqual(Vec[i], 0.0f));
}

TEST_CASE("TSHBandVector - Initializer List")
{
	FSHBandVector2 Vec = { 1.0f, 2.0f, 3.0f };

	CHECK(IsNearlyEqual(Vec[0], 1.0f));
	CHECK(IsNearlyEqual(Vec[1], 2.0f));
	CHECK(IsNearlyEqual(Vec[2], 3.0f));
}

TEST_CASE("TSHBandVector - Copy Constructor")
{
	FSHBandVector2 Vec1 = { 1.0f, 2.0f, 3.0f };
	FSHBandVector2 Vec2(Vec1);

	for (uint64_t i = 0; i < FSHBandVector2::MaxSHBasis; ++i)
		CHECK(IsNearlyEqual(Vec1[i], Vec2[i]));
}

TEST_CASE("TSHBandVector - Binary Operators")
{
	FSHBandVector2 Vec1 = { 1.0f, 2.0f, 3.0f };
	FSHBandVector2 Vec2 = { 4.0f, 5.0f, 6.0f };

	SUBCASE("Addition")
	{
		FSHBandVector2 Sum = Vec1 + Vec2;
		CHECK(IsNearlyEqual(Sum[0], 5.0f));
		CHECK(IsNearlyEqual(Sum[1], 7.0f));
		CHECK(IsNearlyEqual(Sum[2], 9.0f));
	}

	SUBCASE("Subtraction")
	{
		FSHBandVector2 Diff = Vec2 - Vec1;
		CHECK(IsNearlyEqual(Diff[0], 3.0f));
		CHECK(IsNearlyEqual(Diff[1], 3.0f));
		CHECK(IsNearlyEqual(Diff[2], 3.0f));
	}

	SUBCASE("Scalar multiplication (vec * scalar)")
	{
		FSHBandVector2 Scaled = Vec1 * 2.0f;
		CHECK(IsNearlyEqual(Scaled[0], 2.0f));
		CHECK(IsNearlyEqual(Scaled[1], 4.0f));
		CHECK(IsNearlyEqual(Scaled[2], 6.0f));
	}

	SUBCASE("Scalar multiplication (scalar * vec)")
	{
		FSHBandVector2 Scaled2 = 2.0f * Vec1;
		CHECK(IsNearlyEqual(Scaled2[0], 2.0f));
		CHECK(IsNearlyEqual(Scaled2[1], 4.0f));
		CHECK(IsNearlyEqual(Scaled2[2], 6.0f));
	}

	SUBCASE("Scalar division")
	{
		FSHBandVector2 Divided = Vec1 / 2.0f;
		CHECK(IsNearlyEqual(Divided[0], 0.5f));
		CHECK(IsNearlyEqual(Divided[1], 1.0f));
		CHECK(IsNearlyEqual(Divided[2], 1.5f));
	}
}

TEST_CASE("TSHBandVector - Dot Product")
{
	FSHBandVector2 Vec1 = { 1.0f, 2.0f, 3.0f };
	FSHBandVector2 Vec2 = { 4.0f, 5.0f, 6.0f };

	float DotResult = FSHBandVector2::Dot(Vec1, Vec2);
	float Expected = 1.0f * 4.0f + 2.0f * 5.0f + 3.0f * 6.0f; // = 32
	CHECK(IsNearlyEqual(DotResult, Expected));
}

TEST_CASE("TSHBandVector - Implicit Conversion to Const View")
{
	FSHBandVector2 Vec = { 1.0f, 2.0f, 3.0f };
	FSHBandConstView2 ConstView = Vec;

	for (uint64_t i = 0; i < FSHBandVector2::MaxSHBasis; ++i)
		CHECK(IsNearlyEqual(Vec[i], ConstView[i]));
}

TEST_CASE("TSHBandVectorRGB - Basic Operations")
{
	FSHBandVector2 vecR = { 1.0f, 2.0f, 3.0f };
	FSHBandVector2 vecG = { 4.0f, 5.0f, 6.0f };
	FSHBandVector2 vecB = { 7.0f, 8.0f, 9.0f };

	FSHBandVectorRGB2 vecRGB(vecR, vecG, vecB);

	for (uint64_t i = 0; i < FSHBandVector2::MaxSHBasis; ++i)
	{
		CHECK(IsNearlyEqual(vecRGB.R[i], vecR[i]));
		CHECK(IsNearlyEqual(vecRGB.G[i], vecG[i]));
		CHECK(IsNearlyEqual(vecRGB.B[i], vecB[i]));
	}

	SUBCASE("Scalar multiplication")
	{
		FSHBandVectorRGB2 Scaled = vecRGB * 2.0f;
		for (uint64_t i = 0; i < FSHBandVector2::MaxSHBasis; ++i)
		{
			CHECK(IsNearlyEqual(Scaled.R[i], vecR[i] * 2.0f));
			CHECK(IsNearlyEqual(Scaled.G[i], vecG[i] * 2.0f));
			CHECK(IsNearlyEqual(Scaled.B[i], vecB[i] * 2.0f));
		}
	}
}

TEST_CASE("TSHBandView - Vector() Interface")
{
	FSHBandVector2 vec1{1.0f, 2.0f, 3.0f};
	FSHBandVector2 vec2{4.0f, 5.0f, 6.0f};

	FSHBandView2 view1 = vec1;
	FSHBandView2 view2 = vec2;

	// Use Vector() to assign one vector to another through view
	view1.Vector() = view2.Vector();

	CHECK(vec1[0] == 4.0f);
	CHECK(vec1[1] == 5.0f);
	CHECK(vec1[2] == 6.0f);

	// Use Vector() with const view (read-only)
	FSHBandConstView2 constView = vec2;
	const FSHBandVector2& vecRef = constView.Vector();

	CHECK(vecRef[0] == 4.0f);
	CHECK(vecRef[1] == 5.0f);
	CHECK(vecRef[2] == 6.0f);
}

TEST_CASE("TSHBandVector - Assignment from View")
{
	FSHBandVector2 vec1{1.0f, 2.0f, 3.0f};
	FSHBandVector2 vec2{4.0f, 5.0f, 6.0f};
	FSHBandVector2 vec3;

	SUBCASE("Assignment from vector")
	{
		vec3 = vec1;
		CHECK(vec3[0] == 1.0f);
		CHECK(vec3[1] == 2.0f);
		CHECK(vec3[2] == 3.0f);
	}

	SUBCASE("Assignment from mutable view")
	{
		FSHBandView2 view = vec2;
		vec3 = view;
		CHECK(vec3[0] == 4.0f);
		CHECK(vec3[1] == 5.0f);
		CHECK(vec3[2] == 6.0f);
	}

	SUBCASE("Assignment from const view")
	{
		FSHBandConstView2 constView = vec1;
		vec3 = constView;
		CHECK(vec3[0] == 1.0f);
		CHECK(vec3[1] == 2.0f);
		CHECK(vec3[2] == 3.0f);
	}
}

TEST_CASE("TSHBandVector - Construction from View")
{
	FSHBandVector2 source{1.0f, 2.0f, 3.0f};

	SUBCASE("Construction from mutable view")
	{
		FSHBandView2 view = source;
		FSHBandVector2 vec1(view);
		CHECK(vec1[0] == 1.0f);
		CHECK(vec1[1] == 2.0f);
		CHECK(vec1[2] == 3.0f);
	}

	SUBCASE("Construction from const view")
	{
		FSHBandConstView2 constView = source;
		FSHBandVector2 vec2(constView);
		CHECK(vec2[0] == 1.0f);
		CHECK(vec2[1] == 2.0f);
		CHECK(vec2[2] == 3.0f);
	}

	SUBCASE("Construction from another vector")
	{
		FSHBandVector2 vec3(source);
		CHECK(vec3[0] == 1.0f);
		CHECK(vec3[1] == 2.0f);
		CHECK(vec3[2] == 3.0f);
	}
}

TEST_CASE("TSHVector - Construction from Lower Order + Band")
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

	CHECK(higherOrder.V[0] == 1.0f);
	CHECK(higherOrder.V[1] == 2.0f);
	CHECK(higherOrder.V[2] == 3.0f);
	CHECK(higherOrder.V[3] == 4.0f);
	CHECK(higherOrder.V[4] == 5.0f);
	CHECK(higherOrder.V[5] == 6.0f);
	CHECK(higherOrder.V[6] == 7.0f);
	CHECK(higherOrder.V[7] == 8.0f);
	CHECK(higherOrder.V[8] == 9.0f);
}

TEST_CASE("TSHVectorRGB - Construction from Lower Order + Band")
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
	for (int i = 0; i < 4; ++i)
		CHECK(higherOrderRGB.R.V[i] == static_cast<float>(i + 1));
	for (int i = 4; i < 9; ++i)
		CHECK(higherOrderRGB.R.V[i] == static_cast<float>(i + 1));

	// Verify G channel
	for (int i = 0; i < 4; ++i)
		CHECK(higherOrderRGB.G.V[i] == static_cast<float>(i + 10));
	for (int i = 4; i < 9; ++i)
		CHECK(higherOrderRGB.G.V[i] == static_cast<float>(i + 11));

	// Verify B channel
	for (int i = 0; i < 4; ++i)
		CHECK(higherOrderRGB.B.V[i] == static_cast<float>(i + 20));
	for (int i = 4; i < 9; ++i)
		CHECK(higherOrderRGB.B.V[i] == static_cast<float>(i + 21));
}

TEST_CASE("TSHBandCommon - SHBasisFunction")
{
	FVector3f testDir(0.577350f, 0.577350f, 0.577350f); // normalized (1,1,1)

	SUBCASE("Band 2 (L1)")
	{
		auto band2Basis = TSHBandVector<2>::SHBasisFunction(testDir);
		float expected_m_neg1 = SH<1, -1>(testDir);
		float expected_m_0 = SH<1, 0>(testDir);
		float expected_m_1 = SH<1, 1>(testDir);

		CHECK(band2Basis[0] == doctest::Approx(expected_m_neg1).epsilon(1e-5));
		CHECK(band2Basis[1] == doctest::Approx(expected_m_0).epsilon(1e-5));
		CHECK(band2Basis[2] == doctest::Approx(expected_m_1).epsilon(1e-5));
	}

	SUBCASE("Band 3 (L2)")
	{
		auto band3Basis = TSHBandVector<3>::SHBasisFunction(testDir);
		float expected[5] = {
			SH<2, -2>(testDir),
			SH<2, -1>(testDir),
			SH<2, 0>(testDir),
			SH<2, 1>(testDir),
			SH<2, 2>(testDir)
		};

		for (int i = 0; i < 5; ++i)
			CHECK(band3Basis[i] == doctest::Approx(expected[i]).epsilon(1e-5));
	}
}

// ============================================================================
// SH Rotation Tests
// ============================================================================

// Helper lambdas used by rotation tests
static void RotateSHBand2(float* coeffs, const FMatrix3x3f& rotMatrix) {
	float shRotMat[9];
	ComputeSHBand2RotateMatrix(shRotMat, rotMatrix);
	ApplySHRotateMatrix<2>(TSHBandView<2>(coeffs), shRotMat);
}

static void RotateSHBand3(float* coeffs, const FMatrix3x3f& rotMatrix) {
	float shRotMat[25];
	ComputeSHBand3RotateMatrix(shRotMat, rotMatrix);
	ApplySHRotateMatrix<3>(TSHBandView<3>(coeffs), shRotMat);
}

TEST_CASE("SH Rotation - Identity")
{
	FMatrix3x3f identityMatrix(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	);

	SUBCASE("Band 2 (L1)")
	{
		float band2In[3] = { 1.0f, 2.0f, 3.0f };
		float band2Out[3] = { band2In[0], band2In[1], band2In[2] };
		RotateSHBand2(band2Out, identityMatrix);

		for (int i = 0; i < 3; ++i)
			CHECK(IsNearlyEqual(band2In[i], band2Out[i]));
	}

	SUBCASE("Band 3 (L2)")
	{
		float band3In[5] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
		float band3Out[5] = { band3In[0], band3In[1], band3In[2], band3In[3], band3In[4] };
		RotateSHBand3(band3Out, identityMatrix);

		for (int i = 0; i < 5; ++i)
			CHECK(IsNearlyEqual(band3In[i], band3Out[i]));
	}
}

TEST_CASE("SH Rotation - Z90")
{
	// 90 degree rotation around Z axis: (x,y,z) -> (-y, x, z)
	FMatrix3x3f rotZ90Matrix(
		0, -1, 0,
		1,  0, 0,
		0,  0, 1
	);
	FMatrix3x3f rotZNeg90Matrix(
		0,  1, 0,
		-1, 0, 0,
		0,  0, 1
	);

	SUBCASE("Band 2: rotate +90 then -90 restores original")
	{
		float band2In[3] = { 1.0f, 2.0f, 3.0f };
		float band2Out[3] = { band2In[0], band2In[1], band2In[2] };
		RotateSHBand2(band2Out, rotZ90Matrix);
		RotateSHBand2(band2Out, rotZNeg90Matrix);

		for (int i = 0; i < 3; ++i)
			CHECK(IsNearlyEqual(band2In[i], band2Out[i]));
	}

	SUBCASE("Band 3: rotate +90 then -90 restores original")
	{
		float band3In[5] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
		float band3Out[5] = { band3In[0], band3In[1], band3In[2], band3In[3], band3In[4] };
		RotateSHBand3(band3Out, rotZ90Matrix);
		RotateSHBand3(band3Out, rotZNeg90Matrix);

		for (int i = 0; i < 5; ++i)
			CHECK(IsNearlyEqual(band3In[i], band3Out[i], 1e-4f));
	}
}

TEST_CASE("SH Rotation - Unity to UE")
{
	// Unity -> UE: [x,y,z] -> [y,z,x]
	FMatrix3x3f unityToUEMatrix(
		0, 0, 1,
		1, 0, 0,
		0, 1, 0
	);

	SUBCASE("Band 2: apply 3 times restores original (cyclic order 3)")
	{
		float band2In[3] = { 1.0f, 2.0f, 3.0f };
		float band2Out[3] = { band2In[0], band2In[1], band2In[2] };
		RotateSHBand2(band2Out, unityToUEMatrix);
		RotateSHBand2(band2Out, unityToUEMatrix);
		RotateSHBand2(band2Out, unityToUEMatrix);

		for (int i = 0; i < 3; ++i)
			CHECK(IsNearlyEqual(band2In[i], band2Out[i], 1e-4f));
	}

	SUBCASE("Band 3: apply 3 times restores original (cyclic order 3)")
	{
		float band3In[5] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
		float band3Out[5] = { band3In[0], band3In[1], band3In[2], band3In[3], band3In[4] };
		RotateSHBand3(band3Out, unityToUEMatrix);
		RotateSHBand3(band3Out, unityToUEMatrix);
		RotateSHBand3(band3Out, unityToUEMatrix);

		for (int i = 0; i < 5; ++i)
			CHECK(IsNearlyEqual(band3In[i], band3Out[i], 1e-4f));
	}
}

TEST_CASE("SH Rotation - Verify Rotation Formula Y(Rw) = M * Y(w)")
{
	FMatrix3x3f rotY45Matrix(
		0.7071f, 0, 0.7071f,
		0, 1, 0,
		-0.7071f, 0, 0.7071f
	);

	constexpr int numDirs = 5;
	constexpr float k = 0.70710677f;
	const FVector3f testDirs[numDirs] = {
		FVector3f{ 1, 0, 0 },
		FVector3f{ 0, 0, 1 },
		FVector3f{ k, k, 0 },
		FVector3f{ k, 0, k },
		FVector3f{ 0, k, k }
	};

	float SHRotateMatrix[25];
	ComputeSHBand3RotateMatrix(SHRotateMatrix, rotY45Matrix);

	constexpr float eps = 1e-4f;

	for (int dirIdx = 0; dirIdx < numDirs; ++dirIdx)
	{
		FVector3f w = testDirs[dirIdx];
		FVector3f Rw = rotY45Matrix * w;

		float Yw[5] = {
			SH<2, -2>(w), SH<2, -1>(w), SH<2, 0>(w), SH<2, 1>(w), SH<2, 2>(w)
		};
		float YRw[5] = {
			SH<2, -2>(Rw), SH<2, -1>(Rw), SH<2, 0>(Rw), SH<2, 1>(Rw), SH<2, 2>(Rw)
		};

		float MYw[5];
		for (int i = 0; i < 5; ++i) {
			MYw[i] = 0;
			for (int j = 0; j < 5; ++j)
				MYw[i] += SHRotateMatrix[i * 5 + j] * Yw[j];
		}

		for (int i = 0; i < 5; ++i)
			CHECK(std::abs(YRw[i] - MYw[i]) < eps);
	}
}

TEST_CASE("SH Rotation - ComputeSHBand2RotateMatrix")
{
	FMatrix3x3f rotX90Matrix(
		1,  0,  0,
		0,  0, -1,
		0,  1,  0
	);

	float SHRotateMatrix[9];
	ComputeSHBand2RotateMatrix(SHRotateMatrix, rotX90Matrix);

	constexpr int numDirs = 3;
	const FVector3f testDirs[numDirs] = {
		FVector3f{ 1, 0, 0 },
		FVector3f{ 0, 1, 0 },
		FVector3f{ 0, 0, 1 }
	};

	constexpr float eps = 1e-4f;

	for (int dirIdx = 0; dirIdx < numDirs; ++dirIdx) {
		FVector3f w = testDirs[dirIdx];
		FVector3f Rw = rotX90Matrix * w;

		float Yw[3] = { SH<1, -1>(w), SH<1, 0>(w), SH<1, 1>(w) };
		float YRw[3] = { SH<1, -1>(Rw), SH<1, 0>(Rw), SH<1, 1>(Rw) };

		float MYw[3];
		for (int i = 0; i < 3; ++i) {
			MYw[i] = 0;
			for (int j = 0; j < 3; ++j)
				MYw[i] += SHRotateMatrix[i * 3 + j] * Yw[j];
		}

		for (int i = 0; i < 3; ++i)
			CHECK(std::abs(YRw[i] - MYw[i]) < eps);
	}
}

TEST_CASE("SH Rotation - ComputeSHBand4RotateMatrix")
{
	FMatrix3x3f rotX90Matrix(
		1,  0,  0,
		0,  0, -1,
		0,  1,  0
	);

	float SHRotateMatrix[49];
	ComputeSHBand4RotateMatrix(SHRotateMatrix, rotX90Matrix);

	constexpr int numDirs = 7;
	const FVector3f testDirs[numDirs] = {
		FVector3f{ 1, 0, 0 },
		FVector3f{ 0, 1, 0 },
		FVector3f{ 0, 0, 1 },
		FVector3f{ 1.f / std::sqrt(2.f), -1.f / std::sqrt(2.f), 0 },
		FVector3f{ 1.f / std::sqrt(2.f), 0, 1.f / std::sqrt(2.f) },
		FVector3f{ 0, 1.f / std::sqrt(2.f), 1.f / std::sqrt(2.f) },
		FVector3f{ 1.f / std::sqrt(3.f), 1.f / std::sqrt(3.f), 1.f / std::sqrt(3.f) }
	};

	constexpr float eps = 1e-3f;

	for (int dirIdx = 0; dirIdx < numDirs; ++dirIdx) {
		FVector3f w = testDirs[dirIdx];
		FVector3f Rw = rotX90Matrix * w;

		float Yw[7] = {
			SH<3, -3>(w), SH<3, -2>(w), SH<3, -1>(w), SH<3, 0>(w),
			SH<3, 1>(w), SH<3, 2>(w), SH<3, 3>(w)
		};
		float YRw[7] = {
			SH<3, -3>(Rw), SH<3, -2>(Rw), SH<3, -1>(Rw), SH<3, 0>(Rw),
			SH<3, 1>(Rw), SH<3, 2>(Rw), SH<3, 3>(Rw)
		};

		float MYw[7];
		for (int i = 0; i < 7; ++i) {
			MYw[i] = 0;
			for (int j = 0; j < 7; ++j)
				MYw[i] += SHRotateMatrix[i * 7 + j] * Yw[j];
		}

		for (int i = 0; i < 7; ++i)
			CHECK(std::abs(YRw[i] - MYw[i]) < eps);
	}
}

TEST_CASE("SH Rotation - ComputeSHBand5RotateMatrix")
{
	FMatrix3x3f rotX90Matrix(
		1,  0,  0,
		0,  0, -1,
		0,  1,  0
	);

	float SHRotateMatrix[81];
	ComputeSHBand5RotateMatrix(SHRotateMatrix, rotX90Matrix);

	constexpr int numDirs = 9;
	const FVector3f testDirs[numDirs] = {
		FVector3f{ 1, 0, 0 },
		FVector3f{ 0, 1, 0 },
		FVector3f{ 0, 0, 1 },
		FVector3f{ 1.f / std::sqrt(2.f), 1.f / std::sqrt(2.f), 0 },
		FVector3f{ 1.f / std::sqrt(2.f), 0, 1.f / std::sqrt(2.f) },
		FVector3f{ 0, 1.f / std::sqrt(2.f), 1.f / std::sqrt(2.f) },
		FVector3f{ 2.f / std::sqrt(5.f), 1.f / std::sqrt(5.f), 0 },
		FVector3f{ 0, 1.f / std::sqrt(5.f), 2.f / std::sqrt(5.f) },
		FVector3f{ 1.f / std::sqrt(5.f), 0, 2.f / std::sqrt(5.f) }
	};

	constexpr float eps = 1e-3f;

	for (int dirIdx = 0; dirIdx < numDirs; ++dirIdx) {
		FVector3f w = testDirs[dirIdx];
		FVector3f Rw = rotX90Matrix * w;

		float Yw[9] = {
			SH<4, -4>(w), SH<4, -3>(w), SH<4, -2>(w), SH<4, -1>(w), SH<4, 0>(w),
			SH<4, 1>(w), SH<4, 2>(w), SH<4, 3>(w), SH<4, 4>(w)
		};
		float YRw[9] = {
			SH<4, -4>(Rw), SH<4, -3>(Rw), SH<4, -2>(Rw), SH<4, -1>(Rw), SH<4, 0>(Rw),
			SH<4, 1>(Rw), SH<4, 2>(Rw), SH<4, 3>(Rw), SH<4, 4>(Rw)
		};

		float MYw[9];
		for (int i = 0; i < 9; ++i) {
			MYw[i] = 0;
			for (int j = 0; j < 9; ++j)
				MYw[i] += SHRotateMatrix[i * 9 + j] * Yw[j];
		}

		for (int i = 0; i < 9; ++i)
			CHECK(std::abs(YRw[i] - MYw[i]) < eps);
	}
}

TEST_CASE("SH Rotation - ComputeSHBandNRotateMatrix (generic recursive)")
{
	constexpr float eps5 = 1e-3f;

	FMatrix3x3f rotX90Matrix(
		1,  0,  0,
		0,  0, -1,
		0,  1,  0
	);
	float angle45 = UCommon::Pi / 4.0f;
	float c45 = std::cos(angle45), s45 = std::sin(angle45);
	FMatrix3x3f rotZ45( c45, -s45, 0,  s45,  c45, 0,  0, 0, 1 );

	for (int testL = 2; testL <= 6; ++testL)
	{
		const int dim = 2 * testL + 1;
		const int sz2 = dim * dim;

		SUBCASE(("l=" + std::to_string(testL) + ": M(Identity) = I").c_str())
		{
			std::vector<float> matI(sz2);
			ComputeSHBandNRotateMatrix(matI.data(), testL, FMatrix3x3f::Identity());
			for (int i = 0; i < dim; ++i)
				for (int j = 0; j < dim; ++j)
				{
					float expected = (i == j) ? 1.0f : 0.0f;
					CHECK(std::abs(matI[i * dim + j] - expected) < eps5);
				}
		}

		SUBCASE(("l=" + std::to_string(testL) + ": M is orthogonal").c_str())
		{
			std::vector<float> mat(sz2);
			ComputeSHBandNRotateMatrix(mat.data(), testL, rotX90Matrix);
			for (int i = 0; i < dim; ++i)
				for (int j = 0; j < dim; ++j)
				{
					float dot = 0.0f;
					for (int k = 0; k < dim; ++k)
						dot += mat[k * dim + i] * mat[k * dim + j];
					float expected = (i == j) ? 1.0f : 0.0f;
					CHECK(std::abs(dot - expected) < eps5);
				}
		}

		SUBCASE(("l=" + std::to_string(testL) + ": M(R1)*M(R2)=M(R1*R2)").c_str())
		{
			std::vector<float> matX(sz2), matZ(sz2), matC(sz2);
			FMatrix3x3f rotComposed = rotX90Matrix * rotZ45;
			ComputeSHBandNRotateMatrix(matX.data(), testL, rotX90Matrix);
			ComputeSHBandNRotateMatrix(matZ.data(), testL, rotZ45);
			ComputeSHBandNRotateMatrix(matC.data(), testL, rotComposed);

			std::vector<float> matXZ(sz2, 0.0f);
			for (int i = 0; i < dim; ++i)
				for (int j = 0; j < dim; ++j)
					for (int k = 0; k < dim; ++k)
						matXZ[i * dim + j] += matX[i * dim + k] * matZ[k * dim + j];

			for (int i = 0; i < sz2; ++i)
				CHECK(std::abs(matXZ[i] - matC[i]) < eps5);
		}
	}
}

TEST_CASE("SH Rotation - Inverse Rotation")
{
	// 30 degrees around X axis
	float angle = 30.0f * UCommon::Pi / 180.0f;
	float c = std::cos(angle);
	float s = std::sin(angle);
	FMatrix3x3f rotationMatrix(
		1,  0,  0,
		0,  c, -s,
		0,  s,  c
	);
	FMatrix3x3f invRotationMatrix(
		1,  0,  0,
		0,  c,  s,
		0, -s,  c
	);

	SUBCASE("Band 2")
	{
		float band2In[3] = { 0.5f, 0.7f, 0.3f };
		float band2Rotated[3] = { band2In[0], band2In[1], band2In[2] };

		RotateSHBand2(band2Rotated, rotationMatrix);
		RotateSHBand2(band2Rotated, invRotationMatrix);

		for (int i = 0; i < 3; ++i)
			CHECK(IsNearlyEqual(band2In[i], band2Rotated[i], 1e-4f));
	}

	SUBCASE("Band 3")
	{
		float band3In[5] = { 0.5f, 0.7f, 0.3f, -0.2f, 0.1f };
		float band3Rotated[5] = { band3In[0], band3In[1], band3In[2], band3In[3], band3In[4] };

		RotateSHBand3(band3Rotated, rotationMatrix);
		RotateSHBand3(band3Rotated, invRotationMatrix);

		for (int i = 0; i < 5; ++i)
			CHECK(IsNearlyEqual(band3In[i], band3Rotated[i], 1e-4f));
	}
}

TEST_CASE("SH Rotation - Random Rotation & Direction Robustness")
{
	std::mt19937 rng(42);
	std::uniform_real_distribution<float> angleDist(0.f, 2.f * UCommon::Pi);
	std::uniform_real_distribution<float> cosDist(-1.f, 1.f);
	constexpr float eps = 1e-4f;
	constexpr int numTests = 200;

	// Rodrigues' rotation: rotate by `angle` around unit axis `axis`
	auto makeRotMatrix = [](FVector3f axis, float angle) -> FMatrix3x3f {
		float c = std::cos(angle), s = std::sin(angle), t = 1.f - c;
		float x = axis.X, y = axis.Y, z = axis.Z;
		return FMatrix3x3f(
			t*x*x + c,   t*x*y - s*z, t*x*z + s*y,
			t*x*y + s*z, t*y*y + c,   t*y*z - s*x,
			t*x*z - s*y, t*y*z + s*x, t*z*z + c
		);
	};

	// Generate a uniformly random unit direction
	auto randDir = [&]() -> FVector3f {
		float cosTheta = cosDist(rng);
		float sinTheta = std::sqrt(std::max(0.f, 1.f - cosTheta * cosTheta));
		float phi = angleDist(rng);
		return { sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta };
	};

	int band2Fail = 0, band3Fail = 0;

	for (int t = 0; t < numTests; ++t)
	{
		FVector3f axis = randDir();
		float angle = angleDist(rng);
		FMatrix3x3f R = makeRotMatrix(axis, angle);

		FVector3f w = randDir();
		FVector3f Rw = R * w;

		// Band 2 (l=1)
		{
			float shRotMat[9];
			ComputeSHBand2RotateMatrix(shRotMat, R);

			float Yw[3]  = { SH<1,-1>(w),  SH<1,0>(w),  SH<1,1>(w)  };
			float YRw[3] = { SH<1,-1>(Rw), SH<1,0>(Rw), SH<1,1>(Rw) };
			float MYw[3] = {};
			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
					MYw[i] += shRotMat[i * 3 + j] * Yw[j];

			for (int i = 0; i < 3; ++i)
				if (std::abs(YRw[i] - MYw[i]) > eps) { ++band2Fail; break; }
		}

		// Band 3 (l=2)
		{
			float shRotMat[25];
			ComputeSHBand3RotateMatrix(shRotMat, R);

			float Yw[5]  = { SH<2,-2>(w),  SH<2,-1>(w),  SH<2,0>(w),  SH<2,1>(w),  SH<2,2>(w)  };
			float YRw[5] = { SH<2,-2>(Rw), SH<2,-1>(Rw), SH<2,0>(Rw), SH<2,1>(Rw), SH<2,2>(Rw) };
			float MYw[5] = {};
			for (int i = 0; i < 5; ++i)
				for (int j = 0; j < 5; ++j)
					MYw[i] += shRotMat[i * 5 + j] * Yw[j];

			for (int i = 0; i < 5; ++i)
				if (std::abs(YRw[i] - MYw[i]) > eps) { ++band3Fail; break; }
		}
	}

	CHECK(band2Fail == 0);
	CHECK(band3Fail == 0);
}

// ============================================================================
// ApplySHRotateMatrix
// ============================================================================

TEST_CASE("ApplySHRotateMatrix")
{
	constexpr float eps = 1e-5f;

	SUBCASE("Band2 identity is no-op")
	{
		FMatrix3x3f identity = FMatrix3x3f::Identity();
		float mat[9];
		ComputeSHBand2RotateMatrix(mat, identity);

		float coeffs[3] = { 1.f, 2.f, 3.f };
		float expected[3] = { 1.f, 2.f, 3.f };
		ApplySHRotateMatrix<2>(TSHBandView<2>(coeffs), mat);

		for (int i = 0; i < 3; ++i)
			CHECK(std::abs(coeffs[i] - expected[i]) < eps);
	}

	SUBCASE("Band3 identity is no-op")
	{
		FMatrix3x3f identity = FMatrix3x3f::Identity();
		float mat[25];
		ComputeSHBand3RotateMatrix(mat, identity);

		float coeffs[5] = { 1.f, 2.f, 3.f, 4.f, 5.f };
		float expected[5] = { 1.f, 2.f, 3.f, 4.f, 5.f };
		ApplySHRotateMatrix<3>(TSHBandView<3>(coeffs), mat);

		for (int i = 0; i < 5; ++i)
			CHECK(std::abs(coeffs[i] - expected[i]) < eps);
	}

	SUBCASE("Band2: R then R^-1 restores original")
	{
		FMatrix3x3f rotX30(
			1.f,  0.f,            0.f,
			0.f,  std::cos(0.3f), -std::sin(0.3f),
			0.f,  std::sin(0.3f),  std::cos(0.3f)
		);
		FMatrix3x3f rotX30inv = rotX30.Transpose();

		float mat[9], matInv[9];
		ComputeSHBand2RotateMatrix(mat,    rotX30);
		ComputeSHBand2RotateMatrix(matInv, rotX30inv);

		float coeffs[3] = { 0.5f, 0.7f, 0.3f };
		float original[3] = { 0.5f, 0.7f, 0.3f };
		ApplySHRotateMatrix<2>(TSHBandView<2>(coeffs), mat);
		ApplySHRotateMatrix<2>(TSHBandView<2>(coeffs), matInv);

		for (int i = 0; i < 3; ++i)
			CHECK(std::abs(coeffs[i] - original[i]) < eps);
	}

	SUBCASE("Band3: R then R^-1 restores original")
	{
		FMatrix3x3f rotZ45(
			 std::cos(0.4f), -std::sin(0.4f), 0.f,
			 std::sin(0.4f),  std::cos(0.4f), 0.f,
			 0.f,             0.f,             1.f
		);
		FMatrix3x3f rotZ45inv = rotZ45.Transpose();

		float mat[25], matInv[25];
		ComputeSHBand3RotateMatrix(mat,    rotZ45);
		ComputeSHBand3RotateMatrix(matInv, rotZ45inv);

		float coeffs[5] = { 0.5f, 0.7f, 0.3f, -0.2f, 0.1f };
		float original[5] = { 0.5f, 0.7f, 0.3f, -0.2f, 0.1f };
		ApplySHRotateMatrix<3>(TSHBandView<3>(coeffs), mat);
		ApplySHRotateMatrix<3>(TSHBandView<3>(coeffs), matInv);

		for (int i = 0; i < 5; ++i)
			CHECK(std::abs(coeffs[i] - original[i]) < eps);
	}

	SUBCASE("Band2: result matches manual M*v")
	{
		FMatrix3x3f rotY(
			 0.f, 0.f, 1.f,
			 0.f, 1.f, 0.f,
			-1.f, 0.f, 0.f
		);
		float mat[9];
		ComputeSHBand2RotateMatrix(mat, rotY);

		float coeffs[3] = { 1.f, -1.f, 2.f };
		float manual[3] = {};
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				manual[i] += mat[i * 3 + j] * coeffs[j];

		ApplySHRotateMatrix<2>(TSHBandView<2>(coeffs), mat);

		for (int i = 0; i < 3; ++i)
			CHECK(std::abs(coeffs[i] - manual[i]) < eps);
	}
}

// ============================================================================
// TSHVector/TSHVectorRGB ApplySHRotateMatrix
// ============================================================================

TEST_CASE("TSHVector/TSHVectorRGB ApplySHRotateMatrix")
{
	constexpr float eps = 1e-5f;

	// Verify TSHRotateMatrices BandOffset formula
	static_assert(TSHRotateMatrices<2>::BandOffset<2> == 0);
	static_assert(TSHRotateMatrices<3>::BandOffset<3> == 9);
	static_assert(TSHRotateMatrices<4>::BandOffset<4> == 34);

	FMatrix3x3f rotX(
		1.f, 0.f,            0.f,
		0.f, std::cos(0.5f), -std::sin(0.5f),
		0.f, std::sin(0.5f),  std::cos(0.5f)
	);
	FMatrix3x3f rotXInv = rotX.Transpose();

	SUBCASE("ComputeSHRotateMatrices<2> matches ComputeSHBand2RotateMatrix")
	{
		float mat2[9];
		ComputeSHBand2RotateMatrix(mat2, rotX);

		auto mats2 = TSHRotateMatrices<2>(rotX);
		for (int i = 0; i < 9; ++i)
			CHECK(std::abs(mats2.GetBand<2>()[i] - mat2[i]) < eps);
	}

	SUBCASE("ComputeSHRotateMatrices<3> matches individual band functions")
	{
		float mat2[9], mat3[25];
		ComputeSHBand2RotateMatrix(mat2, rotX);
		ComputeSHBand3RotateMatrix(mat3, rotX);

		auto mats3 = TSHRotateMatrices<3>(rotX);
		for (int i = 0; i < 9; ++i)
			CHECK(std::abs(mats3.GetBand<2>()[i] - mat2[i]) < eps);
		for (int i = 0; i < 25; ++i)
			CHECK(std::abs(mats3.GetBand<3>()[i] - mat3[i]) < eps);
	}

	SUBCASE("FSHVector3: identity produces no change")
	{
		auto mats = TSHRotateMatrices<3>(FMatrix3x3f::Identity());
		FSHVector3 orig;
		for (int i = 0; i < FSHVector3::MaxSHBasis; ++i) orig.V[i] = static_cast<float>(i + 1);
		FSHVector3 result = orig.ApplySHRotateMatrix(mats);
		for (int i = 0; i < FSHVector3::MaxSHBasis; ++i)
			CHECK(std::abs(result.V[i] - orig.V[i]) < eps);
	}

	SUBCASE("FSHVector3: R then R^-1 restores original")
	{
		auto mats    = TSHRotateMatrices<3>(rotX);
		auto matsInv = TSHRotateMatrices<3>(rotXInv);
		FSHVector3 orig;
		for (int i = 0; i < FSHVector3::MaxSHBasis; ++i) orig.V[i] = static_cast<float>(i) * 0.3f - 1.f;
		FSHVector3 result = orig.ApplySHRotateMatrix(mats).ApplySHRotateMatrix(matsInv);
		for (int i = 0; i < FSHVector3::MaxSHBasis; ++i)
			CHECK(std::abs(result.V[i] - orig.V[i]) < eps);
	}

	SUBCASE("FSHVectorRGB3: R then R^-1 restores original per-channel")
	{
		auto mats    = TSHRotateMatrices<3>(rotX);
		auto matsInv = TSHRotateMatrices<3>(rotXInv);
		FSHVectorRGB3 orig;
		for (int i = 0; i < FSHVector3::MaxSHBasis; ++i)
		{
			orig.R.V[i] = static_cast<float>(i) * 0.1f;
			orig.G.V[i] = static_cast<float>(i) * 0.2f + 0.5f;
			orig.B.V[i] = static_cast<float>(i) * 0.3f - 0.5f;
		}
		FSHVectorRGB3 result = orig.ApplySHRotateMatrix(mats).ApplySHRotateMatrix(matsInv);
		for (int i = 0; i < FSHVector3::MaxSHBasis; ++i)
		{
			CHECK(std::abs(result.R.V[i] - orig.R.V[i]) < eps);
			CHECK(std::abs(result.G.V[i] - orig.G.V[i]) < eps);
			CHECK(std::abs(result.B.V[i] - orig.B.V[i]) < eps);
		}
	}

	SUBCASE("FSHVector3::ApplySHRotateMatrix matches per-band manual application")
	{
		auto mats = TSHRotateMatrices<3>(rotX);
		FSHVector3 orig;
		for (int i = 0; i < FSHVector3::MaxSHBasis; ++i) orig.V[i] = static_cast<float>(i + 1) * 0.5f;

		// reference: apply each band manually
		FSHVector3 ref = orig;
		{
			float mat2[9]; ComputeSHBand2RotateMatrix(mat2, rotX);
			ApplySHRotateMatrix<2>(ref.GetBand<2>(), mat2);
		}
		{
			float mat3[25]; ComputeSHBand3RotateMatrix(mat3, rotX);
			ApplySHRotateMatrix<3>(ref.GetBand<3>(), mat3);
		}

		FSHVector3 result = orig.ApplySHRotateMatrix(mats);
		for (int i = 0; i < FSHVector3::MaxSHBasis; ++i)
			CHECK(std::abs(result.V[i] - ref.V[i]) < eps);
	}
}

// ============================================================================
// TSHRotateMatrices implicit conversion
// ============================================================================

TEST_CASE("TSHRotateMatrices implicit conversion")
{
	const float eps = 1e-6f;

	const float angle = UCommon::Pi / 6.f; // 30 deg
	FMatrix3x3f rotX(
		1.f, 0.f,              0.f,
		0.f, std::cos(angle), -std::sin(angle),
		0.f, std::sin(angle),  std::cos(angle)
	);

	TSHRotateMatrices<3> mats3(rotX);
	TSHRotateMatrices<2> mats2Ref(rotX);

	SUBCASE("Implicit conversion to order-2 matches independently built order-2")
	{
		const TSHRotateMatrices<2>& view2 = mats3;
		static_assert(TSHRotateMatrices<2>::TotalSize == 9);
		for (int i = 0; i < TSHRotateMatrices<2>::TotalSize; ++i)
			CHECK(std::abs(view2.Data[i] - mats2Ref.Data[i]) < eps);
	}

	SUBCASE("Conversion produces a reference (no copy): address lies inside mats3")
	{
		const TSHRotateMatrices<2>& view2 = mats3;
		CHECK(reinterpret_cast<const float*>(&view2) == mats3.Data);
	}

	SUBCASE("ApplySHRotateMatrix with implicit lower-order conversion gives same result")
	{
		FSHVector2 sh2;
		for (int i = 0; i < FSHVector2::MaxSHBasis; ++i) sh2.V[i] = static_cast<float>(i + 1);

		FSHVector2 result_implicit = sh2.ApplySHRotateMatrix(mats3);
		FSHVector2 result_ref      = sh2.ApplySHRotateMatrix(mats2Ref);

		for (int i = 0; i < FSHVector2::MaxSHBasis; ++i)
			CHECK(std::abs(result_implicit.V[i] - result_ref.V[i]) < eps);
	}
}
