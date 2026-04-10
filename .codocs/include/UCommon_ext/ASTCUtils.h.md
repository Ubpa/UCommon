# ASTCUtils.h

ASTC 纹理压缩/解压。**使用前必须调用 `InitBlockSizeDescriptorMngr`，释放时 `ReleaseBlockSizeDescriptorMngr`。**

- `EASTCProfile`：LDR_SRGB / LDR / HDR_RGB_LDR_A / HDR
- `FASTCBlock`：16 字节块，`GetPixel/GetAllPixels` 解码
- `FASTCConfig`：压缩配置，含 `EFormat`（RGBM/RGBD/RGBV/YCoCg/YCoCgPredition）、Quality、ThreadPool、HDR 参数
- `CompressImageToASTC` / `DecompressASTCImage` / `ToASTC(FTex2D&, ...)`
