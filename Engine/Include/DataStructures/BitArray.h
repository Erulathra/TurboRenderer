#pragma once

#include "DataStructures/Arena.h"

namespace Turbo
{
	struct BitArray
	{
		/* Data */
		u32* mData = nullptr;
		u32 mBitSize = 0;
		u32 mNumWords = 0;

		/* Interface */
		static u32 CalculateNumWords(u32 numBits) { return (numBits + 31) / 32; }
		static u32 CalculateSize(u32 numBits) { return CalculateNumWords(numBits) * sizeof(u32); }

		static BitArray Create(Arena& arena, u32 numBits)
		{
			BitArray result;
			result.Init(arena, numBits);

			return result;
		}

		static BitArray Create(void* data, u32 numBits)
		{
			BitArray result;
			result.Init(data, numBits);

			return result;
		}

		void Init(Arena& arena, u32 numBits)
		{
			mNumWords = CalculateNumWords(numBits);
			mData = (u32*)arena.Allocate(mNumWords);
			mBitSize = numBits;

			MemZero(mData, mNumWords * sizeof(u32));
		}

		void Init(void* data, u32 numBits)
		{
			check(data);

			mData = (u32*)data;
			mBitSize = numBits;
			mNumWords = CalculateNumWords(numBits);

			MemZero(mData, mNumWords * sizeof(u32));
		}

		bool Get(u32 bitIndex)
		{
			check(bitIndex < mBitSize);
			return (mData[GetElementIndex(bitIndex)] & (1 << GetElementBitIndex(bitIndex))) != 0;
		}

		void Set(u32 bitIndex, bool value)
		{
			check(bitIndex < mBitSize);

			const u32 elementIndex = GetElementIndex(bitIndex);
			const u32 elementBitIndex = GetElementBitIndex(bitIndex);

			if (value)
			{
				mData[elementIndex] |= (1 << elementBitIndex);
			}
			else
			{
				mData[elementIndex] &= ~(1 << elementBitIndex);
			}
		}

		template <typename FuncType>
		void IterateOverEnabledBits(const FuncType& func) const
		{
			for (u32 dataIndex = 0; dataIndex < mNumWords; ++dataIndex)
			{
				u32 dataValue = mData[dataIndex];
				while (dataValue != 0)
				{
					const u32 temp = dataValue & u32(-i32(dataValue));
					const u32 trailingZeros = CountTrailingZeros(dataValue);

					const u32 bitIndex = dataIndex * 32 + trailingZeros;

					if (bitIndex < mBitSize)
					{
						Invoke(func, bitIndex);
					}

					dataValue ^= temp;
				}
			}
		}

		/* Returns `mBitSize` if not found */
		u32 FindFirstZero() const
		{
			for (u32 dataIndex = 0; dataIndex < mNumWords; ++dataIndex)
			{
				if (const u32 trailingOnes = CountTrailingOnes(mData[dataIndex]); trailingOnes < 32)
				{
					return dataIndex * 32 + trailingOnes;
				}
			}

			return mBitSize;
		}

		/* Returns `mBitSize` if not found */
		u32 FindFirstOne() const
		{
			for (u32 dataIndex = 0; dataIndex < mNumWords; ++dataIndex)
			{
				if (const u32 trailingOnes = CountTrailingZeros(mData[dataIndex]); trailingOnes < 32)
				{
					return dataIndex * 32 + trailingOnes;
				}
			}

			return mBitSize;
		}

		/* Internal helpers */
		u32 GetElementIndex(u32 bitIndex) const
		{
			check(bitIndex < mBitSize);
			return bitIndex / 32;
		}

		u32 GetElementBitIndex(u32 bitIndex) { return bitIndex % 32; }
	};
} // namespace Turbo
