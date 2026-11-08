#pragma once

#include "DataStructures/BitArray.h"
#include "DataStructures/Handle.h"

namespace Turbo
{
	/*
	 * NOTE(SS): This data structure is especially designed to work with renderer resources. It contains two arrays of two
	 *    structure types. By design the first type is used as a abstraction type, and the second one as a internal
	 *    type that contains data required by the particular Graphics API implementation.
	 *
	 *    To reduce slot saturation, the resource pool uses FIFO queue as the nextFree list.
	 */

	template <typename T, typename InternalT, u32 size>
	struct TResourcePool
	{
		static_assert(size > 2);

		/* Aliases */
		using IndexType = u32;
		using GenType = u32;

		/* Data */
		T data[size];
		InternalT intData[size];
		GenType generations[size];
		BitArray<size> used;

		/* Free slots lookup */
		IndexType nextFree[size];
		IndexType firstFree;
		IndexType lastFree;

		/* Api */

		/* Assumes ZII. */
		void Init()
		{
			firstFree = 1;
			lastFree = Size() - 1;

			nextFree[0] = 0;
			nextFree[Size() - 1] = 0;
			for (IndexType i = 1; i < (Size() - 1); ++i)
			{
				nextFree[i] = i + 1;
			}
		}

		FORCEINLINE u32 Size() const { return size; }

		FORCEINLINE T* Get(THandle<T> handle)
		{
			const IndexType idx = DeRef(handle);
			return idx != 0 ? &data[idx] : nullptr;
		}

		FORCEINLINE const T* Get(THandle<T> handle) const
		{
			const IndexType idx = DeRef(handle);
			return idx != 0 ? &data[idx] : nullptr;
		}

		FORCEINLINE InternalT* GetInternal(THandle<T> handle)
		{
			const IndexType idx = DeRef(handle);
			return idx != 0 ? &intData[idx] : nullptr;
		}

		FORCEINLINE const InternalT* GetInternal(THandle<T> handle) const
		{
			const IndexType idx = DeRef(handle);
			return idx != 0 ? &intData[idx] : nullptr;
		}

		THandle<T> Acquire()
		{
			verifyf(firstFree < Size() && firstFree != 0, "No more resources left.");

			const u32 newIdx = firstFree;
			firstFree = nextFree[newIdx];
			nextFree[newIdx] = 0;

			used.Set(newIdx, true);

			return THandle<T>::Create(newIdx, generations[newIdx]);
		}

		void Release(THandle<T> handle)
		{
			if (u32 idx = DeRef(handle))
			{
				if (firstFree == 0)
				{
					nextFree[idx] = 0;
					firstFree = idx;
					lastFree = idx;
				}
				else
				{
					nextFree[lastFree] = idx;
					lastFree = idx;
				}

				used.Set(idx, false);

				verify(generations[idx] < Handle::genMask);
				generations[idx]++;
			}
		}

		u32 DeRef(THandle<T> handle) const
		{
			const IndexType idx = handle.GetIndex();
			const IndexType gen = handle.GetGen();

			if (idx > 0 && idx < Size()
				&& used.Get(idx)
				&& gen == generations[idx])
			{
				return idx;
			}

			return 0;
		}
	};

} // namespace Turbo
