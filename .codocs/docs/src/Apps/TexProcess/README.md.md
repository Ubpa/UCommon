# README.md — TexProcess

纹理处理命令行工具文档，支持两种操作：

- **threshold**：将低于阈值的像素置零
- **inpainting**：基于 mipmap 传播填充全通道为零的区域

典型工作流：先 threshold 去除低强度像素，再 inpainting 填充空洞区域。支持 PNG（uint8）和 HDR（float）格式。
