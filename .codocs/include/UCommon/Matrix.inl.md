# Matrix.inl

## 职责

`TMatrix3x3` 和 `TMatrix4x4` 中较长的方法体的类外实现，由 `Matrix.h` 末尾的 `#include "Matrix.inl"` 引入。

## 内容

### TMatrix3x3

| 函数 | 说明 |
|------|------|
| `operator*(TMatrix3x3)` | 矩阵乘矩阵：先转置右矩阵再逐行点积 |
| `Rotation(Axis, Angle)` | Rodrigues 公式构造旋转矩阵 |
| `RotationX/Y/Z(Angle)` | 绕坐标轴旋转 |
| `Inverse()` | 伴随矩阵法，奇异时返回 Zero() |

### TMatrix4x4

| 函数 | 说明 |
|------|------|
| `operator*(TMatrix4x4)` | 矩阵乘矩阵：先转置右矩阵再逐行点积 |
| `RotationX/Y/Z(Angle)` | 绕坐标轴旋转 |
| `SetRotation(Axis, Angle)` | 就地写入旋转，保留 w 列和 Rows[3] |
| `SetTRS(Trans, Axis, Angle, Scale)` | TRS 烘焙：Scale 直接乘入旋转列，避免矩阵连乘 |
| `Determinant()` | 仿射专用：`Rows[3].W * det(left 3x3)` |
| `Inverse()` | 仿射专用：3x3 逆 + 逆平移 `-LinearInv * t` |

## 注意事项

- 本文件是纯实现，不定义任何新类型或接口
- 所有函数均与 `Matrix.h` 中的声明一一对应
