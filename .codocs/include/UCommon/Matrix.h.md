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

**Inverse()**：伴随矩阵法（余子式 / det），检测阈值 `UBPA_UCOMMON_DELTA`，奇异时**返回 Zero() 而非抛异常**，调用方须自行判零。

### `TMatrix4x4<T>`

底层是 `TVector4<T> Rows[4]`，平移存于**各行的 W 分量**（即第4列）：`Rows[0].W = tx, Rows[1].W = ty, Rows[2].W = tz`。这是行主序下"平移在右列"的标准布局。

**向量变换区分**：
- `operator*(TVector<T>)` — 补 w=1，变换**点**，丢弃 w 分量
- `operator*(TVector4<T>)` — 完整 4D 乘
- `TransformVector(TVector<T>)` — 补 w=0，变换**方向**

**SetRotation / SetTRS**：就地修改版工厂，避免额外拷贝。`Rotation(Axis, Angle)` 内部委托 `SetRotation`。

**TRS 实现**：`SetTRS` 不是三矩阵连乘，而是把 Scale **直接烘焙进旋转矩阵各列**（每列 *= 对应 Scale 分量），平移写入 W 列，一次完成——等价于 T * R * S 但零矩阵乘开销。

**GetRotation3x3()**：提取左上 3x3，**包含缩放**，不是纯旋转矩阵，若需纯旋转须先用 `GetScale()` 归一化各行。

**GetScale()**：取各行前三分量的向量长度，仅在无剪切时正确。

**Determinant()**：简化实现，仅计算左上 3x3 行列式 × `Rows[3].W`，非完整 4x4 展开，对非仿射矩阵结果错误。

**Inverse()**：仿射变换专用：提取左上 3x3 求逆，奇异性检测用 `LinearInv == Zero()`（与 3x3 的 det 阈值路径不同），然后计算逆平移 `-LinearInv * t`，保留 `Rows[3].W`。

### 类型别名

`FMatrix3x3f` / `FMatrix3x3d` / `FMatrix4x4f` / `FMatrix4x4d`

### 比较运算符

`operator==` / `operator!=` 均定义为自由函数，逐行调用 `TVector` 的 `==`，是精确比较（无容差），主要用于奇异性判断（与 Zero() 比较）。

## 命名空间注入宏

`UBPA_UCOMMON_MATRIX_TO_NAMESPACE(NS)` 将模板类和4个别名注入目标命名空间（以 `using` 形式），已对 `UCommonTest` 调用，跨命名空间使用无需手写 `using`。

## 注意事项

- **行主序**：`M * v` 是行向量左乘，与 GLSL（列主序）相反
- `TMatrix3x3()` 默认不初始化，拿到随机值后做计算是典型 UB 来源
- `TMatrix4x4::Inverse()` 和 `Determinant()` 均**只适用于仿射变换**
- `GetRotation3x3()` 含缩放，若误当纯旋转使用会引入缩放误差
- **奇异性检测**：3x3 用 `det < UBPA_UCOMMON_DELTA`；4x4 Inverse() 用 `LinearInv == Zero()`——两者路径不同
- **非仿射矩阵**调用 `Inverse()` 或 `Determinant()` 会得到错误结果

## 相关文件

- `Vector.h` — `TVector` / `TVector4` 类型，`operator|` 点积
- `Matrix.inl` — 较长的方法体实现（由本文件末尾 `#include` 引入）
