# FP8.inl

## 职责

FFP8 和 FUFP8 模板的完整实现，包括 float↔FP8 转换和查表加速。

## 实现要点

### 查表加速 `TFloatTable`

- 为 `FFP8_E4M3` 和 `FUFP8_E4M4` 预计算了 256 个 float 的位模式查表（编译期 constexpr 生成）
- `FloatTableGet` 通过 `reinterpret_cast` 直接将 `uint32_t` 表项当作 `float` 返回，避免运行时转换
- `operator float()` 中 `if constexpr` 按格式自动选择查表或计算路径

### float → FP8 转换

- 处理特殊值：零、Inf/NaN（映射为最大有限值 `0x7F` / `0xFF`）
- 次正规数（subnormal）：按 `SubNormalScale` 缩放后取整
- 正规数：从 IEEE 754 尾数中提取 M 位，按 `ERound` 模式（截断/四舍五入/随机）舍入，溢出时进位到指数位

### 比较语义

- `FFP8::operator==`：正零（`0x00`）和负零（`0x80`）视为相等
- `FUFP8::operator==`：无符号，直接比较 `Data` 字节

### 模板参数

- `TFPFormat`：描述指数位数 `E`、尾数位数 `M`、是否有符号、偏置 `Bias` 等
- `ERound`：舍入模式，影响 float→FP8 精度分布

## 注意事项

- 查表仅对 `FFP8_E4M3` 和 `FUFP8_E4M4` 两种格式有效；自定义格式走运行时转换
- float→FP8 精度受 `ERound` 影响，`Truncate` 模式系统性偏低，`Round` 模式误差更均匀
- Inf/NaN 映射到最大有限值而非专用位模式（E4M3 格式保留了 NaN 编码，但此库不使用）

## 相关文件

- `FP8.h` — 类型声明、格式特化（E4M3、E4M4）、转换接口
