#include <iostream>
#include <limits>
#include "include/UCommon/Vector.h"

using namespace Ubpa;

int main() {
    std::cout << "Testing IsNaN() methods\n";
    
    // TVector2 test
    TVector2<float> v2_normal(1.0f, 2.0f);
    TVector2<float> v2_nan(std::numeric_limits<float>::quiet_NaN(), 2.0f);
    std::cout << "TVector2: normal = " << (v2_normal.IsNaN() ? "true" : "false") << ", with NaN = " << (v2_nan.IsNaN() ? "true" : "false") << "\n";
    
    // TVector test
    TVector<float> v3_normal(1.0f, 2.0f, 3.0f);
    TVector<float> v3_nan(1.0f, std::numeric_limits<float>::quiet_NaN(), 3.0f);
    std::cout << "TVector(3D): normal = " << (v3_normal.IsNaN() ? "true" : "false") << ", with NaN = " << (v3_nan.IsNaN() ? "true" : "false") << "\n";
    
    // TVector4 test
    TVector4<float> v4_normal(1.0f, 2.0f, 3.0f, 4.0f);
    TVector4<float> v4_nan(1.0f, 2.0f, std::numeric_limits<float>::quiet_NaN(), 4.0f);
    std::cout << "TVector4: normal = " << (v4_normal.IsNaN() ? "true" : "false") << ", with NaN = " << (v4_nan.IsNaN() ? "true" : "false") << "\n";
    
    return 0;
}
