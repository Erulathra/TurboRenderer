#include "String.h"

namespace Turbo
{
#if 0
	// NOTE(SS): Implements Open Addressing hash map
	struct NamesDB
	{
		static constexpr u64 NameArenaSize = 32 * TE_MEBI;

		char _mNameArenaData[NameArenaSize];
		Arena mNameArena = Arena::Create(_mNameArenaData, NameArenaSize);

	};

	Name::Name(CString cstr)
	{

	}
#endif
} // namespace Turbo
