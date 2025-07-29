#include <UCommon/Archive.h>

#include <unordered_map>
#include <fstream>

//////////////
// IArchive //
//////////////

struct UCommon::IArchive::FImpl
{
	FImpl(EState InState)
		: State(InState)
	{
	}

	EState State;
	std::vector<uint64_t> VersionKeys;
	std::unordered_map<uint64_t, int64_t> VersionMap;
};

UCommon::IArchive::IArchive(EState State)
	: Impl(new UBPA_UCOMMON_MALLOC(sizeof(FImpl))FImpl(State))
{
}

UCommon::IArchive::IArchive(IArchive&& Other) noexcept
	: Impl(Other.Impl)
{
	Other.Impl = nullptr;
}

UCommon::IArchive& UCommon::IArchive::operator=(IArchive&& Other) noexcept
{
	IArchive Temp(std::move(Other));
	Swap(Temp);
	return *this;
}

UCommon::IArchive::~IArchive()
{
	if (Impl)
	{
		Impl->~FImpl();
		UBPA_UCOMMON_FREE(Impl);
	}
}

void UCommon::IArchive::UseVersion(uint64_t Key, uint64_t Version)
{
	if (Impl->State == EState::Loading)
	{
		return;
	}

	auto Target = Impl->VersionMap.find(Key);
	if(Target == Impl->VersionMap.end())
	{
		Impl->VersionKeys.push_back(Key);
		Impl->VersionMap[Key] = Version;
	}
	else
	{
		Target->second = Version;
	}
}

void UCommon::IArchive::LoadVersion(uint64_t Key, uint64_t Version)
{
	UBPA_UCOMMON_ASSERT(Impl->VersionMap.find(Key) == Impl->VersionMap.end());
	Impl->VersionKeys.push_back(Key);
	Impl->VersionMap[Key] = Version;
}

int64_t UCommon::IArchive::GetVersion(uint64_t Key) const
{
	return Impl->VersionMap.count(Key) ? Impl->VersionMap.at(Key) : -1;
}

UCommon::TSpan<const uint64_t> UCommon::IArchive::GetVersionKeys() const
{
	return { Impl->VersionKeys.data(), Impl->VersionKeys.size() };
}

void UCommon::IArchive::Swap(IArchive& Other) noexcept
{
	std::swap(Impl, Other.Impl);
}

UCommon::IArchive::EState UCommon::IArchive::GetState() const
{
	return Impl->State;
}

////////////////////
// FMemoryArchive //
////////////////////

struct UCommon::FMemoryArchive::FImpl
{
	FImpl(TSpan<const uint8_t> Storage)
		: ReadStorage(Storage)
	{
	}

	TSpan<const uint8_t> ReadStorage;
	uint64_t ReadIndex = 0;
	std::vector<uint8_t> WriteStorage;
};

UCommon::FMemoryArchive::FMemoryArchive(TSpan<const uint8_t> Storage)
	: IArchive(Storage.Empty() ? EState::Saving : EState::Loading)
	, Impl(new (UBPA_UCOMMON_MALLOC(sizeof(FImpl)))FImpl(Storage))
{
}

UCommon::FMemoryArchive::FMemoryArchive(FMemoryArchive&& Other) noexcept
	: IArchive(std::move(Other))
	, Impl(Other.Impl)
{
	Other.Impl = nullptr;
}

void UCommon::FMemoryArchive::Swap(FMemoryArchive& Other) noexcept
{
	IArchive::Swap(Other);
	std::swap(Impl, Other.Impl);
}

UCommon::FMemoryArchive& UCommon::FMemoryArchive::operator=(FMemoryArchive&& Other) noexcept
{
	FMemoryArchive Temp(std::move(Other));
	Swap(Temp);
	return *this;
}

UCommon::FMemoryArchive::~FMemoryArchive()
{
	if (Impl)
	{
		Impl->~FImpl();
		UBPA_UCOMMON_FREE(Impl);
	}
}

void UCommon::FMemoryArchive::Serialize(void* Pointer, uint64_t Length)
{
	if (GetState() == EState::Loading)
	{
		UBPA_UCOMMON_ASSERT(Impl->ReadIndex + Length <= Impl->ReadStorage.Num());
		std::memcpy(Pointer, Impl->ReadStorage.GetData() + Impl->ReadIndex, Length);
		Impl->ReadIndex += Length;
	}
	else
	{
		const size_t OriginalSize = Impl->WriteStorage.size();
		Impl->WriteStorage.resize(OriginalSize + Length);
		std::memcpy(Impl->WriteStorage.data() + OriginalSize, Pointer, Length);
	}
}

UCommon::TSpan<const uint8_t> UCommon::FMemoryArchive::GetStorage() const
{
	if (GetState() == EState::Loading)
	{
		return Impl->ReadStorage;
	}
	else
	{
		return { Impl->WriteStorage.data(), Impl->WriteStorage.size() };
	}
}

//////////////////
// FFileArchive //
//////////////////

struct UCommon::FFileArchive::FImpl
{
	struct FHeader
	{
		uint64_t VersionMapOffset = 0;
		uint8_t Magic[8] = { 'U', 'B', 'P', 'A', '1', '2', '0', '6' };
	};

	FImpl(EState State, const char* FilePath)
	{
		if (State == EState::Loading)
		{
			Ifs.open(FilePath, std::ifstream::binary);
		}
		else
		{
			Ofs.open(FilePath, std::ofstream::binary);
		}
	}
	std::ifstream Ifs;
	std::ofstream Ofs;
};

UCommon::FFileArchive::FFileArchive(EState State, const char* FilePath)
	: IArchive(State)
	, Impl(new UBPA_UCOMMON_MALLOC(sizeof(FImpl))FImpl(State, FilePath))
{
	if (!IsValid())
	{
		return;
	}
	FImpl::FHeader Header;
	ByteSerialize(Header);
	if (GetState() == EState::Loading)
	{
		UBPA_UCOMMON_ASSERT(std::memcmp(Header.Magic, "UBPA1206", 8) == 0);
		Impl->Ifs.seekg(Header.VersionMapOffset);
		uint64_t Num;
		ByteSerialize(Num);
		for (uint64_t Index = 0; Index < Num; Index++)
		{
			uint64_t Key;
			int64_t Version;
			ByteSerialize(Key);
			ByteSerialize(Version);
			LoadVersion(Key, Version);
		}
		Impl->Ifs.seekg(sizeof(FImpl::FHeader));
	}
}

UCommon::FFileArchive::~FFileArchive()
{
	if (Impl)
	{
		if (GetState() == EState::Saving)
		{
			FImpl::FHeader Header;
			Header.VersionMapOffset = Impl->Ofs.tellp();
			Impl->Ofs.seekp(0);
			ByteSerialize(Header);
			Impl->Ofs.seekp(Header.VersionMapOffset);
			uint64_t NumKeys = GetVersionKeys().Num();
			ByteSerialize(NumKeys);
			for (uint64_t Key : GetVersionKeys())
			{
				ByteSerialize(Key);
				int64_t Version = GetVersion(Key);
				ByteSerialize(Version);
			}
		}

		Impl->~FImpl();
		UBPA_UCOMMON_FREE(Impl);
	}
}

UCommon::FFileArchive::FFileArchive(FFileArchive&& Other) noexcept
	: IArchive(std::move(Other))
	, Impl(Other.Impl)
{
	Other.Impl = nullptr;
}

void UCommon::FFileArchive::Swap(FFileArchive& Other) noexcept
{
	IArchive::Swap(Other);
	std::swap(Impl, Other.Impl);
}

UCommon::FFileArchive& UCommon::FFileArchive::operator=(FFileArchive&& Other) noexcept
{
	FFileArchive Temp(std::move(Other));
	Swap(Temp);
	return *this;
}

bool UCommon::FFileArchive::IsValid() const
{
	return Impl->Ifs.is_open() || Impl->Ofs.is_open();
}

void UCommon::FFileArchive::Serialize(void* Pointer, uint64_t Length)
{
	if (GetState() == EState::Loading)
	{
		Impl->Ifs.read((char*)Pointer, Length);
	}
	else
	{
		Impl->Ofs.write((const char*)Pointer, Length);
	}
}
