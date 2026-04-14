---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/TexCube.h
  source_hash: sha256:622cbc6c04d74c796a578dd5d3afdbd618c4c745a201b83c5dc97d5f873ece5d
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# TexCube.h

## 职责

CubeMap 纹理类型，支持六面索引、方向↔UV 转换、等距柱面投影互转。

## 关键抽象

### `ECubeFace`
- 六个面枚举：`PositiveX/NegativeX/PositiveY/NegativeY/PositiveZ/NegativeZ`

### `FCubePoint` / `FCubeTexcoord`
- `FCubePoint` — 面 + 像素坐标，`Flat()` 转为 2D 平展坐标
- `FCubeTexcoord` — 面 + UV [0,1]²，可从方向向量或 CubePoint 构造
- `Direction()` — UV 转方向向量

### `FGridCube`
- 基于 `FGrid2D`，6 面网格，支持范围 for 迭代
- `Flat()` 返回平展后的 FGrid2D（Width × Height*6）

### `FTexCube`
- 内部存储为平展的 `FTex2D FlatTex2D`
- `BilinearSample` — 按 CubeTexcoord 双线性采样
- `ToEquirectangular()` / `FTex2D::ToTexCube()` — 等距柱面 ↔ CubeMap 互转

### 全局函数
- `EquirectangularDirectionToUV` / `EquirectangularUVToDirection` — 方向 ↔ 等距柱面 UV

## 相关文件
- `Tex2D.h` — FTex2D / FGrid2D 基础类型
