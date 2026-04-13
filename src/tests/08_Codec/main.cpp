/*
MIT License

Copyright (c) 2024 Ubpa
*/

#include <UCommon/Codec.h>
#include <cmath>
#include <vector>
#include <random>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <UCommon_ext/doctest/doctest.h>

using namespace UCommon;

//===========================================
// Visual Space Conversion
//===========================================

inline float ToVisualSpace(float c) noexcept
{
    return std::sqrt(c / (1.f + c));
}

inline FLinearColorRGB ToVisualSpace(const FLinearColorRGB& color) noexcept
{
    return FLinearColorRGB(
        ToVisualSpace(color.X),
        ToVisualSpace(color.Y),
        ToVisualSpace(color.Z)
    );
}

inline float VisualSpaceError(const FLinearColorRGB& original, const FLinearColorRGB& decoded) noexcept
{
    FLinearColorRGB vOriginal = ToVisualSpace(original);
    FLinearColorRGB vDecoded = ToVisualSpace(decoded);
    return (vDecoded - vOriginal).Abs().MaxComponent();
}

inline float Quantize8(float v) noexcept
{
    return std::floor(v * 255.f + 0.5f) / 255.f;
}

//===========================================
// RGBM Tests
//===========================================

TEST_CASE("RGBM - Encode/Decode round-trip")
{
    constexpr float Multiplier = RGBM_DefaultMaxMultiplier;

    struct TestCase { FLinearColorRGB color; float maxRelError; };
    TestCase cases[] = {
        { FLinearColorRGB(0.1f, 0.05f, 0.02f),  0.02f },
        { FLinearColorRGB(0.5f, 0.3f, 0.2f),    0.02f },
        { FLinearColorRGB(1.0f, 0.5f, 0.25f),   0.02f },
        { FLinearColorRGB(5.0f, 3.0f, 1.0f),    0.02f },
        { FLinearColorRGB(50.0f, 30.0f, 10.0f), 0.02f },
    };

    for (const auto& tc : cases)
    {
        FLinearColor encoded = EncodeRGBM(tc.color, Multiplier);
        FLinearColorRGB decoded = DecodeRGBM(encoded, Multiplier);

        float L = tc.color.MaxComponent();
        float error = (decoded - tc.color).Abs().MaxComponent();
        float relError = error / L;

        CHECK(relError < tc.maxRelError);
    }
}

TEST_CASE("RGBM - Zero input")
{
    FLinearColor encoded = EncodeRGBM(FLinearColorRGB(0.f, 0.f, 0.f), RGBM_DefaultMaxMultiplier);
    FLinearColorRGB decoded = DecodeRGBM(encoded, RGBM_DefaultMaxMultiplier);
    CHECK(decoded.X == doctest::Approx(0.f).epsilon(1e-6f));
    CHECK(decoded.Y == doctest::Approx(0.f).epsilon(1e-6f));
    CHECK(decoded.Z == doctest::Approx(0.f).epsilon(1e-6f));
}

//===========================================
// RGBD Tests
//===========================================

TEST_CASE("RGBD - Encode/Decode round-trip")
{
    constexpr float MaxValue = RGBD_DefaultMaxValue;

    struct TestCase { FLinearColorRGB color; float maxRelError; };
    TestCase cases[] = {
        { FLinearColorRGB(0.1f, 0.05f, 0.02f),      0.02f },
        { FLinearColorRGB(0.5f, 0.3f, 0.2f),        0.02f },
        { FLinearColorRGB(1.0f, 0.5f, 0.25f),       0.02f },
        { FLinearColorRGB(5.0f, 3.0f, 1.0f),        0.02f },
        { FLinearColorRGB(50.0f, 30.0f, 10.0f),     0.02f },
        { FLinearColorRGB(500.0f, 300.0f, 100.0f),  0.02f },
    };

    for (const auto& tc : cases)
    {
        FLinearColor encoded = EncodeRGBD(tc.color, MaxValue);
        FLinearColorRGB decoded = DecodeRGBD(encoded, MaxValue);

        float L = tc.color.MaxComponent();
        float error = (decoded - tc.color).Abs().MaxComponent();
        float relError = error / L;

        CHECK(relError < tc.maxRelError);
    }
}

TEST_CASE("RGBD - Zero input")
{
    FLinearColor encoded = EncodeRGBD(FLinearColorRGB(0.f, 0.f, 0.f), RGBD_DefaultMaxValue);
    FLinearColorRGB decoded = DecodeRGBD(encoded, RGBD_DefaultMaxValue);
    CHECK(decoded.X == doctest::Approx(0.f).epsilon(1e-6f));
    CHECK(decoded.Y == doctest::Approx(0.f).epsilon(1e-6f));
    CHECK(decoded.Z == doctest::Approx(0.f).epsilon(1e-6f));
}

TEST_CASE("RGBD - Float encode/decode round-trip")
{
    constexpr float MaxValue = RGBD_DefaultMaxValue;
    float K = RGBD_GetK(MaxValue);

    // Verify K and MaxValue are consistent
    CHECK(RGBD_GetMaxValue(K) == doctest::Approx(MaxValue).epsilon(1e-4f));

    // Encode then decode single float values
    float testValues[] = { 0.1f, 1.f, 10.f, 100.f, 1000.f };
    for (float L : testValues)
    {
        float D = EncodeRGBD(L, MaxValue);
        float decoded = DecodeRGBD(D, MaxValue);
        CHECK(decoded == doctest::Approx(L).epsilon(L * 0.01f));
    }
}

//===========================================
// RGBV Tests
//===========================================

TEST_CASE("RGBV - Encode/Decode round-trip")
{
    constexpr float MaxValue = RGBV_DefaultMaxValue;
    constexpr float S = 1.f;

    struct TestCase { FLinearColorRGB color; float maxRelError; };
    TestCase cases[] = {
        { FLinearColorRGB(0.1f, 0.05f, 0.02f),  0.02f },
        { FLinearColorRGB(0.5f, 0.3f, 0.2f),    0.02f },
        { FLinearColorRGB(1.0f, 0.5f, 0.25f),   0.02f },
        { FLinearColorRGB(5.0f, 3.0f, 1.0f),    0.02f },
        { FLinearColorRGB(50.0f, 30.0f, 10.0f), 0.02f },
    };

    for (const auto& tc : cases)
    {
        FLinearColor encoded = EncodeRGBV(tc.color, MaxValue, S);
        FLinearColorRGB decoded = DecodeRGBV(encoded, MaxValue, S);

        float L = tc.color.MaxComponent();
        float error = (decoded - tc.color).Abs().MaxComponent();
        float relError = error / L;

        CHECK(relError < tc.maxRelError);
    }
}

TEST_CASE("RGBV - Zero input")
{
    FLinearColor encoded = EncodeRGBV(FLinearColorRGB(0.f, 0.f, 0.f), RGBV_DefaultMaxValue, 1.f);
    FLinearColorRGB decoded = DecodeRGBV(encoded, RGBV_DefaultMaxValue, 1.f);
    CHECK(decoded.X == doctest::Approx(0.f).epsilon(1e-6f));
    CHECK(decoded.Y == doctest::Approx(0.f).epsilon(1e-6f));
    CHECK(decoded.Z == doctest::Approx(0.f).epsilon(1e-6f));
}

//===========================================
// Codec Comparison Tests
//===========================================

TEST_CASE("Codec comparison - Visual space error within bounds")
{
    float MaxValues[] = { 128.f, 2048.f };

    for (float MaxValue : MaxValues)
    {
        CAPTURE(MaxValue);

        // Generate test data
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> ratioDist(0.f, 1.f);
        std::uniform_real_distribution<float> lumDist(0.001f, MaxValue);

        constexpr int Samples = 1024;
        double rgbmSum = 0., rgbdSum = 0., rgbvSum = 0.;
        int count = 0;

        for (int i = 0; i < Samples; i++)
        {
            float L = lumDist(rng);
            FLinearColorRGB color(L * ratioDist(rng), L * ratioDist(rng), L * ratioDist(rng));
            if (color.MaxComponent() == 0.f) continue;

            float errM = VisualSpaceError(color, DecodeRGBM(EncodeRGBM(color, MaxValue), MaxValue));
            float errD = VisualSpaceError(color, DecodeRGBD(EncodeRGBD(color, MaxValue), MaxValue));
            float errV = VisualSpaceError(color, DecodeRGBV(EncodeRGBV(color, MaxValue, 1.f), MaxValue, 1.f));

            rgbmSum += errM; rgbdSum += errD; rgbvSum += errV;
            count++;
        }

        // All codecs should have reasonable average error
        double avgM = rgbmSum / count;
        double avgD = rgbdSum / count;
        double avgV = rgbvSum / count;

        CHECK(avgM < 0.05);
        CHECK(avgD < 0.05);
        CHECK(avgV < 0.05);
    }
}

//===========================================
// RGBV_ComputeIntegral and RGBV_SolveS Tests
//===========================================

TEST_CASE("RGBV_ComputeIntegral - s=0 equals M/3")
{
    float MValues[] = { 1.f, 10.f, 128.f, 1000.f };
    for (float M : MValues)
    {
        CAPTURE(M);
        float I = RGBV_ComputeIntegral(M, 0.f);
        CHECK(I == doctest::Approx(M / 3.f).epsilon(1e-5f));
    }
}

TEST_CASE("RGBV_ComputeIntegral - monotonically decreasing")
{
    float M = 128.f;
    float sValues[] = { -1.f / M + 0.001f, -0.5f / M, 0.f, 0.5f, 1.f, 2.f, 10.f };
    float prevI = std::numeric_limits<float>::infinity();
    for (float s : sValues)
    {
        float I = RGBV_ComputeIntegral(M, s);
        CHECK(I < prevI);
        prevI = I;
    }
}

TEST_CASE("RGBV_SolveS - round-trip")
{
    float M = 128.f;
    float sTestValues[] = { -1.f / M + 0.01f, -0.5f / M, 0.f, 0.5f, 1.f, 5.f, 10.f };
    for (float sOriginal : sTestValues)
    {
        CAPTURE(sOriginal);
        float I = RGBV_ComputeIntegral(M, sOriginal);
        float sSolved = RGBV_SolveS(M, I);
        float error = std::abs(sSolved - sOriginal);
        float relError = std::abs(sOriginal) > 1e-6f ? error / std::abs(sOriginal) : error;
        CHECK((relError < 1e-3f || error < 1e-6f));
    }
}

TEST_CASE("RGBV_SolveS - different M values")
{
    float MValues[] = { 1.f, 10.f, 128.f, 1000.f };
    float sValues[] = { 0.f, 1.f, 5.f };
    for (float M : MValues)
    {
        for (float s : sValues)
        {
            CAPTURE(M); CAPTURE(s);
            float I = RGBV_ComputeIntegral(M, s);
            float sSolved = RGBV_SolveS(M, I);
            float error = std::abs(sSolved - s);
            float relError = (std::abs(s) > 1e-6f) ? error / std::abs(s) : error;
            CHECK((error < 1e-3f || relError < 1e-3f));
        }
    }
}

TEST_CASE("RGBV_SolveS - edge cases")
{
    float M = 128.f;

    SUBCASE("Large I (s close to -1/M)")
    {
        float I_large = 100.f;
        float s = RGBV_SolveS(M, I_large);
        float I_verify = RGBV_ComputeIntegral(M, s);
        CHECK(I_verify == doctest::Approx(I_large).epsilon(1e-3f));
    }

    SUBCASE("Small I (large positive s)")
    {
        float I_small = 0.1f;
        float s = RGBV_SolveS(M, I_small);
        float I_verify = RGBV_ComputeIntegral(M, s);
        CHECK(I_verify == doctest::Approx(I_small).epsilon(1e-3f));
    }

    SUBCASE("I = M/3 (s = 0)")
    {
        float I_mid = M / 3.f;
        float s = RGBV_SolveS(M, I_mid);
        CHECK(std::abs(s) < 1e-3f);
    }
}

//===========================================
// RGBV_ComputeIntegral2 and RGBV_SolveS2 Tests
//===========================================

// Independent numerical integration using Simpson's rule
static double NumericalIntegral2(float M, float s, int N = 100000)
{
    if (N % 2 != 0) N++;
    double h = 1.0 / N;

    auto L = [M, s](double v) -> double {
        double v2 = v * v;
        double denom = s * (1.0 - v2) + 1.0 / M;
        return v2 / denom;
    };

    auto f = [&L](double v) -> double {
        double Lv = L(v);
        return Lv * Lv;
    };

    double sum = f(0.0) + f(1.0);
    for (int i = 1; i < N; i++)
    {
        double v = i * h;
        sum += (i % 2 == 0 ? 2.0 : 4.0) * f(v);
    }
    sum *= h / 3.0;
    return sum;
}

TEST_CASE("RGBV_ComputeIntegral2 - s=0 equals M^2/5")
{
    float MValues[] = { 1.f, 10.f, 128.f, 2048.f };
    for (float M : MValues)
    {
        CAPTURE(M);
        float I2 = RGBV_ComputeIntegral2(M, 0.f);
        float expected = M * M / 5.f;
        CHECK(I2 == doctest::Approx(expected).epsilon(expected * 1e-5f));
    }
}

TEST_CASE("RGBV_ComputeIntegral2 - s>0 vs numerical integration")
{
    float MValues[] = { 1.f, 10.f, 128.f, 2048.f };
    float sValues[] = { 0.1f, 0.5f, 1.f, 5.f, 10.f };
    for (float M : MValues)
    {
        for (float s : sValues)
        {
            CAPTURE(M); CAPTURE(s);
            float I2_func = RGBV_ComputeIntegral2(M, s);
            double I2_num = NumericalIntegral2(M, s);
            double relError = std::abs((double)I2_func - I2_num) / I2_num;
            // M=2048, s=10 has ~0.25% error due to float precision in atanh
            CHECK(relError < 3e-3);
        }
    }
}

TEST_CASE("RGBV_ComputeIntegral2 - s<0 vs numerical integration")
{
    float MValues[] = { 1.f, 10.f, 128.f, 2048.f };
    for (float M : MValues)
    {
        float sMin = -1.f / M;
        float fractions[] = { 0.1f, 0.3f, 0.5f, 0.8f, 0.95f };
        for (float frac : fractions)
        {
            float s = sMin * frac;
            CAPTURE(M); CAPTURE(s);
            float I2_func = RGBV_ComputeIntegral2(M, s);
            double I2_num = NumericalIntegral2(M, s);
            double relError = std::abs((double)I2_func - I2_num) / I2_num;
            CHECK(relError < 1e-3);
        }
    }
}

TEST_CASE("RGBV_ComputeIntegral2 - boundary s close to -1/M")
{
    float MValues[] = { 1.f, 128.f, 2048.f };
    for (float M : MValues)
    {
        CAPTURE(M);
        float s = -1.f / M * 0.99f;
        float I2 = RGBV_ComputeIntegral2(M, s);
        float I2_at_zero = M * M / 5.f;
        CHECK(I2 > I2_at_zero); // Should be larger than s=0 case
    }
}

TEST_CASE("RGBV_ComputeIntegral2 - monotonically decreasing")
{
    float M = 128.f;
    float sMin = -1.f / M;
    float sValues[] = { sMin * 0.95f, sMin * 0.5f, sMin * 0.1f, 0.f, 0.5f, 1.f, 5.f, 10.f };
    float prevI2 = std::numeric_limits<float>::infinity();
    for (float s : sValues)
    {
        float I2 = RGBV_ComputeIntegral2(M, s);
        CHECK(I2 < prevI2);
        prevI2 = I2;
    }
}

TEST_CASE("RGBV_SolveS2 - round-trip")
{
    float MValues[] = { 1.f, 10.f, 128.f, 2048.f };
    for (float M : MValues)
    {
        float sMin = -1.f / M;
        float sValues[] = { sMin * 0.9f, sMin * 0.5f, sMin * 0.1f, 0.f, 0.5f, 1.f, 5.f, 10.f };
        for (float sOriginal : sValues)
        {
            CAPTURE(M); CAPTURE(sOriginal);
            float I2 = RGBV_ComputeIntegral2(M, sOriginal);
            float sSolved = RGBV_SolveS2(M, I2);
            float error = std::abs(sSolved - sOriginal);
            float relError = std::abs(sOriginal) > 1e-6f ? error / std::abs(sOriginal) : error;
            CHECK((relError < 1e-2f || error < 1e-3f));
        }
    }
}

TEST_CASE("RGBV_SolveS2 - verify target I2")
{
    float M = 128.f;
    float I2_at_zero = M * M / 5.f;
    float targets[] = {
        I2_at_zero * 2.f,
        I2_at_zero * 1.5f,
        I2_at_zero,
        I2_at_zero * 0.5f,
        I2_at_zero * 0.1f,
    };
    for (float targetI2 : targets)
    {
        CAPTURE(targetI2);
        float sSolved = RGBV_SolveS2(M, targetI2);
        float I2_verify = RGBV_ComputeIntegral2(M, sSolved);
        float relError = std::abs(I2_verify - targetI2) / targetI2;
        CHECK(relError < 1e-3f);
    }
}
