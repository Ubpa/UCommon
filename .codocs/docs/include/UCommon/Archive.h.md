---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/Archive.h
  source_hash: sha256:a04b2de9c072774ee01bee112e05f353d93a9e661e971b744f1368ba7224dcab
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# Archive.h

## 职责

序列化框架，提供 `IArchive` 基类及 `FMemoryArchive` / `FFileArchive` 两种实现，支持 Loading/Saving 双向操作。

## 关键类型

| 类型 | 说明 |
|------|------|
| `IArchive` | 基类，pImpl 模式，支持 Loading/Saving 状态、位置跟踪 |
| `FMemoryArchive` | 内存序列化，内部持有 `std::vector<uint8_t>` |
| `FFileArchive` | 文件序列化，接受文件路径 |
| `FArchiveWrapper` | 包装器，转发到内部 `IArchive&` |

## 操作符重载

`operator<<` 支持：基本类型、`std::string`、`std::vector<T>`、`TSpan<T>`、枚举、`std::array`。

## 注意事项

- `UBPA_UCOMMON_ARCHIVE_TO_NAMESPACE` 宏可将 Archive 类型注入自定义命名空间
- `IArchive` 使用 pImpl，移动可用但不可拷贝
