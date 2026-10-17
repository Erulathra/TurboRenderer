#pragma once

#include <stdlib.h>

#include "Logging.h"

#define WITH_ASSERTIONS TURBO_DEVELOPMENT

#if WITH_ASSERTIONS

#define check(CONDITION)                                                                                               \
	do                                                                                                                  \
	{                                                                                                                   \
		if (!(CONDITION)) [[unlikely]]                                                                                   \
		{                                                                                                                \
			namespace Color = ANSIColor;                                                                           \
			TURBO_LOG(Error, "Condition: %s[%s]%s failed. Aborting.", Color::BoldWhite, #CONDITION, Color::Reset);        \
			Turbo_FlushLog();                                                                                             \
			abort();                                                                                                      \
		}                                                                                                                \
	} while (false)

#else // WITH_ASSERTIONS

#define check(CONDITION) ((void)0)

#endif // else WITH_ASSERTIONS
