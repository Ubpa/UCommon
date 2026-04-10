# main.cpp — TexProcess

## 职责

纹理处理命令行工具，支持 threshold（阈值过滤）和 inpainting（图像修补）两种操作。

## 实现要点

- 用法：`TexProcess <command> <input> <output> [options]`
- **threshold**：调用 `FTex2D::Threshold(float)` 将低于阈值的像素置零
- **inpainting**：
  1. Uint8 输入先转 Float 处理，结束后转回
  2. 自动生成 coverage map：遍历所有像素，全通道为零标记为未覆盖
  3. 调用 `FTex2D::ImageInpainting(CoverageTex)` 执行修补
  4. 全部未覆盖时报错退出，全部覆盖时警告并跳过
- 图片加载/保存通过 `Tex2DIO` 的 `LoadImage`/`SaveImage`

## 相关文件

- `ext/Tex2DIO/Tex2DIO.cpp` — 图片 I/O
- `Runtime/Tex2D.cpp` — FTex2D::Threshold、ImageInpainting
