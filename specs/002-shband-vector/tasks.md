# Tasks: TSHBandVector - Owning SH Band Storage

**Input**: Design documents from `/specs/002-shband-vector/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

**Tests**: Tests are included as this is a library component requiring comprehensive unit testing.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

This is a header-only C++ library. All implementation goes into existing files:
- **Header**: `include/UCommon/SH.h`
- **Inline implementation**: `include/UCommon/SH.inl`
- **Tests**: `src/tests/test_SH.cpp` (or new file if needed)

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Prepare development environment and verify existing codebase structure

- [X] T001 Review existing TSHBandView implementation in include/UCommon/SH.h to understand CRTP pattern and design conventions
- [X] T002 Review existing TSHBandView inline implementations in include/UCommon/SH.inl to understand implementation patterns
- [X] T003 [P] Review existing test structure in src/tests/ to determine test file location (test_SH.cpp or new file)

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [X] T004 Add TSHBandVector<Order> class template declaration in include/UCommon/SH.h (empty class with template parameter, constants MaxSHOrder and MaxSHBasis, private Data[2*Order-1] member)
- [X] T005 Add TSHBandVectorRGB<Order> class template declaration in include/UCommon/SH.h (empty class with public R, G, B members of type TSHBandVector<Order>)
- [X] T006 Update UBPA_UCOMMON_SH_TO_NAMESPACE macro in include/UCommon/SH.h to export TSHBandVector and TSHBandVectorRGB

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Store and Manipulate SH Band Coefficients (Priority: P1) 🎯 MVP

**Goal**: Provide automatic memory management for SH band coefficients with mathematical operations that produce new results without manual memory allocation

**Independent Test**: Create TSHBandVector instances, perform arithmetic operations (addition, subtraction, multiplication), verify results match expected mathematical outcomes

### Tests for User Story 1

> **NOTE: Write these tests FIRST, ensure they FAIL before implementation**

- [X] T007 [P] [US1] Add test for TSHBandVector default construction (zero-initialization) in src/tests/02_SH/main.cpp
- [X] T008 [P] [US1] Add test for TSHBandVector initializer list construction in src/tests/02_SH/main.cpp
- [X] T009 [P] [US1] Add test for TSHBandVector copy construction and copy assignment in src/tests/02_SH/main.cpp
- [X] T010 [P] [US1] Add test for TSHBandVector move construction and move assignment in src/tests/02_SH/main.cpp
- [X] T011 [P] [US1] Add test for TSHBandVector element access (operator[]) in src/tests/02_SH/main.cpp
- [X] T012 [P] [US1] Add test for TSHBandVector binary arithmetic operators (operator+, operator-, operator*, operator/) in src/tests/02_SH/main.cpp
- [X] T013 [P] [US1] Add test for TSHBandVector compound assignment operators (operator+=, operator-=, operator*=, operator/=) in src/tests/02_SH/main.cpp
- [X] T014 [P] [US1] Add test for TSHBandVector Dot() function in src/tests/02_SH/main.cpp

### Implementation for User Story 1

- [X] T015 [US1] Implement TSHBandVector default constructor in include/UCommon/SH.inl (zero-initialize Data array)
- [X] T016 [US1] Implement TSHBandVector initializer list constructor in include/UCommon/SH.inl (copy elements with UBPA_UCOMMON_ASSERT for size validation)
- [X] T017 [US1] Implement TSHBandVector copy constructor and copy assignment in include/UCommon/SH.inl (deep copy Data array)
- [X] T018 [US1] Implement TSHBandVector move constructor and move assignment in include/UCommon/SH.inl (copy Data array, mark as noexcept)
- [X] T019 [US1] Implement TSHBandVector operator[] (const and non-const versions) in include/UCommon/SH.inl with bounds checking via UBPA_UCOMMON_ASSERT
- [X] T020 [US1] Implement TSHBandVector GetData() method (const and non-const versions) in include/UCommon/SH.inl
- [X] T021 [US1] Implement TSHBandVector GetSize() method in include/UCommon/SH.inl (return MaxSHBasis)
- [X] T022 [US1] Implement TSHBandVector compound assignment operators (operator+=, operator-=, operator*=, operator/=) in include/UCommon/SH.inl
- [X] T023 [US1] Implement TSHBandVector Dot() member function in include/UCommon/SH.inl
- [X] T024 [US1] Add non-member binary arithmetic operators for TSHBandView (operator+, operator-, operator/, operator* for view*scalar, operator* for scalar*view) in include/UCommon/SH.h that return TSHBandVector<Order>
- [X] T025 [US1] Implement non-member binary arithmetic operators for TSHBandView in include/UCommon/SH.inl (element-wise operations returning TSHBandVector instances)
- [X] T026 [US1] Add non-member Dot() function overload accepting TSHBandVector arguments in include/UCommon/SH.h
- [X] T027 [US1] Implement non-member Dot() function in include/UCommon/SH.inl
- [X] T028 [US1] Run all User Story 1 tests and verify they pass

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently

---

## Phase 4: User Story 2 - Interoperate with TSHBandView (Priority: P2)

**Goal**: Enable seamless conversion between owning TSHBandVector and non-owning TSHBandView for flexible data access patterns

**Independent Test**: Create TSHBandVector instances, obtain views via implicit conversion, verify modifications through views affect underlying vector data

### Tests for User Story 2

> **NOTE: Write these tests FIRST, ensure they FAIL before implementation**

- [X] T029 [P] [US2] Add test for TSHBandVector implicit conversion to TSHBandView<Order, false> (mutable) in src/tests/02_SH/main.cpp
- [X] T030 [P] [US2] Add test for TSHBandVector implicit conversion to TSHBandView<Order, true> (const) in src/tests/02_SH/main.cpp
- [X] T031 [P] [US2] Add test for TSHBandVector construction from TSHBandView (deep copy) in src/tests/02_SH/main.cpp
- [X] T032 [P] [US2] Add test for TSHBandVector passing to function expecting TSHBandView parameter in src/tests/02_SH/main.cpp
- [X] T033 [P] [US2] Add test for TSHBandView binary operators working with TSHBandVector operands (via implicit conversion) in src/tests/02_SH/main.cpp

### Implementation for User Story 2

- [X] T034 [US2] Add TSHBandVector constructor from TSHBandView declaration in include/UCommon/SH.h (template<bool bConst> explicit TSHBandVector(const TSHBandView<Order, bConst>&))
- [X] T035 [US2] Implement TSHBandVector constructor from TSHBandView in include/UCommon/SH.inl (deep copy with null pointer assertion)
- [X] T036 [US2] Add TSHBandVector implicit conversion operator to TSHBandView<Order, false> declaration in include/UCommon/SH.h
- [X] T037 [US2] Implement TSHBandVector implicit conversion to TSHBandView<Order, false> in include/UCommon/SH.inl (return view of Data array)
- [X] T038 [US2] Add TSHBandVector implicit conversion operator to TSHBandView<Order, true> (const version) declaration in include/UCommon/SH.h
- [X] T039 [US2] Implement TSHBandVector implicit conversion to TSHBandView<Order, true> in include/UCommon/SH.inl (return const view of Data array)
- [X] T040 [US2] Run all User Story 2 tests and verify they pass
- [X] T041 [US2] Verify User Story 1 tests still pass (regression check)

**Checkpoint**: At this point, User Stories 1 AND 2 should both work independently

---

## Phase 5: User Story 3 - RGB Channel Operations (Priority: P3)

**Goal**: Enable working with RGB color data stored as three separate SH band coefficient sets with coordinated operations

**Independent Test**: Create TSHBandVectorRGB instances, perform channel-wise operations, verify all three channels are correctly processed

### Tests for User Story 3

> **NOTE: Write these tests FIRST, ensure they FAIL before implementation**

- [X] T042 [P] [US3] Add test for TSHBandVectorRGB default construction (all channels zero-initialized) in src/tests/02_SH/main.cpp
- [X] T043 [P] [US3] Add test for TSHBandVectorRGB construction from three TSHBandVector instances in src/tests/02_SH/main.cpp
- [X] T044 [P] [US3] Add test for TSHBandVectorRGB copy construction and copy assignment in src/tests/02_SH/main.cpp
- [X] T045 [P] [US3] Add test for TSHBandVectorRGB move construction and move assignment in src/tests/02_SH/main.cpp
- [X] T046 [P] [US3] Add test for TSHBandVectorRGB channel access (R, G, B members) in src/tests/02_SH/main.cpp
- [X] T047 [P] [US3] Add test for TSHBandVectorRGB binary arithmetic operators (channel-wise operations) in src/tests/02_SH/main.cpp
- [X] T048 [P] [US3] Add test for TSHBandVectorRGB compound assignment operators in src/tests/02_SH/main.cpp
- [X] T049 [P] [US3] Add test for TSHBandVectorRGB implicit conversion to TSHBandViewRGB in src/tests/02_SH/main.cpp
- [X] T050 [P] [US3] Add test for TSHBandVectorRGB construction from TSHBandViewRGB (deep copy) in src/tests/02_SH/main.cpp

### Implementation for User Story 3

- [X] T051 [US3] Implement TSHBandVectorRGB default constructor in include/UCommon/SH.inl (default-construct R, G, B members)
- [X] T052 [US3] Add TSHBandVectorRGB constructor from three TSHBandVector instances declaration in include/UCommon/SH.h
- [X] T053 [US3] Implement TSHBandVectorRGB constructor from three TSHBandVector instances in include/UCommon/SH.inl
- [X] T054 [US3] Implement TSHBandVectorRGB copy constructor and copy assignment in include/UCommon/SH.inl (deep copy all channels)
- [X] T055 [US3] Implement TSHBandVectorRGB move constructor and move assignment in include/UCommon/SH.inl
- [X] T056 [US3] Add TSHBandVectorRGB binary arithmetic operators declarations in include/UCommon/SH.h (operator+, operator-, operator*, operator/)
- [X] T057 [US3] Implement TSHBandVectorRGB binary arithmetic operators in include/UCommon/SH.inl (channel-wise operations)
- [X] T058 [US3] Add TSHBandVectorRGB compound assignment operators declarations in include/UCommon/SH.h (operator+=, operator-=, operator*=, operator/=)
- [X] T059 [US3] Implement TSHBandVectorRGB compound assignment operators in include/UCommon/SH.inl (channel-wise in-place operations)
- [X] T060 [US3] Add TSHBandVectorRGB implicit conversion operators to TSHBandViewRGB declarations in include/UCommon/SH.h (mutable and const versions)
- [X] T061 [US3] Implement TSHBandVectorRGB implicit conversion operators in include/UCommon/SH.inl
- [X] T062 [US3] Add TSHBandVectorRGB constructor from TSHBandViewRGB declaration in include/UCommon/SH.h
- [X] T063 [US3] Implement TSHBandVectorRGB constructor from TSHBandViewRGB in include/UCommon/SH.inl (deep copy all channels)
- [X] T064 [US3] Run all User Story 3 tests and verify they pass
- [X] T065 [US3] Verify User Stories 1 and 2 tests still pass (regression check)

**Checkpoint**: All user stories should now be independently functional

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories

- [ ] T066 [P] Add Doxygen documentation comments for TSHBandVector class in include/UCommon/SH.h
- [ ] T067 [P] Add Doxygen documentation comments for TSHBandVectorRGB class in include/UCommon/SH.h
- [ ] T068 [P] Add Doxygen documentation comments for new TSHBandView binary operators in include/UCommon/SH.h
- [X] T069 [P] Review all implementations for const-correctness and noexcept specifications
- [X] T070 [P] Review all implementations for proper UBPA_UCOMMON_ASSERT usage
- [X] T071 Run full test suite to verify all tests pass
- [ ] T072 Compile with multiple compilers (MSVC, Clang, GCC) to verify cross-platform compatibility
- [ ] T073 Run memory sanitizer to verify no memory leaks
- [ ] T074 Validate quickstart.md examples compile and run correctly
- [ ] T075 Code review and cleanup (formatting, naming conventions, Allman braces, tabs)

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3-5)**: All depend on Foundational phase completion
  - User stories can then proceed in parallel (if staffed)
  - Or sequentially in priority order (P1 → P2 → P3)
- **Polish (Phase 6)**: Depends on all desired user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories
- **User Story 2 (P2)**: Can start after Foundational (Phase 2) - Requires User Story 1 implementation (depends on TSHBandVector class existing)
- **User Story 3 (P3)**: Can start after Foundational (Phase 2) - Requires User Story 1 implementation (depends on TSHBandVector class existing)

### Within Each User Story

- Tests MUST be written and FAIL before implementation
- Constructors before operators
- Member functions before non-member functions
- Core implementation before integration
- Story complete before moving to next priority

### Parallel Opportunities

- All Setup tasks marked [P] can run in parallel (T001, T002, T003)
- All tests for a user story marked [P] can run in parallel (write all tests together)
- User Story 2 and User Story 3 can be worked on in parallel after User Story 1 completes (both depend on TSHBandVector existing)
- All Polish tasks marked [P] can run in parallel (T066-T070)

---

## Parallel Example: User Story 1

```bash
# Launch all tests for User Story 1 together:
Task: "Add test for TSHBandVector default construction in src/tests/test_SH.cpp"
Task: "Add test for TSHBandVector initializer list construction in src/tests/test_SH.cpp"
Task: "Add test for TSHBandVector copy construction and copy assignment in src/tests/test_SH.cpp"
Task: "Add test for TSHBandVector move construction and move assignment in src/tests/test_SH.cpp"
Task: "Add test for TSHBandVector element access in src/tests/test_SH.cpp"
Task: "Add test for TSHBandVector binary arithmetic operators in src/tests/test_SH.cpp"
Task: "Add test for TSHBandVector compound assignment operators in src/tests/test_SH.cpp"
Task: "Add test for TSHBandVector Dot() function in src/tests/test_SH.cpp"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup (T001-T003)
2. Complete Phase 2: Foundational (T004-T006) - CRITICAL - blocks all stories
3. Complete Phase 3: User Story 1 (T007-T028)
4. **STOP and VALIDATE**: Run all User Story 1 tests independently
5. Ready for use in production (basic TSHBandVector functionality)

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready
2. Add User Story 1 → Test independently → **MVP Ready** (basic storage and arithmetic)
3. Add User Story 2 → Test independently → **Enhanced** (view interoperability)
4. Add User Story 3 → Test independently → **Complete** (RGB support)
5. Add Polish → **Production Ready** (documentation, cross-platform validation)

### Parallel Team Strategy

With multiple developers:

1. Team completes Setup + Foundational together (T001-T006)
2. Once Foundational is done:
   - Developer A: User Story 1 (T007-T028) - MUST complete first
3. After User Story 1 completes:
   - Developer B: User Story 2 (T029-T041)
   - Developer C: User Story 3 (T042-T065)
4. Stories 2 and 3 can proceed in parallel

---

## Notes

- [P] tasks = different files or independent sections, no dependencies
- [Story] label maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Verify tests fail before implementing (TDD approach)
- Commit after each task or logical group
- Stop at any checkpoint to validate story independently
- All code must follow UCommon constitution (Allman braces, tabs, PascalCase, etc.)
- Use UBPA_UCOMMON_ASSERT for runtime checks, static_assert for compile-time checks
- Mark constructors as explicit where appropriate
- Use constexpr for compile-time constants
- Use noexcept for non-throwing functions

