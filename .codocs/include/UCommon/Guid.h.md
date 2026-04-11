# Guid.h

128 位 GUID，4×uint32_t（字段 A/B/C/D）。`sizeof(FGuid)==16`，跨平台布局一致。

- `NewGuid()` 生成；`IsValid()` 检查非全零（默认构造 = 全零 = 无效）
- `ToString(char[32])` — 32 字符十六进制串，无分隔符，**不写 null 终止符**
- `operator[](0~3)` 访问 A/B/C/D，越界 UB
- 标记 `UBPA_UCOMMON_API`；`UBPA_UCOMMON_GUID_TO_NAMESPACE(NS)` 导入命名空间
