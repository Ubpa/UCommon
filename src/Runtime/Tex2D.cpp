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

#include <UCommon/Tex2D.h>
#include <UCommon/TexCube.h>


//
// FGrid2D
///////////

UCommon::FGrid2D::FGrid2D(uint64_t InWidth, uint64_t InHeight) noexcept :
	Width(InWidth), Height(InHeight) {}

UCommon::FGrid2D::FGrid2D(const FUint64Vector2& Size) noexcept : FGrid2D(Size.X, Size.Y) {}

UCommon::FGrid2D::FGrid2D() noexcept :
	Width(0), Height(0) {
}

uint64_t UCommon::FGrid2D::GetArea() const noexcept { return Width * Height; }

bool UCommon::FGrid2D::IsAreaEmpty() const noexcept { return Width == 0 || Height == 0; }

bool UCommon::FGrid2D::Contains(const FUint64Vector2& Point) const noexcept { return Point.X < Width && Point.Y < Height; }

uint64_t UCommon::FGrid2D::GetIndex(const FUint64Vector2& Point) const noexcept
{
	UBPA_UCOMMON_ASSERT(Contains(Point));
	return Point.Y * Width + Point.X;
}

UCommon::FVector2f UCommon::FGrid2D::GetTexcoord(const FUint64Vector2& Point) const noexcept
{
	return (FVector2f(Point) + 0.5f) / FVector2f(GetExtent());
}

UCommon::FUint64Vector2 UCommon::FGrid2D::GetPoint(const FVector2f& Texcoord) const noexcept
{
	UBPA_UCOMMON_ASSERT(FVector2f(0.f) <= Texcoord && Texcoord <= FVector2f(1.f));
	UBPA_UCOMMON_ASSERT(!IsAreaEmpty());

	return FUint64Vector2(FInt64Vector2(Texcoord - 0.5f).Clamp(FInt64Vector2(0), FInt64Vector2(GetExtent() - 1)));
}

UCommon::FUint64Vector2 UCommon::FGrid2D::GetPoint(uint64_t Index) const noexcept
{
	UBPA_UCOMMON_ASSERT(Index <= GetArea());

	const uint64_t X = Index % Width;
	const uint64_t Y = Index / Width;

	return FUint64Vector2{ X,Y };
}

UCommon::FUint64Vector2 UCommon::FGrid2D::GetExtent() const noexcept
{
	return FUint64Vector2(Width, Height);
}

uint64_t UCommon::FGrid2D::GetNumMips() const noexcept
{
	return std::min(MSB64(Width), MSB64(Height)) + TypedOne<uint8_t>;
}

uint64_t& UCommon::FGrid2D::operator[](uint64_t Index) noexcept
{
	UBPA_UCOMMON_ASSERT(Index < 2);
	return reinterpret_cast<uint64_t*>(this)[Index];
}

const uint64_t& UCommon::FGrid2D::operator[](uint64_t Index) const noexcept
{
	return const_cast<FGrid2D*>(this)->operator[](Index);
}

UCommon::FGrid2DIterator UCommon::FGrid2D::GetIterator(const FUint64Vector2& Point) const noexcept
{
	FGrid2DIterator Iterator;
	Iterator.Grid2D = *this;
	Iterator.Point = Point;
	return Iterator;
}

UCommon::FGrid2DIterator UCommon::FGrid2D::GetIterator(uint64_t Index) const noexcept { return GetIterator(GetPoint(Index)); }

UCommon::FGrid2DIterator UCommon::FGrid2D::begin() const noexcept { return GetIterator(FUint64Vector2(0)); }
UCommon::FGrid2DIterator UCommon::FGrid2D::end() const noexcept { return GetIterator({ 0,Height }); }

namespace UCommon
{
	bool operator==(const FGrid2D& Lhs, const FGrid2D& Rhs) noexcept
	{
		return Lhs.Width == Rhs.Width
			&& Lhs.Height == Rhs.Height;
	}

	bool operator!=(const FGrid2D& Lhs, const FGrid2D& Rhs) noexcept
	{
		return !(Lhs == Rhs);
	}
}

//
// FTex2D
///////////

uint64_t UCommon::FTex2D::GetRequiredStorageSizeInBytes(FGrid2D InGrid2D, uint64_t InNumChannels, EElementType ElementType) noexcept
{
	return InGrid2D.GetArea() * InNumChannels * ElementGetSize(ElementType);
}

uint64_t UCommon::FTex2D::GetNumElements(FGrid2D Grid2D, uint64_t NumChannels) noexcept
{
	return Grid2D.GetArea() * NumChannels;
}

UCommon::FTex2D::FTex2D() noexcept :
	NumChannels(0),
	Ownership(EOwnership::DoNotTakeOwnership),
	ElementType(EElementType::Unknown),
	Storage(nullptr) {}

UCommon::FTex2D::FTex2D(FGrid2D InGrid2D, uint64_t InNumChannels, EOwnership InOwnership, EElementType InElementType, void* InStorage) noexcept :
	Grid2D(InGrid2D),
	NumChannels(InNumChannels),
	Ownership(InOwnership),
	ElementType(InElementType),
	Storage(InStorage)
{
	UBPA_UCOMMON_ASSERT(!InGrid2D.IsAreaEmpty());
	UBPA_UCOMMON_ASSERT(InNumChannels > 0);
	UBPA_UCOMMON_ASSERT(InStorage);
}

UCommon::FTex2D::FTex2D(FGrid2D InGrid2D, uint64_t InNumChannels, EElementType InElementType, const void* InStorage)
	: FTex2D(InGrid2D, InNumChannels, EOwnership::TakeOwnership, InElementType,
		CreateCopy(InStorage, GetRequiredStorageSizeInBytes(InGrid2D, InNumChannels, InElementType))) {}

UCommon::FTex2D::FTex2D(FGrid2D InGrid2D, uint64_t InNumChannels, EElementType InElementType) :
	FTex2D(InGrid2D,
		InNumChannels,
		EOwnership::TakeOwnership,
		InElementType,
		UBPA_UCOMMON_MALLOC(GetRequiredStorageSizeInBytes(InGrid2D, InNumChannels, InElementType))) {
}

UCommon::FTex2D::FTex2D(FTex2D&& Other) noexcept :
	Grid2D(Other.GetGrid2D()),
	NumChannels(Other.NumChannels),
	Ownership(Other.Ownership),
	ElementType(Other.ElementType),
	Storage(Other.Storage)
{
	Other.Grid2D = FGrid2D();
	Other.NumChannels = 0;
	Other.Ownership = EOwnership::DoNotTakeOwnership;
	Other.ElementType = EElementType();
	Other.Storage = nullptr;
}

UCommon::FTex2D::FTex2D(const FTex2D& Other, EOwnership InOwnership, void* InEmptyStorage)
	: Grid2D(Other.GetGrid2D())
	, NumChannels(Other.NumChannels)
	, Ownership(InOwnership)
	, ElementType(Other.ElementType)
	, Storage(InEmptyStorage)
{
	UBPA_UCOMMON_ASSERT(Other.IsValid() || !InEmptyStorage);

	if (Storage || Ownership == EOwnership::TakeOwnership)
	{
		if (!Storage)
		{
			Storage = UBPA_UCOMMON_MALLOC(Other.GetStorageSizeInBytes());
		}
		memcpy(Storage, Other.Storage, Other.GetStorageSizeInBytes());
	}
	else
	{
		UBPA_UCOMMON_ASSERT(!Storage && Ownership == EOwnership::DoNotTakeOwnership);
		Storage = Other.Storage;
	}
}

UCommon::FTex2D::FTex2D(const FTex2D& Other) :
	FTex2D(Other, Other.Ownership, nullptr) {
}

UCommon::FTex2D::~FTex2D()
{
	if (Ownership == EOwnership::TakeOwnership)
	{
		UBPA_UCOMMON_FREE(Storage);
	}
}

bool UCommon::FTex2D::IsValid() const noexcept
{
	return !Grid2D.IsAreaEmpty() && NumChannels > 0 && Storage;
}

uint64_t UCommon::FTex2D::GetIndex(const FUint64Vector2& Point, uint64_t C) const noexcept
{
	UBPA_UCOMMON_ASSERT(C < NumChannels);
	return Grid2D.GetIndex(Point) * NumChannels + C;
}

float UCommon::FTex2D::GetFloat(uint64_t Index) const noexcept
{
	switch (ElementType)
	{
	case UCommon::EElementType::Uint8:
		return ElementUint8ToFloat(At<uint8_t>(Index));
	case UCommon::EElementType::Float:
		return At<float>(Index);
	case UCommon::EElementType::Double:
		return static_cast<float>(At<double>(Index));
	default:
		UBPA_UCOMMON_NO_ENTRY();
		return 0.f;
	}
}

UCommon::FLinearColorRGB UCommon::FTex2D::GetLinearColorRGB(const FUint64Vector2& Point) const noexcept
{
	switch (ElementType)
	{
	case UCommon::EElementType::Uint8:
		return ElementColorToLinearColor(At<FColorRGB>(Point));
	case UCommon::EElementType::Float:
		return At<FLinearColorRGB>(Point);
	case UCommon::EElementType::Double:
		return FLinearColorRGB(At<FDoubleColorRGB>(Point));
	default:
		UBPA_UCOMMON_NO_ENTRY();
		return FLinearColorRGB(0.f);
	}
}

UCommon::FLinearColor UCommon::FTex2D::GetLinearColor(const FUint64Vector2& Point) const noexcept
{
	switch (ElementType)
	{
	case UCommon::EElementType::Uint8:
		return ElementColorToLinearColor(At<FColor>(Point));
	case UCommon::EElementType::Float:
		return At<FLinearColor>(Point);
	case UCommon::EElementType::Double:
		return FLinearColor(At<FDoubleColor>(Point));
	default:
		UBPA_UCOMMON_NO_ENTRY();
		return FLinearColor(0.f);
	}
}

UCommon::FDoubleColorRGB UCommon::FTex2D::GetDoubleColorRGB(const FUint64Vector2& Point) const noexcept
{
	switch (ElementType)
	{
	case UCommon::EElementType::Uint8:
		return ElementColorToDoubleColor(At<FColorRGB>(Point));
	case UCommon::EElementType::Float:
		return FDoubleColorRGB(At<FLinearColorRGB>(Point));
	case UCommon::EElementType::Double:
		return At<FDoubleColorRGB>(Point);
	default:
		UBPA_UCOMMON_NO_ENTRY();
		return FDoubleColorRGB(0.f);
	}
}

UCommon::FDoubleColor UCommon::FTex2D::GetDoubleColor(const FUint64Vector2& Point) const noexcept
{
	switch (ElementType)
	{
	case UCommon::EElementType::Uint8:
		return ElementColorToDoubleColor(At<FColor>(Point));
	case UCommon::EElementType::Float:
		return FDoubleColor(At<FLinearColor>(Point));
	case UCommon::EElementType::Double:
		return At<FDoubleColor>(Point);
	default:
		UBPA_UCOMMON_NO_ENTRY();
		return FDoubleColor(0.f);
	}
}

float UCommon::FTex2D::GetFloat(const FUint64Vector2& Point, uint64_t C) const noexcept
{
	return GetFloat(GetIndex(Point, C));
}

void UCommon::FTex2D::SetFloat(uint64_t Index, float Value) noexcept
{
	switch (ElementType)
	{
	case UCommon::EElementType::Uint8:
		At<uint8_t>(Index) = ElementFloatToUint8(Value);
		break;
	case UCommon::EElementType::Float:
		At<float>(Index) = Value;
		break;
	case UCommon::EElementType::Double:
		At<double>(Index) = static_cast<double>(Value);
		break;
	default:
		UBPA_UCOMMON_NO_ENTRY();
		break;
	}
}

void UCommon::FTex2D::SetFloat(const FUint64Vector2& Point, uint64_t C, float Value) noexcept
{
	SetFloat(GetIndex(Point, C), Value);
}

uint64_t UCommon::FTex2D::GetNumChannels() const noexcept { return NumChannels; }
const UCommon::FGrid2D& UCommon::FTex2D::GetGrid2D() const noexcept { return Grid2D; }
uint64_t UCommon::FTex2D::GetNumElements() const noexcept { return GetNumElements(Grid2D, NumChannels); }
uint64_t UCommon::FTex2D::GetStorageSizeInBytes() const noexcept { return GetRequiredStorageSizeInBytes(Grid2D, NumChannels, ElementType); }
UCommon::EOwnership UCommon::FTex2D::GetStorageOwnership() const noexcept { return Ownership; }
UCommon::EElementType UCommon::FTex2D::GetElementType() const noexcept { return ElementType; }

void* UCommon::FTex2D::GetStorage() noexcept { return Storage; }
const void* UCommon::FTex2D::GetStorage() const noexcept { return Storage; }

void UCommon::FTex2D::Reset() noexcept
{
	if (Ownership == EOwnership::TakeOwnership)
	{
		UBPA_UCOMMON_FREE(Storage);
	}

	Grid2D = FGrid2D();
	NumChannels = 0;

	Ownership = EOwnership::DoNotTakeOwnership;
	ElementType = EElementType();
	Storage = nullptr;
}

bool UCommon::FTex2D::IsLayoutSameWith(const FTex2D& Other) const noexcept
{
	return Grid2D == Other.Grid2D
		&& ElementType == Other.ElementType
		&& NumChannels == Other.NumChannels;
}

void UCommon::FTex2D::BilinearSample(float* Result, const FVector2f& Texcoord) const noexcept
{
	UBPA_UCOMMON_ASSERT(FVector2f(0.f) <= Texcoord && Texcoord <= FVector2f(1.f));

	const FVector2f PointT = Texcoord * FVector2f(Grid2D.GetExtent());
	const FInt64Vector2 IntPoint0 = FInt64Vector2(PointT - 0.5f);
	const FInt64Vector2 IntPoint1 = (IntPoint0 + 1).Min(FInt64Vector2(Grid2D.GetExtent() - 1));

	UBPA_UCOMMON_ASSERT(IntPoint1 >= FInt64Vector2(0));

	const FUint64Vector2 Points[2] =
	{
		FUint64Vector2(IntPoint0.Max(0)),
		FUint64Vector2(IntPoint1),
	};

	const FVector2f LocalTexcoord = (PointT - (FVector2f(IntPoint0) + 0.5f)).Clamp(0.f, 1.f);

	const FVector2f OneMinusLocalTexcoord = FVector2f(1.f) - LocalTexcoord;

	const float Weights[4] =
	{
		OneMinusLocalTexcoord.X * OneMinusLocalTexcoord.Y,
		OneMinusLocalTexcoord.X * LocalTexcoord.Y,
		LocalTexcoord.X * OneMinusLocalTexcoord.Y,
		LocalTexcoord.X * LocalTexcoord.Y,
	};

	const uint64_t Offsets[4] =
	{
		Grid2D.GetIndex({ Points[0].X, Points[0].Y }) * NumChannels,
		Grid2D.GetIndex({ Points[0].X, Points[1].Y }) * NumChannels,
		Grid2D.GetIndex({ Points[1].X, Points[0].Y }) * NumChannels,
		Grid2D.GetIndex({ Points[1].X, Points[1].Y }) * NumChannels,
	};

	for (uint64_t C = 0; C < NumChannels; C++)
	{
		float Val2[4];
		for (uint64_t i = 0; i < 4; i++)
		{
			Val2[i] = GetFloat(Offsets[i] + C);
		}
		*Result++ = UCommon::BilinearInterpolate(Val2, Weights);
	}
}

namespace UCommon
{
	template<typename T>
	constexpr auto ToUpperType(const T& Value) noexcept
	{
		if constexpr (std::is_same_v<T, uint8_t>)
			return static_cast<uint16_t>(Value);
		else
			return Value;
	}
	template<typename T>
	constexpr T DivideRound(const T& Value, const uint64_t& D) noexcept
	{
		if constexpr (std::is_same_v<T, uint16_t>)
			return (Value + (static_cast<T>(D) / 2)) / static_cast<T>(D);
		else
			return Value / static_cast<T>(D);
	}

	template<typename T>
	static void DownSampleImpl(FTex2D& HalfTex, const FTex2D& SrcTex)
	{
		using UpperType = std::decay_t<decltype(ToUpperType(std::declval<T>()))>;
		const auto Buffer = std::make_unique<UpperType[]>(HalfTex.GetNumChannels());
		for (const FUint64Vector2& Point : HalfTex.GetGrid2D())
		{
			for (uint64_t C = 0; C < HalfTex.GetNumChannels(); ++C)
			{
				Buffer[C] = static_cast<T>(0);
			}
			uint64_t Count = 0;
			for (const FUint64Vector2& LocalPoint : FGrid2D(2, 2))
			{
				const FUint64Vector2 SrcPoint = 2 * Point + LocalPoint;
				if (SrcTex.GetGrid2D().Contains(SrcPoint))
				{
					for (uint64_t C = 0; C < HalfTex.GetNumChannels(); ++C)
					{
						Buffer[C] += ToUpperType(SrcTex.At<T>(SrcPoint, C));
					}
					Count += 1;
				}
			}
			for (uint64_t C = 0; C < HalfTex.GetNumChannels(); ++C)
			{
				HalfTex.At<T>(Point, C) = static_cast<T>(DivideRound(Buffer[C], Count));
			}
		}
	}
}

UCommon::FTex2D UCommon::FTex2D::DownSample(EOwnership InOwnership, void* InStorage) const
{
	const FGrid2D HalfGrid2D(std::max<uint64_t>(1, Grid2D.Width / 2), std::max<uint64_t>(1, Grid2D.Height / 2));

	FTex2D HalfTex = InStorage ? FTex2D(HalfGrid2D, NumChannels, InOwnership, ElementType, InStorage)
		: FTex2D(HalfGrid2D, NumChannels, ElementType);

	switch (ElementType)
	{
	case UCommon::EElementType::Uint8:
		DownSampleImpl<uint8_t>(HalfTex, *this);
		break;
	case UCommon::EElementType::Float:
		DownSampleImpl<float>(HalfTex, *this);
		break;
	case UCommon::EElementType::Double:
		DownSampleImpl<double>(HalfTex, *this);
		break;
	default:
		UBPA_UCOMMON_NO_ENTRY();
		break;
	}

	return HalfTex;
}

UCommon::FTex2D UCommon::FTex2D::DownSample() const
{
	return DownSample(EOwnership::DoNotTakeOwnership, nullptr);
}

UCommon::FTex2D UCommon::FTex2D::ToFloat() const
{
	FTex2D Tex(Grid2D, NumChannels, EElementType::Float);
	if (ElementType != EElementType::Float)
	{
		for (const FUint64Vector2& Point : Grid2D)
		{
			for (uint64_t C = 0; C < NumChannels; C++)
			{
				Tex.At<float>(Point, C) = GetFloat(Point, C);
			}
		}
	}
	else
	{
		std::memcpy(Tex.GetStorage(), Storage, GetStorageSizeInBytes());
	}
	return Tex;
}

void UCommon::FTex2D::ToUint8(FTex2D& Tex) const
{
	UBPA_UCOMMON_ASSERT(Tex.ElementType == EElementType::Uint8);
	UBPA_UCOMMON_ASSERT(Tex.Grid2D == Grid2D);
	UBPA_UCOMMON_ASSERT(Tex.NumChannels == NumChannels);

	if (ElementType != EElementType::Uint8)
	{
		for (const FUint64Vector2& Point : Grid2D)
		{
			for (uint64_t C = 0; C < NumChannels; C++)
			{
				Tex.At<uint8_t>(Point, C) = ElementFloatClampToUint8(GetFloat(Point, C));
			}
		}
	}
	else
	{
		std::memcpy(Tex.GetStorage(), Storage, GetStorageSizeInBytes());
	}
}

UCommon::FTex2D UCommon::FTex2D::ToUint8() const
{
	FTex2D Tex(Grid2D, NumChannels, EElementType::Uint8);
	ToUint8(Tex);
	return Tex;
}

void UCommon::FTex2D::ToTexCube(FTexCube& TexCube) const
{
	UBPA_UCOMMON_ASSERT(TexCube.FlatTex2D.IsValid());
	const FGridCube GridCube = TexCube.GetGridCube();
	std::unique_ptr<float[]> Buffer = std::make_unique<float[]>(NumChannels);
	for (const auto& CubePoint : GridCube)
	{
		const FCubeTexcoord CubeTexcoord{ CubePoint, GridCube };
		const FVector3f Direction = CubeTexcoord.Direction();
		const FVector2f UV = EquirectangularDirectionToUV(Direction);
		const FUint64Vector2 FlatPoint = CubePoint.Flat(GridCube);
		BilinearSample(Buffer.get(), UV);
		for (uint64_t ChannelIndex = 0; ChannelIndex < TexCube.FlatTex2D.GetNumChannels(); ChannelIndex++)
		{
			TexCube.FlatTex2D.SetFloat(FlatPoint, ChannelIndex, Buffer[ChannelIndex]);
		}
	}
}

UCommon::FTexCube UCommon::FTex2D::ToTexCube() const
{
	const FGridCube GridCube{ FGrid2D{Grid2D.Width / 4,Grid2D.Height / 2} };
	FTexCube TexCube{ FTex2D{GridCube.Flat(), NumChannels, ElementType} };
	ToTexCube(TexCube);
	return TexCube;
}

UCommon::FTex2D& UCommon::FTex2D::operator=(FTex2D&& Rhs) noexcept
{
	if (this != &Rhs)
	{
		if (Ownership == EOwnership::TakeOwnership)
		{
			UBPA_UCOMMON_FREE(Storage);
		}

		Grid2D = Rhs.Grid2D;
		NumChannels = Rhs.NumChannels;
		Ownership = Rhs.Ownership;
		ElementType = Rhs.ElementType;
		Storage = Rhs.Storage;

		Rhs.Grid2D = FGrid2D();
		Rhs.NumChannels = 0;
		Rhs.Ownership = EOwnership::DoNotTakeOwnership;
		Rhs.ElementType = EElementType();
		Rhs.Storage = nullptr;
	}
	return *this;
}

UCommon::FTex2D& UCommon::FTex2D::operator=(const FTex2D& Rhs)
{
	if (std::addressof(Rhs) != this)
	{
		if (Rhs.IsValid())
		{
			if (IsLayoutSameWith(Rhs))
			{
				memcpy(Storage, Rhs.Storage, Rhs.GetStorageSizeInBytes());
			}
			else
			{
				if (Ownership == EOwnership::TakeOwnership)
				{
					UBPA_UCOMMON_FREE(Storage);
				}

				Grid2D = Rhs.Grid2D;
				NumChannels = Rhs.NumChannels;
				ElementType = Rhs.ElementType;

				if (Rhs.Ownership == EOwnership::TakeOwnership)
				{
					UBPA_UCOMMON_ASSERT(Rhs.Storage);

					Ownership = EOwnership::TakeOwnership;
					Storage = UBPA_UCOMMON_MALLOC(Rhs.GetStorageSizeInBytes());
					if (Storage)
					{
						memcpy(Storage, Rhs.Storage, Rhs.GetStorageSizeInBytes());
					}
					else
					{
						Grid2D = FGrid2D();
						NumChannels = 0;
						ElementType = EElementType();
						Ownership = EOwnership::DoNotTakeOwnership;
						UBPA_UCOMMON_ASSERT(!Storage);
					}
				}
				else
				{
					Ownership = EOwnership::DoNotTakeOwnership;
					Storage = Rhs.Storage;
				}
			}
		}
		else
		{
			Reset();
		}
	}
	return *this;
}

void UCommon::FTex2D::Copy(FTex2D& Dst, const FUint64Vector2& DstPoint, const FTex2D& Src, const FUint64Vector2& SrcPoint, const FUint64Vector2& Range)
{
	UBPA_UCOMMON_ASSERT(Dst.ElementType == Src.ElementType);
	UBPA_UCOMMON_ASSERT(Dst.NumChannels == Src.NumChannels);
	const FGrid2D RangeGrid2D(Range);
	const uint64_t PixelSize = ElementGetSize(Dst.ElementType) * Dst.NumChannels;
	for (const auto& RangePoint : RangeGrid2D)
	{
		uint8_t* DstBuffer = reinterpret_cast<uint8_t*>(Dst.Storage) + Dst.GetGrid2D().GetIndex(DstPoint + RangePoint) * PixelSize;
		const uint8_t* SrcBuffer = reinterpret_cast<uint8_t*>(Src.Storage) + Src.GetGrid2D().GetIndex(SrcPoint + RangePoint) * PixelSize;
		std::memcpy(DstBuffer, SrcBuffer, PixelSize);
	}
}

void UCommon::FTex2D::Serialize(IArchive& Archive)
{
	Archive.ByteSerialize(Grid2D);
	Archive.ByteSerialize(NumChannels);
	Archive.ByteSerialize(Ownership);
	Archive.ByteSerialize(ElementType);
	if (Archive.GetState() == IArchive::EState::Loading)
	{
		UBPA_UCOMMON_ASSERT(Storage == nullptr);
		const uint64_t Size = GetStorageSizeInBytes();
		if (Size == 0)
		{
			Storage = nullptr;
		}
		else
		{
			Storage = UBPA_UCOMMON_MALLOC(Size);
			UBPA_UCOMMON_ASSERT(Storage);
		}
	}
	Archive.Serialize(Storage, GetStorageSizeInBytes());
}
