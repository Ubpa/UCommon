# BQ.cpp

FBQBlock 实现：min/max → Center(FP8_E4M3) + Scale(UFP8_E4M4 上取整) → 每值编码为 7-bit SNorm。两个 uint64 低 8 位存 Center/Scale，高 56 位存 8 个 7-bit 索引。解码无分支，只需位移。
