# Quickstart Guide: SHBand View Design

**Feature**: SHBand View Design  
**Version**: 4.0.0  
**Date**: 2026-01-01

## Overview

This guide helps you migrate from the old `TSHBandVector` (memory-owning) to the new `TSHBandView` (view-based) design. The new design uses template parameters for const-correctness and CRTP for zero-overhead abstraction.

---

## What Changed?

### Type Names

| Old (3.x) | New (4.0) | Notes |
|-----------|-----------|-------|
| `TSHBandVector<Order>` | `TSHBandView<Order, false>` | Mutable view (default) |
| N/A | `TSHBandView<Order, true>` | Const view |
| `TSHBandVectorRGB<Order>` | `TSHBandViewRGB<Order, false>` | Mutable RGB view |
| N/A | `TSHBandViewRGB<Order, true>` | Const RGB view |

### Key Differences

| Aspect | Old Design | New Design |
|--------|------------|------------|
| **Ownership** | Owns data (`float V[2*Order-1]`) | View only (pointer) |
| **Size** | `sizeof(float) * (2*Order-1)` | `sizeof(float*)` = 8 bytes |
| **Const Handling** | Separate classes | Template parameter `bConst` |
| **Number of Types** | 4 separate classes | 2 template classes |
| **Dot() Overloads** | 4 functions | 1 template function |

---

## Migration Examples

### Example 1: Basic Usage

**Before (3.x)**:
```cpp
// Old: Memory-owning
TSHBandVector<2> band;
band[0] = 1.0f;
band[1] = 0.5f;
band[2] = 0.3f;
```

**After (4.0)**:
```cpp
// New: View requires external storage
TSHVector<2> vec;  // Owning vector
TSHBandView<2> band = vec.GetBand<2>();  // View into vec
band[0] = 1.0f;
band[1] = 0.5f;
band[2] = 0.3f;

// Or use raw array:
float data[3] = {1.0f, 0.5f, 0.3f};
TSHBandView<2> band(data);
```

### Example 2: Const Correctness

**Before (3.x)**:
```cpp
void ProcessBand(const TSHBandVector<2>& band)
{
    float val = band[0];  // OK
    // band[0] = 1.0f;    // ERROR - const reference
}
```

**After (4.0)**:
```cpp
void ProcessBand(TSHBandView<2, true> band)  // Const view
{
    float val = band[0];  // OK
    // band[0] = 1.0f;    // ERROR - const view
}

// Implicit conversion from mutable to const
TSHBandView<2, false> mutable_band;
ProcessBand(mutable_band);  // OK - implicit conversion
```

### Example 3: GetBand() Returns Views

**Before (3.x)**:
```cpp
TSHVector<3> vec;
TSHBandVector<2> band = vec.GetBand<2>();  // Copy
band[0] = 1.0f;  // Modifies local copy, NOT vec
```

**After (4.0)**:
```cpp
TSHVector<3> vec;
TSHBandView<2> band = vec.GetBand<2>();  // View (no copy)
band[0] = 1.0f;  // Modifies vec directly!

// Const version:
const TSHVector<3> const_vec;
TSHBandView<2, true> const_band = const_vec.GetBand<2>();
// const_band[0] = 1.0f;  // ERROR - const view
```

### Example 4: RGB Views

**Before (3.x)**:
```cpp
TSHBandVectorRGB<2> rgb;
rgb.R[0] = 1.0f;
rgb.G[0] = 0.5f;
rgb.B[0] = 0.3f;
```

**After (4.0)**:
```cpp
TSHVectorRGB<2> vec;
TSHBandViewRGB<2> rgb = vec.GetBand<2>();
rgb.R[0] = 1.0f;
rgb.G[0] = 0.5f;
rgb.B[0] = 0.3f;

// Or construct from three pointers:
float r[3], g[3], b[3];
TSHBandViewRGB<2> rgb(r, g, b);
```

### Example 5: Dot Product

**Before (3.x)**:
```cpp
// 4 overloads needed for all const combinations
float result = Dot(band1, band2);
```

**After (4.0)**:
```cpp
// Single template handles all combinations
TSHBandView<2, false> mutable_band;
TSHBandView<2, true> const_band;

float result1 = Dot(mutable_band, mutable_band);  // OK
float result2 = Dot(const_band, const_band);      // OK
float result3 = Dot(mutable_band, const_band);    // OK
float result4 = Dot(const_band, mutable_band);    // OK
```

---

## Common Patterns

### Pattern 1: Temporary View for Computation

```cpp
void ComputeSH(TSHVector<3>& vec)
{
    // Extract band 2 as view
    auto band = vec.GetBand<2>();
    
    // Modify in-place
    band *= 2.0f;
    band += some_other_band;
    
    // Changes reflected in vec immediately
}
```

### Pattern 2: Passing Views to Functions

```cpp
// Accept any const-ness
template<bool bConst>
float ComputeNorm(TSHBandView<2, bConst> band)
{
    return std::sqrt(Dot(band, band));
}

// Or explicitly require mutable
void Normalize(TSHBandView<2, false> band)
{
    float norm = ComputeNorm(band);
    band /= norm;
}
```

### Pattern 3: Default-Constructed Views

```cpp
TSHBandView<2> band;  // Data = nullptr

if (condition)
    band = vec1.GetBand<2>();
else
    band = vec2.GetBand<2>();

// Now band points to one of the vectors
```

---

## Breaking Changes Checklist

- [ ] Replace `TSHBandVector<Order>` with `TSHBandView<Order>` or `TSHBandView<Order, false>`
- [ ] Replace `TSHBandVectorRGB<Order>` with `TSHBandViewRGB<Order>` or `TSHBandViewRGB<Order, false>`
- [ ] Update functions accepting const references to use `TSHBandView<Order, true>`
- [ ] Ensure external storage exists for all views (views don't own data)
- [ ] Update `GetBand()` call sites - now returns view, not copy
- [ ] Remove static factory methods (e.g., `SHBasisFunction`) - use owning types instead
- [ ] Update `Dot()` calls - now single template function
- [ ] Verify lifetime: views must not outlive pointed-to data

---

## Performance Notes

### Memory Savings

```cpp
// Old design:
sizeof(TSHBandVector<2>) = 3 * sizeof(float) = 12 bytes
sizeof(TSHBandVectorRGB<2>) = 3 * 12 = 36 bytes

// New design:
sizeof(TSHBandView<2>) = sizeof(float*) = 8 bytes
sizeof(TSHBandViewRGB<2>) = 3 * 8 = 24 bytes
```

### Zero-Overhead Abstraction

- All template instantiation at compile-time
- No virtual functions (CRTP instead)
- Single pointer dereference for array access
- Fully inlined methods

---

## Next Steps

1. **Read the full specification**: `spec.md`
2. **Review API contracts**: `contracts/*.yaml`
3. **Check data model**: `data-model.md`
4. **Run tests**: `test/SH/SHBandView.cpp`

---

## Support

For questions or issues, refer to:
- **Specification**: `specs/1-shband-view/spec.md`
- **Implementation Plan**: `specs/1-shband-view/plan.md`
- **GitHub Issues**: https://github.com/Ubpa/UCommon/issues

