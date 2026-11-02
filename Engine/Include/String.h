#pragma once

#include "DataStructures/Arena.h"
#include "DataStructures/Array.h"

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

	struct Name
	{
		const char* mStringLookUp;
		u64 mId = 0;

		Name() = delete;
		Name(CString cstr);

		bool operator==(const Name& rhs) const { return mId == rhs.mId; }
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

		inline void AppendChar(Arena& stringArena, StringView& dst, char character)
		{
			check(dst.mStart + dst.mSize == stringArena.mTop);
			char* mTop = stringArena.AllocateString(1);

			*mTop = character;
			dst.mSize += 1;
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

		inline void TrimNul(StringView& dst)
		{
			if (dst.mSize > 0 && dst.mStart[dst.mSize - 1] == 0)
			{
				ChopRight(dst, 1);
			}
		}

		inline bool Compare(const StringView& lhs, const StringView& rhs)
		{
			return lhs.mSize == rhs.mSize && (lhs.mSize == 0 || strncmp(lhs.mStart, rhs.mStart, lhs.mSize) == 0);
		}

		inline ByteView ToByteView(const StringView& sv)
		{
			return {(u8*)sv.mStart, sv.mSize};
		}
	}
} // namespace Turbo
