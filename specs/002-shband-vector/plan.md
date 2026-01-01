# Implementation Plan: TSHBandVector - Owning SH Band Storage

**Branch**: `002-shband-vector` | **Date**: 2026-01-01 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/002-shband-vector/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Add TSHBandVector<Order> and TSHBandVectorRGB<Order> as owning containers for spherical harmonic band coefficients, complementing the existing TSHBandView (non-owning view). TSHBandVector uses fixed-size array member (stack-allocated) for 2*Order-1 float coefficients, supports value semantics (copy/move), and provides binary arithmetic operators. Extend TSHBandView with binary operators that return TSHBandVector instances, eliminating the need for manual memory allocation in arithmetic operations.

## Technical Context

**Language/Version**: C++17
**Primary Dependencies**: None (header-only library component)
**Storage**: Stack-allocated fixed-size arrays (float Data[2*Order-1])
**Testing**: Existing UCommon test framework (src/tests/)
**Target Platform**: Cross-platform (Windows/Linux/macOS, MSVC/Clang/GCC)
**Project Type**: Single project (C++ header-only library)
**Performance Goals**: Zero-overhead abstraction (no runtime cost vs. manual array management)
**Constraints**:
  - Must maintain binary compatibility with existing TSHBandView
  - Stack allocation only (no heap allocation)
  - Compile-time size determination (template parameter Order)
  - Must support implicit conversion to TSHBandView for API compatibility
**Scale/Scope**:
  - 2 new class templates (TSHBandVector, TSHBandVectorRGB)
  - Extension of existing TSHBandView with ~8 binary operator overloads
  - ~500-800 lines of code (header + inline implementation)

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### Principle 1: Unified Naming Conventions
- ✅ **PASS**: TSHBandVector uses `T` prefix for template class
- ✅ **PASS**: TSHBandVectorRGB uses `T` prefix for template class
- ✅ **PASS**: Member variables use PascalCase (R, G, B, Data)
- ✅ **PASS**: Methods use PascalCase (GetData, Dot, operator overloads)
- ✅ **PASS**: Template parameters use PascalCase (Order)

### Principle 2: Strict Code Formatting Standards
- ✅ **PASS**: Will use Allman brace style (braces on their own line)
- ✅ **PASS**: Will use tabs for indentation
- ✅ **PASS**: Will add MIT license header to any new source files (header-only, no new files)
- ✅ **PASS**: Will follow spacing conventions (spaces around operators, after commas)

### Principle 3: Type Safety and Explicitness
- ✅ **PASS**: Will mark constructors as `explicit` where appropriate (single-parameter constructors)
- ✅ **PASS**: Will use `constexpr` for compile-time constants (MaxSHOrder, MaxSHBasis)
- ✅ **PASS**: Will use `noexcept` for non-throwing functions
- ✅ **PASS**: Will use `static_assert` for compile-time checks (initializer list size validation)
- ✅ **PASS**: Will use `const` and `const&` appropriately

### Principle 4: Resource Management and Ownership Clarity
- ✅ **PASS**: TSHBandVector owns storage (stack-allocated array)
- ✅ **PASS**: Will implement complete set of constructors (default, copy, move, initializer list)
- ✅ **PASS**: Will implement copy/move assignment operators
- ✅ **PASS**: No dynamic memory allocation (stack-only, no need for UBPA_UCOMMON_MALLOC/FREE)
- ✅ **PASS**: Clear ownership semantics (Vector owns, View does not)

### Principle 5: Modularity and Namespace Organization
- ✅ **PASS**: All code resides within `UCommon` namespace
- ✅ **PASS**: Will update `UBPA_UCOMMON_SH_TO_NAMESPACE` macro to export new types
- ✅ **PASS**: Will use `#pragma once` (already in SH.h)
- ✅ **PASS**: Will add to existing `include/UCommon/SH.h` header
- ✅ **PASS**: Will add inline implementations to existing `include/UCommon/SH.inl`

### Principle 6: Cross-Platform Compatibility
- ✅ **PASS**: Uses standard C++17 features only
- ✅ **PASS**: No compiler-specific extensions
- ✅ **PASS**: Header-only (no build system changes needed)
- ✅ **PASS**: Compatible with MSVC, Clang, GCC

### Principle 7: Performance First
- ✅ **PASS**: Stack allocation (no heap overhead)
- ✅ **PASS**: Will use inline implementations in SH.inl
- ✅ **PASS**: Will use `constexpr` for compile-time computation
- ✅ **PASS**: Will use move semantics to avoid unnecessary copies
- ✅ **PASS**: Template metaprogramming for zero-overhead abstraction

### Principle 8: Comprehensive Error Handling
- ✅ **PASS**: Will use `UBPA_UCOMMON_ASSERT` for precondition checks (null pointer, index bounds)
- ✅ **PASS**: Will use `static_assert` for compile-time validation (initializer list size)
- ✅ **PASS**: Will document preconditions and postconditions
- ✅ **PASS**: No exceptions (consistent with UCommon design)

### Principle 9: Documentation and Testing
- ✅ **PASS**: Will provide Doxygen-style documentation comments
- ✅ **PASS**: Will add unit tests to src/tests/
- ✅ **PASS**: Will provide usage examples in documentation
- ⚠️ **DEFER**: Example code in src/examples/ (can be added post-implementation if needed)

### Principle 10: Version Management and Backward Compatibility
- ✅ **PASS**: No breaking changes to existing APIs
- ✅ **PASS**: TSHBandView extension is additive only (new operators)
- ✅ **PASS**: Maintains binary compatibility with existing code
- ✅ **PASS**: No changes to existing serialization formats

**Overall Status**: ✅ **PASS** - All critical principles satisfied, one optional item deferred

---

### Post-Phase 1 Re-evaluation

After completing Phase 1 design (data-model.md, contracts/, quickstart.md), all constitution principles remain satisfied:

- ✅ **Naming conventions**: All entities follow UCommon naming standards (TSHBandVector, TSHBandVectorRGB)
- ✅ **API contracts**: Fully specified in YAML format with preconditions, postconditions, complexity
- ✅ **Documentation**: Comprehensive quickstart guide and data model documentation created
- ✅ **Type safety**: All contracts specify explicit types, const-correctness, and validation rules
- ✅ **Performance**: Zero-overhead abstraction confirmed in design (stack allocation, inline implementations)
- ✅ **Compatibility**: No breaking changes, additive only (new classes + new operators for existing classes)

**Re-evaluation Result**: ✅ **PASS** - Design phase complete, ready for Phase 2 (task breakdown)

## Project Structure

### Documentation (this feature)

```text
specs/[###-feature]/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
include/UCommon/
├── SH.h                 # Main header - MODIFY to add TSHBandVector, TSHBandVectorRGB
│                        # MODIFY: Add binary operators to TSHBandView
│                        # MODIFY: Update UBPA_UCOMMON_SH_TO_NAMESPACE macro
└── SH.inl               # Inline implementations - MODIFY to add template implementations

src/tests/
└── test_SH.cpp          # Unit tests - ADD tests for TSHBandVector functionality
                         # (or create new test file if needed)
```

**Structure Decision**: This is a header-only library component. All implementation will be added to the existing `include/UCommon/SH.h` header file with inline implementations in `include/UCommon/SH.inl`. No new files are required. Unit tests will be added to the existing test suite in `src/tests/`.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

No violations detected. All design decisions align with UCommon constitution principles.
