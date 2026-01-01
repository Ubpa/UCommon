# Tasks: SHBand View Design

**Input**: Design documents from `/specs/1-shband-view/`
**Prerequisites**: plan.md, spec.md, data-model.md, contracts/
**Branch**: 1-shband-view

**Architecture**: CRTP-based template parameter design with `TSHBandView<Order, bConst>` and `TSHBandViewRGB<Order, bConst>`

**Organization**: Tasks are grouped by functional requirements (FR-1 to FR-8) from spec.md, organized into implementation phases.

## Format: `[ID] [P?] [FR] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[FR]**: Which functional requirement this task implements (e.g., FR-1, FR-2, etc.)
- Include exact file paths in descriptions

## Path Conventions

- **Headers**: `include/UCommon/SH.h`
- **Implementation**: `include/UCommon/SH.inl` (inline implementations)
- **Tests**: `test/SH/` (existing test directory)

---

## Phase 1: Preparation & Analysis

**Purpose**: Understand current implementation and identify all affected code

- [X] T001 Analyze current TSHBandVector implementation in include/UCommon/SH.h (lines 479-497)
- [X] T002 Analyze current TSHBandVectorRGB implementation in include/UCommon/SH.h (lines 500-518)
- [X] T003 [P] Search for all GetBand() call sites in codebase (TSHVector, TSHVectorAC, TSHVectorRGB, TSHVectorACRGB)
- [X] T004 [P] Review existing tests in test/SH/ directory
- [X] T005 Document all files requiring updates in migration checklist

**Checkpoint**: Analysis complete - ready to begin implementation

---

## Phase 2: CRTP Base Class Implementation

**Purpose**: Implement TSHBandViewCommon<Derived, Order, bConst> (FR-2)

**⚠️ CRITICAL**: This phase implements the foundation for all view types

### Remove Old Classes

- [X] T006 Delete TSHBandVector<Order> class definition in include/UCommon/SH.h (lines 479-497)
- [X] T007 Delete TSHBandVectorRGB<Order> class definition in include/UCommon/SH.h (lines 500-518)
- [X] T008 Comment out old type aliases temporarily in include/UCommon/SH.h (lines 738-745)

### Add Forward Declarations

- [X] T009 [FR-2] Add forward declaration: template<int Order, bool bConst = false> class TSHBandView; in include/UCommon/SH.h after line 478
- [X] T010 [FR-2] Add forward declaration: template<int Order, bool bConst = false> class TSHBandViewRGB; in include/UCommon/SH.h

### Implement CRTP Base Class (FR-2)

- [X] T011 [FR-2] Add TSHBandViewCommon<Derived, Order, bConst> class definition in include/UCommon/SH.h
- [X] T012 [FR-2] Add static constexpr members: MaxSHOrder = Order, MaxSHBasis = 2 * Order - 1
- [X] T013 [FR-2] Add type aliases: DataType = std::conditional_t<bConst, const float*, float*>, RGBType = TSHBandViewRGB<Order, bConst>
- [X] T014 [FR-4] Implement const operator[](uint64_t Index) const noexcept with UBPA_UCOMMON_ASSERT in include/UCommon/SH.h
- [X] T015 [FR-4] Implement static constexpr GetSize() noexcept returning MaxSHBasis in include/UCommon/SH.h
- [X] T016 [FR-6] Declare template<bool bOtherConst> float Dot(const TSHBandView<Order, bOtherConst>& Other) const noexcept in include/UCommon/SH.h
- [X] T017 [FR-2] Add protected default constructor: constexpr TSHBandViewCommon() noexcept = default
- [X] T018 [FR-2] Add protected CRTP helpers: const Derived& AsDerived() const noexcept and Derived& AsDerived() noexcept
- [X] T019 [FR-2] Add protected DataType GetData() const noexcept helper that calls AsDerived().GetData()

**Checkpoint**: CRTP base class complete - ready to implement specializations

---

## Phase 3: TSHBandView Partial Specializations

**Purpose**: Implement TSHBandView<Order, false> and TSHBandView<Order, true> (FR-1, FR-3, FR-4, FR-5, FR-6)

### Implement TSHBandView<Order, false> (Mutable Specialization)

- [X] T020 [FR-1] Add partial specialization: template<int Order> class TSHBandView<Order, false> : public TSHBandViewCommon<TSHBandView<Order, false>, Order, false> in include/UCommon/SH.h
- [X] T021 [FR-1] Add using Super = TSHBandViewCommon<TSHBandView<Order, false>, Order, false>; type alias
- [X] T022 [FR-1] Add private member: float* Data;
- [X] T023 [FR-3] Implement default constructor: constexpr TSHBandView() noexcept : Data(nullptr) {}
- [X] T024 [FR-3] Implement pointer constructor: explicit constexpr TSHBandView(float* InData) noexcept : Data(InData) {}
- [X] T025 [FR-5] Implement operator=(const TSHBandView& Other) noexcept with Data = Other.Data (shallow copy)
- [X] T026 [FR-4] Implement mutable operator[](uint64_t Index) noexcept with UBPA_UCOMMON_ASSERT checks
- [X] T027 [FR-4] Add using Super::operator[]; to inherit const version from base class
- [X] T028 [FR-4] Implement float* GetData() noexcept { return Data; }
- [X] T029 [FR-4] Implement const float* GetData() const noexcept { return Data; }
- [X] T030 [FR-6] Declare TSHBandView& operator+=(const TSHBandView& Other) noexcept;
- [X] T031 [FR-6] Declare TSHBandView& operator-=(const TSHBandView& Other) noexcept;
- [X] T032 [FR-6] Declare TSHBandView& operator*=(float Scalar) noexcept;
- [X] T033 [FR-6] Declare TSHBandView& operator/=(float Scalar) noexcept;

### Implement TSHBandView<Order, true> (Const Specialization)

- [X] T034 [P] [FR-1] Add partial specialization: template<int Order> class TSHBandView<Order, true> : public TSHBandViewCommon<TSHBandView<Order, true>, Order, true> in include/UCommon/SH.h
- [X] T035 [P] [FR-1] Add using Super = TSHBandViewCommon<TSHBandView<Order, true>, Order, true>; type alias
- [X] T036 [P] [FR-1] Add private member: const float* Data;
- [X] T037 [P] [FR-3] Implement default constructor: constexpr TSHBandView() noexcept : Data(nullptr) {}
- [X] T038 [P] [FR-3] Implement const pointer constructor: explicit constexpr TSHBandView(const float* InData) noexcept : Data(InData) {}
- [X] T039 [P] [FR-3] Implement conversion constructor: constexpr TSHBandView(const TSHBandView<Order, false>& Other) noexcept : Data(Other.GetData()) {}
- [X] T040 [P] [FR-5] Implement operator=(const TSHBandView& Other) noexcept with Data = Other.Data (shallow copy)
- [X] T041 [P] [FR-4] Implement const float* GetData() const noexcept { return Data; }
- [X] T042 [P] [FR-4] Add using Super::operator[]; to inherit const version from base class

**Checkpoint**: Both TSHBandView specializations complete - can compile and instantiate views

---

## Phase 4: Mathematical Operations Implementation

**Purpose**: Implement in-place operators and Dot functions (FR-6)

### Implement TSHBandView<Order, false> Mathematical Operators in SH.inl

- [X] T043 [FR-6] Implement template<int Order> TSHBandView<Order, false>& TSHBandView<Order, false>::operator+=(const TSHBandView& Other) noexcept in include/UCommon/SH.inl
- [X] T044 [FR-6] Implement template<int Order> TSHBandView<Order, false>& TSHBandView<Order, false>::operator-=(const TSHBandView& Other) noexcept in include/UCommon/SH.inl
- [X] T045 [FR-6] Implement template<int Order> TSHBandView<Order, false>& TSHBandView<Order, false>::operator*=(float Scalar) noexcept in include/UCommon/SH.inl
- [X] T046 [FR-6] Implement template<int Order> TSHBandView<Order, false>& TSHBandView<Order, false>::operator/=(float Scalar) noexcept in include/UCommon/SH.inl

### Implement Dot Functions

- [X] T047 [FR-6] Implement template<int Order, bool bConst1, bool bConst2> float Dot(const TSHBandView<Order, bConst1>& A, const TSHBandView<Order, bConst2>& B) noexcept in include/UCommon/SH.inl
- [X] T048 [FR-6] Implement template<typename Derived, int Order, bool bConst> template<bool bOtherConst> float TSHBandViewCommon<Derived, Order, bConst>::Dot(...) const noexcept in include/UCommon/SH.inl

**Checkpoint**: Mathematical operations complete - views can perform calculations

---

## Phase 5: RGB View Classes Implementation

**Purpose**: Implement TSHBandViewRGB<Order, bConst> (FR-8)

### Implement TSHBandViewRGB<Order, bConst> Primary Template

- [X] T049 [FR-8] Add primary template: template<int Order, bool bConst = false> class TSHBandViewRGB in include/UCommon/SH.h
- [X] T050 [FR-8] Add static constexpr members: MaxSHOrder = Order, MaxSHBasis = 2 * Order - 1
- [X] T051 [FR-8] Add public members: TSHBandView<Order, bConst> R; TSHBandView<Order, bConst> G; TSHBandView<Order, bConst> B;
- [X] T052 [FR-8] Implement default constructor: TSHBandViewRGB() noexcept : R(), G(), B() {}
- [X] T053 [FR-8] Implement pointer constructor: TSHBandViewRGB(std::conditional_t<bConst, const float*, float*> InR, ...) noexcept : R(InR), G(InG), B(InB) {}
- [X] T054 [FR-8] Implement constructor from three views: TSHBandViewRGB(const TSHBandView<Order, bConst>& InR, ...) noexcept
- [X] T055 [FR-8] Implement template constructor from TSHVectorRGB with std::conditional_t for const-correctness
- [X] T056 [FR-8] Implement operator=(const TSHBandViewRGB& Other) noexcept (shallow copy R, G, B)
- [X] T057 [FR-8] Implement mutable operator[](uint64_t Index) noexcept returning TSHBandView<Order, bConst>&
- [X] T058 [FR-8] Implement const operator[](uint64_t Index) const noexcept returning const TSHBandView<Order, bConst>&

### Implement TSHBandViewRGB<Order, false> In-Place Operators (Mutable Only)

- [X] T059 [FR-8] Declare TSHBandViewRGB& operator+=(const TSHBandViewRGB& Other) noexcept; in include/UCommon/SH.h (only for bConst=false)
- [X] T060 [FR-8] Declare TSHBandViewRGB& operator-=(const TSHBandViewRGB& Other) noexcept; in include/UCommon/SH.h (only for bConst=false)
- [X] T061 [FR-8] Declare TSHBandViewRGB& operator*=(float Scalar) noexcept; in include/UCommon/SH.h (only for bConst=false)
- [X] T062 [FR-8] Declare TSHBandViewRGB& operator/=(float Scalar) noexcept; in include/UCommon/SH.h (only for bConst=false)
- [X] T063 [FR-8] Implement operator+= in include/UCommon/SH.inl (R += Other.R; G += Other.G; B += Other.B;)
- [X] T064 [FR-8] Implement operator-= in include/UCommon/SH.inl
- [X] T065 [FR-8] Implement operator*= in include/UCommon/SH.inl
- [X] T066 [FR-8] Implement operator/= in include/UCommon/SH.inl

### Add Implicit Conversion for TSHBandViewRGB

- [X] T067 [FR-8] Add conversion constructor in TSHBandViewRGB<Order, true>: constexpr TSHBandViewRGB(const TSHBandViewRGB<Order, false>& Other) noexcept : R(Other.R), G(Other.G), B(Other.B) {}

**Checkpoint**: RGB view classes complete - can create and manipulate RGB views

---

## Phase 6: Update GetBand() Methods

**Purpose**: Modify all GetBand() methods to return views instead of references (FR-7)

### Update TSHVector<Order>::GetBand() (FR-7)

- [X] T068 [FR-7] Update non-const GetBand() to return TSHBandView<BandOrder, false> in include/UCommon/SH.h (line ~570)
- [X] T069 [FR-7] Update const GetBand() to return TSHBandView<BandOrder, true> in include/UCommon/SH.h (line ~577)
- [X] T070 [FR-7] Change implementation to construct view from pointer: return TSHBandView<BandOrder, false>(&Super::V[IndexBase]);
- [X] T071 [FR-7] Add noexcept specifier to both GetBand() overloads

### Update TSHVectorAC<Order>::GetBand() (FR-7)

- [X] T072 [P] [FR-7] Update non-const GetBand() to return TSHBandView<BandOrder, false> in include/UCommon/SH.h (line ~633)
- [X] T073 [P] [FR-7] Update const GetBand() to return TSHBandView<BandOrder, true> in include/UCommon/SH.h (line ~640)
- [X] T074 [P] [FR-7] Change implementation to construct view from pointer
- [X] T075 [P] [FR-7] Add noexcept specifier to both GetBand() overloads

### Update TSHVectorRGB<Order>::GetBand() (FR-7)

- [X] T076 [FR-7] Add non-const GetBand() returning TSHBandViewRGB<BandOrder, false> in include/UCommon/SH.h (after line 676)
- [X] T077 [FR-7] Update const GetBand() to return TSHBandViewRGB<BandOrder, true> in include/UCommon/SH.h (line ~678)
- [X] T078 [FR-7] Implement using 3-view constructor: return TSHBandViewRGB<BandOrder, false>(Super::R.template GetBand<BandOrder>(), ...)
- [X] T079 [FR-7] Add noexcept specifier to both GetBand() overloads

### Update TSHVectorACRGB<Order>::GetBand() (FR-7)

- [X] T080 [P] [FR-7] Add non-const GetBand() returning TSHBandViewRGB<BandOrder, false> in include/UCommon/SH.h
- [X] T081 [P] [FR-7] Update const GetBand() to return TSHBandViewRGB<BandOrder, true> in include/UCommon/SH.h (line ~724)
- [X] T082 [P] [FR-7] Implement using 3-view constructor
- [X] T083 [P] [FR-7] Add noexcept specifier to both GetBand() overloads

### Update TSHVectorACRGB<Order>::SetBand()

- [X] T084 [FR-7] Update SetBand() parameter type to const TSHBandViewRGB<BandOrder, false>& in include/UCommon/SH.h (line ~730)
- [X] T085 [FR-7] Update SetBand() implementation to use view's GetData() for assignment

**Checkpoint**: All GetBand() methods return views - integration complete

---

## Phase 7: Update Type Aliases and Namespace Macros

**Purpose**: Update type aliases to reflect new naming

### Update Type Aliases in include/UCommon/SH.h

- [X] T086 Update FSHBandVector2 to use TSHBandView<2, false> in include/UCommon/SH.h (line ~738)
- [X] T087 Update FSHBandVector3 to use TSHBandView<3, false> in include/UCommon/SH.h
- [X] T088 Update FSHBandVector4 to use TSHBandView<4, false> in include/UCommon/SH.h
- [X] T089 Update FSHBandVector5 to use TSHBandView<5, false> in include/UCommon/SH.h
- [X] T090 [P] Add using FSHBandConstView2 = TSHBandView<2, true>; in include/UCommon/SH.h
- [X] T091 [P] Add using FSHBandConstView3 = TSHBandView<3, true>; in include/UCommon/SH.h
- [X] T092 [P] Add using FSHBandConstView4 = TSHBandView<4, true>; in include/UCommon/SH.h
- [X] T093 [P] Add using FSHBandConstView5 = TSHBandView<5, true>; in include/UCommon/SH.h
- [X] T094 Update FSHBandVectorRGB2 to use TSHBandViewRGB<2, false> in include/UCommon/SH.h (line ~742)
- [X] T095 Update FSHBandVectorRGB3 to use TSHBandViewRGB<3, false> in include/UCommon/SH.h
- [X] T096 Update FSHBandVectorRGB4 to use TSHBandViewRGB<4, false> in include/UCommon/SH.h
- [X] T097 Update FSHBandVectorRGB5 to use TSHBandViewRGB<5, false> in include/UCommon/SH.h
- [X] T098 [P] Add using FSHBandConstViewRGB2 = TSHBandViewRGB<2, true>; in include/UCommon/SH.h
- [X] T099 [P] Add using FSHBandConstViewRGB3 = TSHBandViewRGB<3, true>; in include/UCommon/SH.h
- [X] T100 [P] Add using FSHBandConstViewRGB4 = TSHBandViewRGB<4, true>; in include/UCommon/SH.h
- [X] T101 [P] Add using FSHBandConstViewRGB5 = TSHBandViewRGB<5, true>; in include/UCommon/SH.h

### Update Namespace Macro in include/UCommon/SH.h

- [X] T102 Update UBPA_UCOMMON_SH_TO_NAMESPACE macro to export TSHBandView instead of TSHBandVector (line ~36)
- [X] T103 Update UBPA_UCOMMON_SH_TO_NAMESPACE macro to export TSHBandViewRGB instead of TSHBandVectorRGB (line ~37)
- [X] T104 [P] Add TSHBandConstView export to UBPA_UCOMMON_SH_TO_NAMESPACE macro
- [X] T105 [P] Add TSHBandConstViewRGB export to UBPA_UCOMMON_SH_TO_NAMESPACE macro

**Checkpoint**: Type aliases updated - public API naming consistent

---

## Phase 8: Compilation and Testing

**Purpose**: Verify implementation compiles and passes basic tests

### Compilation Verification

- [X] T106 Build project with CMake in Debug mode
- [X] T107 Build project with CMake in Release mode
- [X] T108 Verify zero compiler warnings on MSVC
- [ ] T109 [P] Verify zero compiler warnings on Clang (if available)
- [ ] T110 [P] Verify zero compiler warnings on GCC (if available)

### Basic Functionality Tests

- [ ] T111 Create test file test/SH/SHBandView_Basic.cpp for basic view operations
- [ ] T112 Test TSHBandView<2, false> construction from pointer
- [ ] T113 Test TSHBandView<2, true> construction from const pointer
- [ ] T114 Test implicit conversion from TSHBandView<2, false> to TSHBandView<2, true>
- [ ] T115 Test operator[] access (both mutable and const)
- [ ] T116 Test GetData() returns correct pointer
- [ ] T117 Test GetSize() returns correct value (2*Order-1)
- [ ] T118 Test operator+= on TSHBandView<2, false>
- [ ] T119 Test operator-= on TSHBandView<2, false>
- [ ] T120 Test operator*= on TSHBandView<2, false>
- [ ] T121 Test operator/= on TSHBandView<2, false>
- [ ] T122 Test Dot() function with all const combinations
- [ ] T123 Test TSHBandViewRGB<2, false> construction and channel access
- [ ] T124 Test TSHBandViewRGB<2, true> construction and const correctness
- [ ] T125 Test implicit conversion from TSHBandViewRGB<2, false> to TSHBandViewRGB<2, true>

### Integration Tests with Owning Vectors

- [ ] T126 Test TSHVector<3>::GetBand<2>() returns TSHBandView<2, false>
- [ ] T127 Test const TSHVector<3>::GetBand<2>() returns TSHBandView<2, true>
- [ ] T128 Test TSHVectorRGB<3>::GetBand<2>() returns TSHBandViewRGB<2, false>
- [ ] T129 Test const TSHVectorRGB<3>::GetBand<2>() returns TSHBandViewRGB<2, true>
- [ ] T130 Test modifying data through TSHBandView affects owning TSHVector
- [ ] T131 Test modifying data through TSHBandViewRGB affects owning TSHVectorRGB

### Memory Layout Verification

- [ ] T132 Verify sizeof(TSHBandView<2, false>) == sizeof(float*) (8 bytes on 64-bit)
- [ ] T133 Verify sizeof(TSHBandView<2, true>) == sizeof(const float*) (8 bytes on 64-bit)
- [ ] T134 Verify sizeof(TSHBandViewRGB<2, false>) == 3 * sizeof(float*) (24 bytes on 64-bit)
- [ ] T135 Verify sizeof(TSHBandViewRGB<2, true>) == 3 * sizeof(const float*) (24 bytes on 64-bit)

**Checkpoint**: All tests pass - implementation verified

---

## Phase 9: Documentation and Cleanup

**Purpose**: Finalize implementation with documentation

- [ ] T136 Add MIT license header to any new files
- [ ] T137 Add inline comments explaining CRTP pattern in TSHBandViewCommon
- [ ] T138 Add inline comments explaining template partial specialization strategy
- [ ] T139 Document const-correctness guarantees in header comments
- [ ] T140 Update any existing code comments that reference TSHBandVector to TSHBandView
- [ ] T141 Remove temporary comment markers from Phase 2 (T008)
- [ ] T142 Run code formatter (if project has one) on modified files
- [ ] T143 Final compilation check in all configurations

**Checkpoint**: Implementation complete and documented

---

## Summary

**Total Tasks**: 143
**Parallel Opportunities**: ~40 tasks marked [P]
**Critical Path**: Phase 1 → Phase 2 → Phase 3 → Phase 4 → Phase 5 → Phase 6 → Phase 7 → Phase 8 → Phase 9

**Key Dependencies**:
- Phase 2 (CRTP Base) must complete before Phase 3 (Specializations)
- Phase 3 (TSHBandView) must complete before Phase 4 (Math Ops) and Phase 5 (RGB Views)
- Phase 5 (RGB Views) must complete before Phase 6 (GetBand Updates)
- Phase 6 must complete before Phase 8 (Testing)

**Estimated Complexity**:
- Phase 2-3: High (CRTP architecture foundation)
- Phase 4-5: Medium (standard template implementations)
- Phase 6: Medium (integration with existing code)
- Phase 7-9: Low (cleanup and verification)

**MVP Scope** (Minimum Viable Product):
- Phase 1-3: Core view classes (TSHBandView<Order, bConst>)
- Phase 4: Mathematical operations
- Phase 6 (partial): Update TSHVector::GetBand() only
- Phase 8 (partial): Basic compilation and functionality tests

This allows testing the core CRTP architecture before implementing RGB views and full integration.


