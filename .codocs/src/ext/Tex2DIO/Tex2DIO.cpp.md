# Tex2DIO.cpp

## 职责

纹理图片文件 I/O，封装 stb_image/stb_image_write，提供 FTex2D 级别的加载/保存接口。

## 关键抽象

- `LoadImage(FileName, ...)` — 底层版本：自动检测 HDR/LDR 格式，返回原始指针和元信息
- `LoadImage(FileName, DesiredNumChannels)` — 高层版本：直接返回 FTex2D，数据所有权转移（TakeOwnership）
- `SaveImage(FileName, FTex2D)` — 根据 ElementType 自动选择 PNG（Uint8）或 HDR（Float）输出
- `FreeImage` — 释放 stb 加载的数据

## 注意事项

- stb 的 malloc/realloc/free 被重定向到 `UBPA_UCOMMON_MALLOC/REALLOC/FREE`，确保与 Runtime 使用相同的分配器
- `SaveImage` 对 Float 写 HDR，对 Uint8 写 PNG，其他 ElementType 返回失败
- PNG 保存时 stride 默认为 0（紧凑排列）
