# BQ.cpp

## 职责

BQ（Block Quantization）块量化压缩，将 16 个 float 值压缩到两个 uint64 中。

## 实现要点

- `FBQBlock` 构造：求 min/max → 计算 Center（FP8_E4M3）和 Scale（UFP8_E4M4，向上取整）→ 每个值编码为 7-bit 有符号归一化整数
- 存储布局：每个 uint64 的低 8 位存 Center/Scale，高 56 位存 8 个 7-bit 索引（从 bit8 开始，每 7-bit 一个值）
- `GetValue`：从指定索引提取 7-bit SNorm 值，反量化为 `SNorm * Scale + Center`
- 精度：Center 用 FP8_E4M3，Scale 用 UFP8_E4M4（无符号），索引 7-bit

## 性能考量

- 紧凑的 128-bit 块适合缓存友好的纹理数据存储
- 解码只需位移和查表，无分支
