# Tex2DIO.h

## 职责

图像文件加载/保存接口，封装 stb_image 系列（位于 gitignore 的 `stb/` 目录）。

## 关键函数

- `LoadImage(FileName, DesiredNumChannels)` → `FTex2D` — 高级接口，直接返回 FTex2D
- `LoadImage(FileName, ElementType&, Width&, Height&, NumChannels&, DesiredNumChannels)` → `void*` — 低级接口，返回原始指针
- `FreeImage(void*)` — 释放 LoadImage 分配的内存
- `SaveImage(FileName, FTex2D)` — 保存 FTex2D 到文件
- `SaveImage(FileName, W, H, C, float*/uint8*)` — 低级保存

## 注意事项
- `LoadImage` 返回的 `void*` 必须用 `FreeImage` 释放（非 `free`）
- 支持格式取决于 stb_image 编译配置
