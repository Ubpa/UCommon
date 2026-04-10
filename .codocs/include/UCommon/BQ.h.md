# BQ.h

## 职责

Block Quantization（块量化）：将 16 个 float 压缩为 16 字节的量化块。

## 关键抽象

### `FBQBlock`

- 16 字节结构，内含 `Scale`（FUFP8_E4M4）和 `Center`（FFP8_E4M3）两个 FP8 参数，加上 14 字节索引缓冲
- 构造：从 16 个 float 值（数组或 TSpan）量化构建
- `GetValue(Index)` — 反量化，取回第 Index 个浮点值
- `static_assert` 保证 `sizeof == 16`，即 16 个 float → 16 字节，压缩比 4:1

## 相关文件

- `FP8.h` — Scale/Center 使用的 FP8 类型
- `Half.h`、`Utils.h` — 依赖的基础设施
