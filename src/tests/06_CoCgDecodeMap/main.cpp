#include <UCommon/UCommon.h>
#include <UCommon_ext/Tex2DIO.h>

#include <string>
#include <algorithm>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <UCommon_ext/doctest/doctest.h>

using namespace UCommon;

// Standard YCoCg to RGB conversion (non-normalized)
static inline void StandardYCoCgToRGB(float Y, float Co, float Cg, float& R, float& G, float& B)
{
    R = Y + Co - Cg;
    G = Y + Cg;
    B = Y - Co - Cg;
}

// Triangle mapping: UV [0,1]x[0,1] -> valid (Co, Cg) triangle
static inline void UVToNormalizedCoCg(float U, float V, float& Co, float& Cg)
{
    Cg = V * 2.0f - 1.0f;
    const float CoMin = (Cg - 1.0f) * 0.5f;
    const float CoMax = (1.0f - Cg) * 0.5f;
    Co = CoMin + U * (CoMax - CoMin);
}

// Inverse: normalized CoCg to UV
static inline bool NormalizedCoCgToUV(float Co, float Cg, float& U, float& V)
{
    V = (Cg + 1.0f) * 0.5f;
    const float CoMin = (Cg - 1.0f) * 0.5f;
    const float CoMax = (1.0f - Cg) * 0.5f;
    if (Co < CoMin || Co > CoMax) return false;
    const float CoRange = CoMax - CoMin;
    U = (CoRange < 1e-6f) ? 0.5f : (Co - CoMin) / CoRange;
    return true;
}

TEST_CASE("CoCgDecodeMap - Triangle mapping round-trip")
{
    // Verify UV -> CoCg -> UV round-trip
    constexpr int Steps = 32;
    for (int vi = 0; vi <= Steps; ++vi)
    {
        for (int ui = 0; ui <= Steps; ++ui)
        {
            float U = (float)ui / Steps;
            float V = (float)vi / Steps;

            float Co, Cg;
            UVToNormalizedCoCg(U, V, Co, Cg);

            float U2, V2;
            bool valid = NormalizedCoCgToUV(Co, Cg, U2, V2);
            REQUIRE(valid);

            // At V=1 (top vertex), U is degenerate
            if (vi < Steps)
            {
                CHECK(U2 == doctest::Approx(U).epsilon(1e-5f));
            }
            CHECK(V2 == doctest::Approx(V).epsilon(1e-5f));
        }
    }
}

TEST_CASE("CoCgDecodeMap - Valid RGB from triangle mapping")
{
    // All pixels from triangle mapping should produce non-negative RGB
    constexpr float YValue = 1.0f;
    constexpr int Steps = 64;
    for (int vi = 0; vi <= Steps; ++vi)
    {
        for (int ui = 0; ui <= Steps; ++ui)
        {
            float U = (float)ui / Steps;
            float V = (float)vi / Steps;

            float Co, Cg;
            UVToNormalizedCoCg(U, V, Co, Cg);

            float R, G, B;
            YCoCgToRGB(YValue, Co, Cg, R, G, B);

            CHECK(R >= -1e-6f);
            CHECK(G >= -1e-6f);
            CHECK(B >= -1e-6f);
        }
    }
}

TEST_CASE("CoCgDecodeMap - Generate decode maps")
{
    constexpr uint64_t Resolution = 64; // small for test speed
    constexpr float YValue = 1.0f;

    SUBCASE("Standard YCoCg square")
    {
        FTex2D Tex2D(FGrid2D(Resolution, Resolution), 3, EElementType::Float);
        for (uint64_t Y = 0; Y < Resolution; Y++)
        {
            for (uint64_t X = 0; X < Resolution; X++)
            {
                const float Co = (float)X / (float)(Resolution - 1) * 4.0f * YValue - 2.0f * YValue;
                const float Cg = (float)Y / (float)(Resolution - 1) * 2.0f * YValue - YValue;
                float R, G, B;
                StandardYCoCgToRGB(YValue, Co, Cg, R, G, B);
                const FUint64Vector2 Point(X, Y);
                Tex2D.At<float>(Point, 0) = R;
                Tex2D.At<float>(Point, 1) = G;
                Tex2D.At<float>(Point, 2) = B;
            }
        }
        CHECK(SaveImage("CoCg_Standard_Square.hdr", Tex2D));
    }

    SUBCASE("Normalized CoCg square")
    {
        FTex2D Tex2D(FGrid2D(Resolution, Resolution), 3, EElementType::Float);
        for (uint64_t Y = 0; Y < Resolution; Y++)
        {
            for (uint64_t X = 0; X < Resolution; X++)
            {
                const float Co = (float)X / (float)(Resolution - 1) * 2.0f - 1.0f;
                const float Cg = (float)Y / (float)(Resolution - 1) * 2.0f - 1.0f;
                float R, G, B;
                YCoCgToRGB(YValue, Co, Cg, R, G, B);
                const FUint64Vector2 Point(X, Y);
                Tex2D.At<float>(Point, 0) = R;
                Tex2D.At<float>(Point, 1) = G;
                Tex2D.At<float>(Point, 2) = B;
            }
        }
        CHECK(SaveImage("CoCg_Normalized_Square.hdr", Tex2D));
    }

    SUBCASE("Normalized CoCg triangle mapping")
    {
        FTex2D Tex2D(FGrid2D(Resolution, Resolution), 3, EElementType::Float);
        for (uint64_t Y = 0; Y < Resolution; Y++)
        {
            for (uint64_t X = 0; X < Resolution; X++)
            {
                const float U = (float)X / (float)(Resolution - 1);
                const float V = (float)Y / (float)(Resolution - 1);
                float Co, Cg;
                UVToNormalizedCoCg(U, V, Co, Cg);
                float R, G, B;
                YCoCgToRGB(YValue, Co, Cg, R, G, B);
                const FUint64Vector2 Point(X, Y);
                Tex2D.At<float>(Point, 0) = R;
                Tex2D.At<float>(Point, 1) = G;
                Tex2D.At<float>(Point, 2) = B;
            }
        }
        CHECK(SaveImage("CoCg_Normalized_Triangle.hdr", Tex2D));
    }
}
