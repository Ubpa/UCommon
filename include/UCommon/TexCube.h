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

#pragma once

#include "Tex2D.h"

#define UBPA_UCOMMON_TEXCUBE_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
    using ECubeFace = UCommon::ECubeFace; \
    using FCubePoint = UCommon::FCubePoint; \
    using FCubeTexcoord = UCommon::FCubeTexcoord; \
    using FGridCube = UCommon::FGridCube; \
    using FGridCubeIterator = UCommon::FGridCubeIterator; \
    using FTexCube = UCommon::FTexCube; \
}

namespace UCommon
{
	UBPA_UCOMMON_API FVector2f EquirectangularDirectionToUV(const FVector3f& Direction);
	UBPA_UCOMMON_API FVector3f EquirectangularUVToDirection(const FVector2f& UV);

	struct FGridCube;

	enum class ECubeFace : std::uint64_t
	{
		PositiveX = 0,
		NegativeX = 1,
		PositiveY = 2,
		NegativeY = 3,
		PositiveZ = 4,
		NegativeZ = 5,
		NumCubeFaces,
	};

	struct UBPA_UCOMMON_API FCubePoint
	{
		ECubeFace Face = ECubeFace::PositiveX;
		FUint64Vector2 Point;

		FUint64Vector2 Flat(const FGridCube& GridCube) const noexcept;
	};

	struct UBPA_UCOMMON_API FCubeTexcoord
	{
		FCubeTexcoord() noexcept;
		FCubeTexcoord(ECubeFace InFace, const FVector2f& InTexcoord) noexcept;
		FCubeTexcoord(const FCubePoint& CubePoint, const FGridCube& GridCube) noexcept;
		FCubeTexcoord(const FVector3f& Direction) noexcept;

		FVector3f Direction() const noexcept;

		ECubeFace Face = ECubeFace::PositiveX;
		FVector2f Texcoord{ 0.f }; // [0,1]x[0,1]
	};

	struct FGridCubeIterator;

	struct UBPA_UCOMMON_API FGridCube
	{
		FGrid2D Grid2D;

		FGridCube(const FGridCube& InGridCube) noexcept;
		explicit FGridCube(const FGrid2D& InGrid2D) noexcept;
		FGridCube() noexcept;

		FGrid2D Flat() const noexcept;

		/** Width x Height x ECubeFace::NumCubeFaces */
		uint64_t GetArea() const noexcept;

		bool IsAreaEmpty() const noexcept;

		uint64_t GetIndex(const FCubePoint& CubePoint) const noexcept;
		FCubePoint GetPoint(uint64_t Index) const noexcept;

		FGridCubeIterator GetIterator(const FCubePoint& CubePoint) const noexcept;
		FGridCubeIterator GetIterator(uint64_t Index) const noexcept;

		FGridCubeIterator begin() const noexcept;
		FGridCubeIterator end() const noexcept;

		UBPA_UCOMMON_API friend bool operator==(const FGridCube& Lhs, const FGridCube& Rhs) noexcept;
		UBPA_UCOMMON_API friend bool operator!=(const FGridCube& Lhs, const FGridCube& Rhs) noexcept;
	};

	struct UBPA_UCOMMON_API FGridCubeIterator
	{
		FGridCube GridCube;
		FCubePoint CubePoint;

		FGridCubeIterator& operator++() noexcept
		{
			if (++CubePoint.Point.X == GridCube.Grid2D.Width)
			{
				CubePoint.Point.X = 0;
				if (++CubePoint.Point.Y == GridCube.Grid2D.Height)
				{
					CubePoint.Point.Y = 0;
					CubePoint.Face = (ECubeFace)((uint64_t)CubePoint.Face + 1);
				}
			}

			return *this;
		}

		const FCubePoint& operator*() const noexcept
		{
			return CubePoint;
		}

		friend bool operator!=(const FGridCubeIterator& Lhs, const FGridCubeIterator& Rhs) noexcept
		{
			UBPA_UCOMMON_ASSERT(Lhs.GridCube == Rhs.GridCube);
			return Lhs.CubePoint.Point != Rhs.CubePoint.Point || Lhs.CubePoint.Face != Rhs.CubePoint.Face;
		}
	};
	
	class UBPA_UCOMMON_API FTexCube
	{
	public:
		FTex2D FlatTex2D;

		FTexCube() noexcept;

		FTexCube(FTex2D InFlatTex2D) noexcept;

		/**
		 * Copy with propogated ownership.
		 * When the Ownership of Other is TakeOwnership, it causes the internal use of `malloc`.
		 */
		FTexCube(const FTexCube& Other);

		FTexCube(FTexCube&& Other) noexcept;

		~FTexCube();

		FGridCube GetGridCube() const noexcept;

		void BilinearSample(float* Result, const FCubeTexcoord& CubeTexcoord, ETextureAddress AddressMode = ETextureAddress::Wrap) const noexcept;

		void ToEquirectangular(FTex2D& Equirectangular) const;
		FTex2D ToEquirectangular() const;

		/**
		 * If layout is same with Rhs's, just copy the storage,
		 * else copy with propogated ownership.
		 * When the Ownership of Other is TakeOwnership, it causes the internal use of `malloc`.
		 */
		FTexCube& operator=(const FTexCube& Rhs);

		FTexCube& operator=(FTexCube&& Rhs) noexcept;
	};
} // UCommon

UBPA_UCOMMON_TEXCUBE_TO_NAMESPACE(UCommonTest)
