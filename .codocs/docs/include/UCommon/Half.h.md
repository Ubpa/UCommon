---
codocs:
  schema: 1
  source_type: file
  source_path: include/UCommon/Half.h
  source_hash: sha256:e00882eeb9e327cee4afda40d8b8b0f7a95bfbbcf1851fb3f03f3c695e7919a2
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# Half.h

`FHalf` — `half_float::half` 直接别名（非封装），继承全部算术/比较运算符及 float 隐式转换。

`_h` 用户字面量（如 `1.5_h`）需 `using namespace UCommon::literal;`。

`UBPA_UCOMMON_HALF_TO_NAMESPACE(NS)` 将 `FHalf` 和 `literal` 导入目标命名空间。
