---
codocs:
  schema: 1
  source_type: file
  source_path: src/Runtime/BQ.cpp
  source_hash: sha256:9ebd332563c6b7d2cc0b7a5a1a7625cdc21448b055defacad59afc4455c7147b
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# BQ.cpp

编码：遍历 16 个 float 取 min/max → Center（FFP8_E4M3 最近舍入）+ Scale（FUFP8_E4M4 **上取整**，防溢出）→ 每值映射为 7-bit SNorm 索引。

存储：两个 uint64，各低 8 位存 Center/Scale，高 56 位存 8 个 7-bit 索引（写入 `|= (uint64_t)v << (8 + i*7)`）。

解码：`MoveBits[16]` 静态表直接右移取位，无分支。`TSpan` 变体做 assert+reinterpret_cast 后转发。
