# Data Model: TSHBandVector - Owning SH Band Storage

**Feature**: TSHBandVector  
**Date**: 2026-01-01  
**Status**: Complete

## Overview

This document defines the data structures and their relationships for the TSHBandVector feature. All entities are C++ class templates designed for compile-time optimization and zero-overhead abstraction.

---

## Entity 1: TSHBandVector<Order>

### Description
Owning container for spherical harmonic band coefficients. Stores 2*Order-1 float values in a fixed-size stack-allocated array. Supports value semantics (copy/move) and mathematical operations.

### Template Parameters
- **Order** (int): SH band order (1-based), determines storage size (2*Order-1 coefficients)
  - Constraints: Order >= 1
  - Compile-time constant

### Fields

| Field Name | Type | Description | Validation Rules |
|------------|------|-------------|------------------|
| Data | float[2*Order-1] | Fixed-size array storing coefficients | Private member, stack-allocated |

### Constants

| Constant | Type | Value | Description |
|----------|------|-------|-------------|
| MaxSHOrder | static constexpr int | Order | Maximum SH order for this band |
| MaxSHBasis | static constexpr int | 2*Order-1 | Number of coefficients in this band |

### Constructors

| Constructor | Parameters | Behavior | Validation |
|-------------|------------|----------|------------|
| Default | None | Zero-initialize all coefficients | None |
| Initializer list | std::initializer_list<float> | Copy elements to Data array | ASSERT: list size == MaxSHBasis |
| Copy | const TSHBandVector& | Deep copy all coefficients | None |
| Move | TSHBandVector&& | Transfer ownership (trivial for stack array) | None |
| From View | const TSHBandView<Order, bConst>& | Deep copy from view's data | ASSERT: view data != nullptr |

### Operations

| Operation | Signature | Return Type | Behavior |
|-----------|-----------|-------------|----------|
| Element access | operator[](uint64_t) | float& / const float& | Access coefficient by index |
| Copy assignment | operator=(const TSHBandVector&) | TSHBandVector& | Deep copy all coefficients |
| Move assignment | operator=(TSHBandVector&&) | TSHBandVector& | Transfer ownership |
| Addition | operator+(const TSHBandVector&) | TSHBandVector | Element-wise addition, return new instance |
| Subtraction | operator-(const TSHBandVector&) | TSHBandVector | Element-wise subtraction, return new instance |
| Scalar multiply | operator*(float) | TSHBandVector | Multiply all coefficients, return new instance |
| Scalar divide | operator/(float) | TSHBandVector | Divide all coefficients, return new instance |
| Add-assign | operator+=(const TSHBandView&) | TSHBandVector& | In-place element-wise addition |
| Subtract-assign | operator-=(const TSHBandView&) | TSHBandVector& | In-place element-wise subtraction |
| Multiply-assign | operator*=(float) | TSHBandVector& | In-place scalar multiplication |
| Divide-assign | operator/=(float) | TSHBandVector& | In-place scalar division |
| Dot product | Dot(const TSHBandView&) | float | Compute dot product with another band |
| Get data pointer | GetData() | float* / const float* | Access underlying array pointer |
| Implicit conversion | operator TSHBandView<Order, false>() | TSHBandView<Order, false> | Convert to mutable view |
| Implicit conversion | operator TSHBandView<Order, true>() const | TSHBandView<Order, true> | Convert to const view |

### State Transitions
- **Uninitialized → Initialized**: Via any constructor
- **Initialized → Modified**: Via assignment operators or compound assignment operators
- **Initialized → Copied**: Via copy constructor or copy assignment
- **Initialized → Moved**: Via move constructor or move assignment (source remains valid but unspecified)

### Validation Rules
- Index access: 0 <= Index < MaxSHBasis (enforced by UBPA_UCOMMON_ASSERT)
- Initializer list: size must equal MaxSHBasis (enforced by UBPA_UCOMMON_ASSERT)
- View construction: view data pointer must not be null (enforced by UBPA_UCOMMON_ASSERT in debug builds)

---

## Entity 2: TSHBandVectorRGB<Order>

### Description
Owning container for RGB color data stored as three separate SH band coefficient sets. Contains three independent TSHBandVector<Order> members (R, G, B). Supports coordinated operations across all channels.

### Template Parameters
- **Order** (int): SH band order (1-based), same as TSHBandVector
  - Constraints: Order >= 1
  - Compile-time constant

### Fields

| Field Name | Type | Description | Validation Rules |
|------------|------|-------------|------------------|
| R | TSHBandVector<Order> | Red channel coefficients | Public member |
| G | TSHBandVector<Order> | Green channel coefficients | Public member |
| B | TSHBandVector<Order> | Blue channel coefficients | Public member |

### Constants

| Constant | Type | Value | Description |
|----------|------|-------|-------------|
| MaxSHOrder | static constexpr int | Order | Maximum SH order for this band |
| MaxSHBasis | static constexpr int | 2*Order-1 | Number of coefficients per channel |

### Constructors

| Constructor | Parameters | Behavior | Validation |
|-------------|------------|----------|------------|
| Default | None | Default-construct all three channels (zero-initialized) | None |
| From vectors | TSHBandVector<Order> r, g, b | Copy-construct each channel | None |
| Copy | const TSHBandVectorRGB& | Deep copy all three channels | None |
| Move | TSHBandVectorRGB&& | Move all three channels | None |
| From view | const TSHBandViewRGB<Order, bConst>& | Deep copy from view's three channels | None |

### Operations

| Operation | Signature | Return Type | Behavior |
|-----------|-----------|-------------|----------|
| Channel access | .R, .G, .B | TSHBandVector<Order>& | Direct member access |
| Copy assignment | operator=(const TSHBandVectorRGB&) | TSHBandVectorRGB& | Deep copy all channels |
| Move assignment | operator=(TSHBandVectorRGB&&) | TSHBandVectorRGB& | Move all channels |
| Addition | operator+(const TSHBandVectorRGB&) | TSHBandVectorRGB | Channel-wise addition, return new instance |
| Subtraction | operator-(const TSHBandVectorRGB&) | TSHBandVectorRGB | Channel-wise subtraction, return new instance |
| Scalar multiply | operator*(float) | TSHBandVectorRGB | Multiply all channels, return new instance |
| Scalar divide | operator/(float) | TSHBandVectorRGB | Divide all channels, return new instance |
| Add-assign | operator+=(const TSHBandViewRGB&) | TSHBandVectorRGB& | In-place channel-wise addition |
| Subtract-assign | operator-=(const TSHBandViewRGB&) | TSHBandVectorRGB& | In-place channel-wise subtraction |
| Multiply-assign | operator*=(float) | TSHBandVectorRGB& | In-place scalar multiplication (all channels) |
| Divide-assign | operator/=(float) | TSHBandVectorRGB& | In-place scalar division (all channels) |
| Implicit conversion | operator TSHBandViewRGB<Order, false>() | TSHBandViewRGB<Order, false> | Convert to mutable RGB view |
| Implicit conversion | operator TSHBandViewRGB<Order, true>() const | TSHBandViewRGB<Order, true> | Convert to const RGB view |

### State Transitions
- **Uninitialized → Initialized**: Via any constructor
- **Initialized → Modified**: Via assignment operators or compound assignment operators
- **Initialized → Copied**: Via copy constructor or copy assignment
- **Initialized → Moved**: Via move constructor or move assignment

### Validation Rules
- All validation rules from TSHBandVector apply to each channel independently

---

## Entity 3: TSHBandView<Order, bConst> (Extended)

### Description
Existing non-owning view of SH band coefficients. **Extended** with binary arithmetic operators that return TSHBandVector instances.

### New Operations (Added by this feature)

| Operation | Signature | Return Type | Behavior |
|-----------|-----------|-------------|----------|
| Addition | operator+(const TSHBandView&) | TSHBandVector<Order> | Element-wise addition, return new owning instance |
| Subtraction | operator-(const TSHBandView&) | TSHBandVector<Order> | Element-wise subtraction, return new owning instance |
| Scalar multiply | operator*(float) | TSHBandVector<Order> | Multiply all coefficients, return new owning instance |
| Scalar multiply | float * TSHBandView | TSHBandVector<Order> | Commutative scalar multiplication |
| Scalar divide | operator/(float) | TSHBandVector<Order> | Divide all coefficients, return new owning instance |

### Notes
- All new operators are **non-member functions** in the UCommon namespace
- TSHBandVector participates via implicit conversion to TSHBandView
- Existing compound assignment operators (operator+=, etc.) remain unchanged

---

## Relationships

```text
TSHBandVector<Order>
    ├─ Contains: float Data[2*Order-1] (composition, owned)
    ├─ Converts to: TSHBandView<Order, false> (implicit, non-owning)
    ├─ Converts to: TSHBandView<Order, true> (implicit, non-owning, const context)
    └─ Constructs from: TSHBandView<Order, bConst> (deep copy)

TSHBandVectorRGB<Order>
    ├─ Contains: TSHBandVector<Order> R, G, B (composition, owned)
    ├─ Converts to: TSHBandViewRGB<Order, false> (implicit, non-owning)
    ├─ Converts to: TSHBandViewRGB<Order, true> (implicit, non-owning, const context)
    └─ Constructs from: TSHBandViewRGB<Order, bConst> (deep copy)

TSHBandView<Order, bConst>
    ├─ Extended with: Binary operators returning TSHBandVector<Order>
    └─ Accepts: TSHBandVector<Order> via implicit conversion
```

---

## Summary

- **2 new entities**: TSHBandVector<Order>, TSHBandVectorRGB<Order>
- **1 extended entity**: TSHBandView<Order, bConst> (new binary operators)
- **Storage**: Stack-allocated fixed-size arrays (no heap allocation)
- **Ownership**: Clear separation between owning (Vector) and non-owning (View) types
- **Conversions**: Implicit conversion from Vector to View for API compatibility
- **Operations**: Full set of arithmetic operators with value semantics

Ready to proceed to contract generation.

