---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/Matrix.inl
  source_hash: sha256:b89ad2776e4706efeb1f3249ddb2925a9c2717eb930592712a91283b1b821e18
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
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
