# UCommon_ext

## 职责

UCommon 扩展模块，提供依赖外部库的功能（ASTC 压缩、图像 IO），与核心库解耦。

## 内容

| 名称 | 类型 | 职责 |
|------|------|------|
| ASTCUtils.h | 文件 | ASTC 纹理压缩/解压（支持 RGBM/RGBD/RGBV/YCoCg 格式） |
| Tex2DIO.h | 文件 | 图像文件加载/保存（封装 stb_image） |
| stb/ | 目录 | stb 第三方库（gitignore，不入库） |
| doctest/ | 目录 | doctest 测试框架（gitignore，不入库） |
| .gitignore | 文件 | 排除 stb/ 和 doctest/ 目录 |
