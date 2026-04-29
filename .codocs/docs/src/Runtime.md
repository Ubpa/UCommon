---
codocs:
  schema: 1
  source_type: dir
  source_path: src/Runtime
  entries_hash: sha256:4bc422a9a16907483d20cf73fdbaf5a758cb2d7ddf45dc8412eb934cab638c59
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-29T16:40:02.687123+08:00'
  source_hash: null
---
# Runtime

UCommon 核心运行时库，提供纹理、序列化、数学、线程池等基础设施。所有其他模块依赖此库。

## 架构要点

- **构建**：CMakeLists.txt 通过 `Ubpa_UCommon_UsingSharedLibrary` 控制静态/动态；源文件通过 SOURCE 参数自动收集 include 目录下的头文件
- **无相互依赖**：各 .cpp 之间互不依赖，均只依赖 `include/UCommon/`；可按需单独编译
- **SH 的 `l≤5` 特化**：SH.cpp 对 Band 2~5 有手写旋转矩阵特化（比通用递推快约 3~10×），`l≥6` 才走 `ComputeSHBandNRotateMatrix` 通用路径
- **Tex2D 与 FGrid2D 的分工**：FGrid2D 是无类型网格（原始字节+元素数），FTex2D 在其上加 mipmap 链、采样逻辑和序列化
- **Utils.cpp 地址模式**：`ApplyAddressMode` 接受 `int64_t` 坐标（含负数），Wrap 用 `((x%n)+n)%n` 保证非负

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
