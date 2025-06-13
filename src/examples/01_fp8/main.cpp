#include <cstdint>
#include <cmath>
#include <algorithm>
#include <iostream>

template<uint32_t E, uint32_t M, int32_t B>
class FFP8
{
public:
    static constexpr uint32_t BiasE = (1 << (E - 1)) - 1 + B;
    static constexpr uint32_t SubNormalScale = 1 << (BiasE + M - 1);
    static constexpr uint32_t MaskE = (1 << E) - 1;
    static constexpr uint32_t MaskM = (1 << M) - 1;

    FFP8() : data(0x00) {}

    explicit FFP8(float f) {
        auto bits = reinterpret_cast<const uint32_t&>(f);

        const uint32_t sign = bits >> 31;
        const int32_t exp = (bits >> 23) & 0xFF;
        const uint32_t frac = bits & 0x7FFFFF;

        if (f == 0.0f) {
            data = sign << 7;
            return;
        }

        const int32_t e_f = exp - 127;

        if (exp == 0xFF) {
            data = (sign << 7) | 0x7F;
            return;
        }

        int32_t e = e_f + BiasE;

        uint32_t m24 = (1 << 23) | frac;

        uint8_t m = (m24 >> (23 - M)) & MaskM;
        const uint8_t round_bit = (m24 >> (22 - M)) & 0x01;

        if (e <= 0) {
            const float abs_f = std::abs(f);
            constexpr float scale = SubNormalScale;
            const uint32_t m = static_cast<uint32_t>(abs_f * scale + 0.5f);

            if (m > MaskM) {
                data = (sign << 7) | (1 << M);
                return;
            }

            data = (sign << 7) | m;
            return;
        }

        if (round_bit) {
            if (m == MaskM) {
                m = 0;
                e++;
            }
            else {
                m++;
            }
        }

        if (e > MaskE) {
            data = (sign << 7) | 0x7F;
            return;
        }

        data = (sign << 7) | (e << M) | m;
    }

    explicit operator float() const {
        const uint32_t sign = data >> 7;
        const uint32_t exp = (data >> M) & MaskE;
        const uint32_t m = data & MaskM;

        if (exp == 0) {
            constexpr float scale = SubNormalScale;
            float value = static_cast<float>(m) / scale;

            return sign ? -value : value;
        }
        else {
            const int32_t e = exp - BiasE;
            uint32_t bits = 0;
            bits |= (sign << 31);
            bits |= static_cast<uint32_t>(e + 127) << 23;
            bits |= (m << (23 - M));

            auto f = reinterpret_cast<const float&>(bits);
            return f;
        }
    }

    bool operator==(const FFP8& rhs) const {
        return (data == 0 && rhs.data == 0x80)
            || (data == 0x80 && rhs.data == 0)
            || (data == rhs.data);
    }
    bool operator!=(const FFP8& rhs) const { return !(*this == rhs); }


private:
    uint8_t data;
};

int main() {
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
        FFP8<4, 3, 1> f8(f);
        float f_back = static_cast<float>(f8);

        std::cout << f << "," << f_back << std::endl;

        if (std::isnan(f)) continue;

        FFP8<4, 3, 1> f8_again(f_back);
        if (f8 != f8_again) {
            return 1;
        }
    }

    for (uint16_t value = 0; value < 256; value++)
    {
        uint8_t v8 = (uint8_t)value;
        float f = static_cast<float>(*(FFP8<3, 4, 1>*)&v8);
        std::cout << f << std::endl;
    }

    return 0;
}
