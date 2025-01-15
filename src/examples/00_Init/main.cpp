#include <UCommon/UCommon.h>

#include <stdio.h>
#include <iostream>

using namespace UCommon;

int main()
{
	FLinearColorRGB RGB(1.f, 0.5f, 0.3f);
	FLinearColorRGB NewRGB = YCoCgToRGB(RGBToYCoCg(RGB));
	printf("RGB(%f,%f,%f) == NewRGB(%f,%f,%f)\n", RGB.X, RGB.Y, RGB.Z, NewRGB.X, NewRGB.Y, NewRGB.Z);

	const FVector3f N = FVector3f(1.f).SafeNormalize();
	constexpr float Nf = 0.57735026f;

	printf("SHK<0, 0> == %f\n", SHK<0, 0>);
	printf("SHK<1,-1> == %f\n", SHK<1,-1>);
	printf("SHK<1, 0> == %f\n", SHK<1, 0>);
	printf("SHK<1, 1> == %f\n", SHK<1, 1>);
	printf("SHK<2,-2> == %f\n", SHK<2,-2>);
	printf("SHK<2,-1> == %f\n", SHK<2,-1>);
	printf("SHK<2, 0> == %f\n", SHK<2, 0>);
	printf("SHK<2, 1> == %f\n", SHK<2, 1>);
	printf("SHK<2, 2> == %f\n", SHK<2, 2>);

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

	FSHVectorRGB3 SHVectorRGB3 = SHVector * FVector3f(1.f, 2.f, 1.f);

	SHVectorRGB3.ToYCoCg();

	SHVectorRGB3.GetBand<2>().GetSrgbLuminance().GetLinearVector();

	FSHVectorRGB3 A = (SHVectorRGB3 * 2).As<FSHVectorRGB3>();

	TSHBandVector<1> B;

	FSHVector3 C1{ FSHVector2(), FSHBandVector3() };
	FSHVectorRGB3 C2{ FSHVectorRGB2(), FSHBandVectorRGB3() };

	FHalf h(1.f);
	std::cout << (float)h << std::endl;
	
	return 0;
}
