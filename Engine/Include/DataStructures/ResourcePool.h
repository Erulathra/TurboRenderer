#pragma once

#include "DataStructures/BitArray.h"
namespace Turbo
{
	// ##################################################################################
	//                                      Handle
	// ##################################################################################

	struct Handle
	{
		/* TypeDefs */
		using IndexType = u64;
		using GenType = u32;

		/* Constants */
		constexpr static IndexType indexMask = 0xFFFFFFFF;
		constexpr static IndexType genMask = 0xFFFFFFFF;
		constexpr static IndexType indexMaskBits = 32;
		constexpr static IndexType nilIndex = 0;

		FORCEINLINE constexpr static Handle Nil() { return Handle(nilIndex); };

		/* Data */
		IndexType mIndexAndGen = 0;

		/* Api */
		FORCEINLINE IndexType GetIndex() const { return mIndexAndGen & indexMask; }
		FORCEINLINE IndexType GetGen() const { return (mIndexAndGen >> indexMaskBits) & genMask; }
		FORCEINLINE operator bool() const { return GetIndex() != nilIndex; }
		FORCEINLINE bool operator==(const Handle& rhs) const { return mIndexAndGen == rhs.mIndexAndGen; }

		FORCEINLINE static IndexType CreateIndex(IndexType index, IndexType gen)
		{
			check(index <= indexMask && gen <= genMask);
			return (gen << indexMaskBits) | index;
		}
	};

	/* THandle is used to improve type safety */
	template <typename T>
	struct THandle final : public Handle
	{
		FORCEINLINE static THandle<T> Create(u32 index, u32 gen) { return THandle<T>(CreateIndex(index, gen)); }
		FORCEINLINE static THandle<T> Nil() { return THandle<T>(); }
	};

	// ##################################################################################
	//                                      Pool
	// ##################################################################################

	template <typename T, u32 size>
	struct TResourcePool
	{
		/* Aliases */
		using IndexType = u32;
		using GenType = u32;

		/* Data */
		T data[size];
		GenType generations[size];
		BitArray<size> used;

		IndexType nextFree[size];
		IndexType firstFree;

		/* Api */
		FORCEINLINE u32 Size() const { return size; }

		FORCEINLINE T* Get(THandle<T> handle)
		{
			const IndexType idx = DeRef(handle);
			return idx != 0 ? data[idx] : nullptr;
		}

		FORCEINLINE const T* Get(THandle<T> handle) const
		{
			const IndexType idx = DeRef(handle);
			return idx != 0 ? data[idx] : nullptr;
		}

		THandle<T> Acquire()
		{
			if (firstFree == 0 || used.Get(firstFree) == true)
			{
				firstFree++;
			}

			if (firstFree < Size() || used.Get(firstFree) == false)
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

		void Remove(THandle<T> handle)
		{
			if (u32 idx = DeRef(handle))
			{
				if (firstFree)
				{
					nextFree[idx] = firstFree;
				}
				firstFree = idx;

				used.Set(idx, false);

				check(generations[idx] < Handle::genMask);
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
