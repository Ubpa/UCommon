# Guid.h

## 职责

128 位 GUID 类型，支持生成、比较和字符串化。

## 关键抽象

### `FGuid`

- 4 个 `uint32_t`（A/B/C/D），共 16 字节
- `NewGuid()` — 静态工厂，生成新 GUID（平台相关实现）
- `IsValid()` — 检查是否非零
- `ToString(char[32])` — 输出 32 字符十六进制字符串（无分隔符）
- 支持 `operator[]` 按索引访问 4 个分量
