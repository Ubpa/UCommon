# Cpp17.h

C++17 polyfill：`TSpan`、`Clamp`、`InvokeResult`，兼容 C++14。

- `TSpan<T>` — 非拥有连续内存视图，支持指针+大小/C 数组构造，越界断言，可隐式转 `const`
- `Clamp<T>` — 带 `VMin <= VMax` 断言的通用 clamp
- `InvokeResult<F,Args...>` — C++14 回退到 `std::result_of`（`UCOMMON_COMPATIBLE_CPP14` 控制）
