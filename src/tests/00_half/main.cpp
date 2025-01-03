#include <UCommon/Half.h>

#include <cmath>
#include <cassert>
#include <iostream>

using namespace UCommon;
using namespace UCommon::literal;

int main(int argc, char** argv) {
    static_assert(sizeof(FHalf) == 2, "sizeof(FHalf) == 2");

    { // 1 bit sign, 5 bit exp, 10 bit significand
        for (int i = -(0b1 << 11); i < (0b1 << 11); i++)
        {
            float a = (float)i;
            FHalf b = static_cast<FHalf>(a);
            float c = static_cast<float>(b);
            assert(c == a);
        }
    }

    { // 1 bit sign, 5 bit exp, 10 bit significand
        for (int i = 0; i < 64; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                float a = (i - 32) + j / 32.f;
                FHalf b = static_cast<FHalf>(a);
                float c = b;
                assert(c == a);
            }
        }
    }

    return 0;
}
