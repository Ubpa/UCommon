# SH.inl

## 职责

SH 类型系统的全部模板实现：基函数求值、归一化常数、View/Vector/RGB 的算术运算、旋转矩阵构造与应用。

## 实现要点

### SH 基函数求值
- `SH<l,m>(x,y,z)` — 硬编码 l=0..4 的解析公式（参考 "Stupid SH Tricks"）
- `SHKImpl<l,m>()` — 编译期查表返回归一化常数 K(l,m)
- `Details::SHs` — 用 `integer_sequence` 展开，批量填充 V[] 数组

### TSHBandVector / TSHBandView 运算
- View 的 `+=/-=/*=/÷=` 直接修改指向的数据
- 二元运算返回 TSHBandVector（拥有存储）
- RGB 版本按 R/G/B 三通道分别运算

### 旋转矩阵
- `TSHRotateMatrices<Order>` 构造函数：用 `FillSHRotateMatrices` 依次计算 band 2..Order
- Band 2~5 使用特化函数，Band 6+ 使用通用 Ivanic & Ruedenberg 递推
- `ApplySHRotateMatrix` — 按波段应用旋转，L0（DC）不变，L1 由 3x3 矩阵直接隐含

### `ApplySHRotateMatrix` 原地旋转
- `ApplySHRotateMatrix<Order>` — 对单波段 View 做矩阵×向量（带临时缓冲避免覆盖）
- `TSHVectorCommon::ApplySHRotateMatrix` — 逐波段应用，通过 `SHIndexOffset` 区分 DC/AC
