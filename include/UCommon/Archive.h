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
#include "Utils.h"

#define UBPA_UCOMMON_ARCHIVE_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
	using IArchive = UCommon::IArchive; \
	using FMemoryArchive = UCommon::FMemoryArchive; \
	using FFileArchive = UCommon::FFileArchive; \
}

namespace UCommon
{
	class UBPA_UCOMMON_API IArchive
	{
		struct FImpl;
		FImpl* Impl;
	public:
		enum class EState : uint32_t
		{
			Loading = 0,
			Saving = 1,
		};

		IArchive(EState State);
		IArchive(IArchive&& Other) noexcept;
		IArchive& operator=(IArchive&& Other) noexcept;
		void Swap(IArchive& Other) noexcept;

		virtual ~IArchive();

		void UseVersion(uint64_t Key, int64_t Version);
		int64_t GetVersion(uint64_t Key) const;
		TSpan<const uint64_t> GetVersionKeys() const;

		EState GetState() const;

		template<typename T>
		void ByteSerialize(T& Value)
		{
			Serialize(&Value, sizeof(T));
		}

		// size, resize, begin, end
		template<typename T>
		void SequentialContainerObjectSerialize(T& Container)
		{
			uint64_t NumIndices = Container.size();
			ByteSerialize(NumIndices);
			if (GetState() == IArchive::EState::Loading)
			{
				Container.resize(NumIndices);
			}
			for (auto& Element : Container)
			{
				Element.Serialize(*this);
			}
		}

		// size, resize, begin, end
		template<typename T>
		void SequentialContainerByteSerialize(T& Container)
		{
			uint64_t NumIndices = Container.size();
			ByteSerialize(NumIndices);
			if (GetState() == IArchive::EState::Loading)
			{
				Container.resize(NumIndices);
			}
			if (NumIndices > 0)
			{
				Serialize(Container.data(), NumIndices * sizeof(typename T::value_type));
			}
		}

		virtual void Serialize(void* Pointer, uint64_t Length) {}

	protected:
		virtual void Seek(uint64_t Index) { UBPA_UCOMMON_NO_ENTRY(); }
		virtual uint64_t Tell() const { UBPA_UCOMMON_NO_ENTRY(); return 0; }
		void LoadVersion(uint64_t Key, int64_t Version);

		void OnInit();
		void OnDestroy();
	};

	class UBPA_UCOMMON_API FMemoryArchive : public IArchive
	{
		struct FImpl;
		FImpl* Impl;
	public:
		FMemoryArchive(TSpan<const uint8_t> Storage = {});
		FMemoryArchive(FMemoryArchive&& Other) noexcept;
		FMemoryArchive& operator=(FMemoryArchive&& Other) noexcept;
		void Swap(FMemoryArchive& Other) noexcept;
		virtual ~FMemoryArchive();

		virtual void Serialize(void* Pointer, uint64_t Length) override;

		TSpan<const uint8_t> GetStorage() const;

	protected:
		virtual void Seek(uint64_t Index) override;
		virtual uint64_t Tell() const override;
	};

	class UBPA_UCOMMON_API FFileArchive : public IArchive
	{
		struct FImpl;
		FImpl* Impl;
	public:
		FFileArchive(EState State, const char* FilePath);
		FFileArchive(FFileArchive&& Other) noexcept;
		FFileArchive& operator=(FFileArchive&& Other) noexcept;
		void Swap(FFileArchive& Other) noexcept;
		virtual ~FFileArchive();

		bool IsValid() const;

		virtual void Serialize(void* Pointer, uint64_t Length) override;

	protected:
		virtual void Seek(uint64_t Index) override;
		virtual uint64_t Tell() const override;
	};
}

UBPA_UCOMMON_ARCHIVE_TO_NAMESPACE(UCommonTest)
