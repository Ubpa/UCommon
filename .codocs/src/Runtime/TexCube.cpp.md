# TexCube.cpp

## 职责

立方体贴图（Cubemap）核心实现，包括面坐标↔方向向量转换和等距柱状投影（Equirectangular）互转。

## 关键抽象

- `EquirectangularDirectionToUV / UVToDirection` — 方向向量与 equirectangular UV 坐标互转
- `FCubeTexcoord` — 立方体纹理坐标（Face + UV），可从方向向量构造或转换回方向
- `FCubePoint / FGridCube` — 立方体网格点和迭代器，支持展平为 FGrid2D（6 面纵向拼接）
- `FTexCube` — 立方体贴图容器，内部用一个展平的 FTex2D 存储（height = faceHeight × 6）

## 实现要点

- Cubemap 面朝向约定：+X 面 u→-z, v→-y；+Y 面 u→+x, v→+z（OpenGL 风格）
- `BilinearSample` 对每个面使用 Clamp 寻址（面边缘不跨面采样）
- `ToEquirectangular` / `FTex2D::ToTexCube` 通过方向向量双线性采样实现互转
