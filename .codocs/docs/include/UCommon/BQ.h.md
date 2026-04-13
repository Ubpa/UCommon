# BQ.h

Block Quantization：16 个 float → 16 字节量化块（压缩比 4:1）。

## `FBQBlock`

16 字节，含 `Scale`（FUFP8_E4M4）和 `Center`（FFP8_E4M3）+ 14 字节 7-bit 索引。构造时从 float 数组量化，`GetValue(i)` 反量化取回原值。
