# Tex2DIO

纹理图片文件 I/O 库，封装 stb_image/stb_image_write。

## 内容

| 名称 | 类型 | 职责 |
|------|------|------|
| CMakeLists.txt | 文件 | 构建配置，依赖 Runtime + stb |
| Tex2DIO.cpp | 文件 | LoadImage/SaveImage 实现，PNG(Uint8)↔HDR(Float) 自动检测 |
