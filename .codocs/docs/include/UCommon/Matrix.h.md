---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/Matrix.h
  source_hash: sha256:977287556a024cb262e822b46adea8a65da6a7ac7524bb32e4416023538c34f5
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# Matrix.h

## 职责

行主序矩阵类型，提供 3x3 和 4x4 矩阵模板，包含变换、求逆、分解等操作。

## 关键抽象

### `TMatrix3x3<T>`

行主序存储，底层是 `TVector<T> Rows[3]`，连续内存，`GetData()` 返回 `Rows[0]` 的首地址，可直接传给需要 float* 的 API。

**构造**：默认构造**不初始化**；单值构造 `TMatrix3x3(T Value)` 将所有行填充为 `TVector<T>(Value)`（非单位矩阵）。

**访问**：`operator[]` 返回整行引用；`operator()(Row, Col)` 直接访问元素，均有 ASSERT 越界检查。

**矩阵乘矩阵实现**：先转置右矩阵，再逐行点积——利用行向量点积规避列访问的内存跳跃。

**向量变换区分**：
- `operator*(TVector2)` — 补 w=1，变换 2D **点**（受平移影响）
- `TransformVector(TVector2)` — 补 w=0，变换 2D **方向**（不受平移影响）
- `operator*(TVector<T>)` — 直接 3D 向量乘

**Inverse()**：伴随矩阵法，奇异时返回 Zero() 而非抛异常，调用方须自行判零。

### `TMatrix4x4<T>`

底层是 `TVector4<T> Rows[4]`，平移存于**各行的 W 分量**（即第4列）：`Rows[0].W = tx, Rows[1].W = ty, Rows[2].W = tz`。这是行主序下"平移在右列"的标准布局。

**向量变换区分**：
- `operator*(TVector<T>)` — 补 w=1，变换**点**，丢弃 w 分量
- `operator*(TVector4<T>)` — 完整 4D 乘
- `TransformVector(TVector<T>)` — 补 w=0，变换**方向**

**SetRotation / SetTRS**：就地修改版工厂，避免额外拷贝。`Rotation(Axis, Angle)` 内部委托 `SetRotation`。

**TRS 实现**：`SetTRS` 把 Scale 直接烘焙进旋转矩阵各列（等价于 T*R*S 但无矩阵连乘开销）。

**GetRotation3x3()**：提取左上 3x3，**含缩放**，若需纯旋转须先 `GetScale()` 归一化。

**Determinant()**：仅计算左上 3x3 × `Rows[3].W`，**非完整 4x4 展开**，对非仿射矩阵结果错误。Assert 检查 `Rows[3] == (0,0,0,1)`。

**Inverse()**：仿射专用：3x3 求逆 + 逆平移 `-LinearInv * t`。Assert 检查底行为 `(0,0,0,1)`。

### 类型别名

`FMatrix3x3f` / `FMatrix3x3d` / `FMatrix4x4f` / `FMatrix4x4d`

### 比较运算符

`operator==` / `operator!=` 均定义为自由函数，逐行调用 `TVector` 的 `==`，是精确比较（无容差），主要用于奇异性判断（与 Zero() 比较）。

## 命名空间注入宏

`UBPA_UCOMMON_MATRIX_TO_NAMESPACE(NS)` 将模板类和4个别名注入目标命名空间（以 `using` 形式），已对 `UCommonTest` 调用，跨命名空间使用无需手写 `using`。

## 注意事项

- **行主序**：`M * v` 是行向量左乘，与 GLSL（列主序）相反
- `TMatrix3x3()` 默认不初始化，未赋值即使用是典型 UB 来源
- `TMatrix4x4::Inverse()` / `Determinant()` 仅适用于仿射变换（assert 检查底行）
- `GetRotation3x3()` 含缩放，误当纯旋转会引入缩放误差
- 奇异性检测：3x3 用 det 阈值；4x4 Inverse() 用 `LinearInv == Zero()`

## 相关文件

- `Vector.h` — `TVector` / `TVector4` 类型，`operator|` 点积
- `Matrix.inl` — 较长的方法体实现（由本文件末尾 `#include` 引入）
