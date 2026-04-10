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

## 相关文件
- `Vector.h` — 向量/颜色类型
- `Half.h` — FHalf 类型
