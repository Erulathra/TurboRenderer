#include "String.h"

namespace Turbo
{
	// ##################################################################################
	//                                       Names
	// ##################################################################################

	// NOTE(SS): Implements Open Addressing hash map
	struct NameDB
	{
		static constexpr u64 nameArenaSize = 16 * TE_MEBI;
		static constexpr u64 nameSlots = (1<<20); // Must be power of two

		Arena nameArena;
		StringView slots[nameSlots];
	};

	NameDB* gNameDB = nullptr;

	void* String::InitNameDB(Arena& arena)
	{
		TURBO_LOG(Log, "Initializing NameDB.");

		gNameDB = arena.Allocate<NameDB>();
		gNameDB->nameArena.Init(arena.Allocate(NameDB::nameArenaSize), NameDB::nameArenaSize);

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
		checkf(stringView.size > 0, "We don't support empty names.");

		const u64 hash = FNVHash(String::ToByteView(stringView));
		u64 id = hash % NameDB::nameSlots;

		StringView slotWithoutNul = gNameDB->slots[id];
		String::TrimNul(slotWithoutNul);

		u64 probeCount = 0;
		while (slotWithoutNul.size != 0 && String::Compare(stringView, slotWithoutNul) == false)
		{
			id = (id + 1) % NameDB::nameSlots;
			slotWithoutNul = gNameDB->slots[id];
			String::TrimNul(slotWithoutNul);

			++probeCount;
			verifyf(probeCount < 1024, "The name hashmap is too crowded. Try to increase it's size");
		}

		StringView* slot = &gNameDB->slots[id];

		/* New entry */
		if (slot->size == 0)
		{
			slot->size = stringView.size;
			char* newString = gNameDB->nameArena.AllocateString(stringView.size);
			slot->start = newString;
			Memcpy(newString, stringView.start, stringView.size);

			/* We would use that strings in Vulkan so it's safer to add 0 at the end. */
			String::AppendChar(gNameDB->nameArena, *slot, 0);
		}

		return id;
	}

	Name::Name(CString cstr)
	{
		id = FindOrAddName(String::MakeStringView(cstr));
		stringLookUp = gNameDB->slots[id].start;
	}

	Name Name::None()
	{
		checkf(gNameDB, "Woops, you are trying to access NameDB before it's initialization.");
		static Name noneName = Name("None");
		return noneName;
	}
} // namespace Turbo
