#pragma once

/* Base typedefs */
#ifndef TURBO_DEVELOPMENT
#define TURBO_DEVELOPMENT 0
#endif// TURBO_DEVELOPMENT

#ifndef TURBO_SHIPPING
#define TURBO_SHIPPING 0
#endif// TURBO_SHIPPING

/* Common constants */
namespace Turbo
{
	constexpr u64 kKibi = 1 << 10;
	constexpr u64 kMebi = 1 << 20;
	constexpr u64 kGibi = 1 << 30;
}

/* Real platform implementation */
#if PLATFORM_LINUX
#include "PlatformImpl/PlatformLinux.h"
namespace Turbo
{
	namespace Platform = PlatformLinux;
}
#endif // PLATFORM_LINUX
