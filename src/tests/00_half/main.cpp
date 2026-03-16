#include <UCommon/Half.h>
#include <cmath>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <UCommon_ext/doctest/doctest.h>

using namespace UCommon;
using namespace UCommon::literal;

TEST_CASE("Half - Basic type properties") {
    static_assert(sizeof(FHalf) == 2, "sizeof(FHalf) == 2");
}

TEST_CASE("Half - Integer conversion") {
    // 1 bit sign, 5 bit exp, 10 bit significand
    for (int i = -(0b1 << 11); i < (0b1 << 11); i++)
    {
        float a = (float)i;
        FHalf b = static_cast<FHalf>(a);
        float c = static_cast<float>(b);
        CHECK(c == a);
    }
}

TEST_CASE("Half - Fractional conversion") {
    // 1 bit sign, 5 bit exp, 10 bit significand
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            float a = (i - 32) + j / 32.f;
            FHalf b = static_cast<FHalf>(a);
            float c = b;
            CHECK(c == a);
        }
    }
}
