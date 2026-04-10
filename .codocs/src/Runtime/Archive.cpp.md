# Archive.cpp

## 职责

序列化/反序列化框架，提供内存和文件两种 Archive 实现。

## 关键抽象

- `IArchive` — 基类，支持 Loading/Saving 双向状态，内置版本管理系统（key→version map）
- `FMemoryArchive` — 内存序列化，Loading 时从 `TSpan<const uint8_t>` 读取，Saving 时写入内部 `vector<uint8_t>`
- `FArchiveWrapper` — 包装另一个 IArchive，添加 "Ubpa" 魔数头和版本信息。Saving 时先缓存数据再在析构时写出（支持延迟写入 size 字段）。若 Loading 时 size=0 则标记无效
- `FFileArchive` — 文件序列化，使用 ifstream/ofstream。文件格式：Header（Magic + NumVersionKeys + VersionMapOffset）→ 数据 → VersionMap。Loading 时先读 header 再跳转读版本表，最后 seek 回数据区

## 实现要点

- 所有 Impl 使用 placement new + `UBPA_UCOMMON_MALLOC/FREE`，不依赖全局 new/delete
- `FFileArchive` 的版本表存在文件末尾，header 中记录偏移量，支持先写数据后追加版本
- `FArchiveWrapper::IsValid()` 检查内部 Archive 指针（Loading 时若 size=0 会置 null）
- 移动语义通过 Swap 惯用法实现
