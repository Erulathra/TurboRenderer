#include "Engine.h"

#include "signal.h"

i32 main(i32 argc, char* argv[])
{
	constexpr u64 kPermanentSize = 128 * Turbo::kMebi;
	constexpr u64 kTransientSize = 2 * Turbo::kGibi;

	// NOTE(SS): We don't need to free this allocation. It would be freed by OS after program shutdown.
	void* permanentMemory = Turbo::AllocateProgramMemory(kPermanentSize);
	void* transientMemory = Turbo::AllocateProgramMemory(kTransientSize);

	Turbo::Engine engine = {};
	engine.Init(permanentMemory, kPermanentSize, transientMemory, kTransientSize);

	while (engine.Tick())
	{
	}

	engine.Shutdown();
}
