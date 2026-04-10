# FP8.h

8 位浮点类型：有符号 `FFP8<E,M>`（E+M=7）和无符号 `FUFP8<E,M>`（E+M=8）。

支持 float 构造（`ERound`：Nearest/Down/Up）和隐式转 float。预定义别名：`FFP8_E3M4/E4M3/E5M2`、`FUFP8_E3M5/E4M4/E5M3`。

辅助：`FFloat`（IEEE 754 位域）、`FP8.inl`（模板实现）。
