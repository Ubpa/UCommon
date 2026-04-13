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
