#pragma once

namespace Turbo
{
	template<u32 numBits>
	struct BitArray
	{
		/* Static helpers */
		constexpr static u32 NumBits() { return numBits; }
		constexpr static u32 Size() { return NumWords() * sizeof(u32); }
		constexpr static u32 NumWords() { return (numBits + 31) / 32; }

		/* Data */
		u32 data[NumWords()];
		u32 numWords = NumWords();

		FORCEINLINE bool Get(u32 bitIndex)
		{
			check(bitIndex < NumBits());
			return (data[GetElementIndex(bitIndex)] & (1 << GetElementBitIndex(bitIndex))) != 0;
		}

		void Set(u32 bitIndex, bool value)
		{
			check(bitIndex < NumBits());

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

					if (bitIndex < NumBits())
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

			return NumBits();
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

			return NumBits();
		}

		/* Internal helpers */
		u32 GetElementIndex(u32 bitIndex) const
		{
			check(bitIndex < NumBits());
			return bitIndex / 32;
		}

		u32 GetElementBitIndex(u32 bitIndex) { return bitIndex % 32; }
	};
} // namespace Turbo
