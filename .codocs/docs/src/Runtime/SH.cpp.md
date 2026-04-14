---
codocs:
  schema: 1
  source_type: file
  source_path: src/Runtime/SH.cpp
  source_hash: sha256:773034b85398e47d2c4b557fd94720d9e949d933a6aa31f8a2854286ef58b0bd
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
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

## Filmic Worlds 方法（Band 2–5 特化）

核心思路：选取 (2l+1) 个线性无关单位方向 w_k，构建两个矩阵：
- `Y[i,k]`   = SH_i(w_k)         — 原始方向的 SH 基值
- `Y_R[i,k]` = SH_i(R * w_k)    — 旋转后方向的 SH 基值

旋转矩阵 M 满足 `Y_R = M * Y`，即 `M = Y_R * inv(Y)`。

**代码形式**：令 `Yrws[k][row] = SH_row(R * w_k)`（Y_R 的转置索引），则：

```
M[row, col] = sum_k  Yrws[k][row] * invY[k, col]
```

invY 通过解析求逆预先计算为常量，展开时只需遍历每列的非零项，避免矩阵乘法。

**旋转方向的高效计算**：RotateMatrix 是行主序，`R * w` 中 w 为列向量，因此 `R * e_j = (R^T).Rows[j]`（取 R 的第 j 列）。对于对角线方向（如 `w = c*(e0+e1)`），利用线性性直接组合列向量，无需显式矩阵乘法。

### Band 2（l=1，3×3）

采样点：w0=(1,0,0), w1=(0,1,0), w2=(0,0,1)（轴向量）。

SH<1,-1>=-K·y, SH<1,0>=K·z, SH<1,1>=-K·x（K=0.48860252）。Y 矩阵稀疏，invY 每列只有一个非零项（k0=1/K=2.0466533）：
- col 0: `M[row,0] = -k0 * Yrws[1][row]`
- col 1: `M[row,1] = +k0 * Yrws[2][row]`
- col 2: `M[row,2] = -k0 * Yrws[0][row]`

### Band 3（l=2，5×5）

采样点：w0=(1,0,0), w1=(0,0,1), w2=c1·(1,1,0), w3=c1·(1,0,1), w4=c1·(0,1,1)（c1=1/√2）。

invY 常量：k0=0.9152912, k1=2·k0=1.8305824, k2=1.5853308。每列 2–3 个非零项。

### Band 4（l=3，7×7）

采样点：7 个方向包含 3 轴、3 面对角线、1 体对角线 c2·(e0+e1+e2)（c2=1/√3）。体对角方向激活所有 m 分量，包括奇函数分量，保证 Y 矩阵满秩。invY 使用 k0–k10 共 11 个常量。

### Band 5（l=4，9×9）

**关键挑战**：l=4 所有 SH<4,m> 均为偶函数（SH4(-v)=SH4(v)），轴向量和等分量对角线无法激活 `m=-4 ∝ xy(x²-y²)`（因为这些方向有 x=y 或 x/y 为零）。

**解决方案**：引入非对称方向 w6=(2,1,0)/√5, w7=(0,1,2)/√5, w8=(1,0,2)/√5（c2=1/√5），打破 x-y 对称性。这三个方向通过 `c2*(2*col0 + col1)` 等组合由 R 的列直接计算。invY 使用 k0–k29 共 30 个常量（含负值）。

## HallucinateZH 算法

从 Band1 SH 系数推测 ZH 的 L2 分量，用于低阶 SH 近似的修正。

**算法步骤**：
1. 若 `L0 <= Delta`，直接返回 0（避免除零）
2. `Buffer[3] = L0 * 0.28209479f`（L0 对应 Y₀₀，常数 = SHK<0,0>）
3. 从 `FSHVector2` 提取线性矢量（L1 分量），取其长度 `L1`
4. 限制 `L1 <= MaxL1 = (3/2)*(1-t)*0.975*L0`（t∈[0,1] 是软化参数，t 越大 L1 上限越低，防止过饱和）
5. 用经验公式估算 L2 幅值：`p = L1/L0`，`L2 = (0.6*p² + 0.08*p) * L0`
6. `Buffer[i] = LinearVector[i] * Scale * Factor1`（Factor1 = 2/3 * 0.48860252），i=0,1,2
7. `Buffer[3] -= L2 * Factor2`（Factor2 = 1/4 * 0.31539157，即 ZH 的归一化系数）
8. 返回值 = `Factor2 * L2 * 3 / (L1² / (3π))`，代表 ZH 修正的强度因子

**Buffer 布局**：[0..2] 存 L1 各分量乘以 Factor1，[3] 存 L0 的 SH 值减去 L2 修正。

## 通用递推（ComputeSHBandNRotateMatrix）

实现 Ivanic & Ruedenberg 1996 算法（含 1998 勘误），参考 google/spherical-harmonics。

### r1 矩阵构建

在 unsigned Condon-Shortley 基下，l=1 的 SH 旋转矩阵就是旋转矩阵 R 本身，但分量顺序为 {Y,Z,X}（对应 m=-1,0,+1）：

```
r1(-1,-1)=Ryy  r1(-1,0)=Ryz  r1(-1,+1)=Ryx
r1( 0,-1)=Rzy  r1( 0,0)=Rzz  r1( 0,+1)=Rzx
r1(+1,-1)=Rxy  r1(+1,0)=Rxz  r1(+1,+1)=Rxx
```

存储：`r1[(m+1)*3 + (n+1)]`，m,n ∈ {-1,0,+1}。

### 递推核心函数

- **`SHRotGetElem(M, l, m, n)`**：将中心索引 (m,n) ∈ [-l,l] 转换为线性索引访问 `M[(m+l)*(2l+1)+(n+l)]`
- **`SHRotP(i, a, b, l, r1, prev)`**：辅助函数，i∈{-1,0,+1} 选择 r1 的行。b=±l 时需要组合两个 prev 元素（处理边界情况），否则仅读取 `r1[i,0] * prev[a,b]`
- **`SHRotU/V/W(m, n, l)`**：三种贡献项，均通过 SHRotP 构建。V 和 W 对 m=0 和 m≠0 有不同分支；W 在 m=0 时恒为 0
- **`SHRotUVWCoeff(m, n, l)`**：纯整数运算计算标量系数 u,v,w，分母 `denom` 在 `|n|=l` 时用 `2l(2l-1)`，否则用 `(l+n)(l-n)`

### ping-pong 缓冲

```
bufA ← SHRotComputeBand(2, r1, r1)        // band 2，prev=r1
for band = 3..l:
    if band == l:
        SHRotComputeBand(band, r1, bufA) → OutMatrix
    else:
        SHRotComputeBand(band, r1, bufA) → bufB
        swap(bufA, bufB)                   // bufB 可被下轮覆写
```

`bufB` 按需 `resize`（按 `(2*band+1)²` 分配），通过 swap 避免额外拷贝。当 l=2 时直接写 OutMatrix，避免创建临时缓冲。

## 性能与约定注意事项

- Band 2–5 特化：时间复杂度 O((2l+1)²)，invY 常量展开为纯加乘，无分支
- 通用递推：时间复杂度 O(l⁴)（双层 m×n 循环 × P 调用 ×l 阶），仅适合 l≥5 场景
- **符号约定差异**：Band 2-4 与 `SH.h` 中 `SH<l,m>` 模板符号一致；Band 5 及通用递推使用 unsigned Condon-Shortley 基（所有 SH 系数为正），某些 (l,m) 对与 `SH<l,m>` 符号相反，使用时须注意基的一致性
- `ComputeSHBandNRotateMatrix` 输出的矩阵在 unsigned Condon-Shortley 基内自洽，可正确旋转同一基约定下存储的 SH 系数
