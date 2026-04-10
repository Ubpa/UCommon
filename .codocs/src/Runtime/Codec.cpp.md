# Codec.cpp

## 职责

HDR 颜色编码器，实现 RGBM、RGBD、RGBV 三种 HDR→LDR 编码方案。

## 关键抽象

### RGBM

- `EncodeRGBM(Color, Multiplier, LowClamp)` — 编码 RGB 到 RGBM 四通道，A 通道存 sqrt(M)
- 解码：`RGB * M^2 * Multiplier`
- 量化优化：对每个通道分别选择 floor/ceil（8-bit 量化后误差更小的那个）

### RGBD

- `EncodeRGBD(Color, MaxValue, LowClamp)` — 编码 RGB 到 RGBD，A 通道存 D 参数
- 解码：`RGB * (D / (k*D + 1))^2`，其中 `k = 1/sqrt(MaxValue) - 1`
- 通过 k 参数实现非线性映射，改善高动态范围的精度分布

### RGBV

- `EncodeRGBV(Color, MaxValue, S, LowClamp)` — 编码 RGB 到 RGBV，A 通道存 V（亮度编码）
- 解码：`RGB * V^2 / (k*V^2 + b)`，其中 `k = -S, b = S + 1/MaxValue`
- `RGBV_ComputeIntegral` — 计算给定 S 参数下的积分值（用于参数优化）
- `RGBV_SolveS` — 二分法求解使积分等于目标值的 S 参数

## 实现要点

- 三种编码都包含量化优化：对 floor/ceil 量化值选择重建误差更小的
- `MapToValidColor*` 变体：将颜色映射到编码可表示的最近有效值（无 8-bit 量化约束）
- `Encode*With*` 变体：使用预计算的 M/D/V 值编码（用于整块共享参数的场景）
