#pragma once

namespace Turbo
{
	struct Arena
	{
		void* mAllocation = nullptr;
		void* mTop = nullptr;
		void* mTip = nullptr;
		bool bInitialized = false;

		void Init(void* data, u64 size)
		{
			check(data && size > 0);

    		mAllocation = data;

    		mTip = (u8*)mAllocation + size;
    		mTop = (u8*)mAllocation;
         bInitialized = true;
		}

		static Arena Create(void* data, u64 size)
		{
		   Arena arena;
			arena.Init(data, size);
		   return arena;
		}

		u64 Size() const
		{
         return (u8*)mTip - (u8*)mAllocation;
		}

		template<typename T>
		T* Allocate()
		{
			static_assert(16 % alignof(T) == 0);
		   return (T*)Allocate(sizeof(T));
		}

		void* Allocate(u64 size)
		{
			check(mAllocation != nullptr && mTop != nullptr && mTip != nullptr);
			check(size > 0);

			void* result = Align16(mTop);
			void* newTop = (u8*)result + size;
			check(newTop <= mTip);

			mTop = newTop;

			return result;
		}

		void Clear() { mTop = mAllocation; }
	};
} // namespace Turbo

#define AllocateStaticArena(NAME, SIZE) Arena NAME; u8 _##NAME##Data[(SIZE)]; NAME.Init(_##NAME##Data, (SIZE))
