---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/Utils.h
  source_hash: sha256:c82ae87770dddc6cea71af0cb43a8fd3646cb014bfaa6334064d5d767c50684c
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-29T16:39:45.528765+08:00'
---
# Utils.h

## 职责

工具函数与基础类型集合：数学辅助、元素类型转换、采样、哈希、纹理寻址、所有权枚举。

## 关键抽象

### 枚举
- `EForceInit` — `Default` / `Zero`，控制构造初始化行为
- `ETextureAddress` — `Wrap` / `Clamp` / `Mirror`，纹理寻址模式
- `EElementType` — 像素元素类型：`Uint8`/`Half`/`Float`/`Double`/`ASTC_*`/`BC7`/`BQ` 等
- `EOwnership` — `TakeOwnership` / `DoNotTakeOwnership`

### 数学工具
- `Saturate<T>` / `Pow2<T>` / `Pow3<T>` / `Pow5<T>`
- `LinearInterpolate` / `BilinearInterpolate` / `TrilinearInterpolate`（含权重和 UV 两种形式）
- `Pi` / `GlodenRatio`

### 球面采样
- `UniformSampleSphere` / `FibonacciSpherePoint` / `FibonacciHemispherePoint` — 返回 (方向, PDF)
- `Hammersley` — 低差异序列

### 哈希
- `PCGHash` / `PCGHashRand` — GPU 风格 PCG 哈希

### 元素转换（查表加速）
- `ElementUint8ToFloat` / `ElementFloatToUint8` / `ElementFloatClampToUint8` 等
- `ElementUint7SNormToFloat` / `ElementUint8SNormToFloat` — SNorm 查表
- `ElementColorToLinearColor` / `ElementLinearColorToColor` 等 — 颜色类型批量转换

### 元素类型 Traits
- `IsDirectSupported<T>` — 是否为直接支持的标量类型
- `IsSupported<T>` — 标量或其向量类型
- `ElementTypeOf<T>` — 编译期推导 EElementType
- `ElementGetSize(EElementType)` — 运行时获取字节数
- `ElementIsASTC` / `ElementGetBlockSize` / `ElementGetASTC` — ASTC 块格式查询

### 纹理寻址
- `ApplyAddressMode` — float/int64 坐标的 Wrap/Clamp/Mirror 处理

### 其他
- `CreateCopy` — malloc + memcpy
- `MSB64` — 64 位最高有效位（查表实现）
- `MatrixMulVec` — 通用 N×N 矩阵乘向量

## 模板常量工具

`TypedValue<T, V>` / `TypedZero<T>` / `TypedOne<T>` / `TypedInvalid<T>`：编译期类型化常量，`TypedInvalid<T>` = `static_cast<T>(-1)`，在整数类型中通常用作哨兵值（如 `MSB64(0)` 返回 `TypedInvalid<uint8_t>`）。

## 插值函数细节

**BilinearInterpolate（权重版）**：`val2[4]` 索引按**二进制位编码**，低位为 Y 轴（`0b00/0b01` 同 X，`0b10/0b11` 同 X）；权重数组须由调用方保证 `sum(Weights)==1`，无内部校验。

**BilinearInterpolate（UV 版）**：接受 `Texcoord` + `OneMinusTexcoord` 两参数，允许调用方预计算 `1-uv` 避免重复减法；单参数版内部自动计算。

**TrilinearInterpolate vs TrilinearInterpolateZ**：两者插值顺序不同——`TrilinearInterpolate` 按 Z→Y→X 顺序；`TrilinearInterpolateZ` 按 Y→X→Z 顺序（Z 轴最后插值），后者适用于 Z 轴为层级轴（MipMap 层级）的场景。

## 采样函数语义

**UniformSampleSphere(E)**：输入 `E` 为 `[0,1)²` 的均匀样本（如 Hammersley 点），返回 `FVector4f(direction, PDF)`，方向在单位球面均匀分布，PDF = `1/(4π)`。

**FibonacciSpherePoint(N, n)**：对 N 个点的 Fibonacci 球面序列取第 n 个（n 从 0 开始），Z ∈ [-1, 1]，PDF = `1/(4π)`，比随机采样有更低差异性。参考知乎 p/25988652。

**FibonacciHemispherePoint(N, n)**：半球版本，Z ∈ [0, 1]，PDF = `1/(2π)`。注意与球面版 Z 的计算公式不同：球面用 `(2n+1)/N - 1`，半球用 `(n+0.5)/N`。

**Hammersley(N, n)**：返回 `FVector2f`，X 为 `n/N` 均匀分量，Y 为 n 的 Van der Corput 基-2 序列（位反转）。是低差异 Halton 序列在基2的特化，常用于重要性采样积分。

**PCGHashRand(Input)**：输出范围 `[0, (RAND_MAX-1)/RAND_MAX]`（约 [0, 1)），通过 `% RAND_MAX` 取模，分辨率约 1/32767，不是完整 float 精度。

## 元素类型约定与陷阱

**ElementGetSize()**：压缩格式（ASTC_\*、BC7、BQ、AdaptiveASTC、Unknown）会命中 `UBPA_UCOMMON_NO_ENTRY()`（断言失败），不能对块压缩格式调用此函数。

**ElementUint7SNorm 查表**：表长 256 但有效输入范围实为 uint7（0–127），表被周期重复填充了两遍（128项一循环），因此 128–255 的输入与 0–127 结果相同；步进为 2（-127,-125,...+127），**不含 0**。

**ElementUint8SNorm 查表**：256 项无重复，步进也为 2（-255,-253,...+255），同样**不含 0**，对称于 0。与 Uint7SNorm 是不同的量化空间。

**ElementFloatToUint8 vs ElementFloatClampToUint8**：前者用 ASSERT 要求输入在 `[0,1]`（越界行为未定义），后者调用 `Clamp` 截断后再转换，用于不信任输入范围的场景。

**AdaptiveASTC**：`ElementGetBlockSize` 对其返回 `FUint64Vector2(8)`（按 ASTC_8x8 处理）；`ElementIsASTC` 返回 **false**（枚举值超出 ASTC_4x4~ASTC_12x12 范围），两者需同时考虑。

**MSB64(0)**：返回 `TypedInvalid<uint8_t>` = 255，不是 -1 也不是 0，调用方须特判 0 输入。

## 命名空间注入宏

`UBPA_UCOMMON_UTILS_TO_NAMESPACE(NS)` 仅注入**枚举、Pi 常量和 Traits 模板**（`EForceInit/ETextureAddress/EElementType/EOwnership/Pi/IsDirectSupported/IsSupported/ElementTypeOf`），**不导出**函数（`Saturate/Pow2/LinearInterpolate/PCGHash` 等），跨命名空间仍需 `using namespace UCommon` 或全限定名。

## 相关文件
- `Vector.h` — 向量/颜色类型（`FColorRGB/FColor/FLinearColorRGB/FLinearColor` 等）
- `Half.h` — FHalf 类型（`IsDirectSupported` 之一）
