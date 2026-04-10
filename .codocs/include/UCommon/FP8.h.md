# FP8.h

## 职责

8 位浮点类型实现，包含有符号 `FFP8<E,M>`（E+M=7 + 1 sign bit）和无符号 `FUFP8<E,M>`（E+M=8）两族。

## 关键抽象

### `FFP8<E, M>`（有符号 FP8）

- 约束 `E + M = 7`，1 bit 符号
- 支持 float 构造（带 `ERound` 舍入模式）和隐式转 float
- 位域布局：`{Frac:M, Exp:E, Sign:1}` / `Data:uint8_t`
- 预定义别名：`FFP8_E3M4`、`FFP8_E4M3`、`FFP8_E5M2`

### `FUFP8<E, M>`（无符号 FP8）

- 约束 `E + M = 8`，无符号位
- 接口同 FFP8，位域 `{Frac:M, Exp:E}`
- 预定义别名：`FUFP8_E3M5`、`FUFP8_E4M4`、`FUFP8_E5M3`

### `ERound`

舍入模式枚举：`Nearest`（默认）、`Down`、`Up`。

### `FFloat`

IEEE 754 单精度浮点位域辅助结构（`Frac:23, Exp:8, Sign:1`）。

## 相关文件

- `FP8.inl` — 模板实现
- `BQ.h` — FBQBlock 使用 FUFP8_E4M4 和 FFP8_E4M3
