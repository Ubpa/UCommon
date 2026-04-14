---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/Tex2D.inl
  source_hash: sha256:6775e01831588884a1482e481764ae4b26b0463a7d9d716c3cfcddc886b2b7ce
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# Tex2D.inl

## 职责

FTex2D 的模板构造函数和 `At<T>` 访问器实现。

## 实现要点

- 模板构造函数：通过 `ElementTypeOf<Element>` 自动推导元素类型，转发给非模板构造函数
- `At<T>(Index)` — 带 `static_assert` + `ElementType` 断言，`reinterpret_cast` 访问 Storage
- `At<T>(Point, C)` — 标量类型访问指定通道（`static_assert(!IsVector_v<T>)`）
- `At<T>(Point)` — 向量类型访问整个像素（`static_assert(IsVector_v<T>)`）
