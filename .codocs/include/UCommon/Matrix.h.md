# Matrix.h

## 职责

行主序矩阵类型，提供 3x3 和 4x4 矩阵模板，包含变换、求逆、分解等操作。

## 关键抽象

### `TMatrix3x3<T>`
- 行主序存储，3 行 `TVector<T>`
- 访问：`operator[]`（行）、`operator()`（行列）、`GetData()`
- 运算：矩阵乘法、矩阵×向量（列向量）、2D 点/向量变换
- 工厂：`Identity()` / `Zero()` / `Rotation(Axis, Angle)` / `RotationX/Y/Z` / `Scaling`
- 分解：`Transpose()` / `Determinant()` / `Trace()` / `Inverse()`

### `TMatrix4x4<T>`
- 行主序存储，4 行 `TVector4<T>`
- `operator*` 对 `TVector<T>` 自动补 w=1（点变换），`TransformVector` 补 w=0（方向变换）
- 工厂：增加 `Translation` / `TRS`（平移+旋转+缩放一步到位）
- `Inverse()` — **仿射变换优化**：通过 3x3 逆矩阵 + 平移反算，非通用 4x4 求逆
- 分解：`GetRotation3x3()` / `GetTranslation()` / `GetScale()`

### 类型别名
- `FMatrix3x3f` / `FMatrix3x3d` / `FMatrix4x4f` / `FMatrix4x4d`

## 注意事项
- **行主序**：`operator*` 对向量是左乘 `M * v`
- `TMatrix4x4::Inverse()` 假设仿射变换（最后一行为 `[0,0,0,w]`），非通用矩阵会得到错误结果
- `TMatrix4x4::Determinant()` 也是简化版，仅用左上 3x3 余子式 × `Rows[3].W`
- 向量点积用 `operator|`（来自 `Vector.h`）

## 相关文件
- `Vector.h` — TVector / TVector4 类型
