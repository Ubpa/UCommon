# Config.h

## 职责

全局配置头文件，定义可定制宏、平台导出符号、内存分配器接口和功能开关。

## 可定制宏

用户可在包含此头文件前预定义以下宏来替换默认行为：

| 宏 | 默认值 | 说明 |
|----|--------|------|
| `UBPA_UCOMMON_ASSERT(expr)` | `assert(expr)` | 断言 |
| `UBPA_UCOMMON_NO_ENTRY()` | `assert(false)` | 不可达路径 |
| `UBPA_UCOMMON_API` | 自动推导 | DLL 导出/导入符号 |
| `UBPA_UCOMMON_MALLOC/REALLOC/FREE` | 标准库 | 内存分配器（必须三个同时定义） |

## 功能开关

| 宏 | 默认值 | 说明 |
|----|--------|------|
| `UBPA_UCOMMON_DELTA` | `0.000001f` | 全局浮点容差 |
| `UBPA_UCOMMON_WITH_EDITOR` | `1` | 编辑器相关功能 |
| `UBPA_UCOMMON_ASTC_EXP` | `1`（此文件）/ `0`（未定义时） | ASTC 实验性功能 |
| `UBPA_UCOMMON_TVECTOR*_DEFINE` | 已定义 | 控制向量类型是否在库内定义 |

## 注意事项

- `UBPA_UCOMMON_API` 自动处理 Windows dllexport/dllimport 和 GCC visibility
- 内存分配三宏必须同时定义或同时不定义，否则编译报错
