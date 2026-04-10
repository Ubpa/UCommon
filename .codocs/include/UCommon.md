# UCommon

## 职责

UCommon 核心库公共头文件目录，提供向量/矩阵数学、纹理处理、SH、编解码、序列化等基础设施。

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
