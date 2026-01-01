<!--
Sync Impact Report
Version Change: none → 1.0.0
Modified Principles: none (initial version)
Added Sections: All sections (initial creation)
Removed Sections: none
Templates Requiring Updates:
  ✅ .specify/templates/plan-template.md (created)
  ✅ .specify/templates/spec-template.md (created)
  ✅ .specify/templates/tasks-template.md (created)
Follow-up TODOs: none
-->

# UCommon Project Constitution

**Project Name**: UCommon
**Version**: 1.0.0
**Ratification Date**: 2026-01-01
**Last Amended Date**: 2026-01-01
**Maintainer**: Ubpa

---

## Project Overview

UCommon is a C++17 general-purpose utility library providing a comprehensive set of mathematical, graphics, and system utility components, including vector operations, texture processing, thread pool, serialization, and more. The project is licensed under MIT and emphasizes code quality, performance, and maintainability.

---

## Core Principles

### Principle 1: Unified Naming Conventions

**Rules**:
- **Type Naming**: Use PascalCase with prefix conventions:
  - `F` prefix: Regular classes/structs (e.g., `FGuid`, `FTex2D`, `FThreadPool`)
  - `T` prefix: Template classes (e.g., `TVector2<T>`, `TSpan<T>`)
  - `E` prefix: Enum types (e.g., `EForceInit`, `EElementType`, `EOwnership`)
  - `I` prefix: Interfaces/abstract classes (e.g., `IArchive`)
- **Variable Naming**: Use PascalCase (e.g., `NumChannels`, `ElementType`, `Storage`)
- **Function/Method Naming**: Use PascalCase (e.g., `NewGuid()`, `GetVersion()`, `SafeNormalize()`)
- **Parameter Naming**: Use PascalCase, typically with `In` prefix for input parameters
  (e.g., `InArchive`, `InGrid2D`, `InNumChannels`)
- **Constant Naming**: Use PascalCase (e.g., `Pi`, `BiasE`)
- **Macro Naming**: Use UPPER_SNAKE_CASE with `UBPA_UCOMMON_` prefix
  (e.g., `UBPA_UCOMMON_ASSERT`, `UBPA_UCOMMON_API`)
- **Namespace Naming**: Use PascalCase (e.g., `UCommon`)

**Rationale**: Unified naming conventions improve code readability, maintain consistent style across the codebase, and facilitate team collaboration. The prefix system enables quick identification of type categories.

### Principle 2: Strict Code Formatting Standards

**Rules**:
- **Brace Style**: Use Allman style (braces on their own line)
  ```cpp
  void Function()
  {
      // Function body
  }
  ```
- **Indentation**: Use tabs for indentation
- **Spacing**:
  - Add spaces around operators (e.g., `A + B`)
  - Add space after commas (e.g., `Function(A, B, C)`)
  - Add space after control statement keywords (e.g., `if (condition)`)
- **Line Length**: Keep lines reasonably short; break complex expressions appropriately
- **File Headers**: All source files MUST include MIT license header comment

**Rationale**: Uniform formatting standards ensure visual consistency across the codebase, reduce formatting debates in code reviews, and improve code readability.

### Principle 3: Type Safety and Explicitness

**Rules**:
- Use `enum class` instead of traditional `enum`
- Use explicitly-sized integer types (`uint64_t`, `int32_t`, etc.)
- Mark non-throwing functions with `noexcept`
- Mark compile-time constants with `constexpr`
- Use `static_assert` for compile-time checks
- Avoid implicit type conversions; mark single-parameter constructors as `explicit`
- Use `const` and `const&` to protect immutable data

**Rationale**: Type safety catches errors at compile time, reducing runtime issues. Explicit type definitions improve code portability and maintainability.

### Principle 4: Resource Management and Ownership Clarity

**Rules**:
- Use `EOwnership` enum to explicitly indicate resource ownership
  (`TakeOwnership` / `DoNotTakeOwnership`)
- Implement complete set of constructors, move constructors, and move assignment operators
- Properly release resources in destructors
- Use `UBPA_UCOMMON_MALLOC`, `UBPA_UCOMMON_FREE` macros for memory management to enable
  custom allocators
- Avoid raw pointers; prefer smart pointers or RAII wrappers
- After move construction, leave source object in valid but unspecified state (typically cleared)

**Rationale**: Clear resource ownership prevents memory leaks and double-free errors. Unified memory management interface facilitates performance optimization and debugging.

### Principle 5: Modularity and Namespace Organization

**Rules**:
- All code MUST reside within the `UCommon` namespace
- Use `UBPA_UCOMMON_*_TO_NAMESPACE` macros to provide namespace export functionality
- Use `#pragma once` in header files to prevent multiple inclusion
- Each module MUST provide independent header file (e.g., `Archive.h`, `Tex2D.h`, `ThreadPool.h`)
- Main header file `UCommon.h` aggregates all submodules
- Implementation files reside in `src/Runtime` directory
- Header files reside in `include/UCommon` directory

**Rationale**: Clear module separation and namespace organization improve code maintainability, avoid naming conflicts, and allow users to selectively include required modules.

### Principle 6: Cross-Platform Compatibility

**Rules**:
- Use CMake as the build system
- Support MSVC, Clang, and GCC compilers
- Use conditional compilation to handle platform differences
- Use standard C++17 features; avoid compiler-specific extensions
- Provide `UBPA_UCOMMON_API` macro to handle dynamic library exports
- Support both static and dynamic library build modes

**Rationale**: Cross-platform compatibility expands project applicability. Standardized build system lowers the barrier to entry.

### Principle 7: Performance First

**Rules**:
- Use inline functions (`.inl` files) to optimize performance-critical paths
- Use `constexpr` for compile-time computation
- Avoid unnecessary copies; prefer references and move semantics
- Provide SIMD optimization paths (e.g., `__SSE4_2__`)
- Use template metaprogramming to reduce runtime overhead
- Provide performance tests and benchmarks for critical algorithms

**Rationale**: As a low-level utility library, performance is core competitiveness. Compile-time optimization and zero-overhead abstraction align with C++ design philosophy.

### Principle 8: Comprehensive Error Handling

**Rules**:
- Use `UBPA_UCOMMON_ASSERT` for precondition checks
- Provide `IsValid()` and similar state-checking methods
- Return error codes or status enums for critical operations
- Avoid exceptions; use return values or status flags
- Clearly document function preconditions and postconditions

**Rationale**: Explicit error handling mechanisms improve code robustness. Avoiding exceptions aligns with performance-sensitive scenarios.

### Principle 9: Documentation and Testing

**Rules**:
- Public APIs MUST have clear documentation comments
- Each module MUST provide example code (`src/examples`)
- Critical functionality MUST have unit tests (`src/tests`)
- Complex algorithms MUST provide README documentation (e.g., `src/Apps/TexProcess/README.md`)
- Use Doxygen-style comments
- Test coverage MUST include core functionality and edge cases

**Rationale**: Good documentation reduces learning curve. Comprehensive testing ensures code quality and refactoring safety.

### Principle 10: Version Management and Backward Compatibility

**Rules**:
- Use semantic versioning (MAJOR.MINOR.PATCH)
- Maintain project version in `CMakeLists.txt`
- Serialization formats MUST support version control (`UseVersion`, `GetVersion`)
- Breaking API changes MUST increment major version number
- Maintain backward compatibility; mark deprecated APIs with `[[deprecated]]`
- Record all changes in CHANGELOG

**Rationale**: Version management ensures predictability for users during upgrades. Backward compatibility reduces maintenance costs.

---

## Governance

### Amendment Process

1. Constitution amendments require approval from project maintainer
2. Breaking principle changes require major version increment
3. New principles or material expansions require minor version increment
4. Wording clarifications and typo fixes require patch version increment

### Compliance Review

- All code submissions MUST comply with this constitution
- Code reviews MUST check naming conventions, formatting standards, and resource management
- Periodic codebase audits MUST ensure continued compliance

### Exception Handling

- Third-party dependency code (`_deps` directory) may be exempted from formatting standards
- Performance-critical code may be exempted from certain rules with adequate justification
- All exceptions MUST be clearly documented in code comments with rationale

---

## Appendix

### Type Prefix Quick Reference

- `F`: Regular classes/structs
- `T`: Template classes
- `E`: Enum types
- `I`: Interfaces/abstract classes
- `UBPA_UCOMMON_`: Macro prefix

### Recommended Tools

- Build System: CMake 3.14+
- Compilers: MSVC 2019+, Clang 10+, GCC 9+
- C++ Standard: C++17

---

**This constitution is the core governance document for the UCommon project. All contributors MUST comply.**

