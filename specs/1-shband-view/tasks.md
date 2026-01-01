# Tasks: SHBand View Design

**Input**: Design documents from `/specs/1-shband-view/`
**Prerequisites**: plan.md, spec.md
**Branch**: 1-shband-view

**Tests**: Tests are included as this is a library refactoring with existing test infrastructure.

**Organization**: Tasks are grouped by functional requirements (FR-1 to FR-7) from spec.md, organized into implementation phases.

## Format: `[ID] [P?] [FR] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[FR]**: Which functional requirement this task implements (e.g., FR1, FR2, FR3)
- Include exact file paths in descriptions

## Path Conventions

- **Headers**: `include/UCommon/SH.h`
- **Implementation**: `include/UCommon/SH.inl` (inline implementations)
- **Tests**: `test/SH/` (existing test directory)

---

## Phase 1: Preparation & Analysis

**Purpose**: Understand current implementation and identify all affected code

- [ ] T001 Analyze current TSHBandVector implementation in include/UCommon/SH.h (lines 479-497)
- [ ] T002 Analyze current TSHBandVectorRGB implementation in include/UCommon/SH.h (lines 500-518)
- [ ] T003 [P] Search for all GetBand() call sites in codebase
- [ ] T004 [P] Review existing tests in test/SH/ directory
- [ ] T005 Document all files requiring updates in migration checklist

**Checkpoint**: Analysis complete - ready to begin implementation

---

## Phase 2: Core View Classes Implementation

**Purpose**: Implement TSHBandView and TSHBandConstView (FR-1, FR-2, FR-3, FR-4, FR-5)

**⚠️ CRITICAL**: This phase implements the foundation for all view types

### Remove Old Classes

- [ ] T006 Delete TSHBandVector<Order> class definition in include/UCommon/SH.h (lines 479-497)
- [ ] T007 Delete TSHBandVectorRGB<Order> class definition in include/UCommon/SH.h (lines 500-518)
- [ ] T008 Comment out type aliases temporarily in include/UCommon/SH.h (lines 738-745)

### Implement TSHBandView<Order> (FR-1, FR-2, FR-3, FR-4, FR-5)

- [ ] T009 [FR1] Add TSHBandView<Order> class definition in include/UCommon/SH.h after line 478
- [ ] T010 [FR1] Add private member: float* Data
- [ ] T011 [FR1] Add static constexpr members: MaxSHOrder, MaxSHBasis
- [ ] T012 [FR2] Implement default constructor: constexpr TSHBandView() noexcept : Data(nullptr)
- [ ] T013 [FR2] Implement pointer constructor: explicit constexpr TSHBandView(float* InData) noexcept
- [ ] T014 [FR4] Implement operator= (shallow copy) in include/UCommon/SH.h
- [ ] T015 [FR3] Implement operator[] (non-const) with UBPA_UCOMMON_ASSERT checks in include/UCommon/SH.h
- [ ] T016 [FR3] Implement operator[] (const) with UBPA_UCOMMON_ASSERT checks in include/UCommon/SH.h
- [ ] T017 [FR3] Implement GetData() (non-const and const) in include/UCommon/SH.h
- [ ] T018 [FR3] Implement static constexpr GetSize() in include/UCommon/SH.h
- [ ] T019 [FR5] Implement operator+= in include/UCommon/SH.inl
- [ ] T020 [FR5] Implement operator-= in include/UCommon/SH.inl
- [ ] T021 [FR5] Implement operator*= in include/UCommon/SH.inl
- [ ] T022 [FR5] Implement operator/= in include/UCommon/SH.inl
- [ ] T023 [FR5] Implement Dot() member function in include/UCommon/SH.inl

### Implement TSHBandConstView<Order> (FR-1, FR-2, FR-3, FR-5)

- [ ] T024 [P] [FR1] Add TSHBandConstView<Order> class definition in include/UCommon/SH.h
- [ ] T025 [P] [FR1] Add private member: const float* Data
- [ ] T026 [P] [FR1] Add static constexpr members: MaxSHOrder, MaxSHBasis
- [ ] T027 [P] [FR2] Implement default constructor: constexpr TSHBandConstView() noexcept
- [ ] T028 [P] [FR2] Implement const pointer constructor: explicit constexpr TSHBandConstView(const float*)
- [ ] T029 [P] [FR2] Implement implicit conversion constructor from TSHBandView<Order>
- [ ] T030 [P] [FR4] Implement operator= (shallow copy) in include/UCommon/SH.h
- [ ] T031 [P] [FR3] Implement const operator[] with UBPA_UCOMMON_ASSERT checks in include/UCommon/SH.h
- [ ] T032 [P] [FR3] Implement const GetData() in include/UCommon/SH.h
- [ ] T033 [P] [FR3] Implement static constexpr GetSize() in include/UCommon/SH.h
- [ ] T034 [P] [FR5] Implement Dot() member function in include/UCommon/SH.inl

### Implement Non-Member Dot Functions (FR-5)

- [ ] T035 [P] [FR5] Implement Dot(TSHBandView, TSHBandView) in include/UCommon/SH.inl
- [ ] T036 [P] [FR5] Implement Dot(TSHBandConstView, TSHBandConstView) in include/UCommon/SH.inl
- [ ] T037 [P] [FR5] Implement Dot(TSHBandView, TSHBandConstView) in include/UCommon/SH.inl
- [ ] T038 [P] [FR5] Implement Dot(TSHBandConstView, TSHBandView) in include/UCommon/SH.inl

**Checkpoint**: Core view classes complete - can compile and instantiate views

---

## Phase 3: RGB View Classes Implementation

**Purpose**: Implement TSHBandViewRGB and TSHBandConstViewRGB (FR-7)

### Implement TSHBandViewRGB<Order> (FR-7)

- [ ] T039 [FR7] Add TSHBandViewRGB<Order> class definition in include/UCommon/SH.h
- [ ] T040 [FR7] Add public members: TSHBandView<Order> R, G, B
- [ ] T041 [FR7] Add static constexpr members: MaxSHOrder, MaxSHBasis
- [ ] T042 [FR7] Implement default constructor in include/UCommon/SH.h
- [ ] T043 [FR7] Implement 3-pointer constructor in include/UCommon/SH.h
- [ ] T044 [FR7] Implement 3-view constructor in include/UCommon/SH.h
- [ ] T045 [FR7] Implement template constructor from TSHVectorRGB<VectorOrder>& in include/UCommon/SH.h
- [ ] T046 [FR7] Implement operator= (shallow copy all 3 channels) in include/UCommon/SH.h
- [ ] T047 [FR7] Implement operator[] for channel access in include/UCommon/SH.h
- [ ] T048 [FR7] Implement operator+= in include/UCommon/SH.inl
- [ ] T049 [FR7] Implement operator-= in include/UCommon/SH.inl
- [ ] T050 [FR7] Implement operator*= in include/UCommon/SH.inl
- [ ] T051 [FR7] Implement operator/= in include/UCommon/SH.inl

### Implement TSHBandConstViewRGB<Order> (FR-7)

- [ ] T052 [P] [FR7] Add TSHBandConstViewRGB<Order> class definition in include/UCommon/SH.h
- [ ] T053 [P] [FR7] Add public members: TSHBandConstView<Order> R, G, B
- [ ] T054 [P] [FR7] Add static constexpr members: MaxSHOrder, MaxSHBasis
- [ ] T055 [P] [FR7] Implement default constructor in include/UCommon/SH.h
- [ ] T056 [P] [FR7] Implement 3-const-pointer constructor in include/UCommon/SH.h
- [ ] T057 [P] [FR7] Implement 3-const-view constructor in include/UCommon/SH.h
- [ ] T058 [P] [FR7] Implement template constructor from const TSHVectorRGB<VectorOrder>& in include/UCommon/SH.h
- [ ] T059 [P] [FR7] Implement implicit conversion constructor from TSHBandViewRGB in include/UCommon/SH.h
- [ ] T060 [P] [FR7] Implement operator= (shallow copy) in include/UCommon/SH.h
- [ ] T061 [P] [FR7] Implement const operator[] for channel access in include/UCommon/SH.h

**Checkpoint**: All 4 view classes complete - ready to update GetBand() methods

---

## Phase 4: Update GetBand() Methods

**Purpose**: Modify all GetBand() methods to return views instead of references (FR-6)

### Update TSHVector<Order>::GetBand() (FR-6)

- [ ] T062 [FR6] Update non-const GetBand() to return TSHBandView<BandOrder> in include/UCommon/SH.h (line 569)
- [ ] T063 [FR6] Update const GetBand() to return TSHBandConstView<BandOrder> in include/UCommon/SH.h (line 576)
- [ ] T064 [FR6] Add noexcept specifier to both GetBand() overloads in include/UCommon/SH.h

### Update TSHVectorAC<Order>::GetBand() (FR-6)

- [ ] T065 [P] [FR6] Update non-const GetBand() to return TSHBandView<BandOrder> in include/UCommon/SH.h (line 632)
- [ ] T066 [P] [FR6] Update const GetBand() to return TSHBandConstView<BandOrder> in include/UCommon/SH.h (line 639)
- [ ] T067 [P] [FR6] Add noexcept specifier to both GetBand() overloads in include/UCommon/SH.h

### Update TSHVectorRGB<Order>::GetBand() (FR-6)

- [ ] T068 [FR6] Add non-const GetBand() returning TSHBandViewRGB<BandOrder> in include/UCommon/SH.h (after line 676)
- [ ] T069 [FR6] Update const GetBand() to return TSHBandConstViewRGB<BandOrder> in include/UCommon/SH.h (line 677)
- [ ] T070 [FR6] Use 3-view constructor in both GetBand() implementations
- [ ] T071 [FR6] Add noexcept specifier to both GetBand() overloads in include/UCommon/SH.h

### Update TSHVectorACRGB<Order>::GetBand() (FR-6)

- [ ] T072 [P] [FR6] Add non-const GetBand() returning TSHBandViewRGB<BandOrder> in include/UCommon/SH.h
- [ ] T073 [P] [FR6] Add const GetBand() returning TSHBandConstViewRGB<BandOrder> in include/UCommon/SH.h
- [ ] T074 [P] [FR6] Add noexcept specifier to both GetBand() overloads in include/UCommon/SH.h

### Update TSHVectorACRGB<Order>::SetBand()

- [ ] T075 [FR6] Update SetBand() parameter type to TSHBandViewRGB<BandOrder> in include/UCommon/SH.h (line 729)
- [ ] T076 [FR6] Update SetBand() implementation to handle view semantics in include/UCommon/SH.h

**Checkpoint**: All GetBand() methods return views - integration complete

---

## Phase 5: Update Type Aliases

**Purpose**: Update type aliases to reflect new naming

- [ ] T077 Replace FSHBandVector2 with FSHBandView2 in include/UCommon/SH.h (line 738)
- [ ] T078 Replace FSHBandVector3 with FSHBandView3 in include/UCommon/SH.h (line 739)
- [ ] T079 Replace FSHBandVector4 with FSHBandView4 in include/UCommon/SH.h (line 740)
- [ ] T080 Replace FSHBandVector5 with FSHBandView5 in include/UCommon/SH.h (line 741)
- [ ] T081 [P] Add FSHBandConstView2 alias in include/UCommon/SH.h
- [ ] T082 [P] Add FSHBandConstView3 alias in include/UCommon/SH.h
- [ ] T083 [P] Add FSHBandConstView4 alias in include/UCommon/SH.h
- [ ] T084 [P] Add FSHBandConstView5 alias in include/UCommon/SH.h
- [ ] T085 Replace FSHBandVectorRGB2 with FSHBandViewRGB2 in include/UCommon/SH.h (line 742)
- [ ] T086 Replace FSHBandVectorRGB3 with FSHBandViewRGB3 in include/UCommon/SH.h (line 743)
- [ ] T087 Replace FSHBandVectorRGB4 with FSHBandViewRGB4 in include/UCommon/SH.h (line 744)
- [ ] T088 Replace FSHBandVectorRGB5 with FSHBandViewRGB5 in include/UCommon/SH.h (line 745)
- [ ] T089 [P] Add FSHBandConstViewRGB2 alias in include/UCommon/SH.h
- [ ] T090 [P] Add FSHBandConstViewRGB3 alias in include/UCommon/SH.h
- [ ] T091 [P] Add FSHBandConstViewRGB4 alias in include/UCommon/SH.h
- [ ] T092 [P] Add FSHBandConstViewRGB5 alias in include/UCommon/SH.h

**Checkpoint**: Type aliases updated - public API naming consistent

---

## Phase 6: Update Tests

**Purpose**: Fix existing tests and add new tests for view semantics

### Update Existing Tests

- [ ] T093 Find all tests using TSHBandVector in test/SH/ directory
- [ ] T094 Update tests to use TSHVector and extract views via GetBand()
- [ ] T095 Fix tests expecting GetBand() to return reference (now returns by value)
- [ ] T096 Update assignment tests to expect shallow copy semantics
- [ ] T097 Find all tests using TSHBandVectorRGB in test/SH/ directory
- [ ] T098 Update tests to use TSHVectorRGB and extract views via GetBand()

### Add New Tests for View Semantics

- [ ] T099 [P] Add test for TSHBandView default construction (nullptr) in test/SH/
- [ ] T100 [P] Add test for TSHBandView pointer construction in test/SH/
- [ ] T101 [P] Add test for implicit conversion TSHBandView → TSHBandConstView in test/SH/
- [ ] T102 [P] Add test for shallow copy semantics (operator=) in test/SH/
- [ ] T103 [P] Add test for TSHBandView mathematical operators (+=, -=, *=, /=) in test/SH/
- [ ] T104 [P] Add test for Dot() member and non-member functions in test/SH/
- [ ] T105 [P] Add test for TSHBandViewRGB construction from TSHVectorRGB in test/SH/
- [ ] T106 [P] Add test for TSHBandViewRGB channel access via operator[] in test/SH/
- [ ] T107 [P] Add test for TSHBandConstView construction and const correctness in test/SH/
- [ ] T108 [P] Add test for TSHBandConstViewRGB implicit conversion in test/SH/

### Add Size and Performance Tests

- [ ] T109 [P] Add compile-time test: sizeof(TSHBandView<2>) == sizeof(float*) in test/SH/
- [ ] T110 [P] Add compile-time test: sizeof(TSHBandViewRGB<2>) == 3 * sizeof(float*) in test/SH/
- [ ] T111 [P] Add performance benchmark for GetBand() (should be zero-cost) in test/SH/

**Checkpoint**: All tests pass - implementation validated

---

## Phase 7: Documentation & Validation

**Purpose**: Update documentation and validate cross-platform compilation

### Documentation

- [ ] T112 [P] Add Doxygen comments for TSHBandView class in include/UCommon/SH.h
- [ ] T113 [P] Add Doxygen comments for TSHBandConstView class in include/UCommon/SH.h
- [ ] T114 [P] Add Doxygen comments for TSHBandViewRGB class in include/UCommon/SH.h
- [ ] T115 [P] Add Doxygen comments for TSHBandConstViewRGB class in include/UCommon/SH.h
- [ ] T116 [P] Update migration guide examples in specs/1-shband-view/spec.md
- [ ] T117 [P] Document lifetime management requirements in include/UCommon/SH.h
- [ ] T118 [P] Document shallow copy semantics in operator= comments

### Cross-Platform Validation

- [ ] T119 Compile on MSVC 2019+ with zero warnings
- [ ] T120 Compile on Clang 10+ with zero warnings
- [ ] T121 Compile on GCC 9+ with zero warnings
- [ ] T122 Run all tests on Windows (MSVC)
- [ ] T123 Run all tests on Linux (GCC/Clang)
- [ ] T124 Run all tests on macOS (Clang)

### Performance Validation

- [ ] T125 Verify sizeof(TSHBandView<2>) == 8 bytes on 64-bit platforms
- [ ] T126 Verify sizeof(TSHBandView<3>) == 8 bytes on 64-bit platforms
- [ ] T127 Verify sizeof(TSHBandViewRGB<2>) == 24 bytes on 64-bit platforms
- [ ] T128 Run performance benchmarks comparing 3.0.0 vs 4.0.0
- [ ] T129 Verify no performance regression in GetBand() operations

**Checkpoint**: Documentation complete, all platforms validated, performance verified

---

## Phase 8: Polish & Final Review

**Purpose**: Final cleanup and preparation for merge

- [ ] T130 [P] Code review: Check all classes follow Allman brace style
- [ ] T131 [P] Code review: Verify tab indentation throughout
- [ ] T132 [P] Code review: Ensure MIT license headers present
- [ ] T133 [P] Code review: Verify all public methods have Doxygen comments
- [ ] T134 Run static analysis tools on modified files
- [ ] T135 Update CHANGELOG.md with breaking changes for 4.0.0
- [ ] T136 Update version number in CMakeLists.txt (3.0.0 → 4.0.0)
- [ ] T137 Final compilation check: zero warnings on all compilers
- [ ] T138 Final test run: all tests pass on all platforms
- [ ] T139 Create migration guide document for users upgrading from 3.x
- [ ] T140 Prepare PR description with summary of changes

**Checkpoint**: Ready for code review and merge

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Preparation)**: No dependencies - can start immediately
- **Phase 2 (Core View Classes)**: Depends on Phase 1 completion - BLOCKS all subsequent phases
- **Phase 3 (RGB View Classes)**: Depends on Phase 2 completion (needs TSHBandView and TSHBandConstView)
- **Phase 4 (Update GetBand)**: Depends on Phase 3 completion (needs all 4 view classes)
- **Phase 5 (Type Aliases)**: Depends on Phase 4 completion (needs all classes finalized)
- **Phase 6 (Tests)**: Depends on Phase 5 completion (needs complete implementation)
- **Phase 7 (Documentation)**: Can start after Phase 5, parallel with Phase 6
- **Phase 8 (Polish)**: Depends on Phases 6 and 7 completion

### Critical Path

```
Phase 1 → Phase 2 → Phase 3 → Phase 4 → Phase 5 → Phase 6 → Phase 8
                                              ↓
                                         Phase 7 ────────────┘
```

### Functional Requirement Dependencies

- **FR-1 (View-Based Storage)**: Foundation for all other requirements
- **FR-2 (Construction)**: Depends on FR-1
- **FR-3 (Array Access)**: Depends on FR-1
- **FR-4 (Assignment)**: Depends on FR-1
- **FR-5 (Mathematical Operations)**: Depends on FR-1, FR-3
- **FR-6 (GetBand Returns Views)**: Depends on FR-1, FR-2 (all view classes must exist)
- **FR-7 (RGB View Support)**: Depends on FR-1, FR-2 (needs TSHBandView and TSHBandConstView)

### Parallel Opportunities

#### Within Phase 2 (Core View Classes)
- Tasks T024-T034 (TSHBandConstView) can run in parallel with T009-T023 (TSHBandView)
- Tasks T035-T038 (Non-member Dot functions) can run in parallel after both classes exist

#### Within Phase 3 (RGB View Classes)
- Tasks T052-T061 (TSHBandConstViewRGB) can run in parallel with T039-T051 (TSHBandViewRGB)

#### Within Phase 4 (Update GetBand)
- TSHVectorAC updates (T065-T067) can run in parallel with TSHVector updates (T062-T064)
- TSHVectorACRGB updates (T072-T074) can run in parallel with TSHVectorRGB updates (T068-T071)

#### Within Phase 5 (Type Aliases)
- All const view aliases (T081-T084, T089-T092) can run in parallel

#### Within Phase 6 (Tests)
- All new test tasks (T099-T111) can run in parallel after existing tests are updated

#### Within Phase 7 (Documentation)
- All documentation tasks (T112-T118) can run in parallel
- All validation tasks (T119-T129) can run in parallel after implementation is complete

#### Within Phase 8 (Polish)
- Code review tasks (T130-T133) can run in parallel

---

## Parallel Example: Phase 2 (Core View Classes)

```bash
# Parallel Group 1: Implement both view classes simultaneously
Task T009-T023: "Implement TSHBandView<Order>"
Task T024-T034: "Implement TSHBandConstView<Order>"

# Parallel Group 2: After both classes exist
Task T035: "Implement Dot(TSHBandView, TSHBandView)"
Task T036: "Implement Dot(TSHBandConstView, TSHBandConstView)"
Task T037: "Implement Dot(TSHBandView, TSHBandConstView)"
Task T038: "Implement Dot(TSHBandConstView, TSHBandView)"
```

---

## Parallel Example: Phase 6 (New Tests)

```bash
# All new tests can run in parallel:
Task T099: "Test TSHBandView default construction"
Task T100: "Test TSHBandView pointer construction"
Task T101: "Test implicit conversion"
Task T102: "Test shallow copy semantics"
Task T103: "Test mathematical operators"
Task T104: "Test Dot() functions"
Task T105: "Test TSHBandViewRGB construction"
Task T106: "Test channel access"
Task T107: "Test TSHBandConstView"
Task T108: "Test TSHBandConstViewRGB"
Task T109: "Test sizeof(TSHBandView)"
Task T110: "Test sizeof(TSHBandViewRGB)"
Task T111: "Benchmark GetBand()"
```

---

## Implementation Strategy

### Sequential Approach (Single Developer)

1. **Week 1, Day 1-2**: Complete Phase 1 (Preparation) and Phase 2 (Core View Classes)
2. **Week 1, Day 3**: Complete Phase 3 (RGB View Classes)
3. **Week 1, Day 4**: Complete Phase 4 (Update GetBand Methods)
4. **Week 1, Day 5**: Complete Phase 5 (Type Aliases)
5. **Week 2, Day 1-2**: Complete Phase 6 (Update Tests)
6. **Week 2, Day 3**: Complete Phase 7 (Documentation & Validation)
7. **Week 2, Day 4**: Complete Phase 8 (Polish & Final Review)

**Total Estimated Time**: ~9 working days

### Parallel Approach (Multiple Developers)

**Developer A** (Core Implementation):
1. Phase 1: Preparation (0.5 days)
2. Phase 2: TSHBandView implementation (T009-T023)
3. Phase 4: Update TSHVector and TSHVectorAC GetBand() methods
4. Phase 6: Update existing tests

**Developer B** (Const Implementation):
1. Phase 1: Preparation (0.5 days)
2. Phase 2: TSHBandConstView implementation (T024-T034)
3. Phase 4: Update TSHVectorRGB and TSHVectorACRGB GetBand() methods
4. Phase 6: Add new tests for const views

**Developer C** (RGB Implementation):
1. Wait for Phase 2 completion
2. Phase 3: TSHBandViewRGB and TSHBandConstViewRGB implementation
3. Phase 5: Update type aliases
4. Phase 6: Add new tests for RGB views

**Developer D** (Documentation & Validation):
1. Wait for Phase 5 completion
2. Phase 7: Documentation
3. Phase 7: Cross-platform validation
4. Phase 8: Final review and polish

**Total Estimated Time**: ~5 working days (with 4 developers)

### Incremental Delivery Milestones

**Milestone 1**: Core View Classes Complete (After Phase 2)
- Can instantiate TSHBandView and TSHBandConstView
- Basic operations work (construction, array access, mathematical operators)
- **Deliverable**: Proof of concept for view pattern

**Milestone 2**: RGB Views Complete (After Phase 3)
- All 4 view classes implemented
- Can handle both single-band and RGB data
- **Deliverable**: Complete view class hierarchy

**Milestone 3**: Integration Complete (After Phase 4)
- GetBand() methods return views
- Existing owning classes work with new view classes
- **Deliverable**: Functional integration with existing codebase

**Milestone 4**: API Finalized (After Phase 5)
- Type aliases updated
- Public API naming consistent
- **Deliverable**: Ready for external testing

**Milestone 5**: Validated Implementation (After Phase 6)
- All tests pass
- View semantics verified
- **Deliverable**: Production-ready code

**Milestone 6**: Production Ready (After Phase 8)
- Documentation complete
- Cross-platform validated
- Performance verified
- **Deliverable**: Ready to merge and release as 4.0.0

---

## Task Summary

### Total Tasks: 140

### Tasks by Phase:
- **Phase 1 (Preparation)**: 5 tasks
- **Phase 2 (Core View Classes)**: 33 tasks
- **Phase 3 (RGB View Classes)**: 23 tasks
- **Phase 4 (Update GetBand)**: 15 tasks
- **Phase 5 (Type Aliases)**: 16 tasks
- **Phase 6 (Update Tests)**: 19 tasks
- **Phase 7 (Documentation & Validation)**: 18 tasks
- **Phase 8 (Polish & Final Review)**: 11 tasks

### Tasks by Functional Requirement:
- **FR-1 (View-Based Storage)**: 8 tasks
- **FR-2 (Construction)**: 8 tasks
- **FR-3 (Array Access)**: 8 tasks
- **FR-4 (Assignment)**: 4 tasks
- **FR-5 (Mathematical Operations)**: 12 tasks
- **FR-6 (GetBand Returns Views)**: 15 tasks
- **FR-7 (RGB View Support)**: 23 tasks
- **Other (Tests, Documentation, Polish)**: 62 tasks

### Parallelizable Tasks: 68 (48.6%)

### Critical Path Tasks: 72 (51.4%)

---

## Notes

- **[P] marker**: 68 tasks can run in parallel with other tasks in the same phase
- **[FR] label**: Maps task to specific functional requirement for traceability
- **File paths**: All tasks include exact file paths for implementation
- **Checkpoints**: 8 checkpoints to validate progress at each phase
- **Breaking changes**: This is a MAJOR version bump (3.0.0 → 4.0.0) with no backward compatibility
- **Testing**: Comprehensive test coverage including unit tests, size tests, and performance benchmarks
- **Documentation**: Doxygen comments for all public APIs, migration guide for users
- **Validation**: Cross-platform compilation and testing on MSVC, Clang, and GCC

---

## Success Criteria

- [ ] All 140 tasks completed
- [ ] All 4 view classes implemented (TSHBandView, TSHBandConstView, TSHBandViewRGB, TSHBandConstViewRGB)
- [ ] sizeof(TSHBandView<N>) == 8 bytes on 64-bit platforms
- [ ] sizeof(TSHBandViewRGB<N>) == 24 bytes on 64-bit platforms
- [ ] All GetBand() methods return views (not references)
- [ ] All existing tests pass after migration
- [ ] All new tests pass (view semantics, size, performance)
- [ ] Zero compiler warnings on MSVC 2019+, Clang 10+, GCC 9+
- [ ] No performance regression vs 3.0.0
- [ ] Documentation complete (Doxygen comments, migration guide)
- [ ] Ready to merge to main branch and release as 4.0.0

