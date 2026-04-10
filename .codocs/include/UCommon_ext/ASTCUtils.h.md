# ASTCUtils.h

## 职责

ASTC 纹理压缩/解压工具，支持多种 HDR 编码格式和多线程。

## 关键抽象

### `EASTCProfile`
- `LDR_SRGB` / `LDR` / `HDR_RGB_LDR_A` / `HDR` — ASTC 色彩配置

### `FASTCBlock`（16 字节）
- 128 位 ASTC 块数据
- `GetPixel` / `GetAllPixels` / `GetPixels` — 解码像素

### `FASTCConfig`
- 压缩配置：
  - `EFormat` — `None`/`RGBM`/`RGBD`/`RGBV`/`YCoCg`/`YCoCgPredition`
  - `MaxValue` / `RGBV_S` — HDR 编码参数
  - `Quality` — 压缩质量
  - `ThreadPool` — 可选多线程
  - `Weights` / `Swizzel` / `Cw` — 高级压缩选项
  - YCoCg 预测参数：外部 YCoCg 纹理和预测公式参数

### 全局函数
- `CompressImageToASTC` / `DecompressASTCImage` — 原始数据压缩/解压
- `ToASTC(FTex2D&, ...)` — FTex2D 到 ASTC 纹理转换
- `InitBlockSizeDescriptorMngr` / `ReleaseBlockSizeDescriptorMngr` — 块大小描述符管理（需手动初始化/释放）

## 注意事项
- 使用前必须调用 `InitBlockSizeDescriptorMngr` 初始化
- YCoCgPredition 格式需额外提供预测纹理和参数范围

## 相关文件
- `Tex2D.h` — FTex2D 类型
- `Codec.h` — RGBM/RGBD/RGBV 编码函数
