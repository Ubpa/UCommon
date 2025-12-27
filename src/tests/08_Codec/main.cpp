/*
MIT License

Copyright (c) 2024 Ubpa
*/

#include <UCommon/Codec.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <random>

using namespace UCommon;

//===========================================
// Visual Space Conversion
// v = sqrt(c / (1 + c))
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

//===========================================
// RGBM Test
//===========================================

void TestRGBM(FLinearColorRGB Color, float Multiplier, const char* Label)
{
    FLinearColor RGBM = EncodeRGBM(Color, Multiplier);
    FLinearColorRGB Decoded = DecodeRGBM(RGBM, Multiplier);

    float L = Color.MaxComponent();
    float DecodedL = Decoded.MaxComponent();
    float Error = (Decoded - Color).Abs().MaxComponent();
    float RelError = (L > 0.f) ? Error / L : 0.f;

    printf("[%s]\n", Label);
    printf("  Input:   RGB(%.4f, %.4f, %.4f) L=%.4f\n", Color.X, Color.Y, Color.Z, L);
    printf("  Encoded: RGBM(%.4f, %.4f, %.4f, %.4f)\n", RGBM.X, RGBM.Y, RGBM.Z, RGBM.W);
    printf("  Decoded: RGB(%.4f, %.4f, %.4f) L=%.4f\n", Decoded.X, Decoded.Y, Decoded.Z, DecodedL);
    printf("  Error:   %.6f (Rel: %.4f%%)\n\n", Error, RelError * 100.f);
}

//===========================================
// RGBD Test
//===========================================

void TestRGBD(FLinearColorRGB Color, float MaxValue, const char* Label)
{
    FLinearColor RGBD = EncodeRGBD(Color, MaxValue);
    FLinearColorRGB Decoded = DecodeRGBD(RGBD, MaxValue);

    float L = Color.MaxComponent();
    float DecodedL = Decoded.MaxComponent();
    float Error = (Decoded - Color).Abs().MaxComponent();
    float RelError = (L > 0.f) ? Error / L : 0.f;

    printf("[%s]\n", Label);
    printf("  Input:   RGB(%.4f, %.4f, %.4f) L=%.4f\n", Color.X, Color.Y, Color.Z, L);
    printf("  Encoded: RGBD(%.4f, %.4f, %.4f, %.4f)\n", RGBD.X, RGBD.Y, RGBD.Z, RGBD.W);
    printf("  Decoded: RGB(%.4f, %.4f, %.4f) L=%.4f\n", Decoded.X, Decoded.Y, Decoded.Z, DecodedL);
    printf("  Error:   %.6f (Rel: %.4f%%)\n\n", Error, RelError * 100.f);
}

//===========================================
// RGBV Test
//===========================================

void TestRGBV(FLinearColorRGB Color, float MaxValue, const char* Label, float S = 1.f)
{
    FLinearColor RGBV = EncodeRGBV(Color, MaxValue, S);
    FLinearColorRGB Decoded = DecodeRGBV(RGBV, MaxValue, S);

    float L = Color.MaxComponent();
    float DecodedL = Decoded.MaxComponent();
    float Error = (Decoded - Color).Abs().MaxComponent();
    float RelError = (L > 0.f) ? Error / L : 0.f;

    printf("[%s]\n", Label);
    printf("  Input:   RGB(%.4f, %.4f, %.4f) L=%.4f\n", Color.X, Color.Y, Color.Z, L);
    printf("  Encoded: RGBV(%.4f, %.4f, %.4f, %.4f)\n", RGBV.X, RGBV.Y, RGBV.Z, RGBV.W);
    printf("  Decoded: RGB(%.4f, %.4f, %.4f) L=%.4f\n", Decoded.X, Decoded.Y, Decoded.Z, DecodedL);
    printf("  Error:   %.6f (Rel: %.4f%%)\n\n", Error, RelError * 100.f);
}

//===========================================
// Quantize helper
//===========================================
inline float Quantize8(float v) noexcept
{
    return std::floor(v * 255.f + 0.5f) / 255.f;
}

//===========================================
// Comparison Test Function
//===========================================
void RunComparisonTest(float MaxValue)
{
    printf("================================================================================\n");
    printf("=== Comparison Test (MaxValue = %.0f) ===\n", MaxValue);
    printf("================================================================================\n\n");

    // Power of 2 ranges up to MaxValue
    std::vector<std::pair<float, float>> ranges;
    std::vector<std::string> rangeNames;

    float start = 0.f;
    for (float end = 0.25f; end <= MaxValue; end *= 2.f)
    {
        ranges.push_back({start, end});
        char buf[32];
        if (start < 1.f)
            snprintf(buf, sizeof(buf), "%.2f-%.2f", start, end);
        else
            snprintf(buf, sizeof(buf), "%.0f-%.0f", start, end);
        rangeNames.push_back(buf);
        start = end;
    }
    if (start < MaxValue)
    {
        ranges.push_back({start, MaxValue});
        char buf[32];
        snprintf(buf, sizeof(buf), "%.0f-%.0f", start, MaxValue);
        rangeNames.push_back(buf);
    }

    // Generate test data - 1024 samples per range, uniformly distributed within each range
    constexpr int SamplesPerRange = 1024;
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> ratioDist(0.f, 1.f);

    std::vector<FLinearColorRGB> testColors;
    std::vector<float> testLuminances;

    for (const auto& range : ranges)
    {
        std::uniform_real_distribution<float> lumDist(std::max(range.first, 0.001f), range.second);
        for (int i = 0; i < SamplesPerRange; i++)
        {
            float L = lumDist(rng);
            testColors.push_back(FLinearColorRGB(L * ratioDist(rng), L * ratioDist(rng), L * ratioDist(rng)));
            testLuminances.push_back(lumDist(rng));
        }
    }

    //===========================================
    // RGB Color Encoding Test
    //===========================================
    printf("=== RGB Color Encoding (Visual Space Error) ===\n\n");

    double rgbmSum = 0., rgbdSum = 0., rgbvSum = 0.;
    double rgbmMax = 0., rgbdMax = 0., rgbvMax = 0.;
    int count = 0;

    for (const auto& color : testColors)
    {
        if (color.MaxComponent() == 0.f) continue;

        float errM = VisualSpaceError(color, DecodeRGBM(EncodeRGBM(color, MaxValue), MaxValue));
        float errD = VisualSpaceError(color, DecodeRGBD(EncodeRGBD(color, MaxValue), MaxValue));
        float errV = VisualSpaceError(color, DecodeRGBV(EncodeRGBV(color, MaxValue, 1.f), MaxValue, 1.f));

        rgbmSum += errM; rgbdSum += errD; rgbvSum += errV;
        rgbmMax = std::max(rgbmMax, (double)errM);
        rgbdMax = std::max(rgbdMax, (double)errD);
        rgbvMax = std::max(rgbvMax, (double)errV);
        count++;
    }

    printf("Test Count: %d\n\n", count);
    printf("| Codec | Avg Error | Max Error |\n");
    printf("|-------|-----------|----------|\n");
    printf("| RGBM  | %.6f  | %.6f |\n", rgbmSum / count, rgbmMax);
    printf("| RGBD  | %.6f  | %.6f |\n", rgbdSum / count, rgbdMax);
    printf("| RGBV  | %.6f  | %.6f |\n", rgbvSum / count, rgbvMax);

    printf("\n| Range       | RGBM Avg | RGBD Avg | RGBV Avg | Best   |\n");
    printf("|-------------|----------|----------|----------|--------|\n");

    for (size_t r = 0; r < ranges.size(); r++)
    {
        double rM = 0., rD = 0., rV = 0.;
        int rCount = 0;
        for (const auto& color : testColors)
        {
            float L = color.MaxComponent();
            if (L < ranges[r].first || L >= ranges[r].second) continue;
            rM += VisualSpaceError(color, DecodeRGBM(EncodeRGBM(color, MaxValue), MaxValue));
            rD += VisualSpaceError(color, DecodeRGBD(EncodeRGBD(color, MaxValue), MaxValue));
            rV += VisualSpaceError(color, DecodeRGBV(EncodeRGBV(color, MaxValue, 1.f), MaxValue, 1.f));
            rCount++;
        }
        if (rCount > 0)
        {
            double avgM = rM / rCount, avgD = rD / rCount, avgV = rV / rCount;
            const char* best = (avgM <= avgD && avgM <= avgV) ? "RGBM" : (avgD <= avgV) ? "RGBD" : "RGBV";
            printf("| %-11s | %.6f | %.6f | %.6f | %-6s |\n", rangeNames[r].c_str(), avgM, avgD, avgV, best);
        }
    }

    //===========================================
    // Float-only Encoding Test
    //===========================================
    printf("\n=== Float-only Encoding (8-bit quantized, Visual Space Error) ===\n\n");

    double fM = 0., fD = 0., fV = 0.;
    double fMMax = 0., fDMax = 0., fVMax = 0.;
    int fCount = 0;

    for (float L : testLuminances)
    {
        if (L <= 0.f) continue;

        float mEnc = Quantize8(EncodeRGBM(L, MaxValue));
        float mDec = Pow2(mEnc) * MaxValue;
        float dEnc = Quantize8(EncodeRGBD(L, MaxValue));
        float dDec = DecodeRGBD(dEnc, MaxValue);
        float vEnc = Quantize8(EncodeRGBV(L, MaxValue, 1.f));
        float vDec = DecodeRGBV(vEnc, MaxValue, 1.f);

        float errM = std::abs(ToVisualSpace(L) - ToVisualSpace(mDec));
        float errD = std::abs(ToVisualSpace(L) - ToVisualSpace(dDec));
        float errV = std::abs(ToVisualSpace(L) - ToVisualSpace(vDec));

        fM += errM; fD += errD; fV += errV;
        fMMax = std::max(fMMax, (double)errM);
        fDMax = std::max(fDMax, (double)errD);
        fVMax = std::max(fVMax, (double)errV);
        fCount++;
    }

    printf("Test Count: %d\n\n", fCount);
    printf("| Codec | Avg Error | Max Error |\n");
    printf("|-------|-----------|----------|\n");
    printf("| RGBM  | %.6f  | %.6f |\n", fM / fCount, fMMax);
    printf("| RGBD  | %.6f  | %.6f |\n", fD / fCount, fDMax);
    printf("| RGBV  | %.6f  | %.6f |\n", fV / fCount, fVMax);

    printf("\n| Range       | RGBM Avg | RGBD Avg | RGBV Avg | Best   |\n");
    printf("|-------------|----------|----------|----------|--------|\n");

    for (size_t r = 0; r < ranges.size(); r++)
    {
        double rM = 0., rD = 0., rV = 0.;
        int rCount = 0;
        for (float L : testLuminances)
        {
            if (L < ranges[r].first || L >= ranges[r].second) continue;

            float mEnc = Quantize8(EncodeRGBM(L, MaxValue));
            float mDec = Pow2(mEnc) * MaxValue;
            float dEnc = Quantize8(EncodeRGBD(L, MaxValue));
            float dDec = DecodeRGBD(dEnc, MaxValue);
            float vEnc = Quantize8(EncodeRGBV(L, MaxValue, 1.f));
            float vDec = DecodeRGBV(vEnc, MaxValue, 1.f);

            rM += std::abs(ToVisualSpace(L) - ToVisualSpace(mDec));
            rD += std::abs(ToVisualSpace(L) - ToVisualSpace(dDec));
            rV += std::abs(ToVisualSpace(L) - ToVisualSpace(vDec));
            rCount++;
        }
        if (rCount > 0)
        {
            double avgM = rM / rCount, avgD = rD / rCount, avgV = rV / rCount;
            const char* best = (avgM <= avgD && avgM <= avgV) ? "RGBM" : (avgD <= avgV) ? "RGBD" : "RGBV";
            printf("| %-11s | %.6f | %.6f | %.6f | %-6s |\n", rangeNames[r].c_str(), avgM, avgD, avgV, best);
        }
    }

    printf("\n");
}

int main()
{
    //===========================================
    // RGBM Tests
    //===========================================
    constexpr float RGBMMultiplier = RGBM_DefaultMaxMultiplier;
    printf("=== RGBM Encoding Test (Multiplier=%.1f) ===\n\n", RGBMMultiplier);

    TestRGBM(FLinearColorRGB(0.1f, 0.05f, 0.02f), RGBMMultiplier, "Low luminance");
    TestRGBM(FLinearColorRGB(0.5f, 0.3f, 0.2f), RGBMMultiplier, "Mid luminance");
    TestRGBM(FLinearColorRGB(1.0f, 0.5f, 0.25f), RGBMMultiplier, "Unit luminance");
    TestRGBM(FLinearColorRGB(5.0f, 3.0f, 1.0f), RGBMMultiplier, "HDR luminance");
    TestRGBM(FLinearColorRGB(50.0f, 30.0f, 10.0f), RGBMMultiplier, "High HDR luminance");
    TestRGBM(FLinearColorRGB(0.0f, 0.0f, 0.0f), RGBMMultiplier, "Zero");

    //===========================================
    // RGBD Tests
    //===========================================
    constexpr float RGBDMaxValue = RGBD_DefaultMaxValue;
    printf("\n=== RGBD Encoding Test (MaxValue=%.1f) ===\n\n", RGBDMaxValue);

    TestRGBD(FLinearColorRGB(0.1f, 0.05f, 0.02f), RGBDMaxValue, "Low luminance");
    TestRGBD(FLinearColorRGB(0.5f, 0.3f, 0.2f), RGBDMaxValue, "Mid luminance");
    TestRGBD(FLinearColorRGB(1.0f, 0.5f, 0.25f), RGBDMaxValue, "Unit luminance");
    TestRGBD(FLinearColorRGB(5.0f, 3.0f, 1.0f), RGBDMaxValue, "HDR luminance");
    TestRGBD(FLinearColorRGB(50.0f, 30.0f, 10.0f), RGBDMaxValue, "High HDR luminance");
    TestRGBD(FLinearColorRGB(500.0f, 300.0f, 100.0f), RGBDMaxValue, "Very high HDR");
    TestRGBD(FLinearColorRGB(0.0f, 0.0f, 0.0f), RGBDMaxValue, "Zero");

    //===========================================
    // RGBV Tests
    //===========================================
    constexpr float RGBVMaxValue = RGBV_DefaultMaxValue;
    printf("\n=== RGBV Encoding Test (MaxValue=%.1f) ===\n\n", RGBVMaxValue);

    TestRGBV(FLinearColorRGB(0.1f, 0.05f, 0.02f), RGBVMaxValue, "Low luminance");
    TestRGBV(FLinearColorRGB(0.5f, 0.3f, 0.2f), RGBVMaxValue, "Mid luminance");
    TestRGBV(FLinearColorRGB(1.0f, 0.5f, 0.25f), RGBVMaxValue, "Unit luminance");
    TestRGBV(FLinearColorRGB(5.0f, 3.0f, 1.0f), RGBVMaxValue, "HDR luminance");
    TestRGBV(FLinearColorRGB(50.0f, 30.0f, 10.0f), RGBVMaxValue, "High HDR luminance");
    TestRGBV(FLinearColorRGB(0.0f, 0.0f, 0.0f), RGBVMaxValue, "Zero");

    printf("=== All Codec Tests Complete ===\n\n");

    // Run comparison tests for MaxValue = 128 and 2048
    RunComparisonTest(128.f);
    RunComparisonTest(2048.f);

    return 0;
}
