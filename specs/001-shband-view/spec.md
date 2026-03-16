# Specification: SHBand View Design

**Version**: 1.0.0
**Created**: 2026-01-01
**Last Updated**: 2026-01-01
**Author**: Ubpa
**Status**: Draft

---

## Clarifications

### Session 2026-01-01 (Initial Design)

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

### Session 2026-01-01 (Design Revision - Template Parameter Approach)

- Q: Architecture simplification? → A: Use `TSHBandView<Order, bConst=false>` with template parameter instead of separate const/non-const types. Use CRTP + inheritance + partial specialization: base class `TSHBandViewCommon<Derived, Order, bConst>` with common interfaces, then specialize `TSHBandView<Order, false>` and `TSHBandView<Order, true>` for mutable/const-specific members.
- Q: CRTP base class naming? → A: `TSHBandViewCommon<Derived, Order, bConst>` directly in `UCommon` namespace as public base class.
- Q: Implicit conversion mutable→const with template parameter? → A: Add conversion constructor `TSHBandView(const TSHBandView<Order, false>& other)` in `TSHBandView<Order, true>` specialization.
- Q: TSHBandViewRGB member organization? → A: Use `TSHBandView<Order, bConst> R, G, B;` as member types to automatically inherit const correctness.
- Q: Non-member Dot() simplification? → A: Single template function `template<int Order, bool bConst1, bool bConst2> float Dot(const TSHBandView<Order, bConst1>&, const TSHBandView<Order, bConst2>&)` to handle all const combinations.
- Q: GetBand() return type? → A: Non-const version returns `TSHBandView<BandOrder, false>`, const version returns `TSHBandView<BandOrder, true>`.
- Q: CRTP base class naming? → A: `detail::TSHBandViewBase<Derived, Order, bConst>` in `UCommon::detail` namespace.

---

## Overview

### Purpose
Convert `TSHBandVector` and `TSHBandVectorRGB` from memory-owning to view-based classes, renamed to `TSHBandView<Order, bConst>` and `TSHBandViewRGB<Order, bConst>`. Views store only pointers, with compile-time size determination and const-correctness via template parameter. **Breaking change with no backward compatibility.**

### Scope
**In Scope**:
- `TSHBandVector<Order>` → `TSHBandView<Order, bConst=false>` (template parameter for const control)
- `TSHBandVectorRGB<Order>` → `TSHBandViewRGB<Order, bConst=false>` (template parameter for const control)
- CRTP base class `TSHBandViewCommon<Derived, Order, bConst>` for shared functionality
- Partial specialization: `TSHBandView<Order, false>` and `TSHBandView<Order, true>`
- Implicit conversion from `bConst=false` to `bConst=true`
- Shallow-copy `operator=` (rebind pointer)
- Update `GetBand()` to return views with appropriate `bConst`
- Simplified `Dot()` function (single template instead of 4 overloads)
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

#### FR-1: View-Based Storage with Template Parameter
**Priority**: Critical
**Acceptance Criteria**:
- [ ] Template signature: `TSHBandView<int Order, bool bConst = false>`
- [ ] `bConst=false`: `float* Data` member
- [ ] `bConst=true`: `const float* Data` member
- [ ] Size: `sizeof(float*)` = 8 bytes (64-bit)
- [ ] Compile-time size: `2 * Order - 1`

#### FR-2: CRTP Base Class Architecture
**Priority**: Critical
**Acceptance Criteria**:
- [ ] Base class: `TSHBandViewCommon<Derived, Order, bConst>`
- [ ] Contains common interfaces (const-safe methods)
- [ ] Uses CRTP to return correct derived type
- [ ] Partial specializations: `TSHBandView<Order, false>` and `TSHBandView<Order, true>`
- [ ] Mutable-specific members only in `bConst=false` specialization

#### FR-3: Construction
**Priority**: Critical
**Acceptance Criteria**:
- [ ] Default constructor (Data = nullptr)
- [ ] Pointer constructor: `TSHBandView<Order, false>(float* InData)`
- [ ] Const pointer constructor: `TSHBandView<Order, true>(const float* InData)`
- [ ] Implicit conversion: `TSHBandView<Order, false>` → `TSHBandView<Order, true>` via conversion constructor
- [ ] All constructors `noexcept` and `constexpr`

#### FR-4: Array Access
**Priority**: Critical
**Acceptance Criteria**:
- [ ] `operator[]` returns `Data[Index]` reference (mutable for `bConst=false`, const for `bConst=true`)
- [ ] Debug checks: `Data != nullptr` AND `Index < MaxSHBasis`
- [ ] `GetSize()` returns `2 * Order - 1` (constexpr)
- [ ] `GetData()` returns pointer (type matches `bConst`)

#### FR-5: Assignment Operator
**Priority**: High
**Acceptance Criteria**:
- [ ] `operator=` performs shallow copy (rebinds Data pointer)
- [ ] Does NOT copy data content
- [ ] `noexcept`

#### FR-6: Mathematical Operations
**Priority**: High
**Acceptance Criteria**:
- [ ] `operator+=`, `-=`, `*=`, `/=` modify underlying data (only in `bConst=false` specialization)
- [ ] NO binary operators (`+`, `-`, `*`, `/`)
- [ ] `Dot()` member function (available in both specializations)
- [ ] Single non-member `Dot()` template: `template<int Order, bool bConst1, bool bConst2> float Dot(...)`

#### FR-7: GetBand() Returns Views
**Priority**: Critical
**Acceptance Criteria**:
- [ ] `TSHVector::GetBand<Order>()` returns `TSHBandView<Order, false>`
- [ ] `TSHVector::GetBand<Order>() const` returns `TSHBandView<Order, true>`
- [ ] Same for `TSHVectorAC`, `TSHVectorRGB` (returns `TSHBandViewRGB<Order, bConst>`)

#### FR-8: RGB View Support
**Priority**: High
**Acceptance Criteria**:
- [ ] Template signature: `TSHBandViewRGB<int Order, bool bConst = false>`
- [ ] Members: `TSHBandView<Order, bConst> R, G, B;`
- [ ] Constructor from three pointers
- [ ] Constructor from three `TSHBandView<Order, bConst>` objects
- [ ] Constructor from `TSHVectorRGB<VectorOrder>&` (non-const) or `const TSHVectorRGB<VectorOrder>&` (const)
- [ ] Implicit conversion: `TSHBandViewRGB<Order, false>` → `TSHBandViewRGB<Order, true>`
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

### TSHBandViewCommon<Derived, Order, bConst> (CRTP Base Class)

```cpp
template<typename Derived, int Order, bool bConst>
class TSHBandViewCommon
{
public:
	static constexpr int MaxSHOrder = Order;
	static constexpr int MaxSHBasis = 2 * Order - 1;

	using DataType = std::conditional_t<bConst, const float*, float*>;
	using RGBType = TSHBandViewRGB<Order, bConst>;

	// Array access (const version - always available)
	const float& operator[](uint64_t Index) const noexcept
	{
		UBPA_UCOMMON_ASSERT(GetData() != nullptr);
		UBPA_UCOMMON_ASSERT(Index < MaxSHBasis);
		return GetData()[Index];
	}

	static constexpr uint64_t GetSize() noexcept { return MaxSHBasis; }

	// Dot product (member - calls non-member)
	template<bool bOtherConst>
	float Dot(const TSHBandView<Order, bOtherConst>& Other) const noexcept
	{
		return UCommon::Dot(AsDerived(), Other);
	}

protected:
	constexpr TSHBandViewCommon() noexcept = default;

	const Derived& AsDerived() const noexcept { return static_cast<const Derived&>(*this); }
	Derived& AsDerived() noexcept { return static_cast<Derived&>(*this); }

	// Derived class must provide GetData()
	DataType GetData() const noexcept { return AsDerived().GetData(); }
};
```

### TSHBandView<Order, false> (Mutable Specialization)

```cpp
template<int Order>
class TSHBandView<Order, false> : public TSHBandViewCommon<TSHBandView<Order, false>, Order, false>
{
	using Super = TSHBandViewCommon<TSHBandView<Order, false>, Order, false>;

public:
	// Constructors
	constexpr TSHBandView() noexcept : Data(nullptr) {}
	explicit constexpr TSHBandView(float* InData) noexcept : Data(InData) {}

	// Assignment (shallow copy - rebind pointer)
	TSHBandView& operator=(const TSHBandView& Other) noexcept
	{
		Data = Other.Data;
		return *this;
	}

	// Array access (mutable version)
	float& operator[](uint64_t Index) noexcept
	{
		UBPA_UCOMMON_ASSERT(Data != nullptr);
		UBPA_UCOMMON_ASSERT(Index < Super::MaxSHBasis);
		return Data[Index];
	}

	// Inherit const version from base
	using Super::operator[];

	float* GetData() noexcept { return Data; }
	const float* GetData() const noexcept { return Data; }

	// Mathematical operators (in-place) - only in mutable version
	TSHBandView& operator+=(const TSHBandView& Other) noexcept;
	TSHBandView& operator-=(const TSHBandView& Other) noexcept;
	TSHBandView& operator*=(float Scalar) noexcept;
	TSHBandView& operator/=(float Scalar) noexcept;

private:
	float* Data;
};
```

### TSHBandView<Order, true> (Const Specialization)

```cpp
template<int Order>
class TSHBandView<Order, true> : public TSHBandViewCommon<TSHBandView<Order, true>, Order, true>
{
	using Super = TSHBandViewCommon<TSHBandView<Order, true>, Order, true>;

public:
	// Constructors
	constexpr TSHBandView() noexcept : Data(nullptr) {}
	explicit constexpr TSHBandView(const float* InData) noexcept : Data(InData) {}

	// Implicit conversion from mutable view
	constexpr TSHBandView(const TSHBandView<Order, false>& Other) noexcept
		: Data(Other.GetData()) {}

	// Assignment (shallow copy - rebind pointer)
	TSHBandView& operator=(const TSHBandView& Other) noexcept
	{
		Data = Other.Data;
		return *this;
	}

	const float* GetData() const noexcept { return Data; }

	// Array access inherited from base (const only)
	using Super::operator[];

private:
	const float* Data;
};
```

### Non-Member Dot Function (Simplified)

```cpp
// Single template handles all const combinations
template<int Order, bool bConst1, bool bConst2>
float Dot(const TSHBandView<Order, bConst1>& A, const TSHBandView<Order, bConst2>& B) noexcept
{
	float Result = 0.0f;
	for (uint64_t i = 0; i < TSHBandView<Order, bConst1>::MaxSHBasis; ++i)
		Result += A[i] * B[i];
	return Result;
}
```

### TSHBandViewRGB<Order, bConst> (Unified RGB View)

```cpp
template<int Order, bool bConst = false>
class TSHBandViewRGB
{
public:
	static constexpr int MaxSHOrder = Order;
	static constexpr int MaxSHBasis = 2 * Order - 1;

	// Members automatically inherit const correctness
	TSHBandView<Order, bConst> R;
	TSHBandView<Order, bConst> G;
	TSHBandView<Order, bConst> B;

	// Constructors
	TSHBandViewRGB() noexcept : R(), G(), B() {}

	// Constructor from pointers (type matches bConst)
	TSHBandViewRGB(
		std::conditional_t<bConst, const float*, float*> InR,
		std::conditional_t<bConst, const float*, float*> InG,
		std::conditional_t<bConst, const float*, float*> InB) noexcept
		: R(InR), G(InG), B(InB) {}

	// Constructor from three views
	TSHBandViewRGB(
		const TSHBandView<Order, bConst>& InR,
		const TSHBandView<Order, bConst>& InG,
		const TSHBandView<Order, bConst>& InB) noexcept
		: R(InR), G(InG), B(InB) {}

	// Constructor from TSHVectorRGB (const-correct)
	template<int VectorOrder>
	explicit TSHBandViewRGB(
		std::conditional_t<bConst, const TSHVectorRGB<VectorOrder>&, TSHVectorRGB<VectorOrder>&> InVectorRGB) noexcept
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
	TSHBandView<Order, bConst>& operator[](uint64_t Index) noexcept
	{
		UBPA_UCOMMON_ASSERT(Index < 3);
		return (&R)[Index];
	}

	const TSHBandView<Order, bConst>& operator[](uint64_t Index) const noexcept
	{
		UBPA_UCOMMON_ASSERT(Index < 3);
		return (&R)[Index];
	}
};
```

### TSHBandViewRGB<Order, true> Specialization (Const RGB View)

```cpp
template<int Order>
class TSHBandViewRGB<Order, true>
{
public:
	static constexpr int MaxSHOrder = Order;
	static constexpr int MaxSHBasis = 2 * Order - 1;

	TSHBandView<Order, true> R;
	TSHBandView<Order, true> G;
	TSHBandView<Order, true> B;

	// Constructors
	TSHBandViewRGB() noexcept : R(), G(), B() {}

	TSHBandViewRGB(const float* InR, const float* InG, const float* InB) noexcept
		: R(InR), G(InG), B(InB) {}

	TSHBandViewRGB(
		const TSHBandView<Order, true>& InR,
		const TSHBandView<Order, true>& InG,
		const TSHBandView<Order, true>& InB) noexcept
		: R(InR), G(InG), B(InB) {}

	template<int VectorOrder>
	explicit TSHBandViewRGB(const TSHVectorRGB<VectorOrder>& InVectorRGB) noexcept
		: R(InVectorRGB.R.template GetBand<Order>())
		, G(InVectorRGB.G.template GetBand<Order>())
		, B(InVectorRGB.B.template GetBand<Order>())
	{
		static_assert(VectorOrder >= Order, "Vector order must be >= band order");
	}

	// Implicit conversion from mutable RGB view
	TSHBandViewRGB(const TSHBandViewRGB<Order, false>& Other) noexcept
		: R(Other.R), G(Other.G), B(Other.B) {}

	// Assignment (shallow copy)
	TSHBandViewRGB& operator=(const TSHBandViewRGB& Other) noexcept
	{
		R = Other.R;
		G = Other.G;
		B = Other.B;
		return *this;
	}

	// Channel access (const only)
	const TSHBandView<Order, true>& operator[](uint64_t Index) const noexcept
	{
		UBPA_UCOMMON_ASSERT(Index < 3);
		return (&R)[Index];
	}
};
```

### TSHBandViewRGB<Order, false> Specialization (Mutable RGB View)

```cpp
template<int Order>
class TSHBandViewRGB<Order, false>
{
public:
	static constexpr int MaxSHOrder = Order;
	static constexpr int MaxSHBasis = 2 * Order - 1;

	TSHBandView<Order, false> R;
	TSHBandView<Order, false> G;
	TSHBandView<Order, false> B;

	// Constructors
	TSHBandViewRGB() noexcept : R(), G(), B() {}

	TSHBandViewRGB(float* InR, float* InG, float* InB) noexcept
		: R(InR), G(InG), B(InB) {}

	TSHBandViewRGB(
		const TSHBandView<Order, false>& InR,
		const TSHBandView<Order, false>& InG,
		const TSHBandView<Order, false>& InB) noexcept
		: R(InR), G(InG), B(InB) {}

	template<int VectorOrder>
	explicit TSHBandViewRGB(TSHVectorRGB<VectorOrder>& InVectorRGB) noexcept
		: R(InVectorRGB.R.template GetBand<Order>())
		, G(InVectorRGB.G.template GetBand<Order>())
		, B(InVectorRGB.B.template GetBand<Order>())
	{
		static_assert(VectorOrder >= Order, "Vector order must be >= band order");
	}

	// Assignment (shallow copy)
	TSHBandViewRGB& operator=(const TSHBandViewRGB& Other) noexcept
	{
		R = Other.R;
		G = Other.G;
		B = Other.B;
		return *this;
	}

	// Channel access
	TSHBandView<Order, false>& operator[](uint64_t Index) noexcept
	{
		UBPA_UCOMMON_ASSERT(Index < 3);
		return (&R)[Index];
	}

	const TSHBandView<Order, false>& operator[](uint64_t Index) const noexcept
	{
		UBPA_UCOMMON_ASSERT(Index < 3);
		return (&R)[Index];
	}

	// Mathematical operators (in-place) - only in mutable version
	TSHBandViewRGB& operator+=(const TSHBandViewRGB& Other) noexcept;
	TSHBandViewRGB& operator-=(const TSHBandViewRGB& Other) noexcept;
	TSHBandViewRGB& operator*=(float Scalar) noexcept;
	TSHBandViewRGB& operator/=(float Scalar) noexcept;
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
- `TSHBandView<Order, false>` → `TSHBandView<Order, true>`: Implicit conversion allowed via conversion constructor
- `TSHBandView<Order, true>` → `TSHBandView<Order, false>`: NOT allowed (compile error)
- Same for RGB versions: `TSHBandViewRGB<Order, false>` → `TSHBandViewRGB<Order, true>`

### Debug vs Release
- **Debug**: `operator[]` checks `Data != nullptr` and `Index < MaxSHBasis`
- **Release**: No checks, undefined behavior if violated

---

## Migration Guide

### Breaking Changes
- `TSHBandVector<Order>` renamed to `TSHBandView<Order, bConst=false>`
- `TSHBandVectorRGB<Order>` renamed to `TSHBandViewRGB<Order, bConst=false>`
- Template parameter `bConst` added for const control (default `false`)
- Const views: `TSHBandView<Order, true>`, `TSHBandViewRGB<Order, true>`
- CRTP base class `TSHBandViewCommon<Derived, Order, bConst>` for shared functionality
- Views no longer own data (size changes from `sizeof(float) * (2*Order-1)` to `sizeof(float*)`)
- `GetBand()` returns view with appropriate `bConst` instead of copy
- Static factory methods removed
- `operator=` now shallow copy (rebind pointer)
- Non-member `Dot()` simplified from 4 overloads to 1 template function

### Migration Examples

**Before (3.x)**:
```cpp
TSHBandVector<2> band = TSHBandVector<2>::SHBasisFunction(FVector3f(0, 0, 1));
```

**After (4.0 - Mutable View)**:
```cpp
TSHVector<2> vec = TSHVector<2>::SHBasisFunction(FVector3f(0, 0, 1));
TSHBandView<2, false> band = vec.GetBand<2>();  // or just TSHBandView<2>
```

**After (4.0 - Const View)**:
```cpp
const TSHVector<2> vec = TSHVector<2>::SHBasisFunction(FVector3f(0, 0, 1));
TSHBandView<2, true> band = vec.GetBand<2>();  // const version
```

**Simplified Dot() Usage**:
```cpp
// Before: 4 overloads needed
float Dot(const TSHBandView<2>& a, const TSHBandView<2>& b);
float Dot(const TSHBandConstView<2>& a, const TSHBandConstView<2>& b);
float Dot(const TSHBandView<2>& a, const TSHBandConstView<2>& b);
float Dot(const TSHBandConstView<2>& a, const TSHBandView<2>& b);

// After: 1 template handles all
template<bool bConst1, bool bConst2>
float Dot(const TSHBandView<2, bConst1>& a, const TSHBandView<2, bConst2>& b);
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
