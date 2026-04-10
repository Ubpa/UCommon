# Half.h

## 职责

16 位半精度浮点类型别名，封装第三方 `half_float::half`。

## 关键抽象

- `FHalf` — `half_float::half` 的类型别名
- `literal` — 命名空间别名，提供 `_h` 用户字面量

## 相关文件

- `_deps/half.hpp` — 第三方 half 精度实现
- `FP8.h` — 同样依赖 `half.hpp`
