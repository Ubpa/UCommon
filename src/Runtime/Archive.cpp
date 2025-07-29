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

	struct FHeader
	{
		uint8_t Magic[4] = { 'U', 'b', 'p', 'a' };
		uint32_t NumVerisonKeys = 0;
		uint64_t VersionMapOffset = 0;
	};

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

void UCommon::IArchive::UseVersion(uint64_t Key, int64_t Version)
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

void UCommon::IArchive::LoadVersion(uint64_t Key, int64_t Version)
{
	UBPA_UCOMMON_ASSERT(Impl->VersionMap.find(Key) == Impl->VersionMap.end());
	Impl->VersionKeys.push_back(Key);
	Impl->VersionMap[Key] = Version;
}

void UCommon::IArchive::OnInit()
{
	FImpl::FHeader Header;
	ByteSerialize(Header);
	UBPA_UCOMMON_ASSERT(std::memcmp(Header.Magic, "Ubpa", 4) == 0);
	if (GetState() == EState::Loading
		&& Header.NumVerisonKeys > 0)
	{
		Seek(Header.VersionMapOffset);
		for (uint32_t Index = 0; Index < Header.NumVerisonKeys; Index++)
		{
			uint64_t Key;
			int64_t Version;
			ByteSerialize(Key);
			ByteSerialize(Version);
			LoadVersion(Key, Version);
		}
		Seek(sizeof(FImpl::FHeader));
	}
}

void UCommon::IArchive::OnDestroy()
{
	if (!Impl || GetState() != EState::Saving)
	{
		return;
	}

	FImpl::FHeader Header;
	Header.VersionMapOffset = Tell();
	Header.NumVerisonKeys = (uint32_t)GetVersionKeys().Num();
	Seek(0);
	ByteSerialize(Header);
	Seek(Header.VersionMapOffset);
	for (uint64_t Key : GetVersionKeys())
	{
		ByteSerialize(Key);
		int64_t Version = GetVersion(Key);
		ByteSerialize(Version);
	}
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
	std::vector<uint8_t> WriteStorage;
	uint64_t Index = 0;
};

UCommon::FMemoryArchive::FMemoryArchive(TSpan<const uint8_t> Storage)
	: IArchive(Storage.Empty() ? EState::Saving : EState::Loading)
	, Impl(new (UBPA_UCOMMON_MALLOC(sizeof(FImpl)))FImpl(Storage))
{
	OnInit();
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
		OnDestroy();
		Impl->~FImpl();
		UBPA_UCOMMON_FREE(Impl);
	}
}

void UCommon::FMemoryArchive::Serialize(void* Pointer, uint64_t Length)
{
	if (GetState() == EState::Loading)
	{
		UBPA_UCOMMON_ASSERT(Impl->Index + Length <= Impl->ReadStorage.Num());
		std::memcpy(Pointer, Impl->ReadStorage.GetData() + Impl->Index, Length);
		Impl->Index += Length;
	}
	else
	{
		const size_t OriginalSize = Impl->WriteStorage.size();
		Impl->WriteStorage.resize(OriginalSize + Length);
		std::memcpy(Impl->WriteStorage.data() + Impl->Index, Pointer, Length);
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

void UCommon::FMemoryArchive::Seek(uint64_t Index)
{
	Impl->Index = Index;
}

uint64_t UCommon::FMemoryArchive::Tell() const
{
	return Impl->Index;
}

//////////////////
// FFileArchive //
//////////////////

struct UCommon::FFileArchive::FImpl
{
	struct FHeader
	{
		uint8_t Magic[4] = { 'U', 'b', 'p', 'a' };
		uint32_t NumVerisonKeys = 0;
		uint64_t VersionMapOffset = 0;
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
	if (IsValid())
	{
		OnInit();
	}
}

UCommon::FFileArchive::~FFileArchive()
{
	if (Impl)
	{
		OnDestroy();
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
	UBPA_UCOMMON_ASSERT(IsValid());
	if (GetState() == EState::Loading)
	{
		Impl->Ifs.read((char*)Pointer, Length);
	}
	else
	{
		Impl->Ofs.write((const char*)Pointer, Length);
	}
}

void UCommon::FFileArchive::Seek(uint64_t Index)
{
	UBPA_UCOMMON_ASSERT(IsValid());
	if (GetState() == EState::Loading)
	{
		Impl->Ifs.seekg(Index);
	}
	else
	{
		Impl->Ofs.seekp(Index);
	}
}

uint64_t UCommon::FFileArchive::Tell() const
{
	UBPA_UCOMMON_ASSERT(IsValid());
	if (GetState() == EState::Loading)
	{
		return Impl->Ifs.tellg();
	}
	else
	{
		return Impl->Ofs.tellp();
	}
}
