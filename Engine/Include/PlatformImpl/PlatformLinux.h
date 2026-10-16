#pragma once

#include <sys/mman.h>

static_assert(PLATFORM_LINUX, "This file supports only Linux");

namespace Turbo::PlatformLinux
{
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
} // namespace Turbo
