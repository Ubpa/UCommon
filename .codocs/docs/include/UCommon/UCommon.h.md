# UCommon.h

总包含头文件，一次引入 UCommon 所有 15 个公共头：Archive、BQ、Codec、Config、Cpp17、FP8、Guid、Half、Matrix、SH、Tex2D、TexCube、ThreadPool、Utils、Vector。

`UBPA_UCOMMON_TO_NAMESPACE(NS)` 聚合所有模块的 `*_TO_NAMESPACE` 宏，一次性将全部公共类型和命名空间别名注入指定命名空间（如 `UCommonTest`）。各模块也提供独立的 `*_TO_NAMESPACE` 宏，按需单独使用。
