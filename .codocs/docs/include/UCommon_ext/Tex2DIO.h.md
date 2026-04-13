# Tex2DIO.h

图像文件加载/保存，封装 stb_image。

- `LoadImage(file, channels)` → FTex2D（高层）；`LoadImage(file, type&, w&, h&, c&, dc)` → `void*`（低层，需 `FreeImage` 释放）
- `SaveImage(file, FTex2D)` / `SaveImage(file, w, h, c, data)`
