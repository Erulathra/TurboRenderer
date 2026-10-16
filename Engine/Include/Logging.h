#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Logging [TODO(SS): Invent better implementation] */
namespace Turbo
{
	namespace ANSIColor
	{
		const char* BoldWhite  = "\x1B[1;37m";
		const char* BoldYellow = "\x1B[1;33m";
		const char* BoldRed    = "\x1B[1;31m";
		const char* Reset      = "\x1B[0m";
	} // namespace ANSIColor

	enum LogVerbosity
	{
		Log,
		Warning,
		Error
	};

	LiteralString ToLogString(LogVerbosity verbosity)
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

	const char* GetANSIColor(LogVerbosity verbosity)
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
}


void Turbo_LogImpl(Turbo::LogVerbosity verbosity, const char* fileName, u32 lineNumber, const char* fmt, ...)
{
	// TODO(SS): Implement without malloc, and make it thread-safe (ring-buffer ?)

	constexpr u32 logBufferSize = 2048;
	char messageBuffer[logBufferSize];

	va_list args;
	va_start(args, fmt);
	snprintf(messageBuffer, logBufferSize, fmt, args);
	va_end(args);

	fprintf(
		stdout,
		"%s[%s]%s[%s:%u]: %s\n",
		Turbo::GetANSIColor(verbosity),
		ToLogString(verbosity),
		Turbo::ANSIColor::Reset,
		fileName,
		lineNumber,
		messageBuffer
	);
}

void Turbo_FlushLog()
{
	fflush(stdout);
}

#define FILENAME_INTERNAL

#define TURBO_LOG(VERBOSITY, FMT, ...)                                                                                 \
	do                                                                                                                 \
	{                                                                                                                  \
		const char* __filename = (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__);                     \
		Turbo_LogImpl(VERBOSITY, __filename, __LINE__, FMT __VA_OPT__(, ) __VA_ARGS__);                                \
	} while (false)

#undef FILENAME_INTERNAL
