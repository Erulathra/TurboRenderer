#pragma once

#include "DataStructures/BitArray.h"
#include "DataStructures/Handle.h"

namespace Turbo
{
	/*
	 * NOTE(SS): As this is a ResourcePool created especially for renderer I should probably:
	 *    1. Modify allocation strategy to reduce per slot generation usage.
	 *    2. Make resource usage more protected. We can use `nil` slot as a Vulkan resource anyway.
	 */

	template <typename T, typename InternalT, u32 size>
	struct TResourcePool
	{
		/* Aliases */
		using IndexType = u32;
		using GenType = u32;

		/* Data */
		T data[size];
		InternalT intData[size];
		GenType generations[size];
		BitArray<size> used;

		IndexType nextFree[size];
		IndexType firstFree;

		/* Api */
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
			verify(firstFree < Size());

			if (firstFree == 0 || used.Get(firstFree) == true)
			{
				firstFree++;
			}

			if (firstFree >= Size() || used.Get(firstFree) == true)
			{
				return THandle<T>::Nil();
			}

			const u32 newIdx = firstFree;
			if (nextFree[newIdx] != 0)
			{
				firstFree = nextFree[newIdx];
			}

			used.Set(newIdx, true);

			return THandle<T>::Create(newIdx, generations[newIdx]);
		}

		void Release(THandle<T> handle)
		{
			if (u32 idx = DeRef(handle))
			{
				if (firstFree)
				{
					nextFree[idx] = firstFree;
				}
				firstFree = idx;

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
