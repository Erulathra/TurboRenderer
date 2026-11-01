#pragma once

static_assert(PLATFORM_LINUX, "This file supports only Linux");

#define TURBO_ATTRIBUTE_PRINTF(fmtIdx, argsIdx) __attribute__((__format__(__printf__, fmtIdx, argsIdx)))
#define FORCEINLINE __attribute__((always_inline)) inline
#define DEBUG_BREAK() do { __builtin_debugtrap(); } while(false)
