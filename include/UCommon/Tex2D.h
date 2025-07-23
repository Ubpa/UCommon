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

#define UBPA_UCOMMON_TEX2D_TO_NAMESPACE(NameSpace) \
namespace NameSpace \
{ \
    using FGrid2DIterator = UCommon::FGrid2DIterator; \
    using FGrid2D = UCommon::FGrid2D; \
    using FTex2D = UCommon::FTex2D; \
}

namespace UCommon
{
	struct FGrid2DIterator;

	struct UBPA_UCOMMON_API FGrid2D
	{
		/** Width/Height <= MaxSize (max 48-bits number) */
		static constexpr uint64_t MaxSize = 0xFFFFFFFFFFFF;

		uint64_t Width;
		uint64_t Height;

		FGrid2D(uint64_t InWidth, uint64_t InHeight) noexcept;
		FGrid2D(const FUint64Vector2& Size) noexcept;
		FGrid2D() noexcept;

		/** Width x Height x Depth */
		uint64_t GetArea() const noexcept;

		bool IsAreaEmpty() const noexcept;

		bool Contains(const FUint64Vector2& Point) const noexcept;

		uint64_t GetIndex(const FUint64Vector2& Point) const noexcept;

		FVector2f GetTexcoord(const FUint64Vector2& Point) const noexcept;

		FUint64Vector2 GetPoint(const FVector2f& Texcoord) const noexcept;
		FUint64Vector2 GetPoint(uint64_t Index) const noexcept;

		FUint64Vector2 GetExtent() const noexcept;

		FGrid2DIterator GetIterator(const FUint64Vector2& Point) const noexcept;
		FGrid2DIterator GetIterator(uint64_t Index) const noexcept;

		FGrid2DIterator begin() const noexcept;

		FGrid2DIterator end() const noexcept;

		UBPA_UCOMMON_API friend bool operator==(const FGrid2D& Lhs, const FGrid2D& Rhs) noexcept;
		UBPA_UCOMMON_API friend bool operator!=(const FGrid2D& Lhs, const FGrid2D& Rhs) noexcept;
	};

	struct UBPA_UCOMMON_API FGrid2DIterator
	{
		FGrid2D Grid2D;
		FUint64Vector2 Point;

		FGrid2DIterator& operator++() noexcept
		{
			if (++Point.X == Grid2D.Width)
			{
				Point.X = 0;
				++Point.Y;
			}

			return *this;
		}

		const FUint64Vector2& operator*() const noexcept
		{
			return Point;
		}

		friend bool operator!=(const FGrid2DIterator& Lhs, const FGrid2DIterator& Rhs) noexcept
		{
			UBPA_UCOMMON_ASSERT(Lhs.Grid2D == Rhs.Grid2D);
			return Lhs.Point.X != Rhs.Point.X || Lhs.Point.Y != Rhs.Point.Y;
		}
	};

	class UBPA_UCOMMON_API FTex2D
	{
	public:
		/** Required number of floating point numbers for the storage. */
		static uint64_t GetRequiredStorageSizeInBytes(FGrid2D Grid2D, uint64_t NumChannels, EElementType ElementType) noexcept;
		static uint64_t GetNumElements(FGrid2D Grid2D, uint64_t NumChannels) noexcept;

		FTex2D() noexcept;

		/**
		 * An advanced contructor (no initalization).
		 *
		 * @param InGrid2D the Grid2D of the texture.
		 * @param InNumChannels the channel number of the texture.
		 * @param InOwnership control the ownership of InStorage.
		 * @param InElementType the element type of the storage.
		 * @param InStorage the storage of the texture, deleted by `free`.
		 */
		FTex2D(FGrid2D InGrid2D, uint64_t InNumChannels, EOwnership InOwnership, EElementType InElementType, void* InStorage) noexcept;

		FTex2D(FGrid2D InGrid2D, uint64_t InNumChannels, EElementType InElementType, const void* InStorage);

		template<typename Element>
		FTex2D(FGrid2D InGrid2D, uint64_t InNumChannels, EOwnership InOwnership, Element* InStorage) noexcept;

		template<typename Element>
		FTex2D(FGrid2D InGrid2D, uint64_t InNumChannels, const Element* InStorage);

		/**
		 * Allocate a storage internally by `malloc` (no initalization).
		 *
		 * @param InGrid2D the Grid2D of the texture.
		 * @param InNumChannels the channel number of the texture.
		 * @param InElementType the element type of the storage.
		 */
		FTex2D(FGrid2D InGrid2D, uint64_t InNumChannels, EElementType InElementType);

		/**
		 * Copy with the explicitly specified ownership and InEmptyStorage (may be nullptr).
		 *
		 * @param InEmptyStorage is always used when it is not `nullptr`.
		 *        When it is `nullptr` it causes the internal use of `malloc` (`TakeOwnership`)
		 *        or `Other.Storage` (`DoNotTakeOwnership`).
		 * @param InOwnership control the ownership of `InEmptyStorage`.
		 */
		FTex2D(const FTex2D& Other, EOwnership InOwnership, void* InEmptyStorage);

		/**
		 * Copy with propogated ownership.
		 * When the Ownership of Other is TakeOwnership, it causes the internal use of `malloc`.
		 */
		FTex2D(const FTex2D& Other);

		FTex2D(FTex2D&& Other) noexcept;

		~FTex2D();

		bool IsValid() const noexcept;

		FGrid2D GetGrid2D() const noexcept;

		uint64_t GetNumChannels() const noexcept;

		uint64_t GetNumElements() const noexcept;

		EOwnership GetStorageOwnership() const noexcept;

		EElementType GetElementType() const noexcept;

		/** Number of bytes in the storage. */
		uint64_t GetStorageSizeInBytes() const noexcept;

		void* GetStorage() noexcept;
		const void* GetStorage() const noexcept;

		uint64_t GetIndex(const FUint64Vector2& Point, uint64_t C) const noexcept;

		template<typename T>
		T& At(uint64_t Index) noexcept;
		template<typename T>
		const T& At(uint64_t Index) const noexcept;

		template<typename T>
		T& At(const FUint64Vector2& Point, uint64_t C) noexcept;
		template<typename T>
		const T& At(const FUint64Vector2& Point, uint64_t C) const noexcept;

		template<typename T>
		T& At(const FUint64Vector2& Point) noexcept;
		template<typename T>
		const T& At(const FUint64Vector2& Point) const noexcept;

		float GetFloat(uint64_t Index) const noexcept;
		float GetFloat(const FUint64Vector2& Point, uint64_t C) const noexcept;

		FLinearColorRGB GetLinearColorRGB(const FUint64Vector2& Point) const noexcept;
		FLinearColor GetLinearColor(const FUint64Vector2& Point) const noexcept;

		FDoubleColorRGB GetDoubleColorRGB(const FUint64Vector2& Point) const noexcept;
		FDoubleColor GetDoubleColor(const FUint64Vector2& Point) const noexcept;

		void SetFloat(uint64_t Index, float Value) noexcept;
		void SetFloat(const FUint64Vector2& Point, uint64_t C, float Value) noexcept;

		/** Release `Storage` and reset all member variables. */
		void Reset() noexcept;

		/** Whether `Grid2D`, `NumChannels` and `ElementType` are the same as `Other`'s. */
		bool IsLayoutSameWith(const FTex2D& Other) const noexcept;

		void BilinearSample(float* Result, const FVector2f& Texcoord) const noexcept;

		/**
		 * Width = Width/2
		 * Height = Height/2
		 *
		 * @param InOwnership control the ownership of InStorage.
		 * @param InStorage the storage of the texture, deleted by `free`.
		 */
		FTex2D DownSample(EOwnership InOwnership, void* InStorage);

		/**
		 * Width = Width/2
		 * Height = Height/2
		 */
		FTex2D DownSample();

		FTex2D ToFloat() const;

		void ToUint8(FTex2D& Tex) const;

		FTex2D ToUint8() const;

		/**
		 * If layout is same with Rhs's, just copy the storage,
		 * else copy with propogated ownership.
		 * When the Ownership of Other is TakeOwnership, it causes the internal use of `malloc`.
		 */
		FTex2D& operator=(const FTex2D& Rhs);

		FTex2D& operator=(FTex2D&& Rhs) noexcept;

		static void Copy(FTex2D& Dst, const FUint64Vector2& DstPoint, const FTex2D& Src, const FUint64Vector2& SrcPoint, const FUint64Vector2& Range);

	private:
		FGrid2D Grid2D;
		uint64_t NumChannels;
		EOwnership Ownership;
		EElementType ElementType;
		void* Storage;
	};
} // UCommon

UBPA_UCOMMON_TEX2D_TO_NAMESPACE(UCommonTest)

#include "Tex2D.inl"
