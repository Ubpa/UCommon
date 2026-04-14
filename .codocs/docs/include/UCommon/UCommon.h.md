---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/UCommon.h
  source_hash: sha256:1f0fed9afacbf00b133096e490787ec0674aee7ca5712a6fa7fbbf36bf295b6f
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# UCommon.h

总包含头文件，一次引入 UCommon 所有 15 个公共头：Archive、BQ、Codec、Config、Cpp17、FP8、Guid、Half、Matrix、SH、Tex2D、TexCube、ThreadPool、Utils、Vector。

`UBPA_UCOMMON_TO_NAMESPACE(NS)` 聚合所有模块的 `*_TO_NAMESPACE` 宏，一次性将全部公共类型和命名空间别名注入指定命名空间（如 `UCommonTest`）。各模块也提供独立的 `*_TO_NAMESPACE` 宏，按需单独使用。
