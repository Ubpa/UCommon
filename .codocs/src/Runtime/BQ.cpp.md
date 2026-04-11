# BQ.cpp

编码：遍历 16 个 float 取 min/max → Center（FFP8_E4M3 最近舍入）+ Scale（FUFP8_E4M4 **上取整**，防溢出）→ 每值映射为 7-bit SNorm 索引。

存储：两个 uint64，各低 8 位存 Center/Scale，高 56 位存 8 个 7-bit 索引（写入 `|= (uint64_t)v << (8 + i*7)`）。

解码：`MoveBits[16]` 静态表直接右移取位，无分支。`TSpan` 变体做 assert+reinterpret_cast 后转发。
