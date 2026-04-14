---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/SH.inl
  source_hash: sha256:312f0fd0cba71ca38e6b6cbcfe408034ca48299d759f7b79570f946ec6c38a89
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# SH.inl

## 职责

SH 类型系统的全部模板实现：基函数求值、归一化常数、View/Vector/RGB 的算术运算、cross-order 构造、旋转矩阵构造与应用。

## 实现要点

### SH 基函数求值
- `SH<l,m>(x,y,z)` — 硬编码 l=0..4 的解析公式（参考 "Stupid SH Tricks"）
- `SHKImpl<l,m>()` — 编译期查表返回归一化常数 K(l,m)，25 个常数预计算内联
- `Details::SHs` — 用 `integer_sequence` 展开，批量填充 V[] 数组

### TSHBandVector / TSHBandView 运算
- `operator/`：预计算 `1.0f / Scalar` 再乘，避免多次除法
- View 的 `+=/-=/*=/÷=` 直接修改指向的数据
- 二元运算返回 TSHBandVector（拥有存储）
- RGB 版本按 R/G/B 三通道分别运算

### cross-order 构造与 SetBand
- `TSHVectorCommon(lower_order, band_view)` — 从低阶向量 + 高阶 band view 拼接构造；前 N² 个系数来自低阶，后 (2N+1) 个来自 band view
- `TSHVectorAC`：存储 AC 项（L1+），从 `TSHVector` 去掉 DC 项，band view 构造同样支持
- `TSHVectorRGB`：三通道，`R/G/B` 各为一个 `TSHVector`，cross-order 构造分别复制各通道
- `TSHVectorACRGB`：三通道 AC，`R/G/B` 各为一个 `TSHVectorAC`
- `SetBand<BandOrder>(band_view)` — 通过 `SHBandOffset<Order, BandOrder>` 计算目标 V[] 起始索引后 memcpy

### TSHBandView / TSHBandConstView
- `TSHBandView<Order>(float* ptr)` — 轻量包装，不拥有内存，直接指向外部数组
- `TSHBandConstView<Order>` — 只读版本，可从 `TSHBandView` 或 `TSHBandVector` 隐式转换
- `TSHBandView::Vector()` — 返回对底层 `TSHBandVector` 的引用（`reinterpret_cast`），用于整体赋值
- `TSHBandView<Order>` 可隐式转换为 `TSHBandConstView<Order>`，但反向不可行

### HallucinateZH
- `HallucinateZH(FSHVector2, t, Buffer)` — 从 2-阶 SH 生成 ZH（带方向偏置）的参数
- `t ∈ [0,1]` 控制 L1 分量的保留比例：`t=0` 保留全部，`t=1` 压缩到最小
- 输出：`Buffer.xyz` = 缩放后的 L1 方向向量，`Buffer.w` = 直流偏置，`k` = 二次项系数


- `TSHRotateMatrices<Order>` 构造函数：用 `FillSHRotateMatrices` 依次计算 band 2..Order
- Band 2~5 使用特化函数（SH.cpp），Band 6+ 使用通用 Ivanic & Ruedenberg 递推
- `ApplySHRotateMatrix<BandOrder>` — 对单波段 View 做矩阵×向量（带临时缓冲避免覆盖）
- `TSHVectorCommon::ApplySHRotateMatrix` — 逐波段应用，通过 `SHIndexOffset` 区分 DC/AC

## 注意事项

- `SHKImpl` 的查表只覆盖 l=0..4（共 25 项），l≥5 需要运行时计算
- cross-order 构造不检查 band view 长度，调用方需确保长度匹配 `2*BandOrder-1`
- `operator/` 预先计算倒数再乘，仅在 Scalar 确定不为零时安全；库内调用均满足此前提
- Band 2~5 的 `ApplySHRotateMatrix` 展开为固定大小的矩阵乘法，编译器可完全向量化

## 相关文件

- `SH.h` — 类型声明与接口
- `SH.cpp` — Band 2~5 旋转矩阵特化实现（速度比通用递推快 3~10×）
