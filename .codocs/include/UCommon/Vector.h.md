# Vector.h

## 职责

2/3/4 维泛型向量模板，以及颜色类型和 AABB 盒类型别名。全库最底层的数学类型，被几乎所有上层模块依赖。

## 类型体系

### `TVector2<T>` — 2D 向量
成员：`X, Y`（T 类型，按声明顺序连续存储）

### `TVector<T>` — 3D 向量
成员：`X, Y, Z`。注意：类名是 `TVector` 不是 `TVector3`，整库统一用这个名字表示三维向量。

### `TVector4<T>` — 4D 向量
成员：`X, Y, Z, W`

### AABB 盒类型（仅有 Min/Max 两字段）
- `TRange<T>` — 1D，Min/Max 均为 T
- `TBox2<T>` — 2D，Min/Max 均为 TVector2\<T>
- `TBox<T>` — 3D，Min/Max 均为 TVector\<T>
- `TBox4<T>` — 4D，Min/Max 均为 TVector4\<T>
- 别名 `FRange / FBox2 / FBox / FBox4` 均为 float 特化；TBox 系列没有提供整数别名

### 完整类型别名表

| 类型名 | 实际类型 |
|--------|---------|
| `FVector2f / FVector2d` | `TVector2<float/double>` |
| `FInt8/16/32/64Vector2` | `TVector2<int8/16/32/64_t>` |
| `FUint8/16/32/64Vector2` | `TVector2<uint8/16/32/64_t>` |
| `FVector3f / FVector3d` | `TVector<float/double>` |
| `FInt8/16/32/64Vector` | `TVector<int8/16/32/64_t>` |
| `FUint8/16/32/64Vector` | `TVector<uint8/16/32/64_t>` |
| `FVector4f / FVector4d` | `TVector4<float/double>` |
| `FInt8/16/32/64Vector4` | `TVector4<int8/16/32/64_t>` |
| `FUint8/16/32/64Vector4` | `TVector4<uint8/16/32/64_t>` |
| `FColorRGB` | `FUint8Vector`（即 uint8 的 3 分量，RGB）|
| `FLinearColorRGB` | `FVector3f` |
| `FDoubleColorRGB` | `FVector3d` |
| `FColor` | `FUint8Vector4`（RGBA，W = Alpha）|
| `FLinearColor` | `FVector4f` |
| `FDoubleColor` | `FVector4d` |

## 构造规则与陷阱

### 默认构造不初始化
```cpp
TVector V; // X/Y/Z 值未定义，不是零！
```
唯一的广播初始化是 `explicit TVector(T V)`，传单值才会三分量全设为同一值。

### 跨维度构造
- `TVector(TVector2<T> XY, T Z)` — 2D+Z 拼成 3D
- `TVector(T X, TVector2<T> YZ)` — X+2D 拼成 3D
- `TVector4` 支持更多组合：(Vec3, W)、(X, Vec3)、(Vec2, Z, W)、(X, Vec2, W)、(X, Y, Vec2)、(Vec2, Vec2)

### 跨类型转换
- **同维度、不同标量类型**：隐式转换，如 `TVector<float>` → `TVector<double>` 自动发生
- **降维**（高维→低维）：**显式**转换，如 `TVector2(TVector<U> Vec)` 是 `explicit`，不会隐式裁截
- `TVector4` → `TVector` 也是 `explicit`（只取 XYZ）

## 运算符语义

### 易混淆的运算符
| 运算符 | 含义 | 适用 |
|--------|------|------|
| `*` | **逐分量乘法**，不是向量缩放的专用符号 | 2D/3D/4D |
| `\|` | **点积**（Dot product）| 2D/3D/4D |
| `^` | **叉积**（Cross product）| 仅 3D，返回向量 |
| `%` | 逐分量取模，整数类型时有意义 | 2D/3D/4D |

**陷阱**：`A * B` 是 Hadamard 乘积（逐分量），不是缩放。缩放写 `V * K` 或 `K * V`（支持标量在左）。

### TVector2 的 Cross
`TVector2::Cross()` 返回标量 `T`（二维叉积 = `A.X*B.Y - A.Y*B.X`），不是向量。TVector4 没有叉积操作。

### 比较运算符的逻辑约定
- `==` 用 `&&` 连接：所有分量相等才返回 true（全等语义）
- `!=` 用 `||` 连接：任意分量不等即返回 true（与 `==` 互为反）
- `<`、`<=`、`>`、`>=` 同样用 `&&`：所有分量满足才为 true（偏序，不是字典序）

## 关键方法语义

### 归一化族
- `SafeNormalize(SquaredDelta)` — 当模长的平方 < SquaredDelta 时返回零向量，否则归一化后 assert 结果是单位向量。**注意参数是平方 delta**，默认值来自 `UBPA_UCOMMON_DELTA`（Config.h 定义）。
- `IsAlmostUnit(SquaredDelta)` — 同样是平方 delta，检查 `|‖v‖² - 1| < SquaredDelta`，不是 `|‖v‖ - 1|`。
- `IsAlmostZero(Delta)` — 这里 delta 是分量绝对值，不是平方。

### 投影族（要求 N 是单位向量）
- `Project(N)` — 求 *this 在 N 方向上的投影向量，内部 assert N 是单位向量
- `Perpendicular(N)` — 求 *this 垂直于 N 的分量（Gram-Schmidt 减去投影）

### 逐分量数学函数（全部 noexcept）
Abs、Sqrt、Square（即 x²）、Reciprocal（1/x）、Round、Ceil、Floor、Log、Log2、Exp、Exp2

### 统计类方法
- `MinComponent() / MaxComponent()` — 返回最小/最大分量的值（标量）
- `Sum()` — 分量之和
- `Average()` — 分量均值（用乘法实现：`Sum() * (1/N)`）
- `Area()`（2D）/ `Volume()`（3D/4D）— 分量之积，几何意义为面积/体积

### 亮度（仅 TVector/3D）
- `GetLuminance()` — BT.601 权重 (0.3, 0.59, 0.11)，整数类型用整数乘法避免浮点
- `GetSrgbLuminance()` — BT.709 权重 (0.2126, 0.7152, 0.0722)，更符合现代显示标准

### 随机数
`Rand01()` — 静态方法，用 `rand()` 生成 [0,1] 随机向量，**非线程安全**，不宜用于高质量渲染。

## 内存布局与底层访问

### 连续布局保证
成员声明为 `T X; T Y; T Z;`，标准布局（standard-layout struct），内存连续。`GetData()` 返回 `&X` 的裸指针，`operator TSpan<T>()` 也以 `&X` 为基址构造 Span。

### `As<U>()` — 类型双关
```cpp
V.As<SomeOtherType>()  // reinterpret_cast，要求 sizeof(U) == sizeof(Vector)
```
右值版本额外检查 `alignof(Vector) % alignof(U) == 0`，左值版本只检查大小。可用于向 SIMD 类型（如 `__m128`）做零拷贝转换，但调用方需保证对齐。

### `operator[]` 越界行为
用 `reinterpret_cast<T*>(this)[Index]` 实现，越界通过 `UBPA_UCOMMON_ASSERT` 检查（仅 Debug 模式），Release 下越界是 UB。

## 结构化绑定支持

三个类型均特化了 `std::tuple_size` / `std::tuple_element`，并在 `UCommon` 命名空间提供了 `get<I>(V)` 函数，因此可以直接：
```cpp
auto [x, y, z] = someVector3;
```

## 扩展点与宏约定

### `UBPA_UCOMMON_TVECTOR*_DEFINE` 宏
每个类模板在成员开头展开这个宏（由 Config.h 控制），允许用户注入额外成员或禁用整个类型定义。使用前必须检查 Config.h 中是否已启用对应宏。

### `UBPA_UCOMMON_VECTOR_TO_NAMESPACE(NS)` 宏
将所有向量类型别名批量引入目标命名空间。文件末尾已自动引入 `UCommonTest`。

### 运算符宏的左操作数交换支持
`UBPA_UCOMMON_DEFINE_FRIEND_OP` 为每个算术运算额外生成 `K op V` 形式的 friend 函数，因此 `2.0f * vec` 和 `vec * 2.0f` 均有效。

## Traits

```cpp
IsVector_v<T>                  // bool，是否为 TVector2/TVector/TVector4
TRemoveVector<T>::value_type   // 提取标量类型，非向量类型原样返回
```

## 设计陷阱速查

| 陷阱 | 说明 |
|------|------|
| 默认构造不清零 | `TVector V;` 值未定义，务必显式初始化 |
| `*` 是逐分量乘 | 不是点积，点积要用 `\|` |
| `^` 仅 3D 有 | TVector2/TVector4 没有叉积运算符 |
| 降维转换是 explicit | `TVector2` 从 `TVector/TVector4` 构造需要强转 |
| `IsAlmostUnit` 参数是平方 | delta 含义与 `IsAlmostZero` 不同 |
| `Project/Perpendicular` 需单位向量 | Debug 下 assert，Release 下静默错误 |
| `Rand01` 非线程安全 | 底层用 C `rand()` |
| 无 SIMD 对齐保证 | 无 `alignas(16)`，传 SIMD 前需确认对齐 |
| FColor 的 W 是 Alpha | `FColor = FUint8Vector4`，第四分量是透明度 |

## 相关文件
- `Cpp17.h` — `TSpan`、`Clamp` 实现来源
- `Config.h` — `UBPA_UCOMMON_DELTA`、`UBPA_UCOMMON_ASSERT`、类型定义开关 `UBPA_UCOMMON_TVECTOR*_DEFINE`
