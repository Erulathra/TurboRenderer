#pragma once

/* Base typedefs */
#ifndef TURBO_DEVELOPMENT
#define TURBO_DEVELOPMENT 0
#endif // TURBO_DEVELOPMENT

#ifndef TURBO_SHIPPING
#define TURBO_SHIPPING 0
#endif // TURBO_SHIPPING

/* Real platform implementation */
#if PLATFORM_LINUX
#include "PlatformImpl/PlatformLinux.h"
#endif // PLATFORM_LINUX

namespace Turbo
{
	constexpr u64 kGibi = 1 << 30;
	constexpr u64 kMebi = 1 << 20;
	constexpr u64 kKibi = 1 << 10;

	template <typename T>
	constexpr T Align4(T value)
	{
		return (T)(((iPtr)value + 3) & ~3);
	}

	template <typename T>
	constexpr T Align16(T value)
	{
		return (T)(((iPtr)value + 15) & ~15);
	}
} // namespace Turbo
