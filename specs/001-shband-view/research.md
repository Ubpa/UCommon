# Research: SHBand View Design

**Feature**: SHBand View Design  
**Date**: 2026-01-01  
**Status**: Phase 0 Complete

## Overview

This document consolidates research findings for implementing a CRTP-based template parameter design for SHBand view classes. All technical unknowns from the planning phase have been resolved.

---

## Research Task 1: CRTP Pattern Best Practices

### Decision
Use CRTP (Curiously Recurring Template Pattern) with `TSHBandViewCommon<Derived, Order, bConst>` as base class.

### Rationale
1. **Zero-overhead polymorphism**: CRTP enables compile-time polymorphism without virtual function overhead
2. **Type-safe return types**: Base class methods can return correct derived type via `static_cast<Derived&>(*this)`
3. **Code reuse**: Common methods (const `operator[]`, `GetSize()`, `Dot()`) shared across specializations
4. **Standard C++ idiom**: Well-established pattern used in STL (e.g., `std::enable_shared_from_this`)

### Implementation Pattern
```cpp
template<typename Derived, int Order, bool bConst>
class TSHBandViewCommon
{
protected:
    Derived& AsDerived() { return static_cast<Derived&>(*this); }
    const Derived& AsDerived() const { return static_cast<const Derived&>(*this); }
    
public:
    // Common methods that work for both const and non-const
    const float& operator[](uint64_t Index) const noexcept
    {
        return AsDerived().GetData()[Index];
    }
};

template<int Order>
class TSHBandView<Order, false> : public TSHBandViewCommon<TSHBandView<Order, false>, Order, false>
{
    // Mutable-specific members
};
```

### Alternatives Considered
- **Option A: SFINAE with `std::enable_if_t`**: More verbose, harder to read, same performance
- **Option B: Separate classes without inheritance**: Code duplication, maintenance burden
- **Option C: `if constexpr` in single class**: Cannot conditionally add/remove member functions

### References
- Effective C++ Item 41: "Understand implicit interfaces and compile-time polymorphism"
- C++ Templates: The Complete Guide (2nd Ed), Chapter 21.2: "The Curiously Recurring Template Pattern"

---

## Research Task 2: Template Partial Specialization for `bConst`

### Decision
Use partial specialization for `TSHBandView<Order, true>` and `TSHBandView<Order, false>`.

### Rationale
1. **Clear separation**: Mutable and const versions have distinct member sets
2. **Compile-time selection**: No runtime overhead for const checking
3. **Type safety**: Prevents accidental mutation of const views
4. **Standard practice**: Similar to `std::span<T>` vs `std::span<const T>` design

### Implementation Pattern
```cpp
// Primary template (mutable)
template<int Order, bool bConst = false>
class TSHBandView;

// Specialization for mutable (bConst=false)
template<int Order>
class TSHBandView<Order, false> : public TSHBandViewCommon<...>
{
    float* Data;
    float& operator[](uint64_t Index) noexcept;  // Mutable access
    TSHBandView& operator+=(const TSHBandView&);  // Mutable operations
};

// Specialization for const (bConst=true)
template<int Order>
class TSHBandView<Order, true> : public TSHBandViewCommon<...>
{
    const float* Data;
    // No mutable operator[] or operator+=
    // Conversion constructor from TSHBandView<Order, false>
    TSHBandView(const TSHBandView<Order, false>& Other);
};
```

### Alternatives Considered
- **Option A: Single class with SFINAE**: More complex, harder to maintain
- **Option B: `if constexpr` for member functions**: Cannot conditionally declare members
- **Option C: Inheritance without specialization**: Cannot change member types (float* vs const float*)

### References
- C++ Templates: The Complete Guide, Chapter 16: "Specialization and Overloading"
- ISO C++17 Standard, Section 14.5.5: "Class template partial specializations"

---

## Research Task 3: `std::conditional_t` for Pointer Type Selection

### Decision
Use `std::conditional_t<bConst, const float*, float*>` for compile-time pointer type selection in base class.

### Rationale
1. **Type safety**: Compiler enforces const correctness
2. **Zero overhead**: Resolved at compile-time
3. **Standard library**: Part of C++11 `<type_traits>`
4. **Readability**: Clear intent compared to manual template metaprogramming

### Implementation Pattern
```cpp
template<typename Derived, int Order, bool bConst>
class TSHBandViewCommon
{
public:
    using DataType = std::conditional_t<bConst, const float*, float*>;
    
protected:
    DataType GetData() const { return AsDerived().GetData(); }
};
```

### Alternatives Considered
- **Option A: Manual template specialization**: More verbose, reinvents standard library
- **Option B: `const_cast`**: Unsafe, defeats const correctness
- **Option C: Always use `const float*`**: Cannot support mutable views

### References
- cppreference.com: `std::conditional`
- Effective Modern C++ Item 9: "Prefer alias declarations to typedefs"

---

## Research Task 4: Conversion Constructor for Implicit Mutable→Const Conversion

### Decision
Add conversion constructor in `TSHBandView<Order, true>` accepting `const TSHBandView<Order, false>&`.

### Rationale
1. **Standard practice**: Mirrors `std::span<const T>` from `std::span<T>` conversion
2. **Implicit conversion**: Natural C++ semantics (mutable→const is safe)
3. **Type safety**: Prevents reverse conversion (const→mutable)
4. **User-friendly**: Allows passing mutable views to functions expecting const views

### Implementation Pattern
```cpp
template<int Order>
class TSHBandView<Order, true>
{
public:
    // Implicit conversion from mutable view
    constexpr TSHBandView(const TSHBandView<Order, false>& Other) noexcept
        : Data(Other.GetData()) {}
};
```

### Alternatives Considered
- **Option A: Explicit conversion function `ToConst()`**: Less ergonomic, breaks C++ idioms
- **Option B: Conversion operator in mutable class**: Works but less conventional
- **Option C: No conversion**: Forces users to manually extract pointers

### References
- C++ Core Guidelines C.164: "Avoid implicit conversion operators"
- Effective C++ Item 15: "Provide access to raw resources in resource-managing classes"

---

## Research Task 5: Single Template `Dot()` Function

### Decision
Implement single template function `template<int Order, bool bConst1, bool bConst2> float Dot(...)`.

### Rationale
1. **Simplification**: Reduces 4 overloads to 1 template
2. **Maintainability**: Single implementation, no code duplication
3. **Flexibility**: Handles all const combinations automatically
4. **Performance**: Fully inlined, zero overhead

### Implementation Pattern
```cpp
template<int Order, bool bConst1, bool bConst2>
float Dot(const TSHBandView<Order, bConst1>& A, const TSHBandView<Order, bConst2>& B) noexcept
{
    float Result = 0.0f;
    for (uint64_t i = 0; i < TSHBandView<Order, bConst1>::MaxSHBasis; ++i)
        Result += A[i] * B[i];
    return Result;
}
```

### Alternatives Considered
- **Option A: 4 explicit overloads**: Code duplication, maintenance burden
- **Option B: Member function only**: Less flexible, cannot swap operands naturally
- **Option C: Concept-based constraints (C++20)**: Requires C++20, project uses C++17

### References
- C++ Templates: The Complete Guide, Chapter 1: "Function Templates"
- Effective C++ Item 46: "Define non-member functions inside templates when type conversions are desired"

---

## Summary of Resolved Unknowns

| Unknown | Resolution |
|---------|------------|
| CRTP base class design | `TSHBandViewCommon<Derived, Order, bConst>` with protected `AsDerived()` |
| Partial specialization strategy | Specialize for `bConst=false` and `bConst=true` |
| Pointer type selection | `std::conditional_t<bConst, const float*, float*>` |
| Implicit conversion mechanism | Conversion constructor in const specialization |
| Dot() function simplification | Single template with two `bConst` parameters |
| GetBand() return types | Non-const returns `TSHBandView<Order, false>`, const returns `TSHBandView<Order, true>` |

---

## Next Steps

Proceed to **Phase 1: Design & Contracts** to generate:
1. `data-model.md` - Class hierarchy and template instantiation model
2. `contracts/` - API contracts for each view class
3. `quickstart.md` - Migration guide and usage examples

