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
