# Quickstart Guide: TSHBandVector

**Feature**: TSHBandVector - Owning SH Band Storage  
**Date**: 2026-01-01  
**Audience**: Developers using UCommon library

## Overview

TSHBandVector provides owning storage for spherical harmonic (SH) band coefficients with automatic memory management and convenient arithmetic operations. This guide shows you how to use TSHBandVector and TSHBandVectorRGB in your code.

---

## Basic Usage

### 1. Include the Header

```cpp
#include <UCommon/SH.h>

using namespace UCommon;
```

### 2. Create a TSHBandVector

```cpp
// Default construction (zero-initialized)
TSHBandVector<2> vec1;  // 3 coefficients (2*2-1), all zeros

// Initializer list construction
TSHBandVector<2> vec2 = {1.0f, 2.0f, 3.0f};  // Exactly 3 elements required

// Copy construction
TSHBandVector<2> vec3 = vec2;  // Deep copy

// Construct from view (deep copy)
float data[3] = {4.0f, 5.0f, 6.0f};
TSHBandView<2, false> view(data);
TSHBandVector<2> vec4(view);  // Deep copy from view
```

### 3. Access Elements

```cpp
TSHBandVector<2> vec = {1.0f, 2.0f, 3.0f};

// Array-style access
float first = vec[0];   // 1.0f
vec[1] = 10.0f;         // Modify second coefficient

// Get underlying pointer
float* ptr = vec.GetData();
const float* const_ptr = vec.GetData();  // const version

// Get size
uint64_t size = vec.GetSize();  // Returns 3 (2*2-1)
```

### 4. Arithmetic Operations

```cpp
TSHBandVector<2> a = {1.0f, 2.0f, 3.0f};
TSHBandVector<2> b = {4.0f, 5.0f, 6.0f};

// Binary operators (return new instances)
TSHBandVector<2> sum = a + b;        // {5.0f, 7.0f, 9.0f}
TSHBandVector<2> diff = a - b;       // {-3.0f, -3.0f, -3.0f}
TSHBandVector<2> scaled = a * 2.0f;  // {2.0f, 4.0f, 6.0f}
TSHBandVector<2> scaled2 = 3.0f * a; // {3.0f, 6.0f, 9.0f} (commutative)
TSHBandVector<2> divided = a / 2.0f; // {0.5f, 1.0f, 1.5f}

// Compound assignment operators (in-place modification)
a += b;   // a becomes {5.0f, 7.0f, 9.0f}
a -= b;   // a becomes {1.0f, 2.0f, 3.0f}
a *= 2.0f; // a becomes {2.0f, 4.0f, 6.0f}
a /= 2.0f; // a becomes {1.0f, 2.0f, 3.0f}

// Dot product
float dot = a.Dot(b);  // 1*4 + 2*5 + 3*6 = 32.0f
```

### 5. Interoperability with TSHBandView

```cpp
TSHBandVector<2> vec = {1.0f, 2.0f, 3.0f};

// Implicit conversion to mutable view
TSHBandView<2, false> mutable_view = vec;
mutable_view[0] = 10.0f;  // Modifies vec[0]

// Implicit conversion to const view (const context)
const TSHBandVector<2>& const_vec = vec;
TSHBandView<2, true> const_view = const_vec;

// Pass to function expecting view
void ProcessBand(const TSHBandView<2, true>& view);
ProcessBand(vec);  // Implicit conversion
```

---

## RGB Usage

### 1. Create a TSHBandVectorRGB

```cpp
// Default construction (all channels zero-initialized)
TSHBandVectorRGB<2> rgb1;

// Construct from three vectors
TSHBandVector<2> r = {1.0f, 2.0f, 3.0f};
TSHBandVector<2> g = {4.0f, 5.0f, 6.0f};
TSHBandVector<2> b = {7.0f, 8.0f, 9.0f};
TSHBandVectorRGB<2> rgb2(r, g, b);

// Copy construction
TSHBandVectorRGB<2> rgb3 = rgb2;  // Deep copy all channels
```

### 2. Access Channels

```cpp
TSHBandVectorRGB<2> rgb;

// Direct channel access
rgb.R[0] = 1.0f;   // Set first coefficient of red channel
rgb.G[1] = 0.5f;   // Set second coefficient of green channel
rgb.B[2] = 0.25f;  // Set third coefficient of blue channel

// Get channel as vector
TSHBandVector<2>& red_channel = rgb.R;
TSHBandVector<2>& green_channel = rgb.G;
TSHBandVector<2>& blue_channel = rgb.B;
```

### 3. RGB Arithmetic Operations

```cpp
TSHBandVectorRGB<2> rgb1, rgb2;

// Channel-wise operations
TSHBandVectorRGB<2> sum = rgb1 + rgb2;   // Add all channels independently
TSHBandVectorRGB<2> diff = rgb1 - rgb2;  // Subtract all channels
TSHBandVectorRGB<2> scaled = rgb1 * 2.0f; // Scale all channels
TSHBandVectorRGB<2> scaled2 = 3.0f * rgb1; // Commutative
TSHBandVectorRGB<2> divided = rgb1 / 2.0f; // Divide all channels

// Compound assignment
rgb1 += rgb2;   // In-place channel-wise addition
rgb1 -= rgb2;   // In-place channel-wise subtraction
rgb1 *= 2.0f;   // In-place scaling (all channels)
rgb1 /= 2.0f;   // In-place division (all channels)
```

### 4. RGB Interoperability with Views

```cpp
TSHBandVectorRGB<2> rgb;

// Implicit conversion to mutable RGB view
TSHBandViewRGB<2, false> mutable_view = rgb;
mutable_view.R[0] = 10.0f;  // Modifies rgb.R[0]

// Implicit conversion to const RGB view
const TSHBandVectorRGB<2>& const_rgb = rgb;
TSHBandViewRGB<2, true> const_view = const_rgb;

// Construct from RGB view (deep copy)
float r_data[3], g_data[3], b_data[3];
TSHBandViewRGB<2, false> view(r_data, g_data, b_data);
TSHBandVectorRGB<2> rgb_copy(view);  // Deep copy, owns storage
```

---

## Common Patterns

### Pattern 1: Compute and Store Result

```cpp
// Before (manual memory management)
float result_data[3];
TSHBandView<2, false> result_view(result_data);
// ... compute into result_view ...

// After (automatic memory management)
TSHBandVector<2> result = view1 + view2;  // Automatic storage
```

### Pattern 2: Chained Operations

```cpp
TSHBandVector<2> a = {1.0f, 2.0f, 3.0f};
TSHBandVector<2> b = {4.0f, 5.0f, 6.0f};
TSHBandVector<2> c = {7.0f, 8.0f, 9.0f};

// All operations return vectors, enabling chaining
TSHBandVector<2> result = (a + b) * 2.0f - c / 3.0f;
```

### Pattern 3: Function Return Values

```cpp
// Return owning vector from function
TSHBandVector<2> ComputeSHBand()
{
    TSHBandVector<2> result = {1.0f, 2.0f, 3.0f};
    result *= 2.0f;
    return result;  // Move semantics (efficient)
}

// Use returned value
TSHBandVector<2> band = ComputeSHBand();
```

---

## Performance Tips

1. **Use move semantics**: Return values use move semantics automatically (C++17)
2. **Prefer compound assignment**: Use `a += b` instead of `a = a + b` for in-place modification
3. **Stack allocation**: TSHBandVector uses stack allocation (no heap overhead)
4. **Zero-overhead abstraction**: Compiler optimizes to same performance as manual array management

---

## Next Steps

- See [data-model.md](data-model.md) for detailed entity descriptions
- See [contracts/](contracts/) for complete API specifications
- See unit tests in `src/tests/` for more examples

