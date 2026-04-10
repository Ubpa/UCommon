# Tex2D.cpp

## 职责

2D 纹理核心实现，包含 FGrid2D 网格和 FTex2D 纹理数据的全部操作。

## 关键抽象

### FGrid2D
- 2D 网格描述（Width × Height），支持像素坐标↔纹理坐标转换、迭代器、mip 级数计算

### FTex2D
- 2D 纹理容器，支持 Uint8/Half/Float/Double 多种 ElementType
- 内存管理：TakeOwnership（自有）/ DoNotTakeOwnership（外部），使用 `UBPA_UCOMMON_MALLOC/FREE`

## 实现要点

- **类型透明访问**：`GetFloat/SetFloat` 内部按 ElementType 分发，统一 float 接口
- **双线性采样**：`BilinearSample` 支持 Wrap/Clamp/Mirror 寻址模式；`BilinearSampleAlignCorner` 将 [0,1] 映射到角对齐像素中心
- **下采样**：`DownSample()` 2×2 盒式滤波（含边界 clamp），`DownSample(Grid2D)` 级联下采样后双线性插值到目标尺寸
- **Resize**：仅支持 ≥0.5x 的放大/缩小，使用双线性插值
- **类型转换**：`ToFloat()` / `ToUint8()` 全像素逐通道转换
- **ImageInpainting**：mipmap 传播修补算法——
  1. 构建覆盖率感知的 mip chain（按 coverage 加权下采样）
  2. 每级扩展已覆盖像素到相邻未覆盖像素（加权矩阵：正交255，对角1）
  3. 从低分辨率向上双线性上采样填充零覆盖区域
- **序列化**：`Serialize(IArchive&)` 依次写 Grid2D、NumChannels、Ownership、ElementType、Storage 数据
- **区域拷贝**：`Copy` 按像素大小逐行 memcpy

## 注意事项

- Inpainting 的覆盖率标记 -1.f 表示"已通过扩展填充"
- Uint8 下采样使用 uint16 累加避免溢出，DivideRound 四舍五入
