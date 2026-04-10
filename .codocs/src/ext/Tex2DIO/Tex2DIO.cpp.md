# Tex2DIO.cpp

LoadImage/SaveImage 实现，封装 stb_image/stb_image_write。自动检测 HDR/LDR；SaveImage 按 ElementType 选 PNG(Uint8) 或 HDR(Float)。stb 的内存分配重定向到 `UBPA_UCOMMON_MALLOC/REALLOC/FREE`。
