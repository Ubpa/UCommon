# ASTCUtils

ASTC 纹理压缩/解压缩库，封装 astcenc 编解码器。

## 内容

| 名称 | 类型 | 职责 |
|------|------|------|
| CMakeLists.txt | 文件 | 构建配置，下载 astcenc 依赖，启用 SSE4.2 |
| ASTCUtils.cpp | 文件 | 压缩/解压缩实现，含稀疏像素解码和多线程压缩 |
