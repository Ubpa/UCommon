# UCommon

## 职责

UCommon 核心库公共头文件目录，提供向量/矩阵数学、纹理处理、SH、编解码、序列化等基础设施。

## 架构要点

- **命名空间注入宏**：几乎每个模块都提供 `UBPA_UCOMMON_<MODULE>_TO_NAMESPACE(NS)` 宏，将类型注入目标命名空间，避免直接 `using namespace UCommon`
- **TSpan**：统一的 span 类型（来自 Cpp17.h），贯穿整个库的接口，替代裸指针+长度
- **元素类型系统**（Utils.h）：所有纹理和 BQ 均基于统一的 `EElemType` 枚举，格式判断/尺寸/对齐通过工具函数而非 switch 分散
- **模块独立性**：各模块（BQ/Codec/SH/Tex2D/TexCube）可单独包含，不强制依赖其他模块；`UCommon.h` 仅做一站式汇总
- **`_deps/`**：第三方库（half.hpp），不属于 UCommon 本身，只能通过 `Half.h` / `FP8.h` 间接使用

## 模块依赖关系

```
Config.h ← 所有模块
Cpp17.h (TSpan) ← Vector.h ← Matrix.h ← SH.h
                                        ← Tex2D.h ← TexCube.h
Utils.h (EElemType) ← BQ.h ← Codec.h
                    ← Tex2D.h
Half.h / FP8.h (通过 _deps/half.hpp，独立于主依赖链)
Archive.h (独立，仅依赖 Config.h + Cpp17.h)
```

## 关键设计约定

- **行主序矩阵**：Matrix.h 的所有矩阵均为行主序，与 HLSL 默认列主序相反；传 shader 前需转置或用行向量左乘约定
- **坐标系**：TexCube 的六面索引遵循 OpenGL/Vulkan 约定（+X/-X/+Y/-Y/+Z/-Z = 0~5）
- **方向向量 vs 颜色向量**：Vector.h 的 `FVector3f` 同时用于方向和颜色，依赖上下文；`FLinearColor` 别名语义更清晰但底层相同
- **inpainting**：Tex2D 提供 inpainting 填充透明区域，算法为迭代扩散；输出不保证收敛到精确边界插值
- **SNorm 量化**：Utils.h 的 SNorm 查表不含 0，步进 2（-127、-125、...、125、127），避免对称性问题；Uint8 路径用整数乘法避免 float 误差
- **ASTC 特殊处理**：`AdaptiveASTC` 格式在 `ElementIsASTC()` 返回 false，因为它是运行时适配格式，不固定块大小；需单独判断
- **ThreadPool 全局注册**：`ThreadPool.h` 提供 `SetGlobalThreadPool` / `GetGlobalThreadPool`，库内部默认使用全局实例，调用方可替换

## 内容

| 名称 | 类型 | 职责 |
|------|------|------|
| Config.h | 文件 | 全局配置：可定制宏、平台导出、内存分配器、功能开关 |
| Cpp17.h | 文件 | C++17 兼容层：TSpan、Clamp、InvokeResult |
| Vector.h | 文件 | 2/3/4 维泛型向量模板及颜色/AABB 类型别名 |
| Half.h | 文件 | 16 位半精度浮点类型别名（封装 half_float） |
| FP8.h / FP8.inl | 文件 | 8 位浮点类型（有符号 FFP8、无符号 FUFP8） |
| Matrix.h | 文件 | 行主序 3x3/4x4 矩阵模板（旋转、TRS、求逆） |
| Utils.h | 文件 | 数学辅助、元素类型系统、采样、哈希、纹理寻址 |
| Archive.h | 文件 | 二进制序列化框架（内存/文件归档） |
| BQ.h | 文件 | 块量化：16 float → 16 字节 |
| Codec.h | 文件 | HDR 颜色编解码（RGBM/RGBD/RGBV）、YCoCg、方向打包 |
| Guid.h | 文件 | 128 位 GUID 类型 |
| SH.h / SH.inl | 文件 | 球谐函数完整类型系统（2~5 阶，单通道/RGB，旋转） |
| Tex2D.h / Tex2D.inl | 文件 | 2D 纹理类型（多元素类型、采样、缩放、序列化） |
| TexCube.h | 文件 | CubeMap 纹理（六面索引、等距柱面互转） |
| ThreadPool.h | 文件 | 简单线程池及全局单例注册 |
| UCommon.h | 文件 | 一站式总包含头文件 |
| _deps/ | 目录 | 第三方依赖（half.hpp 等） |
