#pragma once

#include "DataStructures/Arena.h"
#include "DataStructures/Array.h"

#define SV_FMT "%.*s"
#define SV_ARG(sv) static_cast<i32>((sv).size), (sv).start
#define stringLen(LiteralString) arrayLen(LiteralString)
#define LiteralStringView(LiteralString) StringView{.start = (LiteralString), .size = stringLen(LiteralString) - 1}

namespace Turbo
{
	struct StringView
	{
		const char* start = nullptr;
		u64 size = 0;
	};

	struct Name
	{
		const char* stringLookUp;
		u64 id = 0;

		Name() = delete;
		Name(CString cstr);

		bool operator==(const Name& rhs) const { return id == rhs.id; }
		bool operator!=(const Name& rhs) const { return !(*this == rhs); }
	};

	namespace String
	{
		// ##################################################################################
		//                                      Names
		// ##################################################################################

		void* InitNameDB(Arena& arena);
		void* InitNameDB(void* nameDB);

		// ##################################################################################
		//                                   String View
		// ##################################################################################

		inline StringView MakeStringView(CString string)
		{
			return {
				.start = string,
				.size = strlen(string),
			};
		}

		inline StringView BeginString(Arena& stringArena)
		{
			return {
				.start = (char*)stringArena.top,
				.size = 0,
			};
		}

		inline void AppendString(Arena& stringArena, StringView& dst, StringView src)
		{
			check(dst.start + dst.size == stringArena.top);
			char* top = stringArena.AllocateString(src.size);

			Memcpy(top, src.start, src.size);
			dst.size += src.size;
		}

		inline void AppendChar(Arena& stringArena, StringView& dst, char character)
		{
			check(dst.start + dst.size == stringArena.top);
			char* top = stringArena.AllocateString(1);

			*top = character;
			dst.size += 1;
		}

		inline void ChopRight(StringView& dst, u64 numCharacters)
		{
			check(dst.size >= numCharacters);
			dst.size -= numCharacters;
		}

		inline void ChopLeft(StringView& dst, u64 numCharacters)
		{
			check(dst.size >= numCharacters);
			dst.start += numCharacters;
			dst.size -= numCharacters;
		}

		inline void TrimNul(StringView& dst)
		{
			if (dst.size > 0 && dst.start[dst.size - 1] == 0)
			{
				ChopRight(dst, 1);
			}
		}

		inline bool Compare(const StringView& lhs, const StringView& rhs)
		{
			return lhs.size == rhs.size && (lhs.size == 0 || strncmp(lhs.start, rhs.start, lhs.size) == 0);
		}

		inline ByteView ToByteView(const StringView& sv)
		{
			return {(u8*)sv.start, sv.size};
		}
	}
} // namespace Turbo
