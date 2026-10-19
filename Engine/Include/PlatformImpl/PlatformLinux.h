#pragma once

#include <sys/mman.h>
#include <string.h>

static_assert(PLATFORM_LINUX, "This file supports only Linux");

#define TURBO_ATTRIBUTE_PRINTF(fmtIdx, argsIdx) __attribute__((__format__(__printf__, fmtIdx, argsIdx)))

namespace Turbo
{

	// ##################################################################################
	//                                      Memory
	// ##################################################################################

	inline void* AllocateProgramMemory(u64 size)
	{
		return mmap(
			nullptr,
			size,
			PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS,
			-1, 0
		);
	}

	inline void MemSet(void* dst, u8 bits, u64 numBytes) { memset(dst, bits, numBytes); }
	inline void MemZero(void* dst, u64 numBytes) { MemSet(dst, 0, numBytes); }
	inline void Memcpy(void* dst, const void* src, u64 numBytes) { memcpy(dst, src, numBytes); }

	// ##################################################################################
	//                                 Bit Intrinsics
	// ##################################################################################

	inline u32 CountTrailingZeros(u32 value) { return __builtin_ctz(value); }
	inline u32 CountLeadingZeros(u32 value) { return __builtin_clz(value); }
	inline u32 CountTrailingOnes(u32 value) { return __builtin_ctz(~value); }
	inline u32 CountLeadingOnes(u32 value) { return __builtin_clz(~value); }

} // namespace Turbo
