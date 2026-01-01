# Feature Specification: TSHBandVector - Owning SH Band Storage

**Feature Branch**: `002-shband-vector`
**Created**: 2026-01-01
**Status**: Draft
**Input**: User description: "新增 TSHBandVector，整体设计上可以参考 TSHBandView（可以看看 spec 01），但不同点是这个是包含存储空间的，同时要让 view 新增一些之前按没做的接口（operator 等），比如加法就可以返回 vector 了，而规避了之前需要新分配空间导致的接口复杂性（因此目前没做）。"

## Clarifications

### Session 2026-01-01

- Q: TSHBandVector 应该如何存储 2*Order-1 个 float 系数？ → A: 固定大小数组成员（如 `float Data[2*Order-1]`），栈上分配
- Q: TSHBandView 的二元运算符（operator+, operator- 等）应该支持哪些操作数组合？ → A: 仅支持 View+View（Vector 通过隐式转换参与）
- Q: TSHBandVectorRGB<Order> 应该如何组织 RGB 三个通道的存储？ → A: 三个独立的 TSHBandVector<Order> 成员（R, G, B）
- Q: 标量乘法运算符应该支持哪些形式？ → A: 同时支持 vector * scalar 和 scalar * vector（需要两个重载）
- Q: TSHBandVector 是否应该支持初始化列表构造函数？ → A: 支持，但仅用于完整初始化（元素数必须匹配 2*Order-1）

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Store and Manipulate SH Band Coefficients (Priority: P1)

Developers need to store spherical harmonic band coefficients with automatic memory management and perform mathematical operations that produce new results without manual memory allocation.

**Why this priority**: Core functionality - provides the fundamental storage and computation capabilities that all other features depend on.

**Independent Test**: Can be fully tested by creating TSHBandVector instances, performing arithmetic operations (addition, subtraction, multiplication), and verifying results match expected mathematical outcomes.

**Acceptance Scenarios**:

1. **Given** no existing data, **When** developer creates a TSHBandVector<2>, **Then** storage for 3 coefficients (2*2-1) is automatically allocated and zero-initialized
2. **Given** developer provides initializer list with exactly 3 values, **When** constructing TSHBandVector<2> v = {1.0f, 2.0f, 3.0f}, **Then** vector is initialized with those values
3. **Given** two TSHBandVector<2> instances with values, **When** developer adds them using operator+, **Then** a new TSHBandVector is returned with element-wise sum without requiring manual memory management
4. **Given** a TSHBandVector<2> instance, **When** developer multiplies by a scalar using operator*, **Then** a new TSHBandVector is returned with all coefficients scaled appropriately

---

### User Story 2 - Interoperate with TSHBandView (Priority: P2)

Developers need seamless conversion between owning TSHBandVector and non-owning TSHBandView to enable flexible data access patterns and algorithm composition.

**Why this priority**: Enables integration with existing view-based APIs while maintaining clear ownership semantics.

**Independent Test**: Can be tested by creating TSHBandVector instances, obtaining views via implicit conversion or explicit methods, and verifying that modifications through views affect the underlying vector data.

**Acceptance Scenarios**:

1. **Given** a TSHBandVector<2> instance, **When** developer passes it to a function expecting TSHBandView<2, false>, **Then** implicit conversion succeeds and the function can read/modify the vector's data
2. **Given** a const TSHBandVector<2> instance, **When** developer obtains a view, **Then** a TSHBandView<2, true> (const view) is provided preventing modifications
3. **Given** a TSHBandView<2, false> pointing to external data, **When** developer constructs a TSHBandVector from it, **Then** data is deep-copied into the new vector's owned storage

---

### User Story 3 - RGB Channel Operations (Priority: P3)

Developers need to work with RGB color data stored as three separate SH band coefficient sets with coordinated operations across all channels.

**Why this priority**: Extends core functionality to common graphics use case, but can be implemented after basic single-channel support.

**Independent Test**: Can be tested by creating TSHBandVectorRGB instances, performing channel-wise operations, and verifying all three channels are correctly processed.

**Acceptance Scenarios**:

1. **Given** no existing data, **When** developer creates TSHBandVectorRGB<2>, **Then** storage for three channels (R, G, B) with 3 coefficients each is allocated
2. **Given** two TSHBandVectorRGB<2> instances, **When** developer adds them, **Then** a new TSHBandVectorRGB is returned with element-wise addition applied to all three channels independently
3. **Given** a TSHBandVectorRGB<2> instance, **When** developer accesses individual channels via .R, .G, .B members, **Then** TSHBandVector<2> references are returned for per-channel manipulation

---

### Edge Cases

- What happens when performing operations on vectors with different orders (e.g., TSHBandVector<2> + TSHBandVector<3>)? → Compile-time error due to template parameter mismatch
- How does the system handle division by zero scalar? → Undefined behavior (consistent with standard floating-point semantics)
- What happens when constructing from a null TSHBandView? → Assert failure in debug builds, undefined behavior in release builds
- How are move semantics handled to avoid unnecessary copies? → Move constructor and move assignment operator transfer ownership of internal storage
- What happens when initializer list has wrong number of elements? → Compile-time error or static_assert failure (element count must match 2*Order-1)

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST provide TSHBandVector<Order> class template that owns storage for 2*Order-1 float coefficients using fixed-size array member (stack-allocated)
- **FR-002**: System MUST provide TSHBandVectorRGB<Order> class template with three independent TSHBandVector<Order> members (R, G, B)
- **FR-003**: TSHBandVector MUST support default construction with zero-initialized coefficients
- **FR-003a**: TSHBandVector MUST support initializer list construction requiring exactly 2*Order-1 elements
- **FR-004**: TSHBandVector MUST support copy construction and copy assignment with deep copy semantics
- **FR-005**: TSHBandVector MUST support move construction and move assignment for efficient ownership transfer
- **FR-006**: TSHBandVector MUST provide implicit conversion to TSHBandView<Order, false> for mutable access
- **FR-007**: TSHBandVector MUST provide implicit conversion to TSHBandView<Order, true> when used in const context
- **FR-008**: TSHBandVector MUST support construction from TSHBandView with deep copy of viewed data
- **FR-009**: TSHBandVector MUST provide operator[] for element access (both const and non-const versions)
- **FR-010**: TSHBandVector MUST provide binary arithmetic operators (operator+, operator-, operator*, operator/) that return new TSHBandVector instances; scalar multiplication MUST support both vector*scalar and scalar*vector forms
- **FR-011**: TSHBandVector MUST provide compound assignment operators (operator+=, operator-=, operator*=, operator/=) for in-place modification
- **FR-012**: TSHBandView MUST be extended with binary arithmetic operators (operator+, operator-, operator*, operator/) accepting TSHBandView operands (TSHBandVector participates via implicit conversion) and returning TSHBandVector instances; scalar multiplication MUST support both view*scalar and scalar*view forms
- **FR-013**: System MUST provide Dot() function overloads accepting TSHBandVector arguments
- **FR-014**: TSHBandVectorRGB MUST provide public R, G, B members of type TSHBandVector<Order>
- **FR-015**: TSHBandVectorRGB MUST support all arithmetic operations with channel-wise application
- **FR-016**: TSHBandVectorRGB MUST provide implicit conversion to TSHBandViewRGB<Order, bConst> with appropriate const-correctness

### Key Entities

- **TSHBandVector<Order>**: Owning container for spherical harmonic band coefficients; stores 2*Order-1 float values in fixed-size array member (stack-allocated); supports value semantics (copy/move) and mathematical operations
- **TSHBandVectorRGB<Order>**: Owning container for RGB color data as SH coefficients; contains three independent TSHBandVector<Order> members (R, G, B); supports coordinated operations across all channels
- **TSHBandView<Order, bConst>**: Non-owning view of SH band coefficients (existing); extended with binary operators that return TSHBandVector for result storage

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Developers can perform arithmetic operations on SH band data without manual memory allocation or deallocation
- **SC-002**: Code using TSHBandVector compiles with zero-overhead abstraction (no runtime cost vs. manual array management)
- **SC-003**: TSHBandVector instances correctly manage memory with no leaks detected by memory sanitizers
- **SC-004**: Arithmetic operations on TSHBandVector produce mathematically correct results (verified against reference implementations)
- **SC-005**: Implicit conversions between TSHBandVector and TSHBandView work seamlessly in function calls without explicit casts
- **SC-006**: Move operations transfer ownership without copying coefficient data (verified via performance profiling)
