# SH.h

## 职责

球谐函数（Spherical Harmonics）完整类型系统，支持 2~5 阶，涵盖单通道/RGB、DC/AC-only、View/Vector、Band/全阶四个正交维度。

## 类型体系（四维正交）

### 全阶系数容器

| 类型 | 说明 |
|------|------|
| `TSHVector<N>` | N*N 个系数，含 DC (L0..L(N-1)) |
| `TSHVectorAC<N>` | N*N-1 个系数，无 DC (L1..L(N-1)) |
| `TSHVectorRGB<N>` | R/G/B 三通道 TSHVector |
| `TSHVectorACRGB<N>` | R/G/B 三通道 TSHVectorAC |

### 单波段系数容器

| 类型 | 说明 |
|------|------|
| `TSHBandView<Order, bConst>` | 非拥有视图，指向 2N-1 个 float |
| `TSHBandVector<Order>` | 拥有存储的单波段 |
| `TSHBandViewRGB<Order, bConst>` | RGB 三通道视图 |
| `TSHBandVectorRGB<Order>` | RGB 三通道拥有存储 |

### 旋转矩阵

- `TSHRotateMatrices<Order>` — 扁平存储 band 2..Order 的旋转矩阵
  - 从 `FMatrix3x3f` 构造
  - `GetBand<K>()` 返回 (2K-1)×(2K-1) 行主序 TSpan
  - 可隐式向下转换为低阶版本（内存前缀兼容）

## 关键操作

- `SHBasisFunction(Vector)` — 在方向上求值 SH 基函数
- `GetBand<K>()` — 从全阶容器提取第 K 波段视图
- `ApplySHRotateMatrix` — 应用预计算旋转矩阵
- `HallucinateZH` — ZH（Zonal Harmonics）近似
- `ComputeSHBand{2..5}RotateMatrix` — 特化旋转矩阵计算
- `ComputeSHBandNRotateMatrix` — 通用 Ivanic & Ruedenberg 递推

## CRTP 基类

- `TSHVectorCommon` — 全阶单通道的算术运算（+/-/*/÷/Dot/Exp/Exp2）
- `TSHVectorRGBCommon` — 全阶 RGB 运算（含 Luminance/Desaturate/YCoCg 转换）
- `TSHBandCommon` — 单波段单通道运算
- `TSHBandRGBCommon` — 单波段 RGB 运算

## 注意事项
- Order 2~5 有预定义 `F` 前缀别名（`FSHVector3`、`FSHBandViewRGB4` 等）
- `TSHVectorAC` 的 `GetBand` 基址偏移 -1（跳过 DC）
- Band 向量点积用 `operator|`（来自 Vector.h）
- `GetLinearVector()` 返回 L1 波段的未归一化方向向量

## 相关文件
- `SH.inl` — 模板实现
- `Matrix.h` — 旋转矩阵类型
- `Vector.h` — FVector3f / FVector4f
