# TexCube.cpp

## Equirectangular 坐标系约定

- U = atan2(Y, X) / (2π) + 0.5（X 轴正方向 = U=0.5，逆时针 +U）
- V = 0.5 - asin(Z) / π（Z 轴朝上 = V=0，Z 轴朝下 = V=1）
- Z 钳到 [-1,1] 防止 asin NaN

## FCubeTexcoord 方向映射（面主轴选择）

比较三轴绝对值，选最大者作主轴，非抢先（tie 时 X > Y > Z 优先）。各面 UV 映射：

| 面 | U 方向 | V 方向 |
|----|--------|--------|
| +X | -Z/X | -Y/X |
| -X | +Z/\|X\| | -Y/\|X\| |
| +Y | +X/Y | +Z/Y |
| -Y | +X/\|Y\| | -Z/\|Y\| |
| +Z | +X/Z | -Y/Z |
| -Z | -X/\|Z\| | -Y/\|Z\| |

反向（`Direction()`）：Texcoord×2-1 后按面还原，`SafeNormalize` 处理角点精度。

## FTexCube 存储布局

内部用一个 `FTex2D FlatTex2D`，高度 = faceHeight × 6，面顺序 PositiveX(0)→...→NegativeZ(5)。

`BilinearSample` 通过偏移指针构造临时 `FTex2D`（`DoNotTakeOwnership`）指向对应面，使用 **Clamp** 寻址（cubemap 面不连续，禁止跨面采样）。

## ToEquirectangular 尺寸约定

无参版本：宽 = faceWidth × 4，高 = faceHeight / 3（与标准 4:1 等距柱面比例一致）。有参版本要求目标纹理预先创建（`IsValid()` 断言）。
