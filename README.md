# UCommon

> **U**bpa **Common**

C++ 基础设施库，提供向量/矩阵数学、纹理处理、球谐函数、HDR 编解码、序列化、线程池等模块。

## 模块概览

### 核心库（`include/UCommon/`）

| 模块 | 职责 |
|------|------|
| `Vector.h` | 2/3/4 维泛型向量、颜色类型（`FLinearColor`、`FLinearColorRGB` 等）、AABB |
| `Matrix.h` / `Matrix.inl` | 行主序 3×3/4×4 矩阵，旋转、TRS、求逆 |
| `SH.h` / `SH.inl` | 球谐函数完整类型系统（2～5 阶，单通道/RGB/AC，旋转矩阵） |
| `Tex2D.h` / `Tex2D.inl` | 2D 纹理（多元素类型、双线性采样、mipmap、inpainting、序列化） |
| `TexCube.h` | CubeMap（六面索引、等距柱面互转） |
| `Codec.h` | HDR 颜色编解码（RGBM/RGBD/RGBV）、YCoCg 色彩空间、方向/色相紧凑打包 |
| `BQ.h` | 块量化（16 float → 128-bit） |
| `Archive.h` | 二进制序列化框架（内存/文件归档，支持版本升级） |
| `Utils.h` | 数学辅助、元素类型系统（`EElementType`）、纹理寻址、哈希 |
| `ThreadPool.h` | 固定线程数任务队列线程池，支持全局单例注册 |
| `Half.h` / `FP8.h` | 16 位半精度、8 位浮点类型 |

### 扩展库（`include/UCommon_ext/`）

| 模块 | 职责 |
|------|------|
| `Tex2DIO.h` | 图像文件读写（基于 stb，支持 HDR/PNG/JPG 等） |
| `ASTCUtils.h` | ASTC 纹理压缩工具 |

### 命令行工具（`src/Apps/`）

| 工具 | 职责 |
|------|------|
| `BinCrop` | 二进制纹理裁剪 |
| `BinToTex` | 二进制转纹理 |
| `TexProcess` | 纹理处理流水线 |

## 构建

```bash
cd ~/UCommon && mkdir -p build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local/Ubpa
cmake --build . -j$(nproc)
cmake --build . --target UCommon_RunTests -j$(nproc)   # 运行测试（13/13 pass）
cmake --build . --target UCommon_InstallAll -j$(nproc) # 安装
```

依赖：[UCMake](https://github.com/Ubpa/UCMake) 0.7.3（通过 `find_package` 本地查找）。

## 使用

```cmake
find_package(UCommon REQUIRED)
target_link_libraries(MyTarget Ubpa::UCommon_Runtime)
```

## 设计约定

- **行主序矩阵**：与 HLSL 默认列主序相反，传 shader 前需转置或用行向量左乘约定
- **命名空间注入宏**：各模块提供 `UBPA_UCOMMON_<MODULE>_TO_NAMESPACE(NS)` 宏，避免直接 `using namespace UCommon`
- **`TSpan`**：统一 span 类型，贯穿库接口，替代裸指针+长度
- **`EElementType`**：所有纹理和 BQ 基于统一的元素类型枚举，格式判断/尺寸/对齐通过工具函数集中处理

## Git Hooks

本仓库使用 **more-hooks** 和 **codocs** 两套 hook 工具，通过 more-hooks dispatcher 统一调度。

### more-hooks

more-hooks 是 git hook 的多工具调度框架，将各工具的 hook 脚本注册到 `.git/hooks.d/<hook-name>/` 下，由 `.git/hooks/<hook-name>` dispatcher 按优先级顺序调用。

当前注册的 hook（`50-codocs`）：

```
.git/hooks/
├── pre-commit       ← dispatcher
└── commit-msg       ← dispatcher

.git/hooks.d/
├── pre-commit/
│   └── 50-codocs    ← codocs pre-commit hook
└── commit-msg/
    └── 50-codocs    ← codocs commit-msg hook
```

### codocs

codocs 是代码语义文档系统，在 `.codocs/` 下维护与代码树严格镜像的 Markdown 文档。

**管控范围**（见 `.codocs/config.json`）：

```json
{
  "roots": ["include/UCommon", "src/Runtime"],
  "exclude": ["*.backup"],
  "exclude_paths": ["include/UCommon/_deps", "src/Runtime/CMakeLists.txt"]
}
```

**Hook 行为**：

| Phase | 触发条件 | 行为 |
|-------|----------|------|
| 1 (pre-commit) | lint：存在 ORPHAN/MISSING 文档 | 硬拦截，必须修复 |
| 2 (pre-commit + commit-msg) | 受管控代码有改动但无 `.codocs/` 改动 | 警告，要求在 commit message 中声明 |
| 3 (pre-commit + commit-msg) | `.codocs/` 子文档已更新但父目录 MD 未同步 | 警告，要求声明 |

**跳过声明**（在 commit message 末尾添加）：

```
## codocs-skip
- .codocs/docs/src/Runtime/Foo.cpp.md: 仅修改内部实现，接口描述未变
- .codocs/docs/include/UCommon.md: 未新增/删除/改名文件，索引表不受影响
```

### 重新安装 hooks

如果 `.git/hooks/` 被意外清空，重新运行 codocs init 即可（会自动安装 more-hooks dispatcher 和 codocs hook）：

```bash
python .codocs/scripts/codocs.py .
```
