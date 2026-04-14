---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/Guid.h
  source_hash: sha256:f5f12819b43a322b5b9ddab75e8159208a638a760b4851e547b3a8824718d2ad
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# Guid.h

128 位 GUID，4×uint32_t（字段 A/B/C/D）。`sizeof(FGuid)==16`，跨平台布局一致。

- `NewGuid()` 生成；`IsValid()` 检查非全零（默认构造 = 全零 = 无效）
- `ToString(char[32])` — 32 字符十六进制串，无分隔符，**不写 null 终止符**
- `operator[](0~3)` 访问 A/B/C/D，越界 UB
- 标记 `UBPA_UCOMMON_API`；`UBPA_UCOMMON_GUID_TO_NAMESPACE(NS)` 导入命名空间
