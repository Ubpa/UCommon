# Half.h

`FHalf` — `half_float::half` 直接别名（非封装），继承全部算术/比较运算符及 float 隐式转换。

`_h` 用户字面量（如 `1.5_h`）需 `using namespace UCommon::literal;`。

`UBPA_UCOMMON_HALF_TO_NAMESPACE(NS)` 将 `FHalf` 和 `literal` 导入目标命名空间。
