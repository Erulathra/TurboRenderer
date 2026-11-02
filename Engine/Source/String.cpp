#include "String.h"

namespace Turbo
{
	// ##################################################################################
	//                                       Names
	// ##################################################################################

	// NOTE(SS): Implements Open Addressing hash map
	struct NameDB
	{
		static constexpr u64 kNameArenaSize = 16 * TE_MEBI;
		static constexpr u64 kNameSlots = (1<<20); // Must be power of two

		Arena mNameArena;
		StringView slots[kNameSlots];
	};

	NameDB* gNameDB = nullptr;

	void* String::InitNameDB(Arena& arena)
	{
		TURBO_LOG(Log, "Initializing NameDB.");

		gNameDB = arena.Allocate<NameDB>();
		gNameDB->mNameArena.Init(arena.Allocate(NameDB::kNameArenaSize), NameDB::kNameArenaSize);

		return gNameDB;
	}

	void* String::InitNameDB(void* nameDB)
	{
		gNameDB = (NameDB*)nameDB;
		return gNameDB;
	}

	u64 FindOrAddName(StringView stringView)
	{
		checkf(gNameDB, "Woops, you are trying to access NameDB before it's initialization.");

		String::TrimNul(stringView);
		checkf(stringView.mSize > 0, "We don't support empty names.");

		const u64 hash = FNVHash(String::ToByteView(stringView));
		u64 id = hash % NameDB::kNameSlots;

		StringView slotWithoutNul = gNameDB->slots[id];
		String::TrimNul(slotWithoutNul);

		u64 probeCount = 0;
		while (slotWithoutNul.mSize != 0 && String::Compare(stringView, slotWithoutNul) == false)
		{
			id = (id + 1) % NameDB::kNameSlots;
			slotWithoutNul = gNameDB->slots[id];
			String::TrimNul(slotWithoutNul);

			++probeCount;
			verifyf(probeCount < 1024, "The name hashmap is too crowded. Try to increase it's size");
		}

		StringView* slot = &gNameDB->slots[id];

		/* New entry */
		if (slot->mSize == 0)
		{
			slot->mSize = stringView.mSize;
			char* newString = gNameDB->mNameArena.AllocateString(stringView.mSize);
			slot->mStart = newString;
			Memcpy(newString, stringView.mStart, stringView.mSize);

			/* We would use that strings in Vulkan so it's safer to add 0 at the end. */
			String::AppendChar(gNameDB->mNameArena, *slot, 0);
		}

		return id;
	}

	Name::Name(CString cstr)
	{
		mId = FindOrAddName(String::MakeStringView(cstr));
		mStringLookUp = gNameDB->slots[mId].mStart;
	}
} // namespace Turbo
