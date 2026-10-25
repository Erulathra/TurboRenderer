#pragma once

#include <stdlib.h>

#include "Logging.h"

#define WITH_ASSERTIONS TURBO_DEVELOPMENT

/* Verify is evaluated in all builds */
#define verify(CONDITION)                                                                                              \
	do                                                                                                                  \
	{                                                                                                                   \
		if (!(CONDITION)) [[unlikely]]                                                                                   \
		{                                                                                                                \
			namespace Color = ANSIColor;                                                                                  \
			TURBO_LOG(Error, "Condition: %s[%s]%s failed. Aborting.", Color::BoldWhite, #CONDITION, Color::Reset);        \
			Turbo_FlushLog();                                                                                             \
			abort();                                                                                                      \
		}                                                                                                                \
	} while (false)

#define verifyf(CONDITION, FMT, ...)                                                                                   \
	do                                                                                                                  \
	{                                                                                                                   \
		if (!(CONDITION)) [[unlikely]]                                                                                   \
		{                                                                                                                \
			TURBO_LOG(Error, FMT " Aborting." __VA_OPT__(, ) __VA_ARGS__);                                                \
			Turbo_FlushLog();                                                                                             \
			abort();                                                                                                      \
		}                                                                                                                \
	} while (false)

#if WITH_ASSERTIONS

#define check(CONDITION) verify(CONDITION)
#define checkf(CONDITION, FMT, ...) verifyf(CONDITION, FMT __VA_OPT__(, ) __VA_ARGS__)

#else // WITH_ASSERTIONS

#define check(CONDITION) ((void)(CONDITION))
#define checkf(CONDITION, FMT, ...) ((void)(CONDITION))

#endif // else WITH_ASSERTIONS

#define mathDiagnosticCheck(CONDITION) check(CONDITION)
