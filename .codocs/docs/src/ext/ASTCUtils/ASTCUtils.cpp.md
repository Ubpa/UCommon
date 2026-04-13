# ASTCUtils.cpp

## 职责

ASTC 纹理压缩/解压缩工具库，封装 astcenc 编解码器，提供按块/按像素的解码和整图压缩功能。

## 关键抽象

- `BlockSizeDescriptorMngr` — 单例，缓存不同 block size 对应的 `block_size_descriptor`，惰性初始化，可通过 `InitBlockSizeDescriptorMngr` 预热
- `FASTCBlock::GetPixels / GetPixel / GetAllPixels` — 解码单个 ASTC 块，支持按指定像素索引稀疏解码（避免解码整块）
- `CompressImageToASTC` — 整图压缩，支持多线程（通过 ThreadPool）、自定义 swizzle、RGBM/RGBD/RGBV/YCoCg 等格式
- `DecompressASTCImage` — 整图解压缩到 FVector4f 数组
- `ToASTC` — 便捷接口：压缩 + 立即解压（用于获取 ASTC 压缩后的近似结果）

## 实现要点

- 稀疏解码：`physical_to_symbolic` 和 `decompress_symbolic_block` 的自定义重载版本接受 `PointIndices`，只解码需要的像素对应的权重和分区，避免全块解码
- ISE（Integer Sequence Encoding）解码：手写 trit/quint/bit 解码，参照 ASTC 规范查表
- 颜色插值使用整数 lerp（`lerp_color_int`），支持 SSE/NEON SIMD 加速
- 支持实验性 YCoCg 压缩格式（`UBPA_UCOMMON_ASTC_EXP` 宏控制）

## 性能考量

- `BlockSizeDescriptorMngr` 缓存避免重复初始化（每个 descriptor 内存较大）
- 稀疏像素解码只计算涉及的权重范围（`begin_weight_index` / `end_weight_index`），减少不必要计算
- 压缩支持 ThreadPool 并行，每个线程调用 `astcenc_compress_image`

## 注意事项

- `#undef PI` 在包含 astcenc 头文件前，避免宏冲突
- 使用自定义内存分配（`UBPA_UCOMMON_MALLOC/FREE`）
- 仅支持 2D 纹理（block_z 固定为 1）
