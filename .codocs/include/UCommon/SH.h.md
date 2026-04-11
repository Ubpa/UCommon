# SH.h

## 职责

球谐函数（Spherical Harmonics）完整类型系统，支持 2~5 阶，涵盖单通道/RGB、DC/AC-only、View/Vector、Band/全阶四个正交维度。

## 类型体系（四维正交）

### 全阶系数容器

| 类型 | 存储 | 说明 |
|------|------|------|
| `TSHVector<N>` | `float V[N*N]` | N*N 个系数，含 DC (L0..L(N-1)) |
| `TSHVectorAC<N>` | `float V[N*N-1]` | N*N-1 个系数，无 DC (L1..L(N-1)) |
| `TSHVectorRGB<N>` | `TSHVector<N> R,G,B` | R/G/B 三通道 TSHVector |
| `TSHVectorACRGB<N>` | `TSHVectorAC<N> R,G,B` | R/G/B 三通道 TSHVectorAC |

### 单波段系数容器

| 类型 | 存储 | 说明 |
|------|------|------|
| `TSHBandView<Order, bConst>` | `float* Data`（非拥有） | 指向 2N-1 个 float 的视图 |
| `TSHBandVector<Order>` | `float Data[2N-1]`（拥有） | 单波段拥有存储 |
| `TSHBandViewRGB<Order, bConst>` | `R,G,B` 各为 TSHBandView | RGB 三通道视图 |
| `TSHBandVectorRGB<Order>` | `R,G,B` 各为 TSHBandVector | RGB 三通道拥有存储 |

### 旋转矩阵

- `TSHRotateMatrices<Order>` — 扁平存储 band 2..Order 的旋转矩阵
  - `float Data[TotalSize]`，`TotalSize = Order*(4*Order²-1)/3 - 1`
  - `BandOffset<B>` — 编译期常量，band B 在 Data 中的 float 偏移；B=2→0, B=3→9, B=4→34
  - 从 `FMatrix3x3f` 显式构造：`TSHRotateMatrices<Order> mats(rotMatrix)`
  - `GetBand<K>()` 返回 (2K-1)×(2K-1) 行主序 `TSpan<float>`
  - 隐式向下转换为低阶版本（`reinterpret_cast`，内存前缀兼容，有 static_assert 保护）

## CRTP 基类

| 基类 | 适用子类 | 提供的能力 |
|------|----------|-----------|
| `TSHVectorCommon<Derived, Order, Basis>` | TSHVector, TSHVectorAC | +/-/*/÷/+=/-=/*=、Dot、Exp/Exp2、GetLinearVector、operator()、ApplySHRotateMatrix |
| `TSHVectorRGBCommon<Derived, TElem, Order, Basis>` | TSHVectorRGB, TSHVectorACRGB | 同上（分 R/G/B）、GetLuminance/GetSrgbLuminance、Desaturate、ToYCoCg、operator[] |
| `TSHBandCommon<Derived, Order>` | TSHBandView, TSHBandVector | +/-/*/÷、Dot、operator*(FVector3f)→RGB、GetLinearVector（Order=2 SFINAE） |
| `TSHBandRGBCommon<Derived, Order>` | TSHBandViewRGB, TSHBandVectorRGB | Dot（两个重载）、operator[]、GetSize |

## 编译期工具

| 名称 | 类型 | 说明 |
|------|------|------|
| `SHK<l,m>` | `constexpr float` | 归一化常数 `sqrt((2l+1)/(4π) * (l-|m|)!/(l+|m|)!)` |
| `SHIndexToL<i>` | `constexpr int` | flat index i → band order l |
| `SHIndexToM<i>` | `constexpr int` | flat index i → band index m（i=0→0, i=1..3→-1..1, …） |
| `SH<l,m>(x,y,z)` / `SH<l,m>(FVector3f)` | 自由函数 | 在方向上求**单个** basis 函数值，与 SHBasisFunction 不同 |

## 关键方法细节

### TSHVectorCommon / TSHVector
- `operator()(FVector3f)` — evaluate SH 在某方向的值（`Dot(*this, SHBasisFunction(v))`）
- `GetLinearVector()` — 返回 L1 波段的未归一化方向向量；在 TSHVectorCommon 上**无 Order 限制**（通过 BaseIndex 公式计算偏移）
- `DC()` → `float` — 返回 V[0]（L0, m=0 系数）
- `TSHVector(TSHVector<K>)` — explicit 跨阶构造：截断（K>N）或补零（K<N）
- `TSHVector(TSHVector<N-1>, TSHBandConstView<N>)` — append 构造，拼接低阶 + 新 band

### TSHVectorRGBCommon / TSHVectorRGB
- `operator()(FVector3f)` → `FVector3f` — evaluate RGB SH 在方向的值
- `DC()` → `FVector3f` — RGB 三通道 DC 分量（R.V[0], G.V[0], B.V[0]）
- `GetLuminance()` — BT.601 灰度：R*0.3 + G*0.59 + B*0.11
- `GetSrgbLuminance()` — BT.709 灰度：R*0.2126 + G*0.7152 + B*0.0722
- `Desaturate(float)` — 与灰度插值去饱和
- `ToYCoCg()` — 所有系数整体转 YCoCg 色彩空间（逐系数应用 RGBToYCoCg）
- `operator[](uint64_t)` → `TElement&` — 0=R, 1=G, 2=B 通道访问
- `SetBand<K>(TSHBandViewRGB<K>)` — 写入某一波段（按 element 拷贝）
- `Dot(DerivedType, TSHVector)` → `FVector3f` — RGB dot with single-channel
- `Dot(DerivedType, FVector3f)` → `TSHVector` — 加权混合 R/G/B 通道
- `ApplySHRotateMatrix(TSHRotateMatrices)` — 成员版本，返回旋转后的新对象

### TSHBandCommon / TSHBandView / TSHBandVector
- `GetLinearVector()` — **仅 Order=2** 时存在（SFINAE `enable_if_t<O==2>`），语义同上
- `operator*(FVector3f)` → `TSHBandVectorRGB` — 单通道 band 乘颜色，产生 RGB band
- `As<U>()` — reinterpret_cast 转型，有 `static_assert(sizeof==sizeof && alignof%alignof==0)`
- `Vector()` on TSHBandView — `reinterpret_cast` 到 `TSHBandVector&`，**要求底层数据连续**，对非连续 view 是 UB
- `operator=` on TSHBandView — **浅拷贝（rebind 指针）**，不拷贝数据；与 TSHBandVector 的深拷贝语义不同

### TSHBandRGBCommon / TSHBandViewRGBCommon
- `GetLuminance()` / `GetSrgbLuminance()` — 同 TSHVectorRGBCommon，Band 粒度
- `Dot(TSHBandConstViewRGB, TSHBandConstView)` → `FVector3f` — 每通道 dot
- `Dot(TSHBandConstViewRGB, FVector3f)` → `TSHBandVector` — 加权混合通道
- `operator=` — 同样是浅拷贝（rebind 三个 View 指针）

## 隐式转换链

```
TSHBandVector<N>   →  TSHBandView<N, false>  →  TSHBandView<N, true>
                       (implicit)                  (implicit, mutable→const)
```

`TSHBandConstViewRGB` 同理。这些转换是 `operator TSHBandView<N, bConst>()` 实现的，允许 Vector 传给接受 View 的接口。

## 算法说明

- `HallucinateZH(FSHVector2, float t, FVector4f& Buffer, float Delta)` — Buffer 是 in/out：
  `z1 = dot(Buffer.xyz, n)`，输出 `Buffer.w + z1*(1 + z1*k)`；将 SH2 近似为 ZH 以提速
- `ComputeSHBandNRotateMatrix(float* out, int l, FMatrix3x3f)` — 基于 **Ivanic & Ruedenberg (1996)** 递推，unsigned Condon-Shortley real-SH 约定；`l >= 6` 时用此函数，**l <= 5 请用特化版本（性能更好）**
- `ApplySHRotateMatrix(TSHBandView<Order>, const float*)` — 自由函数，原地旋转单 band

## 注意事项

- Order 2~5 有预定义 `F` 前缀别名（`FSHVector3`、`FSHBandViewRGB4` 等）；`FSHRotateMatrices` 只到 Order=3
- `TSHVectorAC::GetBand<K>()` 基址偏移 `Pow2(K-1)-1`（跳过 DC），比 TSHVector 少 1
- Band 向量点积用 `operator|`（来自 Vector.h）
- `UBPA_UCOMMON_SH_TO_NAMESPACE(ns)` — 把所有 SH 类型、编译期工具、F-alias 注入目标命名空间；文件末尾已对 `UCommonTest` 调用

## 相关文件
- `SH.inl` — 所有模板方法实现
- `Matrix.h` — FMatrix3x3f
- `Vector.h` — FVector3f / FVector4f
