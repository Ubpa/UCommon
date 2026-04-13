# main.cpp — BinCrop

## 职责

命令行工具，从 FTex2D 序列化二进制文件中裁剪指定矩形区域。

## 实现要点

- 用法：`BinCrop <input.bin> <x> <y> <w> <h> [output.bin]`，输出默认为 `<input>_crop.bin`
- 通过 `FFileArchive` 加载/保存 FTex2D 序列化数据
- 裁剪采用逐行 `memcpy`，按字节偏移计算源/目标位置，支持任意 ElementType 和通道数
- 边界校验：裁剪区域不超出纹理尺寸，宽高不为零

## 相关文件

- `Runtime/Archive.cpp` — FFileArchive 序列化
- `Runtime/Tex2D.cpp` — FTex2D 纹理数据结构
