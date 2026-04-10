# Cpp17.h

## 职责

C++17 polyfill：提供 `TSpan`、`Clamp` 和 `InvokeResult` 的自定义实现，兼容 C++14。

## 关键抽象

### `TSpan<T>`

轻量级非拥有的连续内存视图（类似 `std::span`）。

- 构造：指针+大小、C 数组自动推导、默认空
- `operator[]` 带断言越界检查
- `Num()` / `Empty()` / `GetData()` / `begin()` / `end()`
- 可隐式转换为 `TSpan<const T>`

### `Clamp<T>`

通用 clamp，带 `VMin <= VMax` 断言。

### `InvokeResult<F, Args...>`

兼容 C++14 的 `std::invoke_result` 别名（C++14 回退到 `std::result_of`，通过 `UCOMMON_COMPATIBLE_CPP14` 控制）。
