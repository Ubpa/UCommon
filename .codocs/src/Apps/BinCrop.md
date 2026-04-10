# BinCrop

从 FTex2D 序列化二进制文件中裁剪矩形区域的命令行工具。

## 内容

| 名称 | 类型 | 职责 |
|------|------|------|
| CMakeLists.txt | 文件 | 构建配置，EXE 目标，依赖 Runtime |
| main.cpp | 文件 | 裁剪逻辑：加载 bin → 校验边界 → 逐行 memcpy → 保存 |
