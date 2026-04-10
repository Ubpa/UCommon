# BinToTex

将 FTex2D 序列化二进制文件转换为 PNG/HDR 图片的命令行工具。

## 内容

| 名称 | 类型 | 职责 |
|------|------|------|
| CMakeLists.txt | 文件 | 构建配置，EXE 目标，依赖 Runtime + Tex2DIO |
| README.md | 文件 | 用法说明和格式映射规则 |
| main.cpp | 文件 | 格式转换逻辑：加载 bin → 根据 ElementType 选择输出格式 → SaveImage |
