# Specification: SHBand View Design

**Version**: 1.0.0
**Created**: 2026-01-01
**Last Updated**: 2026-01-01
**Author**: Ubpa
**Status**: Draft

---

## Clarifications

### Session 2026-01-01

- Q: Should we maintain backward compatibility? → A: No, directly discard compatibility.
- Q: How should RGB views be structured? → A: Contains three TSHBandView<Order> members (R, G, B).
- Q: How to distinguish const/non-const? → A: Separate types with appropriate pointer types.
- Q: Implicit conversion mutable→const? → A: Yes, allow implicit conversion.
- Q: Support default construction? → A: Yes, pointing to nullptr.
- Q: TSHBandViewRGB constructors? → A: From three pointers, three views, and TSHVectorRGB& (const& for const version).
- Q: Binary operators? → A: No, users use owning types for computations.
- Q: operator[] debug checks? → A: Check both Data != nullptr and Index < MaxSHBasis.
- Q: VectorOrder vs Order relationship? → A: VectorOrder >= Order (extract lower bands from higher vectors).
- Q: Dot() implementation? → A: Both member and non-member (member calls non-member).
- Q: operator= semantics? → A: Shallow copy (rebind pointer).
- Q: Type names? → A: TSHBandView, TSHBandConstView, TSHBandViewRGB, TSHBandConstViewRGB.
- Q: Feature directory and branch naming? → A: Renamed from `1-shbandvector-view` to `1-shband-view` to match new type names.

---

## Overview

### Purpose
Convert `TSHBandVector` and `TSHBandVectorRGB` from memory-owning to view-based classes, renamed to `TSHBandView` and `TSHBandViewRGB`. Views store only pointers, with compile-time size determination. **Breaking change with no backward compatibility.**

### Scope
**In Scope**:
- `TSHBandVector<Order>` → `TSHBandView<Order>` (mutable view)
- New `TSHBandConstView<Order>` (const view)
- `TSHBandVectorRGB<Order>` → `TSHBandViewRGB<Order>` (mutable RGB view)
- New `TSHBandConstViewRGB<Order>` (const RGB view)
- Shallow-copy `operator=` (rebind pointer)
- Update `GetBand()` to return views
- Remove static factory methods from views

**Out of Scope**:
- Backward compatibility
- Helper owning classes
- Changes to TSHVector/TSHVectorRGB (remain owning)
- Serialization changes

---

## Constitution Alignment

- **Naming**: T prefix, PascalCase (Principle 1)
- **Formatting**: Allman style, tabs, MIT header (Principle 2)
- **Type Safety**: explicit, noexcept, constexpr (Principle 3)
- **Resource Management**: View pattern, no ownership (Principle 4)
- **Performance**: Zero-overhead abstraction (Principle 7)
- **Error Handling**: ASSERT for preconditions (Principle 8)
- **Versioning**: MAJOR bump 3.0.0 → 4.0.0 (Principle 10)

---

## Requirements

### Functional Requirements

#### FR-1: View-Based Storage
**Priority**: Critical
**Acceptance Criteria**:
- [ ] Mutable view: `float* Data` member only
- [ ] Const view: `const float* Data` member only
- [ ] Size: `sizeof(float*)` = 8 bytes (64-bit)
- [ ] Compile-time size: `2 * Order - 1`

#### FR-2: Construction
**Priority**: Critical
**Acceptance Criteria**:
- [ ] Default constructor (Data = nullptr)
- [ ] Pointer constructor: `TSHBandView(float* InData)`
- [ ] Const pointer constructor: `TSHBandConstView(const float* InData)`
- [ ] Implicit conversion: `TSHBandView` → `TSHBandConstView`
- [ ] All constructors `noexcept` and `constexpr`

#### FR-3: Array Access
**Priority**: Critical
**Acceptance Criteria**:
- [ ] `operator[]` returns `Data[Index]` reference
- [ ] Debug checks: `Data != nullptr` AND `Index < MaxSHBasis`
- [ ] `GetSize()` returns `2 * Order - 1` (constexpr)
- [ ] `GetData()` returns pointer

#### FR-4: Assignment Operator
**Priority**: High
**Acceptance Criteria**:
- [ ] `operator=` performs shallow copy (rebinds Data pointer)
- [ ] Does NOT copy data content
- [ ] `noexcept`

#### FR-5: Mathematical Operations
**Priority**: High
**Acceptance Criteria**:
- [ ] `operator+=`, `-=`, `*=`, `/=` modify underlying data
- [ ] NO binary operators (`+`, `-`, `*`, `/`)
- [ ] `Dot()` member function
- [ ] Non-member `Dot()` functions (all const/non-const combinations)

#### FR-6: GetBand() Returns Views
**Priority**: Critical
**Acceptance Criteria**:
- [ ] `TSHVector::GetBand<Order>()` returns `TSHBandView<Order>`
- [ ] `TSHVector::GetBand<Order>() const` returns `TSHBandConstView<Order>`
- [ ] Same for `TSHVectorAC`, `TSHVectorRGB`

#### FR-7: RGB View Support
**Priority**: High
**Acceptance Criteria**:
- [ ] `TSHBandViewRGB` contains `TSHBandView R, G, B` members
- [ ] Constructor from three pointers
- [ ] Constructor from three `TSHBandView` objects
- [ ] Constructor from `TSHVectorRGB<VectorOrder>&` (VectorOrder >= Order)
- [ ] `TSHBandConstViewRGB` with const equivalents
- [ ] Channel access via `operator[]`

### Non-Functional Requirements

#### NFR-1: Performance
- View construction: Zero-cost (pointer copy)
- `operator[]`: Single dereference
- Memory: 8 bytes per view (64-bit)

#### NFR-2: Type Safety
- Compile-time size checking
- Const correctness enforced by type system

#### NFR-3: Compatibility
- C++17 standard
- MSVC 2019+, Clang 10+, GCC 9+

---


## API Specification

### TSHBandView<Order>

```cpp
template<int Order>
class TSHBandView
{
public:
	static constexpr int MaxSHOrder = Order;
	static constexpr int MaxSHBasis = 2 * Order - 1;

	using RGBType = TSHBandViewRGB<Order>;

	// Constructors
	constexpr TSHBandView() noexcept : Data(nullptr) {}
	explicit constexpr TSHBandView(float* InData) noexcept : Data(InData) {}

	// Assignment (shallow copy - rebind pointer)
	TSHBandView& operator=(const TSHBandView& Other) noexcept
	{
		Data = Other.Data;
		return *this;
	}

	// Array access
	float& operator[](uint64_t Index) noexcept
	{
		UBPA_UCOMMON_ASSERT(Data != nullptr);
		UBPA_UCOMMON_ASSERT(Index < MaxSHBasis);
		return Data[Index];
	}

	const float& operator[](uint64_t Index) const noexcept
	{
		UBPA_UCOMMON_ASSERT(Data != nullptr);
		UBPA_UCOMMON_ASSERT(Index < MaxSHBasis);
		return Data[Index];
	}

	static constexpr uint64_t GetSize() noexcept { return MaxSHBasis; }

	float* GetData() noexcept { return Data; }
	const float* GetData() const noexcept { return Data; }

	// Mathematical operators (in-place)
	TSHBandView& operator+=(const TSHBandView& Other) noexcept;
	TSHBandView& operator-=(const TSHBandView& Other) noexcept;
	TSHBandView& operator*=(float Scalar) noexcept;
	TSHBandView& operator/=(float Scalar) noexcept;

	// Dot product (member)
	float Dot(const TSHBandView& Other) const noexcept;

private:
	float* Data;
};
```

### TSHBandConstView<Order>

```cpp
template<int Order>
class TSHBandConstView
{
public:
	static constexpr int MaxSHOrder = Order;
	static constexpr int MaxSHBasis = 2 * Order - 1;

	// Constructors
	constexpr TSHBandConstView() noexcept : Data(nullptr) {}
	explicit constexpr TSHBandConstView(const float* InData) noexcept : Data(InData) {}

	// Implicit conversion from mutable view
	constexpr TSHBandConstView(const TSHBandView<Order>& InView) noexcept
		: Data(InView.GetData()) {}

	// Assignment (shallow copy - rebind pointer)
	TSHBandConstView& operator=(const TSHBandConstView& Other) noexcept
	{
		Data = Other.Data;
		return *this;
	}

	// Array access (const only)
	const float& operator[](uint64_t Index) const noexcept
	{
		UBPA_UCOMMON_ASSERT(Data != nullptr);
		UBPA_UCOMMON_ASSERT(Index < MaxSHBasis);
		return Data[Index];
	}

	static constexpr uint64_t GetSize() noexcept { return MaxSHBasis; }
	const float* GetData() const noexcept { return Data; }

	// Dot product (member)
	float Dot(const TSHBandConstView& Other) const noexcept;

private:
	const float* Data;
};
```

### Non-Member Dot Functions

```cpp
// All combinations of const/non-const views
template<int Order>
float Dot(const TSHBandView<Order>& A, const TSHBandView<Order>& B) noexcept;

template<int Order>
float Dot(const TSHBandConstView<Order>& A, const TSHBandConstView<Order>& B) noexcept;

template<int Order>
float Dot(const TSHBandView<Order>& A, const TSHBandConstView<Order>& B) noexcept;

template<int Order>
float Dot(const TSHBandConstView<Order>& A, const TSHBandView<Order>& B) noexcept;
```

### TSHBandViewRGB<Order>

```cpp
template<int Order>
class TSHBandViewRGB
{
public:
	static constexpr int MaxSHOrder = Order;
	static constexpr int MaxSHBasis = 2 * Order - 1;

	TSHBandView<Order> R;
	TSHBandView<Order> G;
	TSHBandView<Order> B;

	// Constructors
	TSHBandViewRGB() noexcept : R(), G(), B() {}

	TSHBandViewRGB(float* InR, float* InG, float* InB) noexcept
		: R(InR), G(InG), B(InB) {}

	TSHBandViewRGB(const TSHBandView<Order>& InR,
	               const TSHBandView<Order>& InG,
	               const TSHBandView<Order>& InB) noexcept
		: R(InR), G(InG), B(InB) {}

	template<int VectorOrder>
	explicit TSHBandViewRGB(TSHVectorRGB<VectorOrder>& InVectorRGB) noexcept
		: R(InVectorRGB.R.template GetBand<Order>())
		, G(InVectorRGB.G.template GetBand<Order>())
		, B(InVectorRGB.B.template GetBand<Order>())
	{
		static_assert(VectorOrder >= Order, "Vector order must be >= band order");
	}

	// Assignment (shallow copy - rebind all three pointers)
	TSHBandViewRGB& operator=(const TSHBandViewRGB& Other) noexcept
	{
		R = Other.R;
		G = Other.G;
		B = Other.B;
		return *this;
	}

	// Channel access
	TSHBandView<Order>& operator[](uint64_t Index) noexcept
	{
		UBPA_UCOMMON_ASSERT(Index < 3);
		return (&R)[Index];
	}

	const TSHBandView<Order>& operator[](uint64_t Index) const noexcept
	{
		UBPA_UCOMMON_ASSERT(Index < 3);
		return (&R)[Index];
	}

	// Mathematical operators (in-place)
	TSHBandViewRGB& operator+=(const TSHBandViewRGB& Other) noexcept;
	TSHBandViewRGB& operator-=(const TSHBandViewRGB& Other) noexcept;
	TSHBandViewRGB& operator*=(float Scalar) noexcept;
	TSHBandViewRGB& operator/=(float Scalar) noexcept;
};
```

### TSHBandConstViewRGB<Order>

```cpp
template<int Order>
class TSHBandConstViewRGB
{
public:
	static constexpr int MaxSHOrder = Order;
	static constexpr int MaxSHBasis = 2 * Order - 1;

	TSHBandConstView<Order> R;
	TSHBandConstView<Order> G;
	TSHBandConstView<Order> B;

	// Constructors
	TSHBandConstViewRGB() noexcept : R(), G(), B() {}

	TSHBandConstViewRGB(const float* InR, const float* InG, const float* InB) noexcept
		: R(InR), G(InG), B(InB) {}

	TSHBandConstViewRGB(const TSHBandConstView<Order>& InR,
	                    const TSHBandConstView<Order>& InG,
	                    const TSHBandConstView<Order>& InB) noexcept
		: R(InR), G(InG), B(InB) {}

	template<int VectorOrder>
	explicit TSHBandConstViewRGB(const TSHVectorRGB<VectorOrder>& InVectorRGB) noexcept
		: R(InVectorRGB.R.template GetBand<Order>())
		, G(InVectorRGB.G.template GetBand<Order>())
		, B(InVectorRGB.B.template GetBand<Order>())
	{
		static_assert(VectorOrder >= Order, "Vector order must be >= band order");
	}

	// Implicit conversion from mutable RGB view
	TSHBandConstViewRGB(const TSHBandViewRGB<Order>& InView) noexcept
		: R(InView.R), G(InView.G), B(InView.B) {}

	// Assignment (shallow copy)
	TSHBandConstViewRGB& operator=(const TSHBandConstViewRGB& Other) noexcept
	{
		R = Other.R;
		G = Other.G;
		B = Other.B;
		return *this;
	}

	// Channel access (const only)
	const TSHBandConstView<Order>& operator[](uint64_t Index) const noexcept
	{
		UBPA_UCOMMON_ASSERT(Index < 3);
		return (&R)[Index];
	}
};
```

---

## Behavior Specification

### Lifetime Management
- Views do NOT own data
- User responsible for ensuring pointed-to data remains valid
- Accessing view after data destruction = undefined behavior
- Default-constructed views (Data = nullptr) must not be dereferenced

### Assignment Semantics
- `operator=` performs **shallow copy** (rebinds pointer)
- Does NOT copy data content
- Example:
  ```cpp
  float data1[3] = {1, 2, 3};
  float data2[3] = {4, 5, 6};
  TSHBandView<2> view1(data1);
  TSHBandView<2> view2(data2);
  view1 = view2;  // view1.Data now points to data2, data1 unchanged
  ```

### Const Correctness
- `TSHBandView` → `TSHBandConstView`: Implicit conversion allowed
- `TSHBandConstView` → `TSHBandView`: NOT allowed (compile error)
- Same for RGB versions

### Debug vs Release
- **Debug**: `operator[]` checks `Data != nullptr` and `Index < MaxSHBasis`
- **Release**: No checks, undefined behavior if violated

---

## Migration Guide

### Breaking Changes
- `TSHBandVector<Order>` renamed to `TSHBandView<Order>`
- `TSHBandVectorRGB<Order>` renamed to `TSHBandViewRGB<Order>`
- New types: `TSHBandConstView<Order>`, `TSHBandConstViewRGB<Order>`
- Views no longer own data (size changes from `sizeof(float) * (2*Order-1)` to `sizeof(float*)`)
- `GetBand()` returns view instead of copy
- Static factory methods removed
- `operator=` now shallow copy (rebind pointer)

### Migration Examples

**Before**:
```cpp
TSHBandVector<2> band = TSHBandVector<2>::SHBasisFunction(FVector3f(0, 0, 1));
```

**After**:
```cpp
TSHVector<2> vec = TSHVector<2>::SHBasisFunction(FVector3f(0, 0, 1));
TSHBandView<2> band = vec.GetBand<2>();
```

---

## Version Impact

**Current Version**: 3.0.0
**New Version**: 4.0.0 (MAJOR bump - breaking changes)

**Affected Components**:
- `include/UCommon/SH.h`
- All code using `TSHBandVector` or `TSHBandVectorRGB`
- Tests in `test/SH/`

---

## Acceptance Criteria

- [ ] All functional requirements met
- [ ] All tests pass
- [ ] Zero-cost abstraction verified (sizeof checks, assembly inspection)
- [ ] Migration guide complete
- [ ] Documentation updated
- [ ] No compiler warnings

---

## References

- [UCommon Project Constitution](../../.specify/memory/constitution.md)
- Current implementation: `include/UCommon/SH.h`
- Related: `std::span`, `std::string_view` (C++ standard library views)

---

*End of Specification*
