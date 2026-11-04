#pragma once

#define MakeStaticArena(NAME, SIZE) Arena NAME; u8 _##NAME##Data[(SIZE)]; NAME.Init(_##NAME##Data, (SIZE))

namespace Turbo
{
	struct Arena
	{
		void* allocation = nullptr;
		void* top = nullptr;
		void* tip = nullptr;
		bool initialized = false;

		void Init(void* data, u64 size)
		{
			check(data && size > 0);

    		allocation = data;

    		tip = (u8*)allocation + size;
    		top = (u8*)allocation;
         initialized = true;
		}

		static Arena Create(void* data, u64 size)
		{
		   Arena arena;
			arena.Init(data, size);
		   return arena;
		}

		u64 Size() const
		{
         return (u8*)tip - (u8*)allocation;
		}

		template<typename T>
		T* Allocate()
		{
			static_assert(16 % alignof(T) == 0);
		   return (T*)Allocate(sizeof(T));
		}

		void* Allocate(u64 size)
		{
			check(allocation != nullptr && top != nullptr && tip != nullptr);
			check(size > 0);

			void* result = Align16(top);
			void* newTop = (u8*)result + size;
			check(newTop <= tip);

			top = newTop;

			return result;
		}

		char* AllocateString(u64 size)
		{
			check(allocation != nullptr && top != nullptr && tip != nullptr);
			check(size > 0);

			void* result = top;
			void* newTop = (u8*)result + size;
			check(newTop <= tip);

			top = newTop;

			return (char*)result;
		}

		void Clear() { top = allocation; }
	};
} // namespace Turbo
