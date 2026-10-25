#pragma once

#include <sys/mman.h>
#include <string.h>

static_assert(PLATFORM_LINUX, "This file supports only Linux");

#define TURBO_ATTRIBUTE_PRINTF(fmtIdx, argsIdx) __attribute__((__format__(__printf__, fmtIdx, argsIdx)))

#define FORCEINLINE __attribute__((always_inline)) inline

namespace Turbo
{

	// ##################################################################################
	//                                      Memory
	// ##################################################################################

	void* AllocateProgramMemory(u64 size);

	FORCEINLINE void MemSet(void* dst, u8 bits, u64 numBytes) { memset(dst, bits, numBytes); }
	FORCEINLINE void MemZero(void* dst, u64 numBytes) { MemSet(dst, 0, numBytes); }
	FORCEINLINE void Memcpy(void* dst, const void* src, u64 numBytes) { memcpy(dst, src, numBytes); }

	// ##################################################################################
	//                                 Bit Intrinsics
	// ##################################################################################

	FORCEINLINE u32 CountTrailingZeros(u32 value) { return __builtin_ctz(value); }
	FORCEINLINE u32 CountLeadingZeros(u32 value) { return __builtin_clz(value); }
	FORCEINLINE u32 CountTrailingOnes(u32 value) { return __builtin_ctz(~value); }
	FORCEINLINE u32 CountLeadingOnes(u32 value) { return __builtin_clz(~value); }
} // namespace Turbo
