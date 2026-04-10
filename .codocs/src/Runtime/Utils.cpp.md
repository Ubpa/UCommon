# Utils.cpp

## 职责

通用工具函数，包含纹理寻址模式和矩阵运算。

## 关键抽象

- `ApplyAddressMode` — 纹理坐标寻址模式实现（Wrap/Clamp/Mirror），支持 float 和 int64 坐标两种版本
- `MatrixMulVec` — 通用 N×N 行主序矩阵与向量相乘
