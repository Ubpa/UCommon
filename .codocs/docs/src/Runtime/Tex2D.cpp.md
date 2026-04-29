---
codocs:
  schema: 1
  source_type: file
  source_path: src/Runtime/Tex2D.cpp
  source_hash: sha256:2c9b09ba40f0ea2c49b4775e51a6e1a79ae138c968084c92554010a1db490480
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# Tex2D.cpp

## FGrid2D 实现细节

- `GetNumMips()` = `min(MSB64(W), MSB64(H)) + 1`（MSB64 返回最高位位置，1×1 → 1 级）
- `GetTexcoord(Point)` — 像素中心映射：`(Point + 0.5) / Extent`
- `GetPoint(Texcoord)` — 反向：`floor(texcoord * Extent - 0.5)`，clamp 到 `[0, Extent-1]`
- `operator[](0/1)` — reinterpret_cast 直接访问 Width/Height，无边界检查
- 迭代顺序：行主序，Y 方向 end 为 `{0, Height}`（X 回零，Y 越界）

## FTex2D 内存管理

- `TakeOwnership`：构造时 `UBPA_UCOMMON_MALLOC`，析构 `UBPA_UCOMMON_FREE`
- `DoNotTakeOwnership`：不持有指针，析构不释放（用于临时视图，如 TexCube 面切片）
- 拷贝构造：保留原 Ownership 语义——若原为 TakeOwnership 则深拷贝；若 DoNotTakeOwnership 则共享指针（**浅拷贝**）
- `FTex2D(const FTex2D& Other, EOwnership, void* EmptyStorage)` — 可强制指定 Ownership，EmptyStorage=nullptr 时自动分配

## 元素类型与 Half

`GetFloat/SetFloat` 支持 Uint8/Float/Double 三种路径，**Half 类型不走此接口**（`GetFloat` 的 switch 无 Half 分支，会触发 `NO_ENTRY`）。Half 只支持 `Clamp`/`Min`/`Max` 的原地操作。

## BilinearSample 实现

- 先将 UV 映射到像素坐标 `PointT = UV * Extent`，取 `floor(PointT - 0.5)` 得左上角整像素
- 对两个整数坐标分别 `ApplyAddressMode`（接受 int64 含负数）
- 权重：`LocalTexcoord = PointT - (IntPoint0 + 0.5)`，4 个角点双线性权重
- `BilinearSampleAlignCorner`：先将 UV 重映射为 `(UV*(Extent-1)+0.5)/Extent` 再调用标准版本（角对齐语义）

## DownSample 策略

- `DownSample()` — 2×2 盒式平均；Uint8 用 uint16 累加后 `DivideRound`（四舍五入），避免溢出和下取整偏差
- 边界处理：奇数尺寸时边界像素可能只有 1~2 个源像素参与（按实际 Count 除）
- 目标尺寸 = `max(1, W/2) × max(1, H/2)`（最小到 1×1 而非 0×0）
- `DownSample(Grid2D)` — 循环减半直到下一次会小于目标，再 `Resize` 到精确尺寸

## Resize 限制

- 只允许 `InGrid2D >= src / 2`（不能缩小超过 50%），违反触发 assert
- 等尺寸时直接拷贝；否则对目标每像素取 texcoord 后 Clamp 模式双线性采样

## ImageInpainting 算法

- **CoverageData 通道约束**：`CoverageData.NumChannels` 必须为 `1` 或等于 `NumChannels`
  - `== 1`：单通道 coverage 广播到所有颜色通道（Step 2/3 中按 CovC 循环，一次填充所有颜色通道）
  - `== NumChannels`：每通道独立 coverage（原始行为）
1. 构建 mip chain，每级按 coverage 加权下采样（coverage < 0 的像素已是扩展填充，权重不同）
2. 逐级从粗到细：未覆盖像素从低分辨率上采样填充
3. 正交邻居权重 255，对角邻居权重 1（近似 Laplace 扩散）
4. Coverage -1.f 表示"通过扩展填充"，区别于原始有效像素

## 序列化（IArchive）

`FTex2D` 实现 `operator&(IArchive&)`：先序列化 Grid2D 元数据（Width/Height/ElementType/NumChannels），再序列化原始像素数据（`ElementGetSize * NumChannels * W * H` 字节）。反序列化时若当前对象无效则先 `Malloc`（TakeOwnership），有效且布局相同则原地覆写，布局不同则先 `Free` 再 `Malloc`。

## GetFloat/SetFloat 路径

仅支持 `Uint8`、`Float`、`Double` 三种 `EElementType`；`Half` 缺失分支，走到 `default:NO_ENTRY`（运行时断言）。`Clamp`/`Min`/`Max` 内联操作通过模板特化各自处理 Half，不经此接口。

## ApplyAddressMode

`Utils.h` 的地址模式处理函数接受 `int64_t`，支持负坐标：
- `Clamp`：clamp 到 `[0, extent-1]`
- `Wrap`：`((coord % extent) + extent) % extent`
- `Mirror`：按 `2*extent` 周期对折

## 注意事项

- `Copy` 按像素大小（`ElementGetSize * NumChannels`）逐行 `memcpy`，要求布局完全相同
- `IsLayoutSameWith` 比较 Grid2D + ElementType + NumChannels，不比较 Ownership 和指针
- `GetLinearColorRGB` / `GetLinearColor` 等颜色快捷访问要求 `NumChannels` 与类型匹配，否则越界
