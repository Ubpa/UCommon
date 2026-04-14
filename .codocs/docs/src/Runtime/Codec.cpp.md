---
codocs:
  schema: 1
  source_type: file
  source_path: src/Runtime/Codec.cpp
  source_hash: sha256:5df1c00dbfaa5ee12c28773699747e15fdb50481830dec90a4ec458085475f86
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# Codec.cpp

## 职责

HDR 颜色编码器，实现 RGBM、RGBD、RGBV 三种 HDR→LDR 编码方案，全部存入 RGBA 四通道（RGB 存归一化颜色，A 存亮度参数）。

## 共同约定

- `InLowClamp`：防止 A 通道被纹理压缩器截断到零，所有编码的 A 通道都 `max(computed, InLowClamp)`
- `Encode*WithX` 变体：外部预计算亮度参数后传入，用于同一纹理块共享参数的场景
- `MapToValidColor*` 变体：将颜色投影到编码可表示的最近有效值，用于有损压缩前的预处理（无 8-bit 约束，只做值域裁剪）

## RGBM

- A 通道存 `sqrt(M_scale)`（而非 M 本身），使 LDR 精度分布更均匀
- 解码：`RGB_decoded = RGB * M_scale^2 * Multiplier`，其中 `M_scale = A^2`
- `EncodeRGBM(Color, Multiplier, LowClamp)` — 自动计算最优 M_scale，ceil 到下一个 1/255 步进保证不溢出
- `EncodeRGBM(float M, Multiplier, LowClamp)` — 单值版本，仅计算 A 通道（M 为亮度峰值）

## RGBD

- A 通道存 D，解码：`L = (D / (k*D + 1))^2`，其中 `k = 1/sqrt(MaxValue) - 1`
- `RGBD_GetK(MaxValue)` / `RGBD_GetMaxValue(K)` — K 与 MaxValue 互转工具函数
- D 计算：`D = sqrt(L) / (1 - sqrt(L)*k)`，ceil 到下一个 1/255 步进；非线性映射在高亮度区域提供更细的精度分布

## RGBV

- A 通道存 V，解码：`L = V^2 / (k*V^2 + b)`，其中 `k = -S, b = S + 1/MaxValue`
- V 公式：`V = sqrt((S*M+1)/(S*L+1) * L/M)`；S<0 时高亮度区精度更密，S>0 时低亮度区精度更密，S=0 退化为 RGBM 类线性分布
- `RGBV_ComputeIntegral(MaxValue, S)` — 一阶矩 ∫₀¹L dv：三段公式，S=0 时 `M/3`，S>0 用 artanh，S<0 用 arctan；用于度量给定 S 的编码效率
- `RGBV_SolveS(MaxValue, IntegralValue, Tolerance, MaxIter)` — 基于一阶矩的二分法求 S
- `EncodeVisual(float L, MaxValue, S)` — 标量版，公式 `V = sqrt((S*M+1)/(S*L+1) * L/M)`，原 `EncodeRGBV(float)` 改名
- `EncodeVisual(FLinearColorRGB, MaxValue, S)` — RGB 版，对 R/G/B 各通道独立调用标量版
