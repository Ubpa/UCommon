# Implementation Plan: SHBand View Design

**Created**: 2026-01-01
**Author**: Ubpa
**Status**: Draft
**Related Spec**: [spec.md](./spec.md)
**Branch**: 1-shband-view

---

## Executive Summary

### Objective
Convert `TSHBandVector` and `TSHBandVectorRGB` from memory-owning classes (inheriting from `TSHVectorBase`) to lightweight view classes that store only pointers to external data.

### Key Changes
- **Remove inheritance**: Views no longer inherit from `TSHVectorBase` or `TSHVectorRGBBase`
- **Pointer-only storage**: Replace `float V[MaxSHBasis]` array with `float* Data` pointer
- **Type renaming**: `TSHBandVector` → `TSHBandView`, add `TSHBandConstView` for const correctness
- **Update GetBand()**: Return views instead of references to internal data
- **Remove static methods**: `SHBasisFunction()`, `ZHToSHBasisFunction()` removed from views

### Impact
- **Version**: MAJOR bump (3.0.0 → 4.0.0)
- **Breaking**: Yes - no backward compatibility
- **Files affected**: `include/UCommon/SH.h`, `include/UCommon/SH.inl`, tests
- **Memory footprint**: `TSHBandView<2>` reduces from 12 bytes to 8 bytes (64-bit)

---

## Constitution Compliance

✅ **All 10 principles satisfied** (see detailed checklist in spec.md)

**Key alignments**:
- Principle 1: T prefix for template classes
- Principle 3: `explicit`, `noexcept`, `constexpr` used appropriately
- Principle 4: View pattern - no ownership, user manages lifetime
- Principle 7: Zero-overhead abstraction
- Principle 10: MAJOR version bump documented

---

## Technical Architecture

### Current Design (Owning)

```cpp
template<int Order>
class TSHBandVector : public TSHVectorBase<TSHBandVector<Order>, Order, 2*Order-1>
{
    // Inherits: float V[2*Order-1]
    // Owns data on stack
};
```

**Memory**: `sizeof(TSHBandVector<2>)` = 12 bytes (3 floats)

### New Design (View)

```cpp
template<int Order>
class TSHBandView
{
    float* Data;  // 8 bytes (64-bit pointer)
    // Points to external data
};

template<int Order>
class TSHBandConstView
{
    const float* Data;  // 8 bytes
};
```

**Memory**: `sizeof(TSHBandView<2>)` = 8 bytes

### Data Flow

```
Before (Owning):
TSHVector<3>::GetBand<2>()
  └─> reinterpret_cast<TSHBandVector<2>*>(&V[IndexBase])
      └─> Returns reference to internal array

After (View):
TSHVector<3>::GetBand<2>()
  └─> TSHBandView<2>(&V[IndexBase])
      └─> Returns view pointing to internal array
```

---

## Implementation Phases

### Phase 0: Preparation (0.5 days)

**Objectives**:
- Understand current implementation
- Identify all usage sites
- Plan migration strategy

**Tasks**:
1. [ ] Analyze current `TSHBandVector` in `SH.h` (lines 479-497)
2. [ ] Analyze current `TSHBandVectorRGB` in `SH.h` (lines 500-518)
3. [ ] Find all `GetBand()` call sites
4. [ ] Review test files in `test/SH/`
5. [ ] Document breaking changes

**Deliverables**:
- [ ] List of files to modify
- [ ] Migration checklist

---

### Phase 1: Core View Classes (1 day)

**Objectives**:
- Implement 4 new view classes
- Remove old owning classes
- Ensure compilation

**Tasks**:

#### 1.1 Remove Old Classes
- [ ] Delete `TSHBandVector<Order>` class (lines 479-497)
- [ ] Delete `TSHBandVectorRGB<Order>` class (lines 500-518)
- [ ] Keep type aliases for now (FSHBandVector2, etc.) - will update later

#### 1.2 Implement TSHBandView<Order>
- [ ] Add class definition after line 478
- [ ] Implement default constructor: `Data(nullptr)`
- [ ] Implement pointer constructor: `explicit TSHBandView(float* InData)`
- [ ] Implement `operator=` (shallow copy)
- [ ] Implement `operator[]` with `UBPA_UCOMMON_ASSERT(Data != nullptr && Index < MaxSHBasis)`
- [ ] Implement `GetData()`, `GetSize()`
- [ ] Implement `operator+=`, `-=`, `*=`, `/=`
- [ ] Implement `Dot()` member function

#### 1.3 Implement TSHBandConstView<Order>
- [ ] Add class definition
- [ ] Implement constructors (default, const pointer, implicit from TSHBandView)
- [ ] Implement `operator=` (shallow copy)
- [ ] Implement const `operator[]`
- [ ] Implement `GetData()`, `GetSize()`
- [ ] Implement `Dot()` member function

#### 1.4 Implement Non-Member Dot Functions
- [ ] Add 4 overloads for all const/non-const combinations
- [ ] Place after class definitions
### Phase 3: Update GetBand() Methods (1 day)

**Objectives**:
- Modify all `GetBand()` methods to return views
- Ensure const correctness

**Tasks**:

#### 3.1 Update TSHVector<Order>::GetBand()
Current (lines 569-580):
```cpp
template<int BandOrder>
TSHBandVector<BandOrder>& GetBand()
{
    constexpr int IndexBase = Pow2(BandOrder - 1);
    return *reinterpret_cast<TSHBandVector<BandOrder>*>(&Super::V[IndexBase]);
}
```

New:
```cpp
template<int BandOrder>
TSHBandView<BandOrder> GetBand() noexcept
{
    constexpr int IndexBase = Pow2(BandOrder - 1);
    return TSHBandView<BandOrder>(&Super::V[IndexBase]);
}

template<int BandOrder>
TSHBandConstView<BandOrder> GetBand() const noexcept
{
    constexpr int IndexBase = Pow2(BandOrder - 1);
    return TSHBandConstView<BandOrder>(&Super::V[IndexBase]);
}
```

- [ ] Update non-const `GetBand()` to return `TSHBandView<BandOrder>`
- [ ] Update const `GetBand()` to return `TSHBandConstView<BandOrder>`
- [ ] Add `noexcept` specifier

#### 3.2 Update TSHVectorAC<Order>::GetBand()
- [ ] Same changes as TSHVector (lines 632-643)

#### 3.3 Update TSHVectorRGB<Order>::GetBand()
Current (lines 677-681):
```cpp
template<int BandOrder>
const TSHBandVectorRGB<BandOrder> GetBand() const
{
    return { Super::R.template GetBand<BandOrder>(),
             Super::G.template GetBand<BandOrder>(),
             Super::B.template GetBand<BandOrder>() };
}
```

New:
```cpp
template<int BandOrder>
TSHBandViewRGB<BandOrder> GetBand() noexcept
{
    return TSHBandViewRGB<BandOrder>(
        Super::R.template GetBand<BandOrder>(),
        Super::G.template GetBand<BandOrder>(),
        Super::B.template GetBand<BandOrder>()
    );
}

template<int BandOrder>
TSHBandConstViewRGB<BandOrder> GetBand() const noexcept
{
    return TSHBandConstViewRGB<BandOrder>(
        Super::R.template GetBand<BandOrder>(),
        Super::G.template GetBand<BandOrder>(),
        Super::B.template GetBand<BandOrder>()
    );
}
```

- [ ] Add non-const overload returning `TSHBandViewRGB<BandOrder>`
- [ ] Update const overload to return `TSHBandConstViewRGB<BandOrder>`
- [ ] Use 3-view constructor

#### 3.4 Update TSHVectorACRGB<Order>::SetBand()
- [ ] Change parameter type to `TSHBandViewRGB<BandOrder>`
- [ ] Update implementation to handle shallow copy semantics

**Deliverables**:
- [ ] All `GetBand()` methods return views
- [ ] Const correctness maintained
- [ ] Code compiles

---

### Phase 4: Update Type Aliases (0.25 days)

**Objectives**:
- Update type aliases to use new names
- Add const view aliases

**Tasks**:
- [ ] Replace `FSHBandVector*` with `FSHBandView*`
- [ ] Replace `FSHBandVectorRGB*` with `FSHBandViewRGB*`
- [ ] Add `FSHBandConstView*` aliases
- [ ] Add `FSHBandConstViewRGB*` aliases

**Deliverables**:
- [ ] Type aliases updated (lines 738-745)

---

### Phase 5: Update Tests (1 day)

**Objectives**:
- Fix all broken tests
- Add new tests for view semantics

**Tasks**:

#### 5.1 Update Existing Tests
- [ ] Find all tests using `TSHBandVector` or `TSHBandVectorRGB`
- [ ] Update to use owning types (`TSHVector`) and extract views
- [ ] Fix `GetBand()` usage (now returns by value, not reference)
- [ ] Update assignment tests (now shallow copy)

#### 5.2 Add New Tests
- [ ] Test default construction (nullptr)
- [ ] Test pointer construction
- [ ] Test implicit conversion (mutable → const)
- [ ] Test shallow copy semantics
- [ ] Test mathematical operators
- [ ] Test `Dot()` functions
- [ ] Test RGB view construction from `TSHVectorRGB`

**Deliverables**:
- [ ] All existing tests pass
- [ ] New tests added and passing
- [ ] Test coverage >= 90%

---

### Phase 6: Documentation & Validation (0.5 days)

**Objectives**:
- Update documentation
- Validate cross-platform compilation
- Performance benchmarks

**Tasks**:

#### 6.1 Documentation
- [ ] Update comments in `SH.h` for new classes
- [ ] Add Doxygen comments for public APIs
- [ ] Document migration guide

#### 6.2 Validation
- [ ] Compile on MSVC 2019+
- [ ] Compile on Clang 10+
- [ ] Compile on GCC 9+
- [ ] Zero warnings on all compilers
- [ ] Run all tests on all platforms

#### 6.3 Performance
- [ ] Benchmark `GetBand()` performance
- [ ] Verify `sizeof(TSHBandView<N>)` == 8 bytes
- [ ] Verify `sizeof(TSHBandViewRGB<N>)` == 24 bytes

**Deliverables**:
- [ ] Documentation complete
- [ ] All platforms validated
- [ ] Performance benchmarks pass

---

## Risk Assessment

### High Risk
1. **Breaking Changes**: All existing code using `TSHBandVector` will break
   - **Mitigation**: Clear migration guide, version bump to 4.0.0

2. **Lifetime Management**: Users must ensure data outlives views
   - **Mitigation**: Document clearly, add debug assertions

### Medium Risk
1. **Test Coverage**: May miss edge cases in view semantics
   - **Mitigation**: Comprehensive test suite, code review

---

## Success Metrics

- [ ] All 4 view classes implemented and tested
- [ ] `sizeof(TSHBandView<2>)` == 8 bytes (64-bit)
- [ ] All existing tests pass (after migration)
- [ ] Zero compiler warnings (MSVC/Clang/GCC)
- [ ] Performance: no regression vs. 3.0.0
- [ ] Documentation complete

---

## Timeline

| Phase | Duration | Dependencies |
|-------|----------|--------------|
| Phase 0: Preparation | 0.5 days | None |
| Phase 1: Core Views | 1 day | Phase 0 |
| Phase 2: RGB Views | 0.5 days | Phase 1 |
| Phase 3: GetBand() | 1 day | Phase 2 |
| Phase 4: Type Aliases | 0.25 days | Phase 3 |
| Phase 5: Tests | 1 day | Phase 4 |
| Phase 6: Documentation | 0.5 days | Phase 5 |
| **Total** | **4.75 days** | |

---

## Next Steps

After plan approval:
1. Create `tasks.md` from this plan using `/speckit.tasks`
2. Begin Phase 0 (Preparation)
3. Daily progress updates
4. Code review after Phase 3
5. Final review before merge

---

**Plan Status**: ✅ Ready for Review
**Estimated Effort**: ~5 days
**Breaking Changes**: Yes (MAJOR version bump)
**Backward Compatibility**: No
