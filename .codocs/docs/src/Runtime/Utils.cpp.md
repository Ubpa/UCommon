---
codocs:
  schema: 1
  source_type: file
  source_path: src/Runtime/Utils.cpp
  source_hash: sha256:fbdf17420dfa8b41d5fc6edaf66af600c0af38906bef9eb62b0829b5884460fa
  explicit_deps: []
  dep_hash: sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  hash_mode: text-lf-sha256
  verified_at: '2026-04-14T17:16:35.376791+08:00'
---
# Utils.cpp

**ApplyAddressMode**：float（归一化 UV）与 int64（纹素坐标+尺寸）两套重载：
- Wrap：float `fmod` 修正负值；int64 `%` 修正负余数
- Mirror：int64 对负坐标用 `-Coord-1` 保证连续性，再折叠
- Vector2 重载允许 X/Y 各自独立指定模式；未知模式触发 `NO_ENTRY`

**MatrixMulVec**：行主序 N×N 矩阵乘长度 N 向量，结果写入调用方 buffer（raw pointer，O(N²)，无 SIMD）。
