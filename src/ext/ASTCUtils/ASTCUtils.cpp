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

#include <UCommon_ext/ASTCUtils.h>
#include <UCommon/Utils.h>
#include <UCommon/ThreadPool.h>
#include <UCommon/Tex2D.h>

#undef PI
#include "astcenc.h"
#include "astcenc_internal.h"
#include "astcenc_internal_entry.h"

#include <map>
#include <array>

namespace UCommon
{
	template<typename T>
	class TFree
	{
	public:
		void operator()(T* ptr) const
		{
			ptr->~T();
			UBPA_UCOMMON_FREE(ptr);
		}
	};
	using FUniqueBlockSizeDescriptor = std::unique_ptr<block_size_descriptor, TFree<block_size_descriptor>>;

	class BlockSizeDescriptorMngr
	{
	public:
		static BlockSizeDescriptorMngr& GetInstance()
		{
			static BlockSizeDescriptorMngr Instance;
			return Instance;
		}

		const block_size_descriptor& GetBlockSizeDescriptor(const FUint64Vector2& BlockSize)
		{
			const uint64_t Key = BlockSize.Y << 32 | BlockSize.X;
			auto target = BlockSizeDescriptorMap.find(Key);
			if (target == BlockSizeDescriptorMap.end())
			{
				FUniqueBlockSizeDescriptor BlockSizeDescriptor(new(UBPA_UCOMMON_MALLOC(sizeof(block_size_descriptor)))block_size_descriptor);
				init_block_size_descriptor(static_cast<unsigned int>(BlockSize.X), static_cast<unsigned int>(BlockSize.Y), 1u,
					/*can_omit_modes*/ true,
					/*partition_count_cutoff*/ 0xFFFFFFFF,
					/*mode_cutoff*/ 1.f,
					*BlockSizeDescriptor);
				target = BlockSizeDescriptorMap.insert(std::make_pair(Key, std::move(BlockSizeDescriptor))).first;
			}
			return *target->second;
		}

		void Clear()
		{
			BlockSizeDescriptorMap.clear();
		}

		size_t GetAllocatedBytes() const
		{
			size_t AllocatedBytes = 0;
			for (const auto& Pair : BlockSizeDescriptorMap)
			{
				AllocatedBytes += sizeof(block_size_descriptor);
				AllocatedBytes += Pair.second->decimation_tables.capacity() * sizeof(decimation_info);
				for (const auto& decimation : Pair.second->decimation_tables)
				{
					AllocatedBytes += decimation.vtexel_datas.capacity() * sizeof(decimation_info::vtexel_data);
				}
				for (const auto& partitionings : Pair.second->partitionings_234)
				{
					AllocatedBytes += partitionings.capacity() * sizeof(partition_info);
				}
			}
			return AllocatedBytes;
		}

	private:
		BlockSizeDescriptorMngr() = default;
		~BlockSizeDescriptorMngr()
		{
			Clear();
		}

		std::map<uint64_t, FUniqueBlockSizeDescriptor> BlockSizeDescriptorMap;
	};
}

void UCommon::InitBlockSizeDescriptorMngr(TSpan<const uint64_t> Sizes)
{
	BlockSizeDescriptorMngr& Mngr = BlockSizeDescriptorMngr::GetInstance();
	for (uint64_t Size : Sizes)
	{
		Mngr.GetBlockSizeDescriptor(FUint64Vector2(Size, Size));
	}
}

void UCommon::ReleaseBlockSizeDescriptorMngr()
{
	BlockSizeDescriptorMngr& Mngr = BlockSizeDescriptorMngr::GetInstance();
	Mngr.Clear();
}

uint64_t UCommon::GetBlockSizeDescriptorMngrAllocatedBytes()
{
	BlockSizeDescriptorMngr& Mngr = BlockSizeDescriptorMngr::GetInstance();
	return Mngr.GetAllocatedBytes();
}

static float error_color_nan()
{
	if32 v;
	v.u = 0xFFFFE000U;
	return v.f;
}

/**
 * @brief Compute the integer linear interpolation of two color endpoints.
 *
 * @param decode_mode   The ASTC profile (linear or sRGB)
 * @param color0        The endpoint0 color.
 * @param color1        The endpoint1 color.
 * @param weights        The interpolation weight (between 0 and 64).
 *
 * @return The interpolated color.
 */
inline static vint4 lerp_color_int(
	vmask4 u8_mask,
	vint4 color0_64_add_32,
	vint4 color1_minus_color0,
	vint4 weights
) {
	//vint4 weight0 = vint4(64) - weights;
	//vint4 color = (color0 * weight0) + (color1 * weights) + vint4(32);
	vint4 color = color1_minus_color0 * weights + color0_64_add_32;
	color = asr<6>(color);

	// For decode_unorm8 values force the codec to bit replicate. This allows the
	// rest of the codec to assume the full 0xFFFF range for everything and ignore
	// the decode_mode setting
	vint4 color_u8 = asr<8>(color) * vint4(257);
	color = select(color, color_u8, u8_mask);

	return color;
}

static void unpack_weights(
	const block_size_descriptor& bsd,
	const symbolic_compressed_block& scb,
	const decimation_info& di,
	bool is_dual_plane,
	int weights_plane1[BLOCK_MAX_TEXELS],
	int weights_plane2[BLOCK_MAX_TEXELS],
	const UCommon::FUint64Vector2& BlockSize,
	UCommon::TSpan<const uint64_t> PointIndices)
{
	// Safe to overshoot as all arrays are allocated to full size
	if (!is_dual_plane)
	{
		for (uint64_t i : PointIndices)
		{
			uint64_t vt_idx = i / ASTCENC_SIMD_WIDTH;
			uint64_t vt_iidx = i % ASTCENC_SIMD_WIDTH;

			int summed_value = 8;

			uint8_t weight_count = di.vtexel_datas[vt_idx].texel_weight_count[vt_iidx];

			promise(weight_count > 0);
			for (uint8_t j = 0; j < weight_count; j++)
			{
				uint8_t texel_weight = di.vtexel_datas[vt_idx].texel_weights_tr[j][vt_iidx];
				uint8_t texel_weight_int = di.vtexel_datas[vt_idx].texel_weight_contribs_int_tr[j][vt_iidx];

				summed_value += scb.weights[texel_weight] * texel_weight_int;
			}

			weights_plane1[i] = summed_value >> 4;
		}
	}
	else
	{
		for (uint64_t i : PointIndices)
		{
			uint64_t vt_idx = i / ASTCENC_SIMD_WIDTH;
			uint64_t vt_iidx = i % ASTCENC_SIMD_WIDTH;

			int sum_plane1 = 8;
			int sum_plane2 = 8;

			uint8_t weight_count = di.vtexel_datas[vt_idx].texel_weight_count[vt_iidx];

			promise(weight_count > 0);
			for (uint8_t j = 0; j < weight_count; j++)
			{
				uint8_t texel_weight = di.vtexel_datas[vt_idx].texel_weights_tr[j][vt_iidx];
				uint8_t texel_weight_int = di.vtexel_datas[vt_idx].texel_weight_contribs_int_tr[j][vt_iidx];

				sum_plane1 += scb.weights[ 0 + texel_weight] * texel_weight_int;
				sum_plane2 += scb.weights[32 + texel_weight] * texel_weight_int;
			}

			weights_plane1[i] = sum_plane1 >> 4;
			weights_plane2[i] = sum_plane2 >> 4;
		}
	}
}

static inline int read_bits(
	int bitcount,
	int bitoffset,
	const uint8_t* ptr
) {
	int mask = (1 << bitcount) - 1;
	ptr += bitoffset >> 3;
	bitoffset &= 7;
	int value = ptr[0] | (ptr[1] << 8);
	value >>= bitoffset;
	value &= mask;
	return value;
}

static inline int bitrev8(int p)
{
	p = ((p & 0x0F) << 4) | ((p >> 4) & 0x0F);
	p = ((p & 0x33) << 2) | ((p >> 2) & 0x33);
	p = ((p & 0x55) << 1) | ((p >> 1) & 0x55);
	return p;
}

/**
 * @brief The number of bits, trits, and quints needed for a quant level.
 */
struct btq_count
{
	/** @brief The number of bits. */
	uint8_t bits : 6;

	/** @brief The number of trits. */
	uint8_t trits : 1;

	/** @brief The number of quints. */
	uint8_t quints : 1;
};

/**
 * @brief The table of bits, trits, and quints needed for a quant encode.
 */
static const std::array<btq_count, 21> btq_counts{ {
	{ 1, 0, 0 }, // QUANT_2
	{ 0, 1, 0 }, // QUANT_3
	{ 2, 0, 0 }, // QUANT_4
	{ 0, 0, 1 }, // QUANT_5
	{ 1, 1, 0 }, // QUANT_6
	{ 3, 0, 0 }, // QUANT_8
	{ 1, 0, 1 }, // QUANT_10
	{ 2, 1, 0 }, // QUANT_12
	{ 4, 0, 0 }, // QUANT_16
	{ 2, 0, 1 }, // QUANT_20
	{ 3, 1, 0 }, // QUANT_24
	{ 5, 0, 0 }, // QUANT_32
	{ 3, 0, 1 }, // QUANT_40
	{ 4, 1, 0 }, // QUANT_48
	{ 6, 0, 0 }, // QUANT_64
	{ 4, 0, 1 }, // QUANT_80
	{ 5, 1, 0 }, // QUANT_96
	{ 7, 0, 0 }, // QUANT_128
	{ 5, 0, 1 }, // QUANT_160
	{ 6, 1, 0 }, // QUANT_192
	{ 8, 0, 0 }  // QUANT_256
} };

static const uint8_t trits_of_integer[256][5]{
	{0, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {2, 0, 0, 0, 0}, {0, 0, 2, 0, 0},
	{0, 1, 0, 0, 0}, {1, 1, 0, 0, 0}, {2, 1, 0, 0, 0}, {1, 0, 2, 0, 0},
	{0, 2, 0, 0, 0}, {1, 2, 0, 0, 0}, {2, 2, 0, 0, 0}, {2, 0, 2, 0, 0},
	{0, 2, 2, 0, 0}, {1, 2, 2, 0, 0}, {2, 2, 2, 0, 0}, {2, 0, 2, 0, 0},
	{0, 0, 1, 0, 0}, {1, 0, 1, 0, 0}, {2, 0, 1, 0, 0}, {0, 1, 2, 0, 0},
	{0, 1, 1, 0, 0}, {1, 1, 1, 0, 0}, {2, 1, 1, 0, 0}, {1, 1, 2, 0, 0},
	{0, 2, 1, 0, 0}, {1, 2, 1, 0, 0}, {2, 2, 1, 0, 0}, {2, 1, 2, 0, 0},
	{0, 0, 0, 2, 2}, {1, 0, 0, 2, 2}, {2, 0, 0, 2, 2}, {0, 0, 2, 2, 2},
	{0, 0, 0, 1, 0}, {1, 0, 0, 1, 0}, {2, 0, 0, 1, 0}, {0, 0, 2, 1, 0},
	{0, 1, 0, 1, 0}, {1, 1, 0, 1, 0}, {2, 1, 0, 1, 0}, {1, 0, 2, 1, 0},
	{0, 2, 0, 1, 0}, {1, 2, 0, 1, 0}, {2, 2, 0, 1, 0}, {2, 0, 2, 1, 0},
	{0, 2, 2, 1, 0}, {1, 2, 2, 1, 0}, {2, 2, 2, 1, 0}, {2, 0, 2, 1, 0},
	{0, 0, 1, 1, 0}, {1, 0, 1, 1, 0}, {2, 0, 1, 1, 0}, {0, 1, 2, 1, 0},
	{0, 1, 1, 1, 0}, {1, 1, 1, 1, 0}, {2, 1, 1, 1, 0}, {1, 1, 2, 1, 0},
	{0, 2, 1, 1, 0}, {1, 2, 1, 1, 0}, {2, 2, 1, 1, 0}, {2, 1, 2, 1, 0},
	{0, 1, 0, 2, 2}, {1, 1, 0, 2, 2}, {2, 1, 0, 2, 2}, {1, 0, 2, 2, 2},
	{0, 0, 0, 2, 0}, {1, 0, 0, 2, 0}, {2, 0, 0, 2, 0}, {0, 0, 2, 2, 0},
	{0, 1, 0, 2, 0}, {1, 1, 0, 2, 0}, {2, 1, 0, 2, 0}, {1, 0, 2, 2, 0},
	{0, 2, 0, 2, 0}, {1, 2, 0, 2, 0}, {2, 2, 0, 2, 0}, {2, 0, 2, 2, 0},
	{0, 2, 2, 2, 0}, {1, 2, 2, 2, 0}, {2, 2, 2, 2, 0}, {2, 0, 2, 2, 0},
	{0, 0, 1, 2, 0}, {1, 0, 1, 2, 0}, {2, 0, 1, 2, 0}, {0, 1, 2, 2, 0},
	{0, 1, 1, 2, 0}, {1, 1, 1, 2, 0}, {2, 1, 1, 2, 0}, {1, 1, 2, 2, 0},
	{0, 2, 1, 2, 0}, {1, 2, 1, 2, 0}, {2, 2, 1, 2, 0}, {2, 1, 2, 2, 0},
	{0, 2, 0, 2, 2}, {1, 2, 0, 2, 2}, {2, 2, 0, 2, 2}, {2, 0, 2, 2, 2},
	{0, 0, 0, 0, 2}, {1, 0, 0, 0, 2}, {2, 0, 0, 0, 2}, {0, 0, 2, 0, 2},
	{0, 1, 0, 0, 2}, {1, 1, 0, 0, 2}, {2, 1, 0, 0, 2}, {1, 0, 2, 0, 2},
	{0, 2, 0, 0, 2}, {1, 2, 0, 0, 2}, {2, 2, 0, 0, 2}, {2, 0, 2, 0, 2},
	{0, 2, 2, 0, 2}, {1, 2, 2, 0, 2}, {2, 2, 2, 0, 2}, {2, 0, 2, 0, 2},
	{0, 0, 1, 0, 2}, {1, 0, 1, 0, 2}, {2, 0, 1, 0, 2}, {0, 1, 2, 0, 2},
	{0, 1, 1, 0, 2}, {1, 1, 1, 0, 2}, {2, 1, 1, 0, 2}, {1, 1, 2, 0, 2},
	{0, 2, 1, 0, 2}, {1, 2, 1, 0, 2}, {2, 2, 1, 0, 2}, {2, 1, 2, 0, 2},
	{0, 2, 2, 2, 2}, {1, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 0, 2, 2, 2},
	{0, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {2, 0, 0, 0, 1}, {0, 0, 2, 0, 1},
	{0, 1, 0, 0, 1}, {1, 1, 0, 0, 1}, {2, 1, 0, 0, 1}, {1, 0, 2, 0, 1},
	{0, 2, 0, 0, 1}, {1, 2, 0, 0, 1}, {2, 2, 0, 0, 1}, {2, 0, 2, 0, 1},
	{0, 2, 2, 0, 1}, {1, 2, 2, 0, 1}, {2, 2, 2, 0, 1}, {2, 0, 2, 0, 1},
	{0, 0, 1, 0, 1}, {1, 0, 1, 0, 1}, {2, 0, 1, 0, 1}, {0, 1, 2, 0, 1},
	{0, 1, 1, 0, 1}, {1, 1, 1, 0, 1}, {2, 1, 1, 0, 1}, {1, 1, 2, 0, 1},
	{0, 2, 1, 0, 1}, {1, 2, 1, 0, 1}, {2, 2, 1, 0, 1}, {2, 1, 2, 0, 1},
	{0, 0, 1, 2, 2}, {1, 0, 1, 2, 2}, {2, 0, 1, 2, 2}, {0, 1, 2, 2, 2},
	{0, 0, 0, 1, 1}, {1, 0, 0, 1, 1}, {2, 0, 0, 1, 1}, {0, 0, 2, 1, 1},
	{0, 1, 0, 1, 1}, {1, 1, 0, 1, 1}, {2, 1, 0, 1, 1}, {1, 0, 2, 1, 1},
	{0, 2, 0, 1, 1}, {1, 2, 0, 1, 1}, {2, 2, 0, 1, 1}, {2, 0, 2, 1, 1},
	{0, 2, 2, 1, 1}, {1, 2, 2, 1, 1}, {2, 2, 2, 1, 1}, {2, 0, 2, 1, 1},
	{0, 0, 1, 1, 1}, {1, 0, 1, 1, 1}, {2, 0, 1, 1, 1}, {0, 1, 2, 1, 1},
	{0, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {2, 1, 1, 1, 1}, {1, 1, 2, 1, 1},
	{0, 2, 1, 1, 1}, {1, 2, 1, 1, 1}, {2, 2, 1, 1, 1}, {2, 1, 2, 1, 1},
	{0, 1, 1, 2, 2}, {1, 1, 1, 2, 2}, {2, 1, 1, 2, 2}, {1, 1, 2, 2, 2},
	{0, 0, 0, 2, 1}, {1, 0, 0, 2, 1}, {2, 0, 0, 2, 1}, {0, 0, 2, 2, 1},
	{0, 1, 0, 2, 1}, {1, 1, 0, 2, 1}, {2, 1, 0, 2, 1}, {1, 0, 2, 2, 1},
	{0, 2, 0, 2, 1}, {1, 2, 0, 2, 1}, {2, 2, 0, 2, 1}, {2, 0, 2, 2, 1},
	{0, 2, 2, 2, 1}, {1, 2, 2, 2, 1}, {2, 2, 2, 2, 1}, {2, 0, 2, 2, 1},
	{0, 0, 1, 2, 1}, {1, 0, 1, 2, 1}, {2, 0, 1, 2, 1}, {0, 1, 2, 2, 1},
	{0, 1, 1, 2, 1}, {1, 1, 1, 2, 1}, {2, 1, 1, 2, 1}, {1, 1, 2, 2, 1},
	{0, 2, 1, 2, 1}, {1, 2, 1, 2, 1}, {2, 2, 1, 2, 1}, {2, 1, 2, 2, 1},
	{0, 2, 1, 2, 2}, {1, 2, 1, 2, 2}, {2, 2, 1, 2, 2}, {2, 1, 2, 2, 2},
	{0, 0, 0, 1, 2}, {1, 0, 0, 1, 2}, {2, 0, 0, 1, 2}, {0, 0, 2, 1, 2},
	{0, 1, 0, 1, 2}, {1, 1, 0, 1, 2}, {2, 1, 0, 1, 2}, {1, 0, 2, 1, 2},
	{0, 2, 0, 1, 2}, {1, 2, 0, 1, 2}, {2, 2, 0, 1, 2}, {2, 0, 2, 1, 2},
	{0, 2, 2, 1, 2}, {1, 2, 2, 1, 2}, {2, 2, 2, 1, 2}, {2, 0, 2, 1, 2},
	{0, 0, 1, 1, 2}, {1, 0, 1, 1, 2}, {2, 0, 1, 1, 2}, {0, 1, 2, 1, 2},
	{0, 1, 1, 1, 2}, {1, 1, 1, 1, 2}, {2, 1, 1, 1, 2}, {1, 1, 2, 1, 2},
	{0, 2, 1, 1, 2}, {1, 2, 1, 1, 2}, {2, 2, 1, 1, 2}, {2, 1, 2, 1, 2},
	{0, 2, 2, 2, 2}, {1, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 1, 2, 2, 2}
};

static const uint8_t quints_of_integer[128][3]{
	{0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0},
	{4, 0, 0}, {0, 4, 0}, {4, 4, 0}, {4, 4, 4},
	{0, 1, 0}, {1, 1, 0}, {2, 1, 0}, {3, 1, 0},
	{4, 1, 0}, {1, 4, 0}, {4, 4, 1}, {4, 4, 4},
	{0, 2, 0}, {1, 2, 0}, {2, 2, 0}, {3, 2, 0},
	{4, 2, 0}, {2, 4, 0}, {4, 4, 2}, {4, 4, 4},
	{0, 3, 0}, {1, 3, 0}, {2, 3, 0}, {3, 3, 0},
	{4, 3, 0}, {3, 4, 0}, {4, 4, 3}, {4, 4, 4},
	{0, 0, 1}, {1, 0, 1}, {2, 0, 1}, {3, 0, 1},
	{4, 0, 1}, {0, 4, 1}, {4, 0, 4}, {0, 4, 4},
	{0, 1, 1}, {1, 1, 1}, {2, 1, 1}, {3, 1, 1},
	{4, 1, 1}, {1, 4, 1}, {4, 1, 4}, {1, 4, 4},
	{0, 2, 1}, {1, 2, 1}, {2, 2, 1}, {3, 2, 1},
	{4, 2, 1}, {2, 4, 1}, {4, 2, 4}, {2, 4, 4},
	{0, 3, 1}, {1, 3, 1}, {2, 3, 1}, {3, 3, 1},
	{4, 3, 1}, {3, 4, 1}, {4, 3, 4}, {3, 4, 4},
	{0, 0, 2}, {1, 0, 2}, {2, 0, 2}, {3, 0, 2},
	{4, 0, 2}, {0, 4, 2}, {2, 0, 4}, {3, 0, 4},
	{0, 1, 2}, {1, 1, 2}, {2, 1, 2}, {3, 1, 2},
	{4, 1, 2}, {1, 4, 2}, {2, 1, 4}, {3, 1, 4},
	{0, 2, 2}, {1, 2, 2}, {2, 2, 2}, {3, 2, 2},
	{4, 2, 2}, {2, 4, 2}, {2, 2, 4}, {3, 2, 4},
	{0, 3, 2}, {1, 3, 2}, {2, 3, 2}, {3, 3, 2},
	{4, 3, 2}, {3, 4, 2}, {2, 3, 4}, {3, 3, 4},
	{0, 0, 3}, {1, 0, 3}, {2, 0, 3}, {3, 0, 3},
	{4, 0, 3}, {0, 4, 3}, {0, 0, 4}, {1, 0, 4},
	{0, 1, 3}, {1, 1, 3}, {2, 1, 3}, {3, 1, 3},
	{4, 1, 3}, {1, 4, 3}, {0, 1, 4}, {1, 1, 4},
	{0, 2, 3}, {1, 2, 3}, {2, 2, 3}, {3, 2, 3},
	{4, 2, 3}, {2, 4, 3}, {0, 2, 4}, {1, 2, 4},
	{0, 3, 3}, {1, 3, 3}, {2, 3, 3}, {3, 3, 3},
	{4, 3, 3}, {3, 4, 3}, {0, 3, 4}, {1, 3, 4}
};

void decode_ise(
	quant_method quant_level,
	const uint8_t* input_data,
	uint8_t* output_data,
	int begin_block_index, int end_block_index
) {
	unsigned int bits = btq_counts[quant_level].bits;
	unsigned int trits = btq_counts[quant_level].trits;
	unsigned int quints = btq_counts[quant_level].quints;

	if (trits)
	{
		uint8_t tq_blocks[22];
		unsigned int begin_i = begin_block_index * 5;
		unsigned int end_i = end_block_index * 5;
		int bit_offset = begin_block_index * (5 * bits + 8);
		std::memset(tq_blocks + begin_block_index, 0, end_block_index - begin_block_index);
		unsigned int lcounter = 0;
		unsigned int hcounter = begin_block_index;
		// Collect bits for each element, as well as bits for any trit-blocks and quint-blocks.
		for (unsigned int i = begin_i; i < end_i; i++)
		{
			output_data[i] = static_cast<uint8_t>(read_bits(bits, bit_offset, input_data));
			bit_offset += bits;

			static constexpr uint8_t bits_to_read[5]{ 2, 2, 1, 2, 1 };
			static constexpr uint8_t block_shift[5]{ 0, 2, 4, 5, 7 };
			static constexpr uint8_t next_lcounter[5]{ 1, 2, 3, 4, 0 };
			static constexpr uint8_t hcounter_incr[5]{ 0, 0, 0, 0, 1 };
			unsigned int tdata = read_bits(bits_to_read[lcounter], bit_offset, input_data);
			bit_offset += bits_to_read[lcounter];
			tq_blocks[hcounter] |= tdata << block_shift[lcounter];
			hcounter += hcounter_incr[lcounter];
			lcounter = next_lcounter[lcounter];
		}

		// Unpack trit-blocks or quint-blocks as needed
		for (int i = begin_block_index; i < end_block_index; i++)
		{
			const uint8_t* tritptr = trits_of_integer[tq_blocks[i]];
			output_data[5 * i] |= tritptr[0] << bits;
			output_data[5 * i + 1] |= tritptr[1] << bits;
			output_data[5 * i + 2] |= tritptr[2] << bits;
			output_data[5 * i + 3] |= tritptr[3] << bits;
			output_data[5 * i + 4] |= tritptr[4] << bits;
		}
	}
	else if (quints)
	{
		uint8_t tq_blocks[22];
		unsigned int begin_i = begin_block_index * 3;
		unsigned int end_i = end_block_index * 3;
		int bit_offset = begin_block_index * (3 * bits + 7);
		std::memset(tq_blocks + begin_block_index, 0, end_block_index - begin_block_index);
		unsigned int lcounter = 0;
		unsigned int hcounter = begin_block_index;
		// Collect bits for each element, as well as bits for any trit-blocks and quint-blocks.
		for (unsigned int i = begin_i; i < end_i; i++)
		{
			output_data[i] = static_cast<uint8_t>(read_bits(bits, bit_offset, input_data));
			bit_offset += bits;

			static constexpr uint8_t bits_to_read[3]{ 3, 2, 2 };
			static constexpr uint8_t block_shift[3]{ 0, 3, 5 };
			static constexpr uint8_t next_lcounter[3]{ 1, 2, 0 };
			static constexpr uint8_t hcounter_incr[3]{ 0, 0, 1 };
			unsigned int tdata = read_bits(bits_to_read[lcounter], bit_offset, input_data);
			bit_offset += bits_to_read[lcounter];
			tq_blocks[hcounter] |= tdata << block_shift[lcounter];
			hcounter += hcounter_incr[lcounter];
			lcounter = next_lcounter[lcounter];
		}

		// Unpack trit-blocks or quint-blocks as needed
		for (int i = begin_block_index; i < end_block_index; i++)
		{
			const uint8_t* quintptr = quints_of_integer[tq_blocks[i]];
			output_data[3 * i] |= quintptr[0] << bits;
			output_data[3 * i + 1] |= quintptr[1] << bits;
			output_data[3 * i + 2] |= quintptr[2] << bits;
		}
	}
	else
	{
		int bit_offset = begin_block_index * bits;
		// Collect bits for each element, as well as bits for any trit-blocks and quint-blocks.
		for (int i = begin_block_index; i < end_block_index; i++)
		{
			output_data[i] = static_cast<uint8_t>(read_bits(bits, bit_offset, input_data));
			bit_offset += bits;
		}
	}
}

struct ise_size
{
	/** @brief The scaling parameter. */
	uint8_t scale : 6;

	/** @brief The divisor parameter. */
	uint8_t divisor : 2;
};

static const std::array<ise_size, 21> ise_sizes{ {
	{  1, 0 }, // QUANT_2
	{  8, 2 }, // QUANT_3
	{  2, 0 }, // QUANT_4
	{  7, 1 }, // QUANT_5
	{ 13, 2 }, // QUANT_6
	{  3, 0 }, // QUANT_8
	{ 10, 1 }, // QUANT_10
	{ 18, 2 }, // QUANT_12
	{  4, 0 }, // QUANT_16
	{ 13, 1 }, // QUANT_20
	{ 23, 2 }, // QUANT_24
	{  5, 0 }, // QUANT_32
	{ 16, 1 }, // QUANT_40
	{ 28, 2 }, // QUANT_48
	{  6, 0 }, // QUANT_64
	{ 19, 1 }, // QUANT_80
	{ 33, 2 }, // QUANT_96
	{  7, 0 }, // QUANT_128
	{ 22, 1 }, // QUANT_160
	{ 38, 2 }, // QUANT_192
	{  8, 0 }  // QUANT_256
} };

void physical_to_symbolic(
	const block_size_descriptor& bsd,
	const uint8_t pcb[16],
	symbolic_compressed_block& scb,
	const UCommon::FUint64Vector2& BlockSize,
	UCommon::TSpan<const uint64_t> PointIndices
) {
	uint8_t bswapped[16];

	scb.block_type = SYM_BTYPE_NONCONST;

	// Extract header fields
	int block_mode = read_bits(11, 0, pcb);
	if ((block_mode & 0x1FF) == 0x1FC)
	{
		// Constant color block

		// Check what format the data has
		if (block_mode & 0x200)
		{
			scb.block_type = SYM_BTYPE_CONST_F16;
		}
		else
		{
			scb.block_type = SYM_BTYPE_CONST_U16;
		}

		scb.partition_count = 0;
		for (int i = 0; i < 4; i++)
		{
			scb.constant_color[i] = pcb[2 * i + 8] | (pcb[2 * i + 9] << 8);
		}

		// Additionally, check that the void-extent
		if (bsd.zdim == 1)
		{
			// 2D void-extent
			int rsvbits = read_bits(2, 10, pcb);
			if (rsvbits != 3)
			{
				scb.block_type = SYM_BTYPE_ERROR;
				return;
			}

			// Low values span 3 bytes so need two read_bits calls
			int vx_low_s = read_bits(8, 12, pcb) | (read_bits(5, 12 + 8, pcb) << 8);
			int vx_high_s = read_bits(13, 25, pcb);
			int vx_low_t = read_bits(8, 38, pcb) | (read_bits(5, 38 + 8, pcb) << 8);
			int vx_high_t = read_bits(13, 51, pcb);

			int all_ones = vx_low_s == 0x1FFF && vx_high_s == 0x1FFF &&
			               vx_low_t == 0x1FFF && vx_high_t == 0x1FFF;

			if ((vx_low_s >= vx_high_s || vx_low_t >= vx_high_t) && !all_ones)
			{
				scb.block_type = SYM_BTYPE_ERROR;
				return;
			}
		}
		else
		{
			// 3D void-extent
			int vx_low_s = read_bits(9, 10, pcb);
			int vx_high_s = read_bits(9, 19, pcb);
			int vx_low_t = read_bits(9, 28, pcb);
			int vx_high_t = read_bits(9, 37, pcb);
			int vx_low_r = read_bits(9, 46, pcb);
			int vx_high_r = read_bits(9, 55, pcb);

			int all_ones = vx_low_s == 0x1FF && vx_high_s == 0x1FF &&
			               vx_low_t == 0x1FF && vx_high_t == 0x1FF &&
			               vx_low_r == 0x1FF && vx_high_r == 0x1FF;

			if ((vx_low_s >= vx_high_s || vx_low_t >= vx_high_t || vx_low_r >= vx_high_r) && !all_ones)
			{
				scb.block_type = SYM_BTYPE_ERROR;
				return;
			}
		}

		return;
	}

	unsigned int packed_index = bsd.block_mode_packed_index[block_mode];
	if (packed_index == BLOCK_BAD_BLOCK_MODE)
	{
		scb.block_type = SYM_BTYPE_ERROR;
		return;
	}

	const auto& bm = bsd.get_block_mode(block_mode);
	const auto& di = bsd.get_decimation_info(bm.decimation_mode);

	int weight_count = di.weight_count;
	promise(weight_count > 0);

	quant_method weight_quant_method = static_cast<quant_method>(bm.quant_mode);
	int is_dual_plane = bm.is_dual_plane;

	int real_weight_count = is_dual_plane ? 2 * weight_count : weight_count;

	int partition_count = read_bits(2, 11, pcb) + 1;
	promise(partition_count > 0);

	scb.block_mode = static_cast<uint16_t>(block_mode);
	scb.partition_count = static_cast<uint8_t>(partition_count);

	int begin_weight_index = BLOCK_MAX_WEIGHTS;
	int end_weight_index = 0;
	if (!is_dual_plane)
	{
		for (uint64_t i : PointIndices)
		{
			uint64_t vt_idx = i / ASTCENC_SIMD_WIDTH;
			uint64_t vt_iidx = i % ASTCENC_SIMD_WIDTH;

			uint8_t weight_count = di.vtexel_datas[vt_idx].texel_weight_count[vt_iidx];

			promise(weight_count > 0);
			for (uint8_t j = 0; j < weight_count; j++)
			{
				int texel_weight = (int)di.vtexel_datas[vt_idx].texel_weights_tr[j][vt_iidx];
				begin_weight_index = std::min(begin_weight_index, texel_weight);
				end_weight_index = std::max(end_weight_index, texel_weight);
			}
		}
		end_weight_index += 1;
	}
	else
	{
		for (uint64_t i : PointIndices)
		{
			uint64_t vt_idx = i / ASTCENC_SIMD_WIDTH;
			uint64_t vt_iidx = i % ASTCENC_SIMD_WIDTH;

			uint8_t weight_count = di.vtexel_datas[vt_idx].texel_weight_count[vt_iidx];

			promise(weight_count > 0);
			for (uint8_t j = 0; j < weight_count; j++)
			{
				int texel_weight = di.vtexel_datas[vt_idx].texel_weights_tr[j][vt_iidx];
				begin_weight_index = std::min(begin_weight_index, texel_weight);
				end_weight_index = std::max(end_weight_index, texel_weight);
			}
		}
		begin_weight_index = 2 * begin_weight_index;
		end_weight_index = 1 + 2 * end_weight_index + 1;
	}
	unsigned int bits = btq_counts[weight_quant_method].bits;
	unsigned int trits = btq_counts[weight_quant_method].trits;
	unsigned int quints = btq_counts[weight_quant_method].quints;

	int begin_block_index, end_block_index, begin_i, end_i;
	if (trits)
	{
		int blocksize = 5 * bits + 8;
		begin_block_index = begin_weight_index / 5;
		end_block_index = (end_weight_index + 4) / 5;
		begin_i = begin_block_index * blocksize / 8;
		end_i = 1 + end_block_index * blocksize / 8;
	}
	else if (quints)
	{
		int blocksize = 3 * bits + 7;
		begin_block_index = begin_weight_index / 3;
		end_block_index = (end_weight_index + 2) / 3;
		begin_i = begin_block_index * blocksize / 8;
		end_i = 1 + end_block_index * blocksize / 8;
	}
	else
	{
		int blocksize = bits;
		begin_block_index = begin_weight_index;
		end_block_index = end_weight_index;
		begin_i = begin_block_index * blocksize / 8;
		end_i = 1 + end_block_index * blocksize / 8;
	}

	for (int i = begin_i; i < end_i; i++)
	{
		bswapped[i] = static_cast<uint8_t>(bitrev8(pcb[15 - i]));
	}

	int bits_for_weights = get_ise_sequence_bitcount(real_weight_count, weight_quant_method);

	int below_weights_pos = 128 - bits_for_weights;

	uint8_t indices[64];
	const auto& qat = quant_and_xfer_tables[weight_quant_method];

	decode_ise(weight_quant_method, real_weight_count, bswapped, indices, 0);

	if (is_dual_plane)
	{
		for (int i = begin_weight_index / 2; i < end_weight_index / 2; i++)
		{
			scb.weights[i] = qat.unscramble_and_unquant_map[indices[2 * i]];
			scb.weights[i + WEIGHTS_PLANE2_OFFSET] = qat.unscramble_and_unquant_map[indices[2 * i + 1]];
		}
	}
	else
	{
		for (int i = begin_weight_index; i < end_weight_index; i++)
		{
			scb.weights[i] = qat.unscramble_and_unquant_map[indices[i]];
		}
	}

	if (is_dual_plane && partition_count == 4)
	{
		scb.block_type = SYM_BTYPE_ERROR;
		return;
	}

	scb.color_formats_matched = 0;

	// Determine the format of each endpoint pair
	int color_formats[BLOCK_MAX_PARTITIONS];
	int encoded_type_highpart_size = 0;
	if (partition_count == 1)
	{
		color_formats[0] = read_bits(4, 13, pcb);
		scb.partition_index = 0;
	}
	else
	{
		encoded_type_highpart_size = (3 * partition_count) - 4;
		below_weights_pos -= encoded_type_highpart_size;
		int encoded_type = read_bits(6, 13 + PARTITION_INDEX_BITS, pcb) |
		                  (read_bits(encoded_type_highpart_size, below_weights_pos, pcb) << 6);
		int baseclass = encoded_type & 0x3;
		if (baseclass == 0)
		{
			for (int i = 0; i < partition_count; i++)
			{
				color_formats[i] = (encoded_type >> 2) & 0xF;
			}

			below_weights_pos += encoded_type_highpart_size;
			scb.color_formats_matched = 1;
			encoded_type_highpart_size = 0;
		}
		else
		{
			int bitpos = 2;
			baseclass--;

			for (int i = 0; i < partition_count; i++)
			{
				color_formats[i] = (((encoded_type >> bitpos) & 1) + baseclass) << 2;
				bitpos++;
			}

			for (int i = 0; i < partition_count; i++)
			{
				color_formats[i] |= (encoded_type >> bitpos) & 3;
				bitpos += 2;
			}
		}
		scb.partition_index = static_cast<uint16_t>(read_bits(10, 13, pcb));
	}

	for (int i = 0; i < partition_count; i++)
	{
		scb.color_formats[i] = static_cast<uint8_t>(color_formats[i]);
	}

	// Determine number of color endpoint integers
	int color_integer_count = 0;
	for (int i = 0; i < partition_count; i++)
	{
		int endpoint_class = color_formats[i] >> 2;
		color_integer_count += (endpoint_class + 1) * 2;
	}

	if (color_integer_count > 18)
	{
		scb.block_type = SYM_BTYPE_ERROR;
		return;
	}

	// Determine the color endpoint format to use
	static const int color_bits_arr[5] { -1, 115 - 4, 113 - 4 - PARTITION_INDEX_BITS, 113 - 4 - PARTITION_INDEX_BITS, 113 - 4 - PARTITION_INDEX_BITS };
	int color_bits = color_bits_arr[partition_count] - bits_for_weights - encoded_type_highpart_size;
	if (is_dual_plane)
	{
		color_bits -= 2;
	}

	if (color_bits < 0)
	{
		color_bits = 0;
	}

	int color_quant_level = quant_mode_table[color_integer_count >> 1][color_bits];
	if (color_quant_level < QUANT_6)
	{
		scb.block_type = SYM_BTYPE_ERROR;
		return;
	}

	// Unpack the integer color values and assign to endpoints
	scb.quant_mode = static_cast<quant_method>(color_quant_level);

	uint8_t values_to_decode[32];
	decode_ise(static_cast<quant_method>(color_quant_level), color_integer_count, pcb,
	           values_to_decode, (partition_count == 1 ? 17 : 19 + PARTITION_INDEX_BITS));

	int valuecount_to_decode = 0;
	const uint8_t* unpack_table = color_scrambled_pquant_to_uquant_tables[scb.quant_mode - QUANT_6];
	for (int i = 0; i < partition_count; i++)
	{
		int vals = 2 * (color_formats[i] >> 2) + 2;
		for (int j = 0; j < vals; j++)
		{
			scb.color_values[i][j] = unpack_table[values_to_decode[j + valuecount_to_decode]];
		}
		valuecount_to_decode += vals;
	}

	// Fetch component for second-plane in the case of dual plane of weights.
	scb.plane2_component = -1;
	if (is_dual_plane)
	{
		scb.plane2_component = static_cast<int8_t>(read_bits(2, below_weights_pos - 2, pcb));
	}
}

/* See header for documentation. */
static void decompress_symbolic_block(
	astcenc_profile decode_mode,
	const block_size_descriptor& bsd,
	const symbolic_compressed_block& scb,
	const UCommon::FUint64Vector2& BlockSize,
	UCommon::TSpan<const uint64_t> PointIndices,
	UCommon::FVector4f* Pixels
) {
	// If we detected an error-block, blow up immediately.
	if (scb.block_type == SYM_BTYPE_ERROR)
	{
		for (uint64_t Index = 0; Index < PointIndices.Num(); ++Index)
		{
			Pixels[Index] = UCommon::FVector4f(error_color_nan());
		}

		return;
	}

	if ((scb.block_type == SYM_BTYPE_CONST_F16) ||
		(scb.block_type == SYM_BTYPE_CONST_U16))
	{
		vfloat4 color;
		uint8_t use_lns = 0;

		// UNORM16 constant color block
		if (scb.block_type == SYM_BTYPE_CONST_U16)
		{
			vint4 colori(scb.constant_color);

			// Determine the UNORM8 rounding on the decode
			// Decode mode or sRGB forces writing to unorm8 output value
			vmask4 u8_mask = /*blk.decode_unorm8 ||*/ decode_mode == ASTCENC_PRF_LDR_SRGB ? vmask4(true) : vmask4(false);

			// The real decoder would just use the top 8 bits, but we rescale
			// in to a 16-bit value that rounds correctly.
			vint4 colori_u8 = asr<8>(colori) * 257;
			colori = select(colori, colori_u8, u8_mask);

			vint4 colorf16 = unorm16_to_sf16(colori);
			color = float16_to_float(colorf16);
		}
		// FLOAT16 constant color block
		else
		{
			switch (decode_mode)
			{
			case ASTCENC_PRF_LDR_SRGB:
			case ASTCENC_PRF_LDR:
				color = vfloat4(error_color_nan());
				break;
			case ASTCENC_PRF_HDR_RGB_LDR_A:
			case ASTCENC_PRF_HDR:
				// Constant-color block; unpack from FP16 to FP32.
				color = float16_to_float(vint4(scb.constant_color));
				use_lns = 1;
				break;
			}
		}

		for (uint64_t Index = 0; Index < PointIndices.Num(); ++Index)
		{
			Pixels[Index] = UCommon::FVector4f(color.lane<0>(), color.lane<1>(), color.lane<2>(), color.lane<3>());
		}

		return;
	}

	// Get the appropriate partition-table entry
	int partition_count = scb.partition_count;
	const auto& pi = bsd.get_partition_info(partition_count, scb.partition_index);

	// Get the appropriate block descriptors
	const auto& bm = bsd.get_block_mode(scb.block_mode);
	const auto& di = bsd.get_decimation_info(bm.decimation_mode);

	bool is_dual_plane = static_cast<bool>(bm.is_dual_plane);

	// Unquantize and undecimate the weights
	int plane1_weights[BLOCK_MAX_TEXELS];
	int plane2_weights[BLOCK_MAX_TEXELS];
	unpack_weights(bsd, scb, di, is_dual_plane, plane1_weights, plane2_weights
		/*, BlockSize, PointIndices*/);


	// Now that we have endpoint colors and weights, we can unpack texel colors
	int plane2_component = scb.plane2_component;
#if ASTCENC_SSE > 0 || ASTCENC_NEON > 0
	vmask4 plane2_mask = vint4::lane_id() == vint4(plane2_component);
#endif

	vmask4 u8_mask = /*blk.decode_unorm8 ||*/ decode_mode == ASTCENC_PRF_LDR_SRGB ? vmask4(true) : vmask4(false);

	vint4 ep0_64_add_32s[BLOCK_MAX_PARTITIONS];
	vint4 ep1_minus_ep0s[BLOCK_MAX_PARTITIONS];
	bool unpackeds[BLOCK_MAX_PARTITIONS] = { false,false,false,false };
	for (uint64_t Index = 0; Index < PointIndices.Num(); ++Index)
	{
		const uint64_t PixelIndex = PointIndices[Index];
		int partition_index = -1;
		if (partition_count == 1)
		{
			partition_index = 0;
		}
		else
		{
			for (int i = 0; i < partition_count; i++)
			{
				int texel_count = pi.partition_texel_count[i];
				for (int j = 0; j < texel_count; j++)
				{
					uint64_t tix = pi.texels_of_partition[i][j];
					if (tix == PixelIndex)
					{
						partition_index = i;
						break;
					}
				}
				if (partition_index >= 0)
				{
					break;
				}
			}
		}

		// Decode the color endpoints for this partition
		if (!unpackeds[partition_index])
		{
			vint4 ep0, ep1;
			bool rgb_lns;
			bool a_lns;
			unpack_color_endpoints(decode_mode,
				scb.color_formats[partition_index],
				scb.color_values[partition_index],
				rgb_lns, a_lns,
				ep0, ep1);
			UBPA_UCOMMON_ASSERT(!rgb_lns && !a_lns);
			unpackeds[partition_index] = true;
			ep1_minus_ep0s[partition_index] = ep1 - ep0;
			ep0_64_add_32s[partition_index] = ep0 * 64 + 32;
		}

#if ASTCENC_SSE > 0 || ASTCENC_NEON > 0
		vint4 weight = select(vint4(plane1_weights[PixelIndex]), vint4(plane2_weights[PixelIndex]), plane2_mask);
#else
		vint4 weight = vint4(plane1_weights[PixelIndex]);
		if (plane2_component >= 0)
		{
			weight.m[plane2_component] = plane2_weights[PixelIndex];
		}
#endif
		vint4 color = lerp_color_int(u8_mask, ep0_64_add_32s[partition_index], ep1_minus_ep0s[partition_index], weight);
		
		vfloat4 colorf = float16_to_float(unorm16_to_sf16(color));

		Pixels[Index] = UCommon::FVector4f(colorf.lane<0>(), colorf.lane<1>(), colorf.lane<2>(), colorf.lane<3>());
	}
}

void UCommon::FASTCBlock::GetPixels(EASTCProfile Profile, const FUint64Vector2& BlockSize, TSpan<const uint64_t> PointIndices, FVector4f* Pixels) const
{
	const block_size_descriptor& bsd = BlockSizeDescriptorMngr::GetInstance().GetBlockSizeDescriptor(BlockSize);

	const uint8_t(&pcb)[16] = Data;
	symbolic_compressed_block scb;

	physical_to_symbolic(bsd, pcb, scb, BlockSize, PointIndices);

	decompress_symbolic_block(
		(astcenc_profile)Profile,
		bsd,
		scb,
		BlockSize, PointIndices, Pixels
	);
}

UCommon::FVector4f UCommon::FASTCBlock::GetPixel(EASTCProfile Profile, const FUint64Vector2& BlockSize, uint64_t PointIndex) const
{
	FVector4f Result;
	GetPixels(Profile, BlockSize, { &PointIndex, 1 }, &Result);
	return Result;
}

void UCommon::FASTCBlock::GetAllPixels(EASTCProfile Profile, const FUint64Vector2& BlockSize, FVector4f* Pixels) const
{
	const block_size_descriptor& bsd = BlockSizeDescriptorMngr::GetInstance().GetBlockSizeDescriptor(BlockSize);

	const uint8_t(&pcb)[16] = Data;
	symbolic_compressed_block scb;

	physical_to_symbolic(bsd, pcb, scb);

	image_block blk{};
	decompress_symbolic_block(
		(astcenc_profile)Profile,
		bsd,
		0,
		0,
		0,
		scb,
		blk);

	for (uint64_t Index = 0; Index < BlockSize.Area(); Index++)
	{
		Pixels[Index] = FVector4f(blk.data_r[Index], blk.data_g[Index], blk.data_b[Index], blk.data_a[Index]);
	}
}

void UCommon::CompressImageToASTC(UCommon::FASTCBlock* Blocks, EASTCProfile Profile, EElementType ElementType, const void* Image, const FUint64Vector2& ImageSize, const FUint64Vector2& BlockSize, FASTCConfig ASTCConfig)
{
	UBPA_UCOMMON_ASSERT(Blocks && Image);
	UBPA_UCOMMON_ASSERT(ElementType == EElementType::Uint8 || ElementType == EElementType::Half || ElementType == EElementType::Float);

	const unsigned int block_x = static_cast<unsigned int>(BlockSize.X);
	const unsigned int block_y = static_cast<unsigned int>(BlockSize.Y);
	const unsigned int block_z = 1;

	// Initialize cli_config_options with default values
	astcenc_swizzle swz_encode{ ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A };
	if (ASTCConfig.Swizzel)
	{
		astcenc_swz swizzle_components[4];
		for (int i = 0; i < 4; i++)
		{
			switch (ASTCConfig.Swizzel[i])
			{
			case 'r': swizzle_components[i] = ASTCENC_SWZ_R; break;
			case 'g': swizzle_components[i] = ASTCENC_SWZ_G; break;
			case 'b': swizzle_components[i] = ASTCENC_SWZ_B; break;
			case 'a': swizzle_components[i] = ASTCENC_SWZ_A; break;
			case '0': swizzle_components[i] = ASTCENC_SWZ_0; break;
			case '1': swizzle_components[i] = ASTCENC_SWZ_1; break;
			default:
				UBPA_UCOMMON_NO_ENTRY();
				swizzle_components[i] = static_cast<astcenc_swz>((int)ASTCENC_SWZ_R + i);
				break;
			}
		}

		swz_encode.r = swizzle_components[0];
		swz_encode.g = swizzle_components[1];
		swz_encode.b = swizzle_components[2];
		swz_encode.a = swizzle_components[3];
	}

	astcenc_config config{};
	unsigned int flags = 0;
	switch (ASTCConfig.Format)
	{
	case FASTCConfig::EFormat::RGBM:
		flags |= ASTCENC_FLG_MAP_RGBM;
		break;
	case FASTCConfig::EFormat::RGBD:
		flags |= ASTCENC_FLG_MAP_RGBD;
		break;
	case FASTCConfig::EFormat::RGBV:
		flags |= ASTCENC_FLG_MAP_RGBV;
		break;
	default:
		break;
	}
	astcenc_config_init((astcenc_profile)Profile, block_x, block_y, block_z, ASTCConfig.Quality, flags, &config);
	if (ASTCConfig.Cw)
	{
		config.cw_r_weight = ASTCConfig.Cw[0];
		config.cw_g_weight = ASTCConfig.Cw[1];
		config.cw_b_weight = ASTCConfig.Cw[2];
		config.cw_a_weight = ASTCConfig.Cw[3];
	}
	switch (ASTCConfig.Format)
	{
	case FASTCConfig::EFormat::RGBM:
		config.rgbm_m_scale = std::max(1.f, ASTCConfig.MaxValue);
		config.cw_a_weight = 2.0f * config.rgbm_m_scale;
		break;
	case FASTCConfig::EFormat::RGBD:
	{
		float rgbd_max = std::max(1.f, ASTCConfig.MaxValue);
		config.rgbd_k = 1.f / std::sqrt(rgbd_max) - 1.f;
		config.cw_a_weight = rgbd_max;
	}
	break;
	case FASTCConfig::EFormat::RGBV:
	{
		float rgbv_max = std::max(1.f, ASTCConfig.MaxValue);
		float rgbv_s = std::max(0.f, ASTCConfig.RGBV_S);
		config.rgbv_k = -rgbv_s;
		config.rgbv_b = rgbv_s + 1.f / rgbv_max;
		config.cw_a_weight = rgbv_max;
	}
	break;
	default:
		break;
	}
	config.cw_weights = (float*)ASTCConfig.Weights;

	astcenc_image* image_uncomp_in = nullptr;

	astcenc_context* codec_context;

	const unsigned int thread_count = ASTCConfig.ThreadPool ? ASTCConfig.ThreadPool->GetNumThreads() : 1u;

	const astcenc_error codec_status = astcenc_context_alloc(&config, thread_count, &codec_context);
	UBPA_UCOMMON_ASSERT(codec_status == ASTCENC_SUCCESS);

	astcenc_image image_uncomp_in_data;
	image_uncomp_in_data.dim_x = static_cast<unsigned int>(ImageSize.X);
	image_uncomp_in_data.dim_y = static_cast<unsigned int>(ImageSize.Y);
	image_uncomp_in_data.dim_z = 1u;
	image_uncomp_in_data.data_type = ElementType == EElementType::Float ? ASTCENC_TYPE_F32
		: (ElementType == EElementType::Half ? ASTCENC_TYPE_F16
		: ASTCENC_TYPE_U8);
	image_uncomp_in_data.data = const_cast<void**>(&Image);

	image_uncomp_in = &image_uncomp_in_data;

	const unsigned int blocks_x = (image_uncomp_in->dim_x + config.block_x - 1) / config.block_x;
	const unsigned int blocks_y = (image_uncomp_in->dim_y + config.block_y - 1) / config.block_y;
	const unsigned int blocks_z = (image_uncomp_in->dim_z + config.block_z - 1) / config.block_z;
	const size_t buffer_size = blocks_x * blocks_y * blocks_z * 16;

	auto work = [&](uint64_t ID)
	{
		const astcenc_error work_error = astcenc_compress_image(codec_context, image_uncomp_in, &swz_encode, (uint8_t*)Blocks, buffer_size, (unsigned int)ID);
		UBPA_UCOMMON_ASSERT(work_error == ASTCENC_SUCCESS);
	};

	if (ASTCConfig.ThreadPool)
	{
		std::vector<std::future<void>> Tasks;
		for (uint64_t ID = 0; ID < codec_context->context.thread_count; ID++)
		{
			Tasks.push_back(ASTCConfig.ThreadPool->Enqueue(work, ID));
		}
		for (auto& Task : Tasks)
		{
			Task.wait();
		}
	}
	else
	{
		work(0);
	}

	astcenc_context_free(codec_context);
}

void UCommon::DecompressASTCImage(FVector4f* Image, EASTCProfile Profile, const FASTCBlock* Blocks, const FUint64Vector2& ImageSize, const FUint64Vector2& BlockSize)
{
	UBPA_UCOMMON_ASSERT(Image && Blocks);

	const FUint64Vector2 BlockGrid2D = (ImageSize + BlockSize - 1) / BlockSize;

	FVector4f Pixels[BLOCK_MAX_TEXELS];
	for (uint64_t BlockIndexY = 0; BlockIndexY < BlockGrid2D.Y; BlockIndexY++)
	{
		for (uint64_t BlockIndexX = 0; BlockIndexX < BlockGrid2D.X; BlockIndexX++)
		{
			const uint64_t BlockIndex = BlockIndexX + BlockIndexY * BlockGrid2D.X;
			Blocks[BlockIndex].GetAllPixels(Profile, BlockSize, Pixels);

			for (uint64_t Y = 0; Y < BlockSize.Y; Y++)
			{
				const uint64_t ImageIndexY = BlockIndexY * BlockSize.Y + Y;
				if (ImageIndexY >= ImageSize.Y)
				{
					break;
				}

				for (uint64_t X = 0; X < BlockSize.X; X++)
				{
					const uint64_t ImageIndexX = BlockIndexX * BlockSize.X + X;
					if (ImageIndexX >= ImageSize.X)
					{
						break;
					}

					const uint64_t ImageIndex = ImageIndexX + ImageIndexY * ImageSize.X;

					Image[ImageIndex] = Pixels[X + Y * BlockSize.X];
				}
			}
		}
	}
}

void UCommon::ToASTC(FTex2D& Tex, EASTCProfile Profile, const FTex2D& This, uint64_t BlockSize, FASTCConfig Config, FASTCBlock* BlockBuffer)
{
	UBPA_UCOMMON_ASSERT(BlockSize == 4 || BlockSize == 6 || BlockSize == 8 || BlockSize == 10 || BlockSize == 12);
	UBPA_UCOMMON_ASSERT(Tex.GetNumChannels() == 4);
	UBPA_UCOMMON_ASSERT(Tex.GetGrid2D() == This.GetGrid2D());

	bool bNewBlockBuffer = false;
	if (!BlockBuffer)
	{
		const uint64_t NumBlocks = ((This.GetGrid2D().GetExtent() + BlockSize - 1) / BlockSize).Area();
		BlockBuffer = new FASTCBlock[NumBlocks];
		bNewBlockBuffer = true;
	}
	const FUint64Vector2 ImageSize = This.GetGrid2D().GetExtent();
	CompressImageToASTC(BlockBuffer, Profile, This.GetElementType(), This.GetStorage(), ImageSize, FUint64Vector2(BlockSize), Config);
	DecompressASTCImage((FVector4f*)Tex.GetStorage(), Profile, BlockBuffer, ImageSize, FUint64Vector2(BlockSize));
	if (bNewBlockBuffer)
	{
		delete[] BlockBuffer;
	}
}

UCommon::FTex2D UCommon::ToASTC(EASTCProfile Profile, const FTex2D& This, uint64_t BlockSize, FASTCConfig ASTCConfig, FASTCBlock* BlockBuffer)
{
	UCommon::FTex2D Tex(This.GetGrid2D(), 4, EElementType::Float);
	ToASTC(Tex, Profile, This, BlockSize, ASTCConfig, BlockBuffer);
	return Tex;
}
