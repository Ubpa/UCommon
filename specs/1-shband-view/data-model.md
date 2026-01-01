# Data Model: SHBand View Design

**Feature**: SHBand View Design
**Date**: 2026-01-01
**Status**: Phase 1 Complete

## Overview

This document defines the class hierarchy, template instantiation model, and data relationships for the SHBand view classes. The design uses CRTP + partial specialization to achieve zero-overhead const-correct views.

---

## Class Hierarchy

```
┌─────────────────────────────────────────────────────────────┐
│ TSHBandViewCommon<Derived, Order, bConst>                   │
│ (CRTP Base Class)                                           │
│                                                             │
│ + static constexpr MaxSHOrder: int                          │
│ + static constexpr MaxSHBasis: int                          │
│ + using DataType = conditional_t<bConst, const float*, float*> │
│ + using RGBType = TSHBandViewRGB<Order, bConst>             │
│                                                             │
│ + operator[](Index) const -> const float&                   │
│ + GetSize() const -> uint64_t                               │
│ + Dot(Other) const -> float                                 │
│                                                             │
│ # AsDerived() -> Derived&                                   │
│ # AsDerived() const -> const Derived&                       │
└─────────────────────────────────────────────────────────────┘
                          △
                          │ inherits
         ┌────────────────┴────────────────┐
         │                                 │
┌────────┴─────────────────┐  ┌───────────┴──────────────────┐
│ TSHBandView<Order, false>│  │ TSHBandView<Order, true>     │
│ (Mutable Specialization) │  │ (Const Specialization)       │
│                          │  │                              │
│ - Data: float*           │  │ - Data: const float*         │
│                          │  │                              │
│ + TSHBandView()          │  │ + TSHBandView()              │
│ + TSHBandView(float*)    │  │ + TSHBandView(const float*)  │
│                          │  │ + TSHBandView(TSHBandView<Order, false>&) │
│ + operator=(...)         │  │ + operator=(...)             │
│ + operator[](Index)      │  │                              │
│   -> float&              │  │ + GetData() const            │
│ + GetData() -> float*    │  │   -> const float*            │
│ + GetData() const        │  │                              │
│   -> const float*        │  │                              │
│                          │  │                              │
│ + operator+=(...)        │  │                              │
│ + operator-=(...)        │  │                              │
│ + operator*=(...)        │  │                              │
│ + operator/=(...)        │  │                              │
└──────────────────────────┘  └──────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ TSHBandViewRGB<Order, bConst>                               │
│ (Aggregate of 3 TSHBandView)                                │
│                                                             │
│ + R: TSHBandView<Order, bConst>                             │
│ + G: TSHBandView<Order, bConst>                             │
│ + B: TSHBandView<Order, bConst>                             │
│                                                             │
│ + TSHBandViewRGB()                                          │
│ + TSHBandViewRGB(DataType*, DataType*, DataType*)           │
│ + TSHBandViewRGB(TSHBandView, TSHBandView, TSHBandView)     │
│ + TSHBandViewRGB(TSHVectorRGB<VectorOrder>&)                │
│                                                             │
│ + operator[](Index) -> TSHBandView<Order, bConst>&          │
│ + operator=(...)                                            │
│                                                             │
│ [bConst=false only]:                                        │
│ + operator+=(...)                                           │
│ + operator-=(...)                                           │
│ + operator*=(...)                                           │
│ + operator/=(...)                                           │
└─────────────────────────────────────────────────────────────┘
```

---

## Template Parameters

### TSHBandView<Order, bConst>

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `Order` | `int` | (required) | SH band order (1-based), determines size = 2*Order-1 |
| `bConst` | `bool` | `false` | Const-correctness flag: `false` = mutable, `true` = const |

**Valid Instantiations**:
- `TSHBandView<1, false>` - Mutable view of 1 coefficient (DC band)
- `TSHBandView<2, false>` - Mutable view of 3 coefficients (linear band)
- `TSHBandView<3, false>` - Mutable view of 5 coefficients (quadratic band)
- `TSHBandView<1, true>` - Const view of 1 coefficient
- `TSHBandView<2, true>` - Const view of 3 coefficients
- `TSHBandView<3, true>` - Const view of 5 coefficients

### TSHBandViewRGB<Order, bConst>

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `Order` | `int` | (required) | SH band order (1-based) |
| `bConst` | `bool` | `false` | Const-correctness flag |

**Member Types**:
- `R`, `G`, `B`: `TSHBandView<Order, bConst>` (automatically inherits const-ness)

---

## Data Members

### TSHBandView<Order, false> (Mutable)

| Member | Type | Description | Constraints |
|--------|------|-------------|-------------|
| `Data` | `float*` | Pointer to external data | May be `nullptr` (default-constructed) |

### TSHBandView<Order, true> (Const)

| Member | Type | Description | Constraints |
|--------|------|-------------|-------------|
| `Data` | `const float*` | Pointer to external const data | May be `nullptr` (default-constructed) |

### TSHBandViewRGB<Order, bConst>

| Member | Type | Description | Constraints |
|--------|------|-------------|-------------|
| `R` | `TSHBandView<Order, bConst>` | Red channel view | Inherits const-ness from `bConst` |
| `G` | `TSHBandView<Order, bConst>` | Green channel view | Inherits const-ness from `bConst` |
| `B` | `TSHBandView<Order, bConst>` | Blue channel view | Inherits const-ness from `bConst` |

---

## State Transitions

### View Lifecycle

```
┌─────────────────┐
│ Default         │
│ Constructed     │
│ (Data=nullptr)  │
└────────┬────────┘
         │ Construct with pointer
         ▼
┌─────────────────┐
│ Bound to Data   │
│ (Data!=nullptr) │
└────────┬────────┘
         │ operator= (shallow copy)
         ▼
┌─────────────────┐
│ Rebound to      │
│ Different Data  │
└─────────────────┘
```

**Note**: Views do NOT own data. Lifetime management is user's responsibility.

### Const Conversion

```
TSHBandView<Order, false>  ──implicit──>  TSHBandView<Order, true>
    (mutable)                                  (const)

TSHBandView<Order, true>   ──BLOCKED──>  TSHBandView<Order, false>
    (const)                                  (mutable)
                                         (compile error)
```

---

## Validation Rules

### Compile-Time Checks

| Rule | Enforcement | Error Message |
|------|-------------|---------------|
| `Order >= 1` | `static_assert` | "SH order must be >= 1" |
| `VectorOrder >= Order` (GetBand) | `static_assert` | "Vector order must be >= band order" |
| Const→Mutable conversion | Type system | "Cannot convert const view to mutable view" |

### Runtime Checks (Debug Only)

| Rule | Check Location | Assertion |
|------|----------------|-----------|
| `Data != nullptr` | `operator[]` | `UBPA_UCOMMON_ASSERT(Data != nullptr)` |
| `Index < MaxSHBasis` | `operator[]` | `UBPA_UCOMMON_ASSERT(Index < MaxSHBasis)` |
| `Index < 3` | `TSHBandViewRGB::operator[]` | `UBPA_UCOMMON_ASSERT(Index < 3)` |

---

## Relationships

### Composition

- `TSHBandViewRGB<Order, bConst>` **contains** 3 × `TSHBandView<Order, bConst>`
- `TSHBandView<Order, bConst>` **points to** external `float` array (not owned)

### Inheritance

- `TSHBandView<Order, false>` **inherits from** `TSHBandViewCommon<TSHBandView<Order, false>, Order, false>`
- `TSHBandView<Order, true>` **inherits from** `TSHBandViewCommon<TSHBandView<Order, true>, Order, true>`

### Conversion

- `TSHBandView<Order, false>` **implicitly converts to** `TSHBandView<Order, true>`
- `TSHBandViewRGB<Order, false>` **implicitly converts to** `TSHBandViewRGB<Order, true>`

### Integration with Owning Types

- `TSHVector<Order>::GetBand<BandOrder>()` **returns** `TSHBandView<BandOrder, false>`
- `TSHVector<Order>::GetBand<BandOrder>() const` **returns** `TSHBandView<BandOrder, true>`
- `TSHVectorRGB<Order>::GetBand<BandOrder>()` **returns** `TSHBandViewRGB<BandOrder, false>`
- `TSHVectorRGB<Order>::GetBand<BandOrder>() const` **returns** `TSHBandViewRGB<BandOrder, true>`

---

## Memory Layout

### TSHBandView<Order, bConst>

```
┌──────────────┐
│ Data (8 bytes)│ ──────> External float array [2*Order-1 elements]
└──────────────┘
sizeof = 8 bytes (64-bit pointer)
```

### TSHBandViewRGB<Order, bConst>

```
┌──────────────┐
│ R.Data (8 B) │ ──────> External float array (R channel)
├──────────────┤
│ G.Data (8 B) │ ──────> External float array (G channel)
├──────────────┤
│ B.Data (8 B) │ ──────> External float array (B channel)
└──────────────┘
sizeof = 24 bytes (3 × 8-byte pointers)
```

**Comparison with Old Design**:
- Old `TSHBandVector<2>`: `sizeof = 3 * sizeof(float) = 12 bytes`
- New `TSHBandView<2>`: `sizeof = sizeof(float*) = 8 bytes`
- Old `TSHBandVectorRGB<2>`: `sizeof = 3 * 12 = 36 bytes`
- New `TSHBandViewRGB<2>`: `sizeof = 3 * 8 = 24 bytes`

---

## Next Steps

Proceed to generate:
1. **API Contracts** (`contracts/` directory)
2. **Quickstart Guide** (`quickstart.md`)
