---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/Config.h
  source_hash: sha256:43e26e54c28e92e27a027602725104781a94633db4a2c28688f3c87273389889
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# Config.h

全局配置头，定义可定制宏、平台导出符号、内存分配器接口和功能开关。

## 可定制宏

| 宏 | 默认值 |
|----|--------|
| `UBPA_UCOMMON_ASSERT(expr)` | `assert(expr)` |
| `UBPA_UCOMMON_NO_ENTRY()` | `assert(false)` |
| `UBPA_UCOMMON_API` | 自动推导 dllexport/dllimport/visibility |
| `UBPA_UCOMMON_MALLOC/REALLOC/FREE` | 标准库（三者必须同时定义） |
| `UBPA_UCOMMON_DELTA` | `0.000001f` 浮点容差 |
| `UBPA_UCOMMON_WITH_EDITOR` | `1` |
