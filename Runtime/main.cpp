#include "Engine.h"

i32 main(i32 argc, char* argv[])
{
	constexpr u64 kPermamentSize = 128 * Turbo::kMebi;
	constexpr u64 kTransientSize = 2 * Turbo::kGibi;

	// NOTE(SS): We don't need to free this allocation. It would be freed by OS after program shutdown.
	void* permamentMemory = Turbo::AllocateProgramMemory(kPermamentSize);
	void* transientMemory = Turbo::AllocateProgramMemory(kTransientSize);

	Turbo::Engine engine = {};
	engine.Init(permamentMemory, kPermamentSize, transientMemory, kTransientSize);
	engine.Shutdown();
}
