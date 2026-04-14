---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/FP8.h
  source_hash: sha256:ee70ac0139588e60f1d687de9b76cae7ccab06755119e597dd32c0281d7b969b
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# FP8.h

8 位浮点类型模板：有符号 `FFP8<E,M>`（E+M=7，1 位符号位）和无符号 `FUFP8<E,M>`（E+M=8，无符号位）。

- `ERound`：Nearest/Down/Up，float→FP8 构造时的舍入模式，默认 Nearest；Scale 编码时用 Up 防止解码溢出
- 内部静态常量：`BiasE=(1<<(E-1))-1`（指数偏置），`SubNormalScale=1<<(BiasE+M-1)`（次正规化缩放因子），`MaskE/M/R` 用于位操作；转换实现在 `FP8.inl`
- 存储：union `{ Components{ Frac:M, Exp:E [, Sign:1] }, Data:uint8_t }`，支持直接按字节访问
- 预定义别名：`FFP8_E3M4/E4M3/E5M2`、`FUFP8_E3M5/E4M4/E5M3`（E4M3 精度高，E5M2 动态范围大）
- 辅助：`FFloat`（IEEE 754 float 位域结构）供转换算法使用；转换实现见 `FP8.inl`
- `UBPA_UCOMMON_FP8_TO_NAMESPACE(NS)` 将 `FFP8`、`FUFP8` 及各预定义别名导入目标命名空间
