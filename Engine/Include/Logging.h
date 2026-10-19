#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define WITH_LOGS 1

#if TURBO_DEVELOPMENT
#define LOG_VERBOSITY (Log)
#elif TURBO_SHIPPING
#define LOG_VERBOSITY (Warning)
#endif // TURBO_SHIPPING

namespace ANSIColor
{
	inline const char* BoldWhite  = "\x1B[1;37m";
	inline const char* BoldYellow = "\x1B[1;33m";
	inline const char* BoldRed    = "\x1B[1;31m";
	inline const char* Reset      = "\x1B[0m";
} // namespace ANSIColor

enum LogVerbosity : u8
{
	Log,
	Warning,
	Error
};

inline LiteralString ToLogString(LogVerbosity verbosity)
{
	switch (verbosity)
	{
	case Log:
		return "  Log  ";
	case Warning:
		return "Warning";
	case Error:
		return " Error ";

	default:
		break;
	}

	return "Invalid";
}

inline const char* GetANSIColor(LogVerbosity verbosity)
{
	switch (verbosity)
	{
	case Log:
		return ANSIColor::BoldWhite;
	case Warning:
		return ANSIColor::BoldYellow;
	case Error:
		return ANSIColor::BoldRed;

	default:
		break;
	}

	return "";

}

TURBO_ATTRIBUTE_PRINTF(4, 5)
inline void Turbo_LogImpl(LogVerbosity verbosity, const char* fileName, u32 lineNumber, const char* fmt, ...)
{
	// TODO(SS): Implement without malloc, and make it thread-safe (ring-buffer ?)
	constexpr u32 logBufferSize = 2048;
	char messageBuffer[logBufferSize];

	va_list args;
	va_start(args, fmt);
	vsnprintf(messageBuffer, logBufferSize, fmt, args);
	va_end(args);

	fprintf(
		stdout,
		"%s[%s]%s[%s:%u]: %s\n",
		GetANSIColor(verbosity),
		ToLogString(verbosity),
		ANSIColor::Reset,
		fileName,
		lineNumber,
		messageBuffer
	);
}

inline void Turbo_FlushLog()
{
	fflush(stdout);
}

#if WITH_LOGS

#define TURBO_LOG(VERBOSITY, FMT, ...)                                                                                 \
	do                                                                                                                  \
	{                                                                                                                   \
		if constexpr (VERBOSITY >= LOG_VERBOSITY)                                                                        \
		{                                                                                                                \
			const char* __filename = (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__);                    \
			Turbo_LogImpl(VERBOSITY, __filename, __LINE__, FMT __VA_OPT__(, ) __VA_ARGS__);                               \
		}                                                                                                                \
	} while (false)

#else

#define TURBO_LOG(VERBOSITY, FMT, ...) do {} while(false)

#endif // TURBO_SHIPPING
