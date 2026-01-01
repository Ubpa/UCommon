# Research: TSHBandVector - Owning SH Band Storage

**Feature**: TSHBandVector  
**Date**: 2026-01-01  
**Status**: Complete

## Overview

This document consolidates research findings for implementing TSHBandVector as an owning container for spherical harmonic band coefficients. All technical decisions have been clarified during the specification phase.

---

## Decision 1: Storage Implementation Strategy

### Context
TSHBandVector needs to own storage for 2*Order-1 float coefficients. The choice of storage mechanism affects performance, memory layout, and API design.

### Decision
Use **fixed-size array member** (stack-allocated): `float Data[2*Order-1];`

### Rationale
1. **Zero-overhead abstraction**: No heap allocation overhead, no pointer indirection
2. **Compile-time size determination**: Template parameter Order determines array size at compile time
3. **Consistent with TSHBandView design**: TSHBandView expects contiguous memory, stack allocation provides this naturally
4. **Performance**: Stack allocation is faster than heap allocation, no fragmentation
5. **Simplicity**: No need for custom allocators, destructors automatically handle cleanup
6. **Value semantics**: Natural support for copy/move operations with compiler-generated defaults

### Alternatives Considered
- **Dynamic heap allocation** (`std::unique_ptr<float[]>`): Rejected due to heap overhead and complexity
- **std::array wrapper**: Rejected as unnecessary abstraction layer (raw array is simpler and equally safe)

---

## Decision 2: Binary Operator Design Pattern

### Context
TSHBandView needs binary operators (operator+, operator-, etc.) that return TSHBandVector instances. The design must support both View+View and Vector+Vector operations.

### Decision
Implement binary operators as **non-member functions accepting TSHBandView parameters**. TSHBandVector participates via implicit conversion to TSHBandView.

### Rationale
1. **Unified interface**: Single set of operators handles all combinations (View+View, Vector+Vector, View+Vector, Vector+View)
2. **Implicit conversion**: TSHBandVector provides implicit conversion operators to TSHBandView<Order, false> and TSHBandView<Order, true>
3. **Const-correctness**: Const TSHBandVector converts to TSHBandView<Order, true>, mutable converts to TSHBandView<Order, false>
4. **Consistency**: Matches existing TSHBandView design patterns
5. **Simplicity**: Avoids operator overload proliferation

### Implementation Pattern
```cpp
// Non-member binary operator (in UCommon namespace)
template<int Order>
TSHBandVector<Order> operator+(const TSHBandView<Order, true>& Lhs, const TSHBandView<Order, true>& Rhs) noexcept
{
	TSHBandVector<Order> Result;
	for (uint64_t i = 0; i < TSHBandVector<Order>::MaxSHBasis; ++i)
		Result[i] = Lhs[i] + Rhs[i];
	return Result;
}

// TSHBandVector participates via implicit conversion
TSHBandVector<2> a, b;
TSHBandVector<2> c = a + b;  // OK: a and b implicitly convert to TSHBandView
```

### Alternatives Considered
- **Separate overloads for all combinations**: Rejected due to code duplication and maintenance burden
- **Member operators only**: Rejected as it doesn't support View+View operations cleanly

---

## Decision 3: Scalar Multiplication Operator Forms

### Context
Scalar multiplication should support both `vector * scalar` and `scalar * vector` for mathematical intuitiveness.

### Decision
Provide **two non-member operator* overloads**: one for `TSHBandView * float` and one for `float * TSHBandView`.

### Rationale
1. **Mathematical convention**: Scalar multiplication is commutative (a * k = k * a)
2. **User experience**: Developers expect both forms to work
3. **Standard practice**: Common in C++ math libraries (e.g., Eigen, GLM)
4. **Minimal cost**: Two simple forwarding functions

### Implementation Pattern
```cpp
template<int Order>
TSHBandVector<Order> operator*(const TSHBandView<Order, true>& View, float Scalar) noexcept;

template<int Order>
TSHBandVector<Order> operator*(float Scalar, const TSHBandView<Order, true>& View) noexcept
{
	return View * Scalar;  // Forward to first overload
}
```

---

## Decision 4: Initializer List Constructor Design

### Context
Initializer list construction provides convenient initialization syntax. The design must balance convenience with type safety.

### Decision
Support initializer list construction **requiring exactly 2*Order-1 elements**. Use `static_assert` or constructor constraints to enforce this at compile time.

### Rationale
1. **Type safety**: Prevents accidental partial initialization or over-initialization
2. **Compile-time validation**: Errors caught at compile time, not runtime
3. **Clear semantics**: No ambiguity about what happens with wrong element count
4. **Consistency**: Matches the fixed-size nature of the container

### Implementation Pattern
```cpp
template<int Order>
class TSHBandVector
{
public:
	TSHBandVector(std::initializer_list<float> InitList) noexcept
	{
		static_assert(InitList.size() == MaxSHBasis, "Initializer list must contain exactly 2*Order-1 elements");
		// Note: static_assert on runtime value won't work, need alternative approach
		// Use UBPA_UCOMMON_ASSERT for runtime check
		UBPA_UCOMMON_ASSERT(InitList.size() == MaxSHBasis);
		std::copy(InitList.begin(), InitList.end(), Data);
	}
};
```

### Alternatives Considered
- **Flexible initialization** (allow any count, zero-fill or truncate): Rejected as error-prone
- **No initializer list support**: Rejected as it reduces usability for testing and examples

---

## Decision 5: TSHBandVectorRGB Storage Layout

### Context
TSHBandVectorRGB needs to store RGB color data as three separate SH band coefficient sets.

### Decision
Use **three independent TSHBandVector<Order> members** (R, G, B).

### Rationale
1. **Consistency with TSHBandViewRGB**: Existing design uses three TSHBandView members
2. **Clear semantics**: Each channel is an independent owning container
3. **Easy channel access**: Direct member access (rgb.R, rgb.G, rgb.B)
4. **Automatic memory management**: Each member manages its own storage
5. **Simplicity**: No need for custom layout or indexing logic

### Implementation Pattern
```cpp
template<int Order>
class TSHBandVectorRGB
{
public:
	TSHBandVector<Order> R;
	TSHBandVector<Order> G;
	TSHBandVector<Order> B;
	
	// Constructors, operators, etc.
};
```

### Alternatives Considered
- **Single contiguous array** (interleaved or blocked): Rejected due to complexity and inconsistency with TSHBandViewRGB
- **std::array<TSHBandVector<Order>, 3>**: Rejected as it complicates named channel access

---

## Summary

All technical decisions have been finalized:

1. ✅ **Storage**: Fixed-size array member (stack-allocated)
2. ✅ **Binary operators**: Non-member functions accepting TSHBandView parameters
3. ✅ **Scalar multiplication**: Both vector*scalar and scalar*vector forms
4. ✅ **Initializer list**: Require exactly 2*Order-1 elements
5. ✅ **RGB layout**: Three independent TSHBandVector<Order> members

No further research required. Ready to proceed to Phase 1 (Design & Contracts).

