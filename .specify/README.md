# UCommon Project Governance

This directory contains the governance documents and templates for the UCommon project.

---

## Directory Structure

```
.specify/
├── memory/
│   └── constitution.md          # Project constitution (core governance document)
├── templates/
│   ├── plan-template.md         # Template for feature planning documents
│   ├── spec-template.md         # Template for technical specifications
│   ├── tasks-template.md        # Template for task tracking
│   └── commands/                # Command templates (future use)
└── README.md                    # This file
```

---

## Core Documents

### Constitution (`memory/constitution.md`)

The **Project Constitution** is the foundational governance document that defines:
- 10 core development principles
- Naming and formatting standards
- Resource management policies
- Error handling strategies
- Testing and documentation requirements
- Version management rules

**All contributors MUST comply with the constitution.**

---

## Templates

### Plan Template (`templates/plan-template.md`)

Use this template when planning new features or major changes.

**When to use**:
- Planning a new module or feature
- Proposing architectural changes
- Planning refactoring efforts

**Key sections**:
- Constitution compliance checklist
- Technical design
- Implementation phases
- Testing strategy
- Risk assessment

**How to use**:
1. Copy the template to a new file (e.g., `plans/feature-name.md`)
2. Fill in all sections marked with `[PLACEHOLDERS]`
3. Complete the constitution compliance checklist
4. Submit for review

### Spec Template (`templates/spec-template.md`)

Use this template for detailed technical specifications.

**When to use**:
- Defining API contracts
- Specifying behavior requirements
- Documenting complex features

**Key sections**:
- Constitution alignment statement
- Functional and non-functional requirements
- API specification with code examples
- Behavior specification
- Testing requirements

**How to use**:
1. Copy the template to a new file (e.g., `specs/feature-name.md`)
2. Fill in all sections marked with `[PLACEHOLDERS]`
3. Ensure API design follows naming conventions
4. Submit for review

### Tasks Template (`templates/tasks-template.md`)

Use this template for tracking implementation tasks.

**When to use**:
- Breaking down implementation work
- Tracking progress on features
- Managing sprints or milestones

**Key sections**:
- Constitution compliance tasks (pre-filled)
- Implementation tasks
- Testing tasks
- Documentation tasks
- Review and QA tasks

**How to use**:
1. Copy the template to a new file (e.g., `tasks/feature-name.md`)
2. Review the constitution compliance tasks (TASK-001 to TASK-051)
3. Add feature-specific implementation tasks
4. Track progress by checking off completed tasks

---

## Workflow

### For New Features

1. **Plan** → Create a plan document using `plan-template.md`
   - Define scope and success criteria
   - Complete constitution compliance checklist
   - Design architecture and API

2. **Specify** → Create a spec document using `spec-template.md`
   - Define detailed requirements
   - Specify API contracts
   - Document behavior and constraints

3. **Task** → Create a task list using `tasks-template.md`
   - Break down implementation work
   - Assign owners and estimates
   - Track progress

4. **Implement** → Write code following the constitution
   - Follow naming conventions (Principle 1)
   - Use Allman brace style (Principle 2)
   - Ensure type safety (Principle 3)
   - Manage resources properly (Principle 4)
   - Organize in modules (Principle 5)
   - Support cross-platform (Principle 6)
   - Optimize performance (Principle 7)
   - Handle errors correctly (Principle 8)
   - Document and test (Principle 9)
   - Manage versions (Principle 10)

5. **Review** → Code review and constitution compliance check
   - Verify naming conventions
   - Check formatting standards
   - Review resource management
   - Validate error handling
   - Ensure documentation and tests

6. **Merge** → Integrate into main branch
   - Update CHANGELOG
   - Update version if needed
   - Tag release if applicable

---

## Constitution Principles Quick Reference

1. **Unified Naming Conventions** - F/T/E/I prefixes, PascalCase
2. **Strict Code Formatting Standards** - Allman braces, tabs, MIT header
3. **Type Safety and Explicitness** - enum class, explicit types, noexcept
4. **Resource Management and Ownership Clarity** - EOwnership, RAII, move semantics
5. **Modularity and Namespace Organization** - UCommon namespace, independent headers
6. **Cross-Platform Compatibility** - CMake, MSVC/Clang/GCC, C++17
7. **Performance First** - constexpr, inline, SIMD, zero-overhead
8. **Comprehensive Error Handling** - ASSERT, no exceptions, status codes
9. **Documentation and Testing** - examples, tests, Doxygen comments
10. **Version Management and Backward Compatibility** - semantic versioning, deprecation

---

## Examples

### Example: Adding a New Module

Let's say you want to add a new `FMatrix` class:

1. **Create Plan**: `plans/matrix-module.md`
   ```markdown
   # Plan: Matrix Module
   
   ## Overview
   Add a matrix math library to UCommon...
   
   ## Constitution Compliance Check
   - [x] Class name uses F prefix: FMatrix
   - [x] Methods use PascalCase: Multiply(), Transpose()
   ...
   ```

2. **Create Spec**: `specs/matrix-module.md`
   ```markdown
   # Specification: Matrix Module
   
   ## API Specification
   ```cpp
   namespace UCommon
   {
       class FMatrix
       {
       public:
           FMatrix(uint64_t InRows, uint64_t InCols);
           ...
       };
   }
   ```
   ```

3. **Create Tasks**: `tasks/matrix-module.md`
   ```markdown
   # Tasks: Matrix Module
   
   ## Constitution Compliance Tasks
   - [x] TASK-001: Verify FMatrix uses F prefix ✓
   - [x] TASK-004: Apply Allman brace style ✓
   ...
   
   ## Implementation Tasks
   - [ ] TASK-101: Implement FMatrix constructor
   - [ ] TASK-102: Implement Multiply() method
   ...
   ```

4. **Implement**:
   - Create `include/UCommon/Matrix.h`
   - Create `src/Runtime/Matrix.cpp`
   - Follow all constitution principles

5. **Test**:
   - Create `src/tests/XX_Matrix/main.cpp`
   - Create `src/examples/XX_Matrix/main.cpp`

6. **Document**:
   - Add Doxygen comments
   - Update main README if needed

---

## Contributing

When contributing to UCommon:

1. **Read the Constitution** - Understand all 10 principles
2. **Use the Templates** - Don't start from scratch
3. **Check Compliance** - Use the checklists in templates
4. **Get Reviews** - Have your work reviewed by maintainers
5. **Update Documentation** - Keep docs in sync with code

---

## Questions?

If you have questions about the governance process or templates:

1. Review the [Constitution](memory/constitution.md)
2. Check existing plans/specs/tasks for examples
3. Ask the project maintainer: Ubpa

---

**Last Updated**: 2026-01-01

