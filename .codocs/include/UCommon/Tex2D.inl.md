# Tex2D.inl

## 职责

FTex2D 的模板构造函数和 `At<T>` 访问器实现。

## 实现要点

- 模板构造函数：通过 `ElementTypeOf<Element>` 自动推导元素类型，转发给非模板构造函数
- `At<T>(Index)` — 带 `static_assert` + `ElementType` 断言，`reinterpret_cast` 访问 Storage
- `At<T>(Point, C)` — 标量类型访问指定通道（`static_assert(!IsVector_v<T>)`）
- `At<T>(Point)` — 向量类型访问整个像素（`static_assert(IsVector_v<T>)`）
