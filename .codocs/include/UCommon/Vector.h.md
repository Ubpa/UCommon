# Vector.h

## 职责

2/3/4 维泛型向量模板，以及颜色和 AABB 盒类型别名。全库最底层的数学类型。

## 关键抽象

### `TVector2<T>`
- 2D 向量，成员 `X, Y`
- 算术运算、点积（`operator|`）、长度、归一化、Clamp、Saturate 等

### `TVector<T>`
- 3D 向量，成员 `X, Y, Z`
- 额外支持叉积（`operator^`）、`Project` / `Perpendicular`
- `SafeNormalize`、`IsAlmostUnit`、`MaxComponent` 等

### `TVector4<T>`
- 4D 向量，成员 `X, Y, Z, W`
- 从 TVector + W 构造

### 类型别名
- 浮点：`FVector2f/d`、`FVector3f/d`、`FVector4f/d`
- 整数：`FInt8/16/32/64Vector2/3/4`、`FUint8/16/32/64Vector2/3/4`
- 颜色：`FColor(RGB)` = uint8 向量、`FLinearColor(RGB)` = float 向量、`FDoubleColor(RGB)` = double 向量
- AABB：`FRange` / `FBox2` / `FBox` / `FBox4` — Min/Max 对

### 向量 Traits
- `IsVector_v<T>` — 是否为向量类型
- `TRemoveVector<T>::value_type` — 提取标量类型

## 注意事项
- 点积用 `operator|`，叉积用 `operator^`
- `UBPA_UCOMMON_TVECTOR*_DEFINE` 宏控制是否在库内定义这些类型（见 Config.h）
- 可通过 `operator TVector<U>()` 进行标量类型转换
- 隐式转换为 `TSpan<T>`，方便传入函数

## 相关文件
- `Cpp17.h` — TSpan / Clamp 依赖
- `Config.h` — DELTA / 断言 / 类型定义开关
