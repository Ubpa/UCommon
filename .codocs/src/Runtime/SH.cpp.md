# SH.cpp

## 职责

球谐函数（Spherical Harmonics）旋转矩阵计算，支持 Band 2-5 的特化实现和任意阶通用递推。

## 关键抽象

- `ComputeSHBand2RotateMatrix` — l=1 SH 旋转矩阵（3×3），基于 {y,z,x} 基排列
- `ComputeSHBand3RotateMatrix` — l=2 SH 旋转矩阵（5×5）
- `ComputeSHBand4RotateMatrix` — l=3 SH 旋转矩阵（7×7）
- `ComputeSHBand5RotateMatrix` — l=4 SH 旋转矩阵（9×9）
- `ComputeSHBandNRotateMatrix` — l≥2 通用递推实现
- `HallucinateZH` — 从 L0/L1 球谐系数推测 L2 Zonal Harmonic 分量

## 实现要点

- Band 2-5 使用 Filmic Worlds 方法：选取 (2l+1) 个线性无关采样方向，构建 `Y` 矩阵，计算 `M = Y_R * Y^{-1}`
  - `invY` 系数通过解析求逆预计算为常量
  - Band 5 (l=4) 需要 `(2,1,0)/√5` 类方向以激活偶函数的 m=-4 分量
- 通用递推 `ComputeSHBandNRotateMatrix` 实现 Ivanic & Ruedenberg 1996 算法：
  - 从 r1 (l=1 的 3×3 矩阵) 逐阶递推到目标阶
  - 使用 ping-pong 缓冲避免额外分配
  - 使用 unsigned Condon-Shortley 基约定

## 注意事项

- Band 2-4 的特化实现与 `SH.h` 中 `SH<l,m>` 模板的符号约定一致
- Band 5 及通用递推使用 unsigned Condon-Shortley 基，某些 (l,m) 对的符号可能与 `SH<l,m>` 不同
