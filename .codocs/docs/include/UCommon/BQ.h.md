---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/BQ.h
  source_hash: sha256:56cc1f34eb6e22a69242912333468bb33f0badc3b2335707d92df28c3eab9a42
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# BQ.h

Block Quantization：16 个 float → 16 字节量化块（压缩比 4:1）。

## `FBQBlock`

16 字节，含 `Scale`（FUFP8_E4M4）和 `Center`（FFP8_E4M3）+ 14 字节 7-bit 索引。构造时从 float 数组量化，`GetValue(i)` 反量化取回原值。
