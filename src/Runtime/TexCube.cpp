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

UCommon::FGrid2D UCommon::FGridCube::GetFlatGrid2D() const noexcept
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
UCommon::FGridCubeIterator UCommon::FGridCube::end() const noexcept { return GetIterator(FCubePoint{ ECubeFace::NegativeZ, { 0, 0 } }); }

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
