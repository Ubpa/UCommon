# FP8.inl

## 职责

FFP8 和 FUFP8 模板的完整实现，包括 float↔FP8 转换和查表加速。

## 实现要点

### 查表加速 `TFloatTable`

- 为 `FFP8_E4M3` 和 `FUFP8_E4M4` 预计算了 256 个 float 的位模式查表
- `FloatTableGet` 通过 `reinterpret_cast` 直接将 `uint32_t` 表项当作 `float` 返回
- `operator float()` 中 `if constexpr` 自动选择查表或计算路径

### float → FP8 转换

- 处理特殊值：零、Inf/NaN（映射为最大值 `0x7F` / `0xFF`）
- 次正规数：按 `SubNormalScale` 缩放后取整
- 正规数：从 IEEE 754 尾数中提取 M 位，按 `ERound` 模式舍入，溢出时进位到指数

### 比较语义

- `FFP8::operator==`：正零和负零视为相等（`0x00 == 0x80`）
- `FUFP8::operator==`：无符号，直接比较 `Data`
