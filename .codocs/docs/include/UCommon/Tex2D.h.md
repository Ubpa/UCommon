---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/Tex2D.h
  source_hash: sha256:acabccec262505f4992973b1f91cab3e499313a7085cb65aa4259786858ffe27
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# Tex2D.h

## 职责

2D 纹理类型，支持多元素类型（uint8/half/float/double）、所有权管理、采样、缩放和序列化。

## 关键抽象

### `FGrid2D`
- 宽高二维网格，提供索引↔坐标↔UV 互转
- 支持范围 for 迭代（`FGrid2DIterator`，行优先遍历）
- `GetNumMips()` 返回 mip 层级数

### `FTex2D`
- 存储：`void* Storage` + `EElementType` + `EOwnership`（拥有/借用）
- **构造**：可传入外部存储（TakeOwnership/DoNotTake）、或内部 malloc 分配
- **元素访问**：`At<T>(Index)`、`At<T>(Point, Channel)`、`At<T>(Point)`（向量类型）
- **浮点访问**：`GetFloat` / `SetFloat` / `GetLinearColorRGB` / `GetDoubleColor` 等
- **采样**：`BilinearSample` / `BilinearSampleAlignCorner`（支持 Wrap/Clamp 寻址）
- **缩放**：`DownSample()` / `Resize(Grid2D)` / `DownSample(Grid2D)`
- **格式转换**：`ToFloat()` / `ToUint8()`
- **值操作**：`Clamp` / `Min` / `Max` / `Threshold`
- **图像修复**：`ImageInpainting(CoverageData)` — mipmap 传播填充空洞
- **CubeMap**：`ToTexCube()` — 等距柱面投影转 CubeMap
- **序列化**：`Serialize(IArchive&)`
- **拷贝**：`Copy(Dst, DstPoint, Src, SrcPoint, Range)` — 区域拷贝

## 注意事项
- `EOwnership::TakeOwnership` 时 Storage 由 `free` 释放，必须用 `malloc` 分配
- `Resize` 要求目标尺寸 ≥ 原始尺寸的 1/2
- `At<T>` 有 `static_assert` 检查向量/标量类型匹配

## 相关文件
- `Tex2D.inl` — 模板构造函数和 At<T> 实现
- `TexCube.h` — FTexCube，ToTexCube 目标类型
- `Archive.h` — 序列化支持
- `Utils.h` — EElementType / EOwnership / ETextureAddress
