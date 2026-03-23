# SHRotate

> Reference: http://filmicworlds.com/blog/simple-and-fast-spherical-harmonic-rotation/

需要注意的是，这里旋转矩阵是用于旋转方向的，而不是旋转函数的，函数的旋转应该是方向旋转的逆

方向固定的情况下，![img](https://cdn.nlark.com/yuque/__latex/bfe7ad17584c287695e9c24255c18ed5.svg)可预计算，若旋转也固定，则![img](https://cdn.nlark.com/yuque/__latex/9757a32e1c90fad3632e28306d2a251e.svg)可预计算，则![img](https://cdn.nlark.com/yuque/__latex/0d179e2e631a77dc73502ed82d7fc530.svg)也可预计算

## 1 阶情形（l=1）

**球谐基函数**：
- Y_{1,-1}(x,y,z) = -K*y，其中 K = 0.48860252
- Y_{1,0}(x,y,z) = K*z
- Y_{1,1}(x,y,z) = -K*x

**采样点**（使用坐标轴，3维正交）：
```
w0 = (1, 0, 0)
w1 = (0, 1, 0)
w2 = (0, 0, 1)
```

**Y 矩阵**（行 = SH 阶数 m，列 = 采样点）：
```
Y = [  0  -K   0 ]    (m=-1)
    [  0   0   K ]    (m= 0)
    [ -K   0   0 ]    (m=+1)
```

**Y^{-1} 矩阵推导**：
- det(Y) = K^3
- 通过伴随矩阵法（adjugate matrix）求逆，或直接验证：
```
k0 = 1/K = 2.0466533

Y^{-1} = [  0   0  -k0 ]    (row 0)
         [ -k0  0   0  ]    (row 1)
         [  0  k0   0  ]    (row 2)
```

**验证方法**（MATLAB）：
```matlab
Y_1_n1=@(v) -0.48860252 * v(2);
Y_1_0 =@(v)  0.48860252 * v(3);
Y_1_1 =@(v) -0.48860252 * v(1);
Y_1   =@(v) [Y_1_n1(v);Y_1_0(v);Y_1_1(v)];
w1_0=[1;0;0];
w1_1=[0;1;0];
w1_2=[0;0;1];
Y1=[Y_1(w1_0),Y_1(w1_1),Y_1(w1_2)]
invY1=inv(Y1)  % 验证得到上述 Y^{-1}
```

**旋转矩阵计算示例**：
```matlab
R=[0,1,0;0,0,1;1,0,0]';  % 某个旋转矩阵（行优先）
Y1_R=[Y_1(R*w1_0),Y_1(R*w1_1),Y_1(R*w1_2)]
M1_R=invY1*Y1_R  % 得到旋转后的 SH 系数变换矩阵
```

## 2 阶情形（l=2）

**球谐基函数**：
- Y_{2,-2}(x,y,z) = a*x*y，其中 a = 1.0925485
- Y_{2,-1}(x,y,z) = -a*y*z
- Y_{2,0}(x,y,z) = c2*(3z^2 - 1)，其中 c2 = 0.31539157
- Y_{2,1}(x,y,z) = -a*x*z
- Y_{2,2}(x,y,z) = b*(x^2 - y^2)，其中 b = 0.54627424

**采样点**（5 个线性独立方向）：
```
c1 = 1/sqrt(2) = 0.70710677
w0 = (1, 0, 0)
w1 = (0, 0, 1)
w2 = c1*(1, 1, 0)
w3 = c1*(1, 0, 1)
w4 = c1*(0, 1, 1)
```

**Y 矩阵结构**（5×5，行 = SH 阶数 m，列 = 采样点）：
```
Y = [  Y_{2,-2}(w0)  Y_{2,-2}(w1)  ...  Y_{2,-2}(w4) ]   (m=-2)
    [  Y_{2,-1}(w0)  Y_{2,-1}(w1)  ...  Y_{2,-1}(w4) ]   (m=-1)
    [  Y_{2,0}(w0)   Y_{2,0}(w1)   ...  Y_{2,0}(w4)  ]   (m= 0)
    [  Y_{2,1}(w0)   Y_{2,1}(w1)   ...  Y_{2,1}(w4)  ]   (m= 1)
    [  Y_{2,2}(w0)   Y_{2,2}(w1)   ...  Y_{2,2}(w4)  ]   (m= 2)
```

**Y^{-1} 矩阵推导**：
- 5×5 矩阵的解析逆通过伴随矩阵法得到（或数值计算）
- 非零系数标记为 k0, k1, k2：
```
k0 = 0.9152912
k1 = 1.8305824  (= 2*k0, 重要关系)
k2 = 1.5853308

invY = [  0  -k0   0   k0   k1 ]
       [ k0    0  k2   k0   k0 ]
       [ k1    0   0    0    0 ]
       [  0    0   0  -k1    0 ]
       [  0  -k1   0    0    0 ]
```

**验证方法**（MATLAB）：
```matlab
Y_2_n2=@(v)  1.0925485 * v(1) * v(2);
Y_2_n1=@(v) -1.0925485 * v(2) * v(3);
Y_2_0 =@(v) 0.31539157 * (3 * v(3) * v(3) - 1);
Y_2_1 =@(v) -1.0925485 * v(1) * v(3);
Y_2_2 =@(v) 0.54627424 * (v(1) * v(1) - v(2) * v(2));
Y_2   =@(v) [Y_2_n2(v);Y_2_n1(v);Y_2_0(v);Y_2_1(v);Y_2_2(v)];
c1=1/sqrt(2);
w2_0=[1;0;0];
w2_1=[0;0;1];
w2_2=[c1;c1;0];
w2_3=[c1;0;c1];
w2_4=[0;c1;c1];
Y2=[Y_2(w2_0),Y_2(w2_1),Y_2(w2_2),Y_2(w2_3),Y_2(w2_4)]
invY2=inv(Y2)  % 验证得到上述 Y^{-1}
```

**旋转矩阵计算示例**：
```matlab
R=[0,1,0;0,0,1;1,0,0]';  % 某个旋转矩阵（行优先）
Y2_R=[Y_2(R*w2_0),Y_2(R*w2_1),Y_2(R*w2_2),Y_2(R*w2_3),Y_2(R*w2_4)]
M2_R=invY2*Y2_R  % 得到旋转后的 SH 系数变换矩阵
```

## 3 阶情形（l=3）

**球谐基函数**：
- Y_{3,-3}(x,y,z) = c*x*(5x^2 - 3(y^2 + z^2))，其中 c = 0.5900435
- Y_{3,-2}(x,y,z) = a*x*y*(5z^2 - 1)，其中 a = 2.8906115
- Y_{3,-1}(x,y,z) = b*y*(5z^2 - 1)，其中 b = 0.45704579
- Y_{3,0}(x,y,z) = c2*(5z^3 - 3z)，其中 c2 = 0.3156899
- Y_{3,1}(x,y,z) = b*x*(5z^2 - 1)
- Y_{3,2}(x,y,z) = a*(x^2 - y^2)*(5z^2 - 1)/2
- Y_{3,3}(x,y,z) = c*z*(5z^2 - 3(x^2 + y^2))

**采样点**（7 个线性独立方向）：

关键观察：l=3 SH 函数是奇函数，满足 SH3(m, -v) = -SH3(m, v)，因此对称的采样点集会产生线性相关性。

采样点设计如下（c1 = 1/√2，c2 = 1/√3）：
```
w0 = (1,  0,  0)          = e0
w1 = (0,  1,  0)          = e1
w2 = (0,  0,  1)          = e2
w3 = c1*(1, -1,  0)       = c1*(e0 - e1)   -> 唯一的负号，打破对称性
w4 = c1*(1,  0,  1)       = c1*(e0 + e2)
w5 = c1*(0,  1,  1)       = c1*(e1 + e2)
w6 = c2*(1,  1,  1)       = c2*(e0 + e1 + e2)
```

**关键设计要点**：
- 前 3 个采样点为标准坐标轴
- 第 4 个采样点 w3 = c1*(e0 - e1) 打破了 S3 置换对称性（若使用 c1*(e0 + e1)，则与 e0, e1 线性相关）
- 其余采样点为坐标轴两两和与三轴和的归一化
- 这 7 个方向在 l=3 SH 下张成完整的 7 维空间

**Y 矩阵结构**（7×7，行 = SH 阶数 m，列 = 采样点）：
```
Y = [  Y_{3,-3}(w0)  Y_{3,-3}(w1)  ...  Y_{3,-3}(w6) ]   (m=-3)
    [  Y_{3,-2}(w0)  Y_{3,-2}(w1)  ...  Y_{3,-2}(w6) ]   (m=-2)
    [  Y_{3,-1}(w0)  Y_{3,-1}(w1)  ...  Y_{3,-1}(w6) ]   (m=-1)
    [  Y_{3,0}(w0)   Y_{3,0}(w1)   ...  Y_{3,0}(w6)  ]   (m= 0)
    [  Y_{3,1}(w0)   Y_{3,1}(w1)   ...  Y_{3,1}(w6)  ]   (m=+1)
    [  Y_{3,2}(w0)   Y_{3,2}(w1)   ...  Y_{3,2}(w6)  ]   (m=+2)
    [  Y_{3,3}(w0)   Y_{3,3}(w1)   ...  Y_{3,3}(w6)  ]   (m=+3)
```

**Y^{-1} 矩阵推导**：
- 7×7 矩阵的数值逆（MATLAB 计算，误差 < 1e-15）
- invY 按列展开表示（M 的计算公式）：
```
col 0 (m=-3): k0*Y[0] + k8*Y[1] + k0*Y[2] + k3*Y[3] + k3*Y[4] + k3*Y[5]
col 1 (m=-2): k4*Y[0] + k4*Y[1] + k4*Y[2]                       + k10*Y[6]
col 2 (m=-1): -k1*Y[0] + k6*Y[1] - k1*Y[2] - k5*Y[3] - k5*Y[4] - k5*Y[5]
col 3 (m= 0): k9*Y[2]
col 4 (m=+1): k6*Y[0] - k1*Y[1] - k1*Y[2] + k5*Y[3] - k5*Y[4] - k5*Y[5]
col 5 (m=+2): k2*Y[0] - k2*Y[1]            + k7*Y[3] + k7*Y[4] - k7*Y[5]
col 6 (m=+3): -k8*Y[0] - k0*Y[1] - k0*Y[2]  + k3*Y[3] - k3*Y[4] - k3*Y[5]

其中 11 个不同的系数：
k0   = 0.2118488
k1   = 0.2734956
k2   = 0.3459476
k3   = 0.5991988
k4   = 0.6918951
k5   = 0.7735623
k6   = 0.8204867
k7   = 0.9784875
k8   = 1.0592438
k9   = 1.3398491
k10  = 1.7975963
```

**验证方法**（MATLAB）：
```matlab
Y_3_n3=@(v) 0.5900435 * v(1) * (5*v(1)*v(1) - 3*(v(2)*v(2) + v(3)*v(3)));
Y_3_n2=@(v) 2.8906115 * v(1) * v(2) * (5*v(3)*v(3) - 1);
Y_3_n1=@(v) 0.45704579 * v(2) * (5*v(3)*v(3) - 1);
Y_3_0 =@(v) 0.3156899 * (5*v(3)*v(3)*v(3) - 3*v(3));
Y_3_1 =@(v) 0.45704579 * v(1) * (5*v(3)*v(3) - 1);
Y_3_2 =@(v) 2.8906115 * (v(1)*v(1) - v(2)*v(2)) * (5*v(3)*v(3) - 1) / 2;
Y_3_3 =@(v) 0.5900435 * v(3) * (5*v(3)*v(3) - 3*(v(1)*v(1) + v(2)*v(2)));
Y_3   =@(v) [Y_3_n3(v);Y_3_n2(v);Y_3_n1(v);Y_3_0(v);Y_3_1(v);Y_3_2(v);Y_3_3(v)];

c1=1/sqrt(2);
c2=1/sqrt(3);
w3_0=[1;0;0];
w3_1=[0;1;0];
w3_2=[0;0;1];
w3_3=[c1;-c1;0];
w3_4=[c1;0;c1];
w3_5=[0;c1;c1];
w3_6=[c2;c2;c2];
Y3=[Y_3(w3_0),Y_3(w3_1),Y_3(w3_2),Y_3(w3_3),Y_3(w3_4),Y_3(w3_5),Y_3(w3_6)]
invY3=inv(Y3)  % 验证得到上述 Y^{-1}
```

**旋转矩阵计算示例**：
```matlab
R=[0,1,0;0,0,1;1,0,0]';  % 某个旋转矩阵（行优先）
Y3_R=[Y_3(R*w3_0),Y_3(R*w3_1),Y_3(R*w3_2),Y_3(R*w3_3),Y_3(R*w3_4),Y_3(R*w3_5),Y_3(R*w3_6)]
M3_R=invY3*Y3_R  % 得到旋转后的 SH 系数变换矩阵
```

## 4 阶情形（l=4）

**球谐基函数**：
- Y_{4,-4}(x,y,z) = 2.5033429 * xy(x²-y²)
- Y_{4,-3}(x,y,z) = -1.7701308 * y(3x²-y²)z
- Y_{4,-2}(x,y,z) = 0.94617470 * xy(7z²-1)
- Y_{4,-1}(x,y,z) = 0.66904650 * y(7z²-3)z
- Y_{4, 0}(x,y,z) = 0.10578555 * (35z⁴-30z²+3)
- Y_{4,+1}(x,y,z) = -0.66904650 * x(7z²-3)z
- Y_{4,+2}(x,y,z) = 0.47308734 * (x²-y²)(7z²-1)
- Y_{4,+3}(x,y,z) = -1.7701308 * x(x²-3y²)z
- Y_{4,+4}(x,y,z) = 0.62583572 * (x⁴-6x²y²+y⁴)

**采样点**（9 个线性独立方向）：

关键观察：l=4 的所有 SH 基函数都是**偶函数**（`SH4(-v) = SH4(v)`），并且：
- 轴对齐点仅能激活 m=0, ±2, ±4（共 5 个），其余 4 个基函数在轴对齐点上恒为零；
- `m=-4 = 2.5033·xy(x²-y²)` 在 `x=y` 时恒为零，而常见的面对角线方向（如 `(1,1,0)/√2`、`(1,1,1)/√3`）均满足 `x=y` 或 `x²=y²`；
- 因此**必须**引入满足 `x≠y` 的方向才能激活 m=-4，例如 `(2,1,0)/√5`。

采样点设计如下（c1 = 1/√2，c2 = 1/√5）：
```
w0 = (1, 0, 0)              = e0
w1 = (0, 1, 0)              = e1
w2 = (0, 0, 1)              = e2
w3 = c1*(1, 1, 0)           = c1*(e0+e1)
w4 = c1*(1, 0, 1)           = c1*(e0+e2)
w5 = c1*(0, 1, 1)           = c1*(e1+e2)
w6 = c2*(2, 1, 0)           = c2*(2e0+e1)   -> 打破 x-y 对称性，激活 m=-4
w7 = c2*(0, 1, 2)           = c2*(e1+2e2)
w8 = c2*(1, 0, 2)           = c2*(e0+2e2)
```

**关键设计要点**：
- 前 3 个为标准坐标轴，w3~w5 为面对角线（√2 归一化）；
- w6~w8 为 `(2,1,0)` 型方向（√5 归一化），保证 `x²-y² = 3/5 ≠ 0`，从而激活 m=-4 列；
- 三类方向的归一化系数仅涉及 1、1/√2、1/√5，便于 C++ 中以线性组合计算旋转后的采样点。

**invY^{-1} 矩阵推导**：
- 数值逆（Python/NumPy 计算，误差 < 5e-16）
- invY 按列展开表示（M 的计算公式）：
```
col 0 (m=-4): k8*Y[0]  + k14*Y[1] + k15*Y[2] + k4*Y[3]  + k28*Y[6]
col 1 (m=-3): k3*Y[0]  + k21*Y[1] + k24*Y[2] + k29*Y[5] + k10*Y[7]
col 2 (m=-2): k6*Y[0]  + k6*Y[1]  + k27*Y[2] + k1*Y[3]
col 3 (m=-1): k13*Y[0] + k17*Y[1] + k11*Y[2] + k11*Y[5] + k26*Y[7]
col 4 (m= 0): k23*Y[2]
col 5 (m=+1): k12*Y[0] + k16*Y[1] + k18*Y[2] + k18*Y[4] + k2*Y[8]
col 6 (m=+2): k6*Y[0]  + k22*Y[1]
col 7 (m=+3): k7*Y[0]  + k25*Y[1] + k5*Y[2]  + k0*Y[4]  + k19*Y[8]
col 8 (m=+4): k20*Y[0] + k20*Y[1] + k9*Y[2]

其中 30 个不同的系数（Y[k] = Yrws[k][row]）：
k0  = -2.4480300175
k1  = -2.1137745492
k2  = -1.5569421059
k3  = -1.5535575111
k4  = -1.3315528341
k5  = -1.3181700094
k6  = -1.0568872970
k7  = -0.9650887569
k8  = -0.9320869839
k9  = -0.5991987802
k10 = -0.5884687542
k11 = -0.4982214739
k12 = -0.4359437897
k13 = -0.3736661054
k14 =  0.0665776417
k15 =  0.1997329251
k16 =  0.3736661054
k17 =  0.4359437897
k18 =  0.4982214739
k19 =  0.5884687542
k20 =  0.7989317069
k21 =  0.9650887569
k22 =  1.0568872970
k23 =  1.1816358661
k24 =  1.3181700094
k25 =  1.5535575111
k26 =  1.5569421059
k27 =  1.5853309119
k28 =  1.6644410427
k29 =  2.4480300175
```

注意 invY 的稀疏结构：m=0 列仅 1 项，m=±2 列仅 2 项，其余列 4~5 项，共 35/81 个非零元素。

**验证方法**（Python）：
```python
import numpy as np
from scipy.linalg import inv

def SH4(v, m):
    x, y, z = v[0], v[1], v[2]
    if m == -4: return 2.5033429 * x * y * (x*x - y*y)
    elif m == -3: return -1.7701308 * y * (3*x*x - y*y) * z
    elif m == -2: return 0.94617470 * x * y * (7*z*z - 1)
    elif m == -1: return 0.66904650 * y * (7*z*z - 3) * z
    elif m ==  0: return 0.10578555 * (35*z**4 - 30*z*z + 3)
    elif m == +1: return -0.66904650 * x * (7*z*z - 3) * z
    elif m == +2: return 0.47308734 * (x*x - y*y) * (7*z*z - 1)
    elif m == +3: return -1.7701308 * x * (x*x - 3*y*y) * z
    elif m == +4: return 0.62583572 * (x**4 - 6*x*x*y*y + y**4)

c1 = 1/np.sqrt(2)
c2 = 1/np.sqrt(5)
pts_raw = [
    [1,0,0], [0,1,0], [0,0,1],
    [1,1,0], [1,0,1], [0,1,1],
    [2,1,0], [0,1,2], [1,0,2]
]
pts = [np.array(p, dtype=float)/np.linalg.norm(p) for p in pts_raw]

Y = np.zeros((9, 9))
for j, w in enumerate(pts):
    for i, m in enumerate(range(-4, 5)):
        Y[i, j] = SH4(w, m)

print(f"Rank: {np.linalg.matrix_rank(Y)}")  # 应为 9
invY = inv(Y)
print(f"Error: {np.max(np.abs(Y @ invY - np.eye(9))):.2e}")  # 应 < 1e-15
```

**旋转矩阵计算示例**（Python）：
```python
# 绕 X 轴旋转 90 度
R = np.array([[1,0,0],[0,0,-1],[0,1,0]], dtype=float)

# 旋转后的采样点
Rpts = [R @ p for p in pts]
YR = np.zeros((9, 9))
for j, w in enumerate(Rpts):
    for i, m in enumerate(range(-4, 5)):
        YR[i, j] = SH4(w, m)

M = YR @ invY  # 9x9 旋转矩阵

# 验证：M * Y(w) == Y(R*w)
w_test = np.array([1, 2, 3], dtype=float)
w_test /= np.linalg.norm(w_test)
Yw = np.array([SH4(w_test, m) for m in range(-4, 5)])
YRw = np.array([SH4(R @ w_test, m) for m in range(-4, 5)])
print(f"Max error: {np.max(np.abs(M @ Yw - YRw)):.2e}")  # 应 < 1e-14
```

## 通用递推算法（l ≥ 5）

对于 l ≥ 5（Band6 及以上），手工推导 invY 系数的工作量极大（l=4 已需要 30 个系数），因此改用 **Ivanic & Ruedenberg（1996）递推算法**，直接由低阶旋转矩阵 M[l-1] 递推出 M[l]，无需采样点和矩阵求逆。

### 算法来源

- **论文**：*"Rotation Matrices for Real Spherical Harmonics. Direct Determination by Recursion"*，J. Ivanic & K. Ruedenberg，*J. Phys. Chem.*，1996，DOI: 10.1021/jp953350u（含 1998 勘误）
- **开源参考实现**：[google/spherical-harmonics](https://github.com/google/spherical-harmonics)（`sh/spherical_harmonics.cc`）

### 算法概述

**目标**：给定 3×3 旋转矩阵 R，逐 band 计算 M[0], M[1], M[2], ..., M[l]，其中 M[l] 是 (2l+1)×(2l+1) 的实球谐旋转矩阵。

**初始化**：
- `M[0] = [1]`（标量，trivial）
- `M[1]` 直接来自旋转矩阵 R，但需按实球谐排序 m = -1, 0, +1：

```
M[1](m, n)  =  R 在实球谐基下的矩阵元素
```

具体地，用 R 的分量 R[x,y,z] 按如下对应关系填充（`r[1]` 即 M[1]）：

| (m, n) | 值 |
|--------|----|
| (-1,-1) | R[y,y] | (-1, 0) | R[y,z] | (-1,+1) | R[y,x] |
| ( 0,-1) | R[z,y] | ( 0, 0) | R[z,z] | ( 0,+1) | R[z,x] |
| (+1,-1) | R[x,y] | (+1, 0) | R[x,z] | (+1,+1) | R[x,x] |

**递推步骤**：对每个 l ≥ 2，逐一计算 M[l] 的每个元素 `M[l](m, n)`（其中 -l ≤ m, n ≤ l）：

```
M[l](m, n) = u(m,n,l) * U(m,n,l) + v(m,n,l) * V(m,n,l) + w(m,n,l) * W(m,n,l)
```

---

### 标量系数 u, v, w

这三个系数只依赖整数 m, n, l（纯代数，无超越函数）：

```
d = KroneckerDelta(m, 0)          // d=1 当 m=0，否则 d=0
denom = |n|==l ? 2l(2l-1) : (l+n)(l-n)

u(m,n,l) = sqrt( (l+m)(l-m) / denom )

v(m,n,l) = 0.5 * sqrt( (1+d)(l+|m|-1)(l+|m|) / denom ) * (1 - 2d)

w(m,n,l) = -0.5 * sqrt( (l-|m|-1)(l-|m|) / denom ) * (1 - d)
```

注意：
- 当 `|m| == l` 时，w 的根号内 `(l-l-1)(l-l) = (-1)·0 = 0`，故 `w = 0`
- 当 `m = 0` 时，d=1，故 `w = 0`（即 m=0 行无 W 项）

---

### 矩阵贡献项 U, V, W

这三项由辅助函数 `P(i, a, b, l)` 构成，而 `P` 读取 M[1]（即旋转矩阵 R）和已计算好的 M[l-1]：

#### P 函数定义

设 `r[1]` = M[1]（3×3，索引 -1,0,+1），`r[l-1]` = M[l-1]（用 "centered index"，即下标偏移 l-1 使 m 从 -(l-1) 到 l-1）：

```
P(i, a, b, l):
  if b == l:
    return r[1](i, +1) * r[l-1](a, l-1) - r[1](i, -1) * r[l-1](a, -(l-1))
  elif b == -l:
    return r[1](i, +1) * r[l-1](a, -(l-1)) + r[1](i, -1) * r[l-1](a, l-1)
  else:
    return r[1](i, 0) * r[l-1](a, b)
```

其中 `i ∈ {-1, 0, +1}` 对应旋转矩阵 R 的 y, z, x 列。

#### U 函数

```
U(m, n, l) = P(0, m, n, l)
```

即使用 `r[1](0, ·)` = R 的 z 行。

#### V 函数（依赖 m 的符号）

```
V(m, n, l):
  if m == 0:
    return P(+1, +1, n, l) + P(-1, -1, n, l)
  elif m > 0:
    d1 = KroneckerDelta(m, 1)
    return P(+1, m-1, n, l) * sqrt(1 + d1) - P(-1, -(m-1), n, l) * (1 - d1)
  else:  // m < 0
    d1 = KroneckerDelta(m, -1)
    return P(+1, m+1, n, l) * (1 - d1) + P(-1, -(m+1), n, l) * sqrt(1 + d1)
```

#### W 函数（依赖 m 的符号，m=0 时恒为 0）

```
W(m, n, l):
  if m == 0:
    return 0.0
  elif m > 0:
    return P(+1, m+1, n, l) + P(-1, -(m+1), n, l)
  else:  // m < 0
    return P(+1, m-1, n, l) - P(-1, -(m-1), n, l)
```

---

### C++ 参考实现

以下是基于 [google/spherical-harmonics](https://github.com/google/spherical-harmonics) 的精简实现框架（去除 Eigen 依赖，改为裸数组）：

```cpp
// 带中心索引的矩阵访问：M 是 (2*l+1) x (2*l+1)，m/n 范围 [-l, l]
static float GetElem(const float* M, int l, int m, int n) {
    return M[(m + l) * (2 * l + 1) + (n + l)];
}

// P(i, a, b, l, r1, prev) —— r1: M[1] (3x3), prev: M[l-1]
static float P(int i, int a, int b, int l, const float* r1, const float* prev) {
    // r1 是 3x3，用 GetElem(r1, 1, i, j) 访问
    if (b == l) {
        return GetElem(r1, 1, i, 1) * GetElem(prev, l-1, a, l-1)
             - GetElem(r1, 1, i,-1) * GetElem(prev, l-1, a,-(l-1));
    } else if (b == -l) {
        return GetElem(r1, 1, i, 1) * GetElem(prev, l-1, a,-(l-1))
             + GetElem(r1, 1, i,-1) * GetElem(prev, l-1, a, l-1);
    } else {
        return GetElem(r1, 1, i, 0) * GetElem(prev, l-1, a, b);
    }
}

static float U(int m, int n, int l, const float* r1, const float* prev) {
    return P(0, m, n, l, r1, prev);
}

static float V(int m, int n, int l, const float* r1, const float* prev) {
    if (m == 0) {
        return P(1, 1, n, l, r1, prev) + P(-1, -1, n, l, r1, prev);
    } else if (m > 0) {
        float d1 = (m == 1) ? 1.0f : 0.0f;
        return P(1, m-1, n, l, r1, prev) * std::sqrt(1.0f + d1)
             - P(-1,-(m-1), n, l, r1, prev) * (1.0f - d1);
    } else {
        float d1 = (m == -1) ? 1.0f : 0.0f;
        return P(1, m+1, n, l, r1, prev) * (1.0f - d1)
             + P(-1,-(m+1), n, l, r1, prev) * std::sqrt(1.0f + d1);
    }
}

static float W(int m, int n, int l, const float* r1, const float* prev) {
    if (m == 0) return 0.0f;
    else if (m > 0)
        return P(1, m+1, n, l, r1, prev) + P(-1,-(m+1), n, l, r1, prev);
    else
        return P(1, m-1, n, l, r1, prev) - P(-1,-(m-1), n, l, r1, prev);
}

// 计算标量系数 u, v, w
static void ComputeUVWCoeff(int m, int n, int l, float& u, float& v, float& w) {
    float d = (m == 0) ? 1.0f : 0.0f;
    float denom = (std::abs(n) == l)
                  ? float(2 * l * (2 * l - 1))
                  : float((l + n) * (l - n));
    u = std::sqrt(float((l + m) * (l - m)) / denom);
    v = 0.5f * std::sqrt((1.0f + d) * float((l + std::abs(m) - 1) * (l + std::abs(m))) / denom)
            * (1.0f - 2.0f * d);
    w = -0.5f * std::sqrt(float((l - std::abs(m) - 1) * (l - std::abs(m))) / denom)
             * (1.0f - d);
}

// 计算第 l 阶旋转矩阵（写入 out，大小 (2l+1)^2）
// r1: M[1]（3x3），prev: M[l-1]（(2l-1)^2）
void ComputeBandRotation(int l, const float* r1, const float* prev, float* out) {
    int sz = 2 * l + 1;
    for (int m = -l; m <= l; m++) {
        for (int n = -l; n <= l; n++) {
            float u, v, w;
            ComputeUVWCoeff(m, n, l, u, v, w);
            float val = 0.0f;
            if (u != 0.0f) val += u * U(m, n, l, r1, prev);
            if (v != 0.0f) val += v * V(m, n, l, r1, prev);
            if (w != 0.0f) val += w * W(m, n, l, r1, prev);
            out[(m + l) * sz + (n + l)] = val;
        }
    }
}
```

**使用方式**：

```cpp
// 1. 构建 M[1]（3x3）：直接用旋转矩阵 R
float r1[9];  // M[1]，按 (m+1)*3+(n+1)，m,n ∈ {-1,0,+1}
// 对应关系：r1(-1,-1)=R[y,y], r1(-1,0)=R[y,z], r1(-1,+1)=R[y,x]
//           r1( 0,-1)=R[z,y], r1( 0, 0)=R[z,z], r1( 0,+1)=R[z,x]
//           r1(+1,-1)=R[x,y], r1(+1, 0)=R[x,z], r1(+1,+1)=R[x,x]

// 2. 逐阶递推
float prev[25];  // M[l-1]，按需分配
float curr[49];  // M[l]
// ComputeBandRotation(2, r1, prev, curr) -> M[2] (5x5)
// ComputeBandRotation(3, r1, curr, next) -> M[3] (7x7)
// ... 以此类推
```

---

### 复杂度分析

| 操作 | 复杂度 |
|------|--------|
| 计算单个 M[l] | O(l²) 个元素，每个元素 O(1) |
| 计算所有 M[0..L] | O(L³) 总体 |
| 存储所有 M[0..L] | O(L³) 字节 |

---

### 与 invY 方法的对比

| 特性 | invY 方法（Band2~5） | 递推方法（Band6+） |
|------|---------------------|-------------------|
| 实现复杂度 | 需手工推导/数值求逆 | 统一代码，无需预计算 |
| 运行时开销 | 极低（固定系数，稀疏） | 略高（O(l²) per band，但仍快速） |
| 数值精度 | 精确（float 系数） | 递推，误差随 l 略微累积 |
| 适用范围 | l ≤ 4（Band5 以内） | l ≥ 2（任意阶，向后兼容） |
| 代码可维护性 | 每阶需单独函数 | 一套代码覆盖所有阶 |

推荐策略：**l ≤ 4 使用 invY 方法（性能最优），l ≥ 5 使用递推方法（通用性）**。

## 研究方法总结

### 一般框架（适用所有阶）

对于 l 阶球谐旋转，通用方法为：

**1. 采样点选择**：
   - 选择 (2l+1) 个线性独立的三维方向
   - 要求：这些方向在 l 阶 SH 空间中张成完整的 (2l+1) 维子空间
   - 策略：优先使用轴对齐方向，然后选择低阶组合避免线性相关

**2. Y 矩阵构造**：
   - 行 = SH 基函数（共 2l+1 个）
   - 列 = 采样方向（共 2l+1 个）
   - 元素：Y[i,j] = SH_l^m(w_j)，其中 m = i - l

**3. 逆矩阵求解**：
   - 解析求逆（低阶）：使用伴随矩阵法或 Cramer 法则
   - 数值求逆（高阶）：使用 MATLAB/Python 数值线性代数库，精度验证 < 1e-15

**4. 旋转矩阵公式**：
   - M = Y_R * invY，其中 Y_R = Y(R*w)
   - 代码实现：逐行计算 M[row, col] = sum_k Yrws[k][row] * invY[k, col]

**5. 线性性优化**：
   - 利用向量加法的线性性，将组合方向表示为基方向的线性组合
   - 例：R * (c1*e0 + c1*e1) = c1*R*e0 + c1*R*e1 = c1*(Rws[0] + Rws[1])

### 各阶对比

| 阶数 | l | 维数 | 采样点数 | 归一化系数 | invY 系数数 | 主要挑战 | 解决方案 |
|------|---|------|---------|-----------|------------|---------|---------|
| Band2 | 1 | 3 | 3 | — | — | — | 直接使用坐标轴 |
| Band3 | 2 | 5 | 5 | 1/√2 | 3 | — | 轴 + 面对角线 |
| Band4 | 3 | 7 | 7 | 1/√2, 1/√3 | 11 | 奇函数对称性 | 打破对称：w3 = c1*(e0-e1) |
| Band5 | 4 | 9 | 9 | 1/√2, 1/√5 | 30 | 偶函数 + m=-4 零化 | 引入 (2,1,0)/√5 型方向 |

## 代码中 Rws 的含义（重要）

在 C++ 代码中计算旋转矩阵时，需要提取旋转矩阵 R 作用在采样点上的结果：
```
Rws[j] = R * w_j
```

由于 R 是**行优先矩阵**存储（`RotateMatrix.Rows[i]` = 第 i 行），而 `R * w_j` 等价于 R 的第 j 列（在数学意义上），所以：

**关键发现**：
```
R * w_j = (R^T).Rows[j]
```

即：**R 左乘向量得到的结果，等于 R 转置的第 j 行**。

这意味着我们可以通过一次矩阵转置，然后直接访问转置矩阵的行来高效提取所需列：

```cpp
const FMatrix3x3f RT = RotateMatrix.Transpose();
const FVector3f Rws[3] = {
    RT.Rows[0],  // R * w0 = first column of R
    RT.Rows[1],  // R * w1 = second column of R
    RT.Rows[2],  // R * w2 = third column of R
};
```

对于非轴对齐的采样点（如 Band3 中的 w2, w3, w4），利用线性性质：
```cpp
const FMatrix3x3f RT = RotateMatrix.Transpose();
const float c1 = 0.70710677f;  // 1/sqrt(2)
const FVector3f Rws[5] = {
    RT.Rows[0],                      // R * w0
    RT.Rows[2],                      // R * w1
    (RT.Rows[0] + RT.Rows[1]) * c1,  // R * w2 = c1 * (col0 + col1)
    (RT.Rows[0] + RT.Rows[2]) * c1,  // R * w3 = c1 * (col0 + col2)
    (RT.Rows[1] + RT.Rows[2]) * c1,  // R * w4 = c1 * (col1 + col2)
};
```

对于 Band4 采样点的实现：
```cpp
const FMatrix3x3f RT = RotateMatrix.Transpose();
const float c1 = 0.70710677f;  // 1/sqrt(2)
const float c2 = 0.57735027f;  // 1/sqrt(3)
const FVector3f Rws[7] = {
    RT.Rows[0],                                        // R * w0 = e0 column
    RT.Rows[1],                                        // R * w1 = e1 column
    RT.Rows[2],                                        // R * w2 = e2 column
    (RT.Rows[0] - RT.Rows[1]) * c1,                   // R * w3 = c1*(e0-e1)
    (RT.Rows[0] + RT.Rows[2]) * c1,                   // R * w4 = c1*(e0+e2)
    (RT.Rows[1] + RT.Rows[2]) * c1,                   // R * w5 = c1*(e1+e2)
    (RT.Rows[0] + RT.Rows[1] + RT.Rows[2]) * c2,      // R * w6 = c2*(e0+e1+e2)
};
```

对于 Band5 采样点的实现：
```cpp
const FMatrix3x3f RT = RotateMatrix.Transpose();
const float c1 = 0.70710678f;  // 1/sqrt(2)
const float c2 = 0.44721360f;  // 1/sqrt(5)
const FVector3f Rws[9] = {
    RT.Rows[0],                                              // R * w0 = e0
    RT.Rows[1],                                              // R * w1 = e1
    RT.Rows[2],                                              // R * w2 = e2
    (RT.Rows[0] + RT.Rows[1]) * c1,                         // R * w3 = c1*(e0+e1)
    (RT.Rows[0] + RT.Rows[2]) * c1,                         // R * w4 = c1*(e0+e2)
    (RT.Rows[1] + RT.Rows[2]) * c1,                         // R * w5 = c1*(e1+e2)
    (RT.Rows[0] * 2.0f + RT.Rows[1]) * c2,                  // R * w6 = c2*(2e0+e1)
    (RT.Rows[1] + RT.Rows[2] * 2.0f) * c2,                  // R * w7 = c2*(e1+2e2)
    (RT.Rows[0] + RT.Rows[2] * 2.0f) * c2,                  // R * w8 = c2*(e0+2e2)
};
```

这种方法相比逐元素手动提取列要更简洁，代码也更容易维护。

## 应用实例

另外，在坐标系变换中，如果不是很清楚旋转变换 ![img](https://cdn.nlark.com/yuque/__latex/1227f701376733061841f20c2cacfce0.svg) 是什么，但有一个旋转变换函数![img](https://cdn.nlark.com/yuque/__latex/0ee8f3eda530dbc776b1dfe8c2106dd5.svg)，那么其实也可以用类似方法推导出旋转变换矩阵，即

![img](https://cdn.nlark.com/yuque/__latex/a5e25f27f3ca21505bef6abf607c96d1.svg)

为了简单，方向取坐标轴即可

**实例**

对于UE烘焙得到的球谐，想要给 Unity 用的话，在不变换球谐的情况下，需要将 Unity 的方向旋转变换到 UE 的坐标系中，然后再计算球谐。

因此这个变换是 **Unity->UE**，现在有了变化函数，那就可以先求变换矩阵，然后就可以求旋转变换后的球谐系数，从而后续不再需要对方向进行坐标变化了

具体变换为

[1;0;0] -> [0;1;0]

[0;1;0] -> [0;0;1]

[0;0;1] -> [1;0;0]

R=[0,1,0;0,0,1;1,0,0]';