---
codocs:
  schema: 1
  source_type: file
  source_path: src/Runtime/Guid.cpp
  source_hash: sha256:96823dec4a7e3c6f4033bb3cc5fb6d1329b7fa4ad787b758af7f5eac4a53fe87
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# Guid.cpp

## 职责

GUID 生成与操作。

## 实现要点

- `NewGuid()` 使用 `std::random_device` 生成 128-bit 随机 GUID
- `ToString(Buffer[32])` 输出 32 字符十六进制字符串（大写，无分隔符）
- `IsValid()` 检查 A|B|C|D 是否全零
