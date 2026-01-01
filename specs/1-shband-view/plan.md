# Implementation Plan: SHBand View Design

**Branch**: `1-shband-view` | **Date**: 2026-01-01 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/1-shband-view/spec.md`

**Note**: This plan implements a CRTP-based template parameter design for SHBand views, replacing the previous 4-class design with a unified 2-template approach.

## Summary

Convert `TSHBandVector` and `TSHBandVectorRGB` from memory-owning classes to lightweight view classes using template parameters for const-correctness. The new design uses `TSHBandView<Order, bConst=false>` and `TSHBandViewRGB<Order, bConst=false>` with CRTP base class `TSHBandViewCommon<Derived, Order, bConst>` for shared functionality. This simplifies the API from 4 separate types to 2 template types, reduces `Dot()` overloads from 4 to 1, and maintains zero-overhead abstraction with compile-time const checking.

**Key Technical Approach**:
- CRTP (Curiously Recurring Template Pattern) for type-safe base class
- Partial template specialization for `bConst=false` (mutable) and `bConst=true` (const)
- `std::conditional_t` for compile-time pointer type selection
- Single template `Dot()` function handling all const combinations

## Technical Context

**Language/Version**: C++17
**Primary Dependencies**: C++ Standard Library (`<type_traits>`, `<cstdint>`), UCommon internal headers
**Storage**: N/A (header-only library)
**Testing**: Existing UCommon test framework in `test/SH/`
**Target Platform**: Cross-platform (Windows/Linux/macOS), MSVC 2019+, Clang 10+, GCC 9+
**Project Type**: Single library (header-only C++ template library)
**Performance Goals**: Zero-overhead abstraction (sizeof(view) = sizeof(pointer), single dereference for access)
**Constraints**:
  - Must compile with C++17 (no C++20 features)
  - Zero runtime overhead (all template instantiation at compile-time)
  - Maintain ABI compatibility within 4.x series (after initial 4.0.0 release)
  - Support all three major compilers (MSVC, Clang, GCC)
**Scale/Scope**:
  - 2 template classes + 1 CRTP base class
  - ~500 lines of template code in `include/UCommon/SH.h`
  - Update ~10 `GetBand()` methods across owning vector classes
  - Refactor existing tests in `test/SH/`

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### Principle 1: Unified Naming Conventions
- ✅ **PASS**: Template classes use `T` prefix (`TSHBandView`, `TSHBandViewRGB`, `TSHBandViewCommon`)
- ✅ **PASS**: PascalCase for all types, methods, parameters
- ✅ **PASS**: `bConst` parameter follows bool naming convention
- ✅ **PASS**: `In` prefix for constructor parameters (e.g., `InData`, `InR`, `InG`, `InB`)

### Principle 2: Strict Code Formatting Standards
- ✅ **PASS**: Allman brace style required
- ✅ **PASS**: Tab indentation required
- ✅ **PASS**: MIT license header required in all modified files
- ✅ **PASS**: Consistent spacing around operators

### Principle 3: Type Safety and Explicitness
- ✅ **PASS**: `noexcept` on all non-throwing functions
- ✅ **PASS**: `constexpr` on compile-time constructors
- ✅ **PASS**: `explicit` on single-parameter constructors (pointer constructors)
- ✅ **PASS**: `static_assert` for compile-time checks (VectorOrder >= Order)
- ✅ **PASS**: Explicitly-sized types (`uint64_t` for indices)
- ✅ **PASS**: `const` correctness enforced via template parameter

### Principle 4: Resource Management and Ownership Clarity
- ✅ **PASS**: Views explicitly do NOT own data (documented in spec)
- ✅ **PASS**: Shallow copy semantics clearly documented
- ✅ **PASS**: User responsible for lifetime management (documented)
- ⚠️ **NOTE**: No `EOwnership` enum needed (views never own)

### Principle 5: Modularity and Namespace Organization
- ✅ **PASS**: All code in `UCommon` namespace
- ✅ **PASS**: CRTP base class in `UCommon` namespace (public, not `detail::`)
- ✅ **PASS**: Single header file organization (`include/UCommon/SH.h`)

### Principle 6: Cross-Platform Compatibility
- ✅ **PASS**: C++17 standard (no C++20 features)
- ✅ **PASS**: Target compilers: MSVC 2019+, Clang 10+, GCC 9+
- ✅ **PASS**: Platform-independent code (no OS-specific APIs)

### Principle 7: Performance Optimization
- ✅ **PASS**: Zero-overhead abstraction (template-based, fully inlined)
- ✅ **PASS**: sizeof(view) = sizeof(pointer) verified
- ✅ **PASS**: Single dereference for array access
- ✅ **PASS**: Compile-time size determination

### Principle 8: Error Handling and Debugging
- ✅ **PASS**: `UBPA_UCOMMON_ASSERT` for precondition checks
- ✅ **PASS**: Debug checks for nullptr and bounds
- ✅ **PASS**: No exceptions (library policy)

### Principle 9: Documentation and Testing
- ✅ **PASS**: Doxygen comments required for all public APIs
- ✅ **PASS**: Migration guide included in spec
- ✅ **PASS**: Existing test infrastructure in `test/SH/`
- ✅ **PASS**: Tests required for all new functionality

### Principle 10: Version Management and Compatibility
- ✅ **PASS**: MAJOR version bump (3.0.0 → 4.0.0) for breaking changes
- ✅ **PASS**: Breaking changes documented in spec
- ✅ **PASS**: No backward compatibility (explicitly stated)

**Constitution Check Result**: ✅ **ALL GATES PASSED** - Proceed to Phase 0

## Project Structure

### Documentation (this feature)

```text
specs/1-shband-view/
├── spec.md              # Feature specification (completed)
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (CRTP patterns, template best practices)
├── data-model.md        # Phase 1 output (class hierarchy, template instantiations)
├── quickstart.md        # Phase 1 output (migration examples, usage patterns)
├── contracts/           # Phase 1 output (API contracts for view classes)
│   ├── TSHBandView.yaml
│   ├── TSHBandViewRGB.yaml
│   └── TSHBandViewCommon.yaml
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
include/UCommon/
├── SH.h                 # Main header - contains all SH classes
│                        # MODIFY: Add TSHBandViewCommon, update TSHBandView/RGB
│                        # MODIFY: Update GetBand() methods in TSHVector, TSHVectorRGB, etc.
└── SH.inl               # Inline implementations
                         # MODIFY: Add template implementations for new view classes

test/SH/
├── SHBandView.cpp       # NEW: Unit tests for TSHBandView<Order, bConst>
├── SHBandViewRGB.cpp    # NEW: Unit tests for TSHBandViewRGB<Order, bConst>
├── SHVector.cpp         # MODIFY: Update GetBand() tests
└── SHVectorRGB.cpp      # MODIFY: Update GetBand() tests

CMakeLists.txt           # MODIFY: Add new test files if needed
README.md                # MODIFY: Update version to 4.0.0, document breaking changes
```

**Structure Decision**: Single library project (header-only C++ template library). All implementation in `include/UCommon/SH.h` and `SH.inl`. Tests in `test/SH/` directory. This is the standard UCommon structure for header-only template libraries.

**Key Files**:
- **Primary Implementation**: `include/UCommon/SH.h` (lines 479-518 currently contain old TSHBandVector)
- **Inline Implementations**: `include/UCommon/SH.inl` (template method bodies)
- **Test Files**: `test/SH/*.cpp` (existing test infrastructure)

## Complexity Tracking

> **No violations detected** - All constitution checks passed. CRTP pattern is a standard C++ idiom for zero-overhead polymorphism and is appropriate for this use case.


---

## Phase 0: Research (COMPLETE)

**Status**: ✅ Complete
**Output**: `research.md`

### Research Tasks Completed

1. ✅ **CRTP Pattern Best Practices**
   - Decision: Use `TSHBandViewCommon<Derived, Order, bConst>` as CRTP base
   - Rationale: Zero-overhead polymorphism, type-safe return types
   - Implementation pattern documented

2. ✅ **Template Partial Specialization for `bConst`**
   - Decision: Specialize for `bConst=false` (mutable) and `bConst=true` (const)
   - Rationale: Clear separation, compile-time selection, type safety
   - Implementation pattern documented

3. ✅ **`std::conditional_t` for Pointer Type Selection**
   - Decision: Use `std::conditional_t<bConst, const float*, float*>`
   - Rationale: Type safety, zero overhead, standard library
   - Implementation pattern documented

4. ✅ **Conversion Constructor for Implicit Mutable→Const Conversion**
   - Decision: Add conversion constructor in `TSHBandView<Order, true>`
   - Rationale: Standard practice, implicit conversion, type safety
   - Implementation pattern documented

5. ✅ **Single Template `Dot()` Function**
   - Decision: `template<int Order, bool bConst1, bool bConst2> float Dot(...)`
   - Rationale: Simplification (4→1), maintainability, zero overhead
   - Implementation pattern documented

6. ✅ **GetBand() Return Types**
   - Decision: Non-const returns `TSHBandView<Order, false>`, const returns `TSHBandView<Order, true>`
   - Rationale: Const-correctness, type safety
   - Implementation pattern documented

**All technical unknowns resolved. Proceed to Phase 1.**

---

## Phase 1: Design & Contracts (COMPLETE)

**Status**: ✅ Complete
**Outputs**: `data-model.md`, `contracts/`, `quickstart.md`

### Artifacts Generated

1. ✅ **data-model.md**
   - Class hierarchy diagram (CRTP base + 2 specializations)
   - Template parameter specifications
   - Data member definitions
   - State transition diagrams
   - Validation rules (compile-time and runtime)
   - Memory layout comparison (old vs new)

2. ✅ **contracts/TSHBandViewCommon.yaml**
   - CRTP base class API contract
   - Template parameters, type aliases, constants
   - Common methods: `operator[]`, `GetSize()`, `Dot()`
   - Protected CRTP helpers: `AsDerived()`
   - Invariants and usage examples

3. ✅ **contracts/TSHBandView.yaml**
   - Mutable specialization (`bConst=false`) contract
   - Const specialization (`bConst=true`) contract
   - Constructors, operators, methods for each
   - Conversion constructor documentation
   - Usage examples for both specializations

4. ✅ **contracts/TSHBandViewRGB.yaml**
   - RGB view class API contract
   - Data members (R, G, B channels)
   - Constructors (from pointers, views, owning vectors)
   - Operators (channel access, in-place operations)
   - Implicit conversion support

5. ✅ **quickstart.md**
   - Migration guide from 3.x to 4.0
   - Type name mapping table
   - 5 migration examples (basic usage, const correctness, GetBand, RGB, Dot)
   - Common usage patterns
   - Breaking changes checklist
   - Performance notes

### Constitution Re-Check (Post-Design)

**Status**: ✅ All gates still passing

- ✅ Naming conventions maintained in all generated contracts
- ✅ Type safety enforced via template parameters and specialization
- ✅ Zero-overhead abstraction verified in design
- ✅ Documentation complete (contracts, quickstart, data model)
- ✅ No new violations introduced

---

## Phase 2: Task Generation (PENDING)

**Status**: ⏸️ Pending - Use `/speckit.tasks` command
**Output**: `tasks.md`

This phase is NOT part of `/speckit.plan` command. Run `/speckit.tasks` to generate detailed implementation tasks.

---

## Summary

### Completed Phases

| Phase | Status | Outputs | Notes |
|-------|--------|---------|-------|
| **Phase 0: Research** | ✅ Complete | `research.md` | All technical unknowns resolved |
| **Phase 1: Design** | ✅ Complete | `data-model.md`, `contracts/`, `quickstart.md` | API contracts and migration guide ready |
| **Phase 2: Tasks** | ⏸️ Pending | `tasks.md` | Run `/speckit.tasks` command |

### Key Decisions

1. **CRTP Architecture**: `TSHBandViewCommon<Derived, Order, bConst>` base class
2. **Partial Specialization**: Separate `bConst=false` and `bConst=true` specializations
3. **Implicit Conversion**: Mutable→const via conversion constructor
4. **Simplified Dot()**: Single template function instead of 4 overloads
5. **Const-Correct GetBand()**: Returns appropriate `bConst` based on caller const-ness

### Design Simplification Achieved

- **Types**: 4 classes → 2 template classes (50% reduction)
- **Dot() Functions**: 4 overloads → 1 template (75% reduction)
- **Code Duplication**: Eliminated via CRTP base class
- **Maintainability**: Single source of truth for common functionality

### Next Steps

1. ✅ **Planning Complete** - All design artifacts generated
2. ⏭️ **Run `/speckit.tasks`** - Generate detailed implementation tasks
3. ⏭️ **Run `/speckit.implement`** - Execute implementation (after tasks generated)

---

## Appendix: File Locations

### Documentation
- **Specification**: `D:\Workspace\UCommon\specs\1-shband-view\spec.md`
- **Plan**: `D:\Workspace\UCommon\specs\1-shband-view\plan.md` (this file)
- **Research**: `D:\Workspace\UCommon\specs\1-shband-view\research.md`
- **Data Model**: `D:\Workspace\UCommon\specs\1-shband-view\data-model.md`
- **Quickstart**: `D:\Workspace\UCommon\specs\1-shband-view\quickstart.md`

### Contracts
- **Base Class**: `D:\Workspace\UCommon\specs\1-shband-view\contracts\TSHBandViewCommon.yaml`
- **View Class**: `D:\Workspace\UCommon\specs\1-shband-view\contracts\TSHBandView.yaml`
- **RGB View**: `D:\Workspace\UCommon\specs\1-shband-view\contracts\TSHBandViewRGB.yaml`

### Implementation (To Be Modified)
- **Header**: `D:\Workspace\UCommon\include\UCommon\SH.h`
- **Inline**: `D:\Workspace\UCommon\include\UCommon\SH.inl`
- **Tests**: `D:\Workspace\UCommon\test\SH\*.cpp`

