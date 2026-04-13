# main.cpp — BinToTex

## 职责

命令行工具，将 FTex2D 序列化二进制文件导出为 PNG 或 HDR 图片。

## 实现要点

- 用法：`BinToTex <input.bin> <output.png/hdr> [--force-png]`
- 根据 ElementType 自动选择输出格式：Uint8→PNG、Float→HDR、Double→转 Float 后 HDR
- `--force-png` 强制转换为 Uint8 并输出 PNG
- 图片写入通过 `Tex2DIO::SaveImage` 完成

## 相关文件

- `ext/Tex2DIO/Tex2DIO.cpp` — SaveImage 实现
- `Runtime/Tex2D.cpp` — FTex2D、ToUint8/ToFloat 转换
