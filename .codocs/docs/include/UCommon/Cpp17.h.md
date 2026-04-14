---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/Cpp17.h
  source_hash: sha256:8471f2fd35703eab7e2351ccfb28cbcfad8248e16f51058ae0b5ac0db889f19a
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# Cpp17.h

C++17 polyfill：`TSpan`、`Clamp`、`InvokeResult`，兼容 C++14。

- `TSpan<T>` — 非拥有连续内存视图，支持指针+大小/C 数组构造，越界断言，可隐式转 `const`
- `Clamp<T>` — 带 `VMin <= VMax` 断言的通用 clamp
- `InvokeResult<F,Args...>` — C++14 回退到 `std::result_of`（`UCOMMON_COMPATIBLE_CPP14` 控制）
