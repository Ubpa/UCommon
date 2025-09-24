#include <UCommon/UCommon.h>
#include <UCommon_ext/Tex2DIO.h>

#include <iostream>

using namespace UCommon;

int main()
{
	FFileArchive Reader(IArchive::EState::Loading, "Cubemap_128.bin");
	FTex2D TexCubeTex2D;
	TexCubeTex2D.Serialize(Reader);
	FTexCube TexCube(TexCubeTex2D);
	const FGridCube GridCube = TexCube.GetGridCube();
	const FGrid2D Grid2D{ GridCube.Grid2D.Width * 4,GridCube.Grid2D.Height * 2 };
	FTex2D Equirectangular(Grid2D, 3, EElementType::Float);
	SaveImage("Cubemap_128.hdr", TexCubeTex2D);
	for (const auto& Point : Grid2D)
	{
		const FVector2f UV = Grid2D.GetTexcoord(Point);
		const FVector3f Direction = EquirectangularUVToDirection(UV);
		const FCubeTexcoord CubeTexcoord{ Direction };
		TexCube.BilinearSample(Equirectangular.At<FVector3f>(Point).GetData(), CubeTexcoord);
	}
	SaveImage("Cubemap_128_Equirectangular.hdr", Equirectangular);
	for (const auto& CubePoint : GridCube)
	{
		const FCubeTexcoord CubeTexcoord{ CubePoint, GridCube };
		const FVector3f Direction = CubeTexcoord.Direction();
		const FVector2f UV = EquirectangularDirectionToUV(Direction);
		const FUint64Vector2 FlatPoint = CubePoint.Flat(GridCube);
		Equirectangular.BilinearSample(TexCube.Tex2D.At<FVector3f>(FlatPoint).GetData(), UV);
	}
	SaveImage("Cubemap_128_TexCube.hdr", TexCube.Tex2D);

	return 0;
}
