# Guid.cpp

## 职责

GUID 生成与操作。

## 实现要点

- `NewGuid()` 使用 `std::random_device` 生成 128-bit 随机 GUID
- `ToString(Buffer[32])` 输出 32 字符十六进制字符串（大写，无分隔符）
- `IsValid()` 检查 A|B|C|D 是否全零
