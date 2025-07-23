#include <UCommon/FP8.h>

#include <cstdint>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace UCommon;

template<typename FP8>
void Test(ERound Round)
{
    const float test_values[] = {
        1.0f, -1.0f, 0.5f, -0.5f,
        0.125f, -0.125f,
        3.14f, -3.14f,
        240.0f, -240.0f,
        256.0f, -256.0f,
        0.015625f, -0.015625f,
        0.001953125f, -0.001953125f,
    };

    for (float f : test_values) {
        FP8 f8(f, Round);
        float f_back = static_cast<float>(f8);

        std::cout << f << "," << f_back << std::endl;

        if (std::isnan(f)) continue;

        FP8 f8_again(f_back, Round);
        if (f8 != f8_again) {
            return;
        }
    }

    for (uint16_t value = 0; value < 256; value++)
    {
        uint8_t v8 = (uint8_t)value;
        float f = static_cast<float>(*(FP8*)&v8);
        std::cout << f << std::endl;
    }
}

int main()
{
    Test<FFP8_E5M2>(ERound::Nearest);
    Test<FUFP8_E3M5>(ERound::Nearest);
    Test<FUFP8_E4M4>(ERound::Nearest);
    Test<FUFP8_E5M3>(ERound::Nearest);
    Test<FFP8_E4M3>(ERound::Nearest);
    Test<FFP8_E5M2>(ERound::Nearest);
    Test<FUFP8_E4M4>(ERound::Nearest);
    Test<FUFP8_E5M3>(ERound::Nearest);

    return 0;
}
