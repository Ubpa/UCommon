# Codec.h

## 职责

HDR 颜色编解码与色彩空间转换工具集，包含 RGBM/RGBD/RGBV 编码、YCoCg 色彩空间、以及方向/色相的紧凑打包。

## 关键抽象

### RGBM 编码

- M 存储在 sqrt 空间（参考 Unity 做法），默认 `MaxMultiplier = 100`
- `EncodeRGBM` / `DecodeRGBM` — 编解码，支持 float/FColor 输入
- `LowClamp = 16/255` — 防止 ASTC 压缩时 M 量化为零

### RGBD 编码

- D = divide，公式 `L = (D/(kD+1))^2`，默认 `MaxValue = 2048`
- `RGBD_GetK` / `RGBD_GetMaxValue` — K 因子与最大值互转

### RGBV 编码

- 自定义编码方案，公式 `L = v^2 / (k*v^2 + b)`，其中 `k = -s, b = s + 1/M`
- `RGBV_ComputeIntegral` — 计算一阶矩积分用于参数求解
- `RGBV_ComputeIntegral2` — 计算二阶矩积分，高亮度权重更大
- `RGBV_SolveS` — 基于一阶矩的二分法求解参数 s
- `RGBV_SolveS2` — 基于二阶矩的二分法求解参数 s，优先保证高亮度精度
- 保证 `L=0 → v=0`，`L=M → v=1`

### YCoCg 色彩空间

- `RGBToYCoCg` / `YCoCgToRGB` — RGB ↔ YCoCg 双向转换
- `RGBToCoCg` / `CoCgToRGB` — 仅色度（无亮度）版本
- `CoCgToSquareCoCg` / `SquareCoCgToCoCg` — CoCg 三角域 ↔ 方形域映射
- `ClampRGBwithYCoCg` — 在 YCoCg 空间做 clamp 后转回 RGB

### `FPackedHue`

2 字节打包色相。利用三角映射将 CoCg 有效三角域映射到 UV [0,1]²，空间利用率 100%。

### `FPackedHemiOct`

2 字节打包半球方向。将半球八面体坐标旋转 45° 映射到 [0,1]² 方形域。

- `VectorToHemiOctL` — 向量 → (HemiOctX, HemiOctY, L1 范数)
- `HemiOctToDir` / `HemiOctLToVector` — 反向重建

## 注意事项

- 所有 Encode 函数都有 `MapToValidColor*` 对应版本，确保颜色在编码范围内
- `LowClamp` 是 ASTC 压缩的硬性要求，编码时需保留
- CoCg 空间的有效域是三角形而非方形，直接 clamp UV 会引入误差

## 相关文件

- `Vector.h` — FLinearColor / FLinearColorRGB / FVector2f 等颜色向量类型
