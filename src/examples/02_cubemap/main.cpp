#include <UCommon/UCommon.h>
#include <UCommon_ext/Tex2DIO.h>

#include <iostream>

using namespace UCommon;

int main()
{
	FTex2D TexCubeTex2D;
	{
		FFileArchive Reader(IArchive::EState::Loading, "Cubemap_128.bin");
		TexCubeTex2D.Serialize(Reader);
	}
	SaveImage("Cubemap_128.hdr", TexCubeTex2D);
	FTexCube TexCube(TexCubeTex2D);
	const FTex2D Equirectangular = TexCube.ToEquirectangular();
	SaveImage("Cubemap_128_Equirectangular.hdr", Equirectangular);
	TexCube = Equirectangular.ToTexCube();
	SaveImage("Cubemap_128_TexCube.hdr", TexCube.Tex2D);

	return 0;
}
