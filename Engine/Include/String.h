#pragma once

#include "DataStructures/Arena.h"

#define SV_FMT "%.*s"
#define SV_ARG(sv) static_cast<i32>((sv).mSize), (sv).mStart
#define stringLen(LiteralString) arrayLen(LiteralString)
#define LiteralStringView(LiteralString) StringView{.mStart = (LiteralString), .mSize = stringLen(LiteralString) - 1}

namespace Turbo
{
	struct StringView
	{
		const char* mStart = nullptr;
		u64 mSize = 0;
	};

	namespace String
	{
		inline StringView MakeStringView(CString string)
		{
			return {
				.mStart = string,
				.mSize = strlen(string),
			};
		}

		inline StringView BeginString(Arena& stringArena)
		{
			return {
				.mStart = (char*)stringArena.mTop,
				.mSize = 0,
			};
		}

		inline void AppendString(Arena& stringArena, StringView& dst, StringView src)
		{
			check(dst.mStart + dst.mSize == stringArena.mTop);
			char* mTop = stringArena.AllocateString(src.mSize);

			Memcpy(mTop, src.mStart, src.mSize);
			dst.mSize += src.mSize;
		}

		inline void ChopRight(StringView& dst, u64 numCharacters)
		{
			check(dst.mSize >= numCharacters);
			dst.mSize -= numCharacters;
		}

		inline void ChopLeft(StringView& dst, u64 numCharacters)
		{
			check(dst.mSize >= numCharacters);
			dst.mStart += numCharacters;
			dst.mSize -= numCharacters;
		}
	}
} // namespace Turbo
