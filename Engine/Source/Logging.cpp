#include "Logging.h"

void Turbo_LogImpl(LogVerbosity verbosity, const char* fileName, u32 lineNumber, const char* fmt, ...)
{
	// TODO(SS): Implement without malloc, and make it thread-safe (ring-buffer ?)
	char messageBuffer[4096];

	// TODO(SS): Move this to some logging system initialization.
	static u64 firstLogTimeStamp = Turbo::GetRelativeTime();
	const u64 logTimeStamp = Turbo::GetRelativeTime() - firstLogTimeStamp;
	char formattedTime[11] = "";
	snprintf(formattedTime, arrayLen(formattedTime), "%10.4f", (double)logTimeStamp * (double)TE_NANO);

	va_list args;
	va_start(args, fmt);
	vsnprintf(messageBuffer, arrayLen(messageBuffer), fmt, args);
	va_end(args);

	fprintf(
		stdout,
		"%s[%s]%s[%s][%s:%u]: %s\n",
		GetANSIColor(verbosity),
		ToLogString(verbosity),
		ANSIColor::Reset,
		formattedTime,
		fileName,
		lineNumber,
		messageBuffer
	);
}

void Turbo_FlushLog()
{
#if WITH_LOGS
	fflush(stdout);
#endif // WITH_LOGS
}
