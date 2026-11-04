#pragma once

#include "DataStructures/Arena.h"

namespace Turbo
{
	struct BitArray
	{
		/* Data */
		u32* data = nullptr;
		u32 bitSize = 0;
		u32 numWords = 0;

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
			numWords = CalculateNumWords(numBits);
			data = (u32*)arena.Allocate(numWords);
			bitSize = numBits;

			MemZero(data, numWords * sizeof(u32));
		}

		void Init(void* initData, u32 numBits)
		{
			check(initData);

			data = (u32*)initData;
			bitSize = numBits;
			numWords = CalculateNumWords(numBits);

			MemZero(data, numWords * sizeof(u32));
		}

		bool Get(u32 bitIndex)
		{
			check(bitIndex < bitSize);
			return (data[GetElementIndex(bitIndex)] & (1 << GetElementBitIndex(bitIndex))) != 0;
		}

		void Set(u32 bitIndex, bool value)
		{
			check(bitIndex < bitSize);

			const u32 elementIndex = GetElementIndex(bitIndex);
			const u32 elementBitIndex = GetElementBitIndex(bitIndex);

			if (value)
			{
				data[elementIndex] |= (1 << elementBitIndex);
			}
			else
			{
				data[elementIndex] &= ~(1 << elementBitIndex);
			}
		}

		template <typename FuncType>
		void IterateOverEnabledBits(const FuncType& func) const
		{
			for (u32 dataIndex = 0; dataIndex < numWords; ++dataIndex)
			{
				u32 dataValue = data[dataIndex];
				while (dataValue != 0)
				{
					const u32 temp = dataValue & u32(-i32(dataValue));
					const u32 trailingZeros = CountTrailingZeros(dataValue);

					const u32 bitIndex = dataIndex * 32 + trailingZeros;

					if (bitIndex < bitSize)
					{
						Invoke(func, bitIndex);
					}

					dataValue ^= temp;
				}
			}
		}

		/* Returns `bitSize` if not found */
		u32 FindFirstZero() const
		{
			for (u32 dataIndex = 0; dataIndex < numWords; ++dataIndex)
			{
				if (const u32 trailingOnes = CountTrailingOnes(data[dataIndex]); trailingOnes < 32)
				{
					return dataIndex * 32 + trailingOnes;
				}
			}

			return bitSize;
		}

		/* Returns `bitSize` if not found */
		u32 FindFirstOne() const
		{
			for (u32 dataIndex = 0; dataIndex < numWords; ++dataIndex)
			{
				if (const u32 trailingOnes = CountTrailingZeros(data[dataIndex]); trailingOnes < 32)
				{
					return dataIndex * 32 + trailingOnes;
				}
			}

			return bitSize;
		}

		/* Internal helpers */
		u32 GetElementIndex(u32 bitIndex) const
		{
			check(bitIndex < bitSize);
			return bitIndex / 32;
		}

		u32 GetElementBitIndex(u32 bitIndex) { return bitIndex % 32; }
	};
} // namespace Turbo
