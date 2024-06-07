#include <UCommon/UCommon.h>

#include <stdio.h>
#include <iostream>

using namespace UCommon;

int main()
{
	FVector RGB(1.f, 0.5f, 0.3f);
	FVector NewRGB = YCoCgToRGB(RGBToYCoCg(RGB));
	printf("RGB(%f,%f,%f) == NewRGB(%f,%f,%f)\n", RGB.X, RGB.Y, RGB.Z, NewRGB.X, NewRGB.Y, NewRGB.Z);

	const FVector N = FVector(1.f).SafeNormalize();
	constexpr float Nf = 0.57735026f;

	printf("SH<0, 0>(%f,%f,%f) == %f\n", Nf, Nf, Nf, SH<0, 0>(Nf, Nf, Nf));
	printf("SH<1,-1>(%f,%f,%f) == %f\n", Nf, Nf, Nf, SH<1,-1>(Nf, Nf, Nf));
	printf("SH<1, 0>(%f,%f,%f) == %f\n", Nf, Nf, Nf, SH<1, 0>(Nf, Nf, Nf));
	printf("SH<1, 1>(%f,%f,%f) == %f\n", Nf, Nf, Nf, SH<1, 1>(Nf, Nf, Nf));
	printf("SH<2,-2>(%f,%f,%f) == %f\n", Nf, Nf, Nf, SH<2,-2>(Nf, Nf, Nf));
	printf("SH<2,-1>(%f,%f,%f) == %f\n", Nf, Nf, Nf, SH<2,-1>(Nf, Nf, Nf));
	printf("SH<2, 0>(%f,%f,%f) == %f\n", Nf, Nf, Nf, SH<2, 0>(Nf, Nf, Nf));
	printf("SH<2, 1>(%f,%f,%f) == %f\n", Nf, Nf, Nf, SH<2, 1>(Nf, Nf, Nf));
	printf("SH<2, 2>(%f,%f,%f) == %f\n", Nf, Nf, Nf, SH<2, 2>(Nf, Nf, Nf));

	FSHVector3 SHVector;
	for (int Index = 0; Index < FSHVector3::MaxSHBasis; Index++)
	{
		SHVector.V[Index] = 1.f;
	}

	FSHVectorRGB3 SHVectorRGB3;
	SHVectorRGB3.R = SHVector;
	SHVectorRGB3.G = SHVector * 2.f;
	SHVectorRGB3.B = SHVector;

	SHVectorRGB3.ToYCoCg();
	
	return 0;
}
