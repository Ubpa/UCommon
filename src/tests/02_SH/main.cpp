#include <UCommon/UCommon.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <random>
#include <functional>

int Factorial(int n)
{
    if (n <= 1) return 1;
	return n * Factorial(n - 1);
}

float SHK(int l, int m)
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

    // 单位球面面积是4π，但蒙特卡洛积分需要除以样本数
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

int main(int argc, char** argv)
{
	for (int l = 0; l <= 4; l++)
	{
		for (int m = -l; m <= l; m++)
		{
			// float 12 decimal places
			std::cout << "l: " << l << ", m: " << m << ", SHK: " << std::fixed << std::setprecision(12) << SHK(l, m) << std::endl;
		}
	}

	// test template parameters l=0,1,2,3,4 and m=-l..l without for-loop for testNormalization<l,m>()
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

    return 0;
}
