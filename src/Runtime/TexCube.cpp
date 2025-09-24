/*
MIT License

Copyright (c) 2024 Ubpa

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <UCommon/TexCube.h>

UCommon::FVector2f UCommon::EquirectangularDirectionToUV(const FVector3f& Direction)
{
	const float U = 0.5f + std::atan2(Direction.Y, Direction.X) / (2 * UCommon::Pi);
	const float V = 0.5f - std::asin(std::clamp(Direction.Z, -1.f, 1.f)) / UCommon::Pi;
	return FVector2f(U, V).Saturate();
}

UCommon::FVector3f UCommon::EquirectangularUVToDirection(const FVector2f& UV)
{
	const FVector2f ClampedUV = UV.Saturate();

	const float Azimuth = (ClampedUV.X - 0.5f) * 2 * UCommon::Pi;
	const float Altitude = (0.5f - ClampedUV.Y) * UCommon::Pi;

	const float CosAltitude = std::cos(Altitude);
	const float X = CosAltitude * std::cos(Azimuth);
	const float Y = CosAltitude * std::sin(Azimuth);
	const float Z = std::sin(Altitude);

	return FVector3f(X, Y, Z);
}

UCommon::FCubeTexcoord::FCubeTexcoord() noexcept = default;
UCommon::FCubeTexcoord::FCubeTexcoord(ECubeFace InFace, const FVector2f& InTexcoord) noexcept
	: Face{ InFace }, Texcoord{ InTexcoord } {}
UCommon::FCubeTexcoord::FCubeTexcoord(const FCubePoint& CubePoint, const FGridCube& GridCube) noexcept
	: Face{ CubePoint.Face }, Texcoord{ GridCube.Grid2D.GetTexcoord(CubePoint.Point) } {}

UCommon::FCubeTexcoord::FCubeTexcoord(const FVector3f& Direction) noexcept
{
	UBPA_UCOMMON_ASSERT(Direction.IsAlmostUnit());
	const FVector3f AbsDirection = Direction.Abs();
	if (AbsDirection.X >= AbsDirection.Y && AbsDirection.X >= AbsDirection.Z)
	{
		if (Direction.X > 0) // +X
		{
			Face = ECubeFace::PositiveX;
			Texcoord.X = 0.5f * (-Direction.Z / AbsDirection.X + 1.f);
			Texcoord.Y = 0.5f * (-Direction.Y / AbsDirection.X + 1.f);
		}
		else // -X
		{
			Face = ECubeFace::NegativeX;
			Texcoord.X = 0.5f * (Direction.Z / AbsDirection.X + 1.f);
			Texcoord.Y = 0.5f * (-Direction.Y / AbsDirection.X + 1.f);
		}
	}
	else if (AbsDirection.Y >= AbsDirection.X && AbsDirection.Y >= AbsDirection.Z)
	{
		if (Direction.Y > 0) // +Y
		{
			Face = ECubeFace::PositiveY;
			Texcoord.X = 0.5f * (Direction.X / AbsDirection.Y + 1.f);
			Texcoord.Y = 0.5f * (Direction.Z / AbsDirection.Y + 1.f);
		}
		else // -Y
		{
			Face = ECubeFace::NegativeY;
			Texcoord.X = 0.5f * (Direction.X / AbsDirection.Y + 1.f);
			Texcoord.Y = 0.5f * (-Direction.Z / AbsDirection.Y + 1.f);
		}
	}
	else // Z is largest
	{
		if (Direction.Z > 0) // +Z
		{
			Face = ECubeFace::PositiveZ;
			Texcoord.X = 0.5f * (Direction.X / AbsDirection.Z + 1.f);
			Texcoord.Y = 0.5f * (-Direction.Y / AbsDirection.Z + 1.f);
		}
		else // -Z
		{
			Face = ECubeFace::NegativeZ;
			Texcoord.X = 0.5f * (-Direction.X / AbsDirection.Z + 1.f);
			Texcoord.Y = 0.5f * (-Direction.Y / AbsDirection.Z + 1.f);
		}
	}
}

UCommon::FVector3f UCommon::FCubeTexcoord::Direction() const noexcept
{
	const FVector2f CenteredTexcoord = Texcoord * 2.f - 1.f; // [-1,1]x[-1,1]
	switch (Face)
	{
	case ECubeFace::PositiveX:
		return FVector3f(1.f, -CenteredTexcoord.Y, -CenteredTexcoord.X).SafeNormalize();
	case ECubeFace::NegativeX:
		return FVector3f(-1.f, -CenteredTexcoord.Y, CenteredTexcoord.X).SafeNormalize();
	case ECubeFace::PositiveY:
		return FVector3f(CenteredTexcoord.X, 1.f, CenteredTexcoord.Y).SafeNormalize();
	case ECubeFace::NegativeY:
		return FVector3f(CenteredTexcoord.X, -1.f, -CenteredTexcoord.Y).SafeNormalize();
	case ECubeFace::PositiveZ:
		return FVector3f(CenteredTexcoord.X, -CenteredTexcoord.Y, 1.f).SafeNormalize();
	case ECubeFace::NegativeZ:
		return FVector3f(-CenteredTexcoord.X, -CenteredTexcoord.Y, -1.f).SafeNormalize();
	default:
		UBPA_UCOMMON_NO_ENTRY();
		return FVector3f(0.f, 0.f, 0.f);
	}
}

//
// FGridCube
//////////////

UCommon::FUint64Vector2 UCommon::FCubePoint::Flat(const FGridCube& GridCube) const noexcept
{
	return { Point.X, Point.Y + (uint64_t)Face * GridCube.Grid2D.Height };
}

UCommon::FGridCube::FGridCube(const FGridCube& InGridCube) noexcept = default;
UCommon::FGridCube::FGridCube(const FGrid2D& InGrid2D) noexcept : Grid2D{ InGrid2D } {}
UCommon::FGridCube::FGridCube() noexcept = default;

UCommon::FGrid2D UCommon::FGridCube::Flat() const noexcept
{
	return { Grid2D.Width, Grid2D.Height * (uint64_t)ECubeFace::NumCubeFaces };
}

uint64_t UCommon::FGridCube::GetArea() const noexcept { return Grid2D.GetArea() * (uint64_t)ECubeFace::NumCubeFaces; }

uint64_t UCommon::FGridCube::GetIndex(const FCubePoint& CubePoint) const noexcept
{
	return Grid2D.GetIndex(CubePoint.Point) + Grid2D.GetArea() * (uint64_t)CubePoint.Face;
}

UCommon::FCubePoint UCommon::FGridCube::GetPoint(uint64_t Index) const noexcept
{
	UBPA_UCOMMON_ASSERT(Index < (uint64_t)ECubeFace::NumCubeFaces * Grid2D.GetArea());
	return { ECubeFace(Index / Grid2D.GetArea()), Grid2D.GetPoint(Index % Grid2D.GetArea()) };
}

UCommon::FGridCubeIterator UCommon::FGridCube::GetIterator(const FCubePoint& CubePoint) const noexcept
{
	FGridCubeIterator Iterator;
	Iterator.GridCube = *this;
	Iterator.CubePoint = CubePoint;
	return Iterator;
}

UCommon::FGridCubeIterator UCommon::FGridCube::GetIterator(uint64_t Index) const noexcept { return GetIterator(GetPoint(Index)); }

UCommon::FGridCubeIterator UCommon::FGridCube::begin() const noexcept { return GetIterator(FCubePoint{ ECubeFace::PositiveX, { 0, 0 } }); }
UCommon::FGridCubeIterator UCommon::FGridCube::end() const noexcept { return GetIterator(FCubePoint{ ECubeFace::NumCubeFaces, { 0, 0 } }); }

namespace UCommon
{
	bool operator==(const FGridCube& Lhs, const FGridCube& Rhs) noexcept
	{
		return Lhs.Grid2D == Rhs.Grid2D;
	}

	bool operator!=(const FGridCube& Lhs, const FGridCube& Rhs) noexcept
	{
		return !(Lhs == Rhs);
	}
}

//
// FTexCube
///////////

UCommon::FTexCube::FTexCube() noexcept = default;

UCommon::FTexCube::FTexCube(FTex2D InTex2D) noexcept :
	Tex2D{ std::move(InTex2D) } {}

UCommon::FTexCube::FTexCube(FTexCube&& Other) noexcept :
	Tex2D(std::move(Other.Tex2D)) {}

UCommon::FTexCube::FTexCube(const FTexCube& Other) :
	Tex2D(Other.Tex2D) {}

UCommon::FTexCube::~FTexCube() = default;

UCommon::FGridCube UCommon::FTexCube::GetGridCube() const noexcept
{
	const FGrid2D Grid2D = Tex2D.GetGrid2D();
	return FGridCube{ FGrid2D{ Grid2D.Width, Grid2D.Height / (uint64_t)ECubeFace::NumCubeFaces } };
}

void UCommon::FTexCube::BilinearSample(float* Result, const FCubeTexcoord& CubeTexcoord) const noexcept
{
	const void* Storage = Tex2D.GetStorage();
	const uint64_t Offset = Tex2D.GetStorageSizeInBytes() / (uint64_t)ECubeFace::NumCubeFaces * (uint64_t)CubeTexcoord.Face;
	const void* OffsetedStorage = (const uint8_t*)Storage + Offset;
	const FTex2D FaceTex2D(GetGridCube().Grid2D, Tex2D.GetNumChannels(), Tex2D.GetElementType(), OffsetedStorage);
	FaceTex2D.BilinearSample(Result, CubeTexcoord.Texcoord);
}

void UCommon::FTexCube::ToEquirectangular(FTex2D& Equirectangular) const
{
	UBPA_UCOMMON_ASSERT(Equirectangular.IsValid());
	std::unique_ptr<float[]> Buffer = std::make_unique<float[]>(Tex2D.GetNumChannels());
	for (const auto& Point : Equirectangular.GetGrid2D())
	{
		const FVector2f UV = Equirectangular.GetGrid2D().GetTexcoord(Point);
		const FVector3f Direction = EquirectangularUVToDirection(UV);
		const FCubeTexcoord CubeTexcoord{ Direction };
		BilinearSample(Buffer.get(), CubeTexcoord);
		for (uint64_t ChannelIndex = 0; ChannelIndex < Equirectangular.GetNumChannels(); ChannelIndex++)
		{
			Equirectangular.SetFloat(Point, ChannelIndex, Buffer[ChannelIndex]);
		}
	}
}

UCommon::FTex2D UCommon::FTexCube::ToEquirectangular() const
{
	FTex2D Equirectangular({ Tex2D.GetGrid2D().Width * 4,Tex2D.GetGrid2D().Height / 3 }, Tex2D.GetNumChannels(), Tex2D.GetElementType());
	ToEquirectangular(Equirectangular);
	return Equirectangular;
}

UCommon::FTexCube& UCommon::FTexCube::operator=(FTexCube&& Rhs) noexcept
{
	Tex2D = std::move(Rhs.Tex2D);
	return *this;
}

UCommon::FTexCube& UCommon::FTexCube::operator=(const FTexCube& Rhs)
{
	Tex2D = Rhs.Tex2D;
	return *this;
}
