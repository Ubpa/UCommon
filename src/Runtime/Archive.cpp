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

#include <UCommon/Archive.h>

#include <vector>
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
		: ReadStorage(Storage) {}

	TSpan<const uint8_t> ReadStorage;
	std::vector<uint8_t> WriteStorage;
	uint64_t ReadIndex = 0;
};

UCommon::FMemoryArchive::FMemoryArchive(TSpan<const uint8_t> Storage)
	: IArchive(Storage.Empty() ? EState::Saving : EState::Loading)
	, Impl(new (UBPA_UCOMMON_MALLOC(sizeof(FImpl)))FImpl(Storage)) {}

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

/////////////////////
// FArchiveWrapper //
/////////////////////

struct UCommon::FArchiveWrapper::FImpl
{
	struct FHeader
	{
		uint8_t Magic[4] = { 'U', 'b', 'p', 'a' };
		uint32_t NumVersionKeys = 0;
		uint64_t Size = 0;
	};
	FImpl(IArchive* InArchive)
		: Archive(InArchive)
	{
		UBPA_UCOMMON_ASSERT(InArchive);
	}

	IArchive* Archive = nullptr;
	std::vector<uint8_t> WriteStorage;
};

UCommon::FArchiveWrapper::FArchiveWrapper(IArchive* InArchive)
	: IArchive(InArchive->GetState())
	, Impl(new (UBPA_UCOMMON_MALLOC(sizeof(FImpl)))FImpl(InArchive))
{
	for (uint64_t Key : Impl->Archive->GetVersionKeys())
	{
		UseVersion(Key, Impl->Archive->GetVersion(Key));
	}

	if (GetState() == EState::Loading)
	{
		FImpl::FHeader Header;
		Impl->Archive->ByteSerialize(Header);
		UBPA_UCOMMON_ASSERT(std::memcmp(Header.Magic, "Ubpa", 4) == 0);
		for (uint32_t Index = 0; Index < Header.NumVersionKeys; Index++)
		{
			uint64_t Key;
			int64_t Version;
			Impl->Archive->ByteSerialize(Key);
			Impl->Archive->ByteSerialize(Version);
			LoadVersion(Key, Version);
		}
		if (Header.Size == 0)
		{
			Impl->Archive = nullptr;
		}
	}
}

UCommon::FArchiveWrapper::FArchiveWrapper(FArchiveWrapper&& Other) noexcept
	: IArchive(std::move(Other))
	, Impl(Other.Impl)
{
	Other.Impl = nullptr;
}

void UCommon::FArchiveWrapper::Swap(FArchiveWrapper& Other) noexcept
{
	IArchive::Swap(Other);
	std::swap(Impl, Other.Impl);
}

UCommon::FArchiveWrapper& UCommon::FArchiveWrapper::operator=(FArchiveWrapper&& Other) noexcept
{
	FArchiveWrapper Temp(std::move(Other));
	Swap(Temp);
	return *this;
}

UCommon::FArchiveWrapper::~FArchiveWrapper()
{
	if (Impl)
	{
		if (GetState() == EState::Saving)
		{
			FImpl::FHeader Header;
			Header.Size = Impl->WriteStorage.size();
			Header.NumVersionKeys = Header.Size > 0 ? (uint32_t)GetVersionKeys().Num() : 0;
			Impl->Archive->ByteSerialize(Header);
			if (Header.NumVersionKeys > 0)
			{
				for (uint64_t Key : GetVersionKeys())
				{
					int64_t Version = GetVersion(Key);
					Impl->Archive->ByteSerialize(Key);
					Impl->Archive->ByteSerialize(Version);
				}
			}
			if (Header.Size > 0)
			{
				Impl->Archive->Serialize(Impl->WriteStorage.data(), Impl->WriteStorage.size());
			}
		}
		Impl->~FImpl();
		UBPA_UCOMMON_FREE(Impl);
	}
}

bool UCommon::FArchiveWrapper::IsValid() const noexcept
{
	return Impl->Archive;
}

void UCommon::FArchiveWrapper::Serialize(void* Pointer, uint64_t Length)
{
	UBPA_UCOMMON_ASSERT(IsValid());
	if (GetState() == EState::Loading)
	{
		Impl->Archive->Serialize(Pointer, Length);
	}
	else
	{
		const size_t OriginalSize = Impl->WriteStorage.size();
		Impl->WriteStorage.resize(OriginalSize + Length);
		std::memcpy(Impl->WriteStorage.data() + OriginalSize, Pointer, Length);
	}
}

//////////////////
// FFileArchive //
//////////////////

struct UCommon::FFileArchive::FImpl
{
	struct FHeader
	{
		uint8_t Magic[4] = { 'U', 'b', 'p', 'a' };
		uint32_t NumVersionKeys = 0;
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
	if (!IsValid())
	{
		return;
	}

	FImpl::FHeader Header;
	ByteSerialize(Header);
	UBPA_UCOMMON_ASSERT(std::memcmp(Header.Magic, "Ubpa", 4) == 0);
	if (GetState() == EState::Loading
		&& Header.NumVersionKeys > 0)
	{
		Seek(Header.VersionMapOffset);
		for (uint32_t Index = 0; Index < Header.NumVersionKeys; Index++)
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

UCommon::FFileArchive::~FFileArchive()
{
	if (Impl)
	{
		if (GetState() == EState::Saving)
		{
			FImpl::FHeader Header;
			Header.VersionMapOffset = Tell();
			Header.NumVersionKeys = (uint32_t)GetVersionKeys().Num();
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
