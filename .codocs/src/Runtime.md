# Runtime

UCommon 核心运行时库，提供纹理、序列化、数学、线程池等基础设施。所有其他模块依赖此库。

## 内容

| 名称 | 类型 | 职责 |
|------|------|------|
| CMakeLists.txt | 文件 | 核心库构建配置（静态/动态） |
| Archive.cpp | 文件 | 序列化框架：IArchive、FMemoryArchive、FFileArchive、FArchiveWrapper |
| BQ.cpp | 文件 | BQ 块量化压缩（16 float → 128-bit） |
| Codec.cpp | 文件 | HDR 颜色编码：RGBM、RGBD、RGBV |
| Guid.cpp | 文件 | GUID 生成与字符串化 |
| SH.cpp | 文件 | 球谐函数旋转矩阵（Band 2-5 特化 + 通用递推） |
| Tex2D.cpp | 文件 | 2D 纹理核心：FGrid2D + FTex2D（采样、下采样、类型转换、inpainting、序列化） |
| TexCube.cpp | 文件 | 立方体贴图：面坐标/方向转换、equirectangular 互转 |
| ThreadPool.cpp | 文件 | 固定线程数任务队列线程池 |
| Utils.cpp | 文件 | 纹理寻址模式、矩阵向量乘法 |
