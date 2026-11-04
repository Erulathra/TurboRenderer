#include "Engine.h"

#include "signal.h"

i32 main(i32 argc, char* argv[])
{
	constexpr u64 permanentSize = 128 * TE_MEBI;
	constexpr u64 transientSize = 2 * TE_GIBI;

	// NOTE(SS): We don't need to free this allocation. It would be freed by OS after program shutdown.
	void* permanentMemory = Turbo::AllocateProgramMemory(permanentSize);
	void* transientMemory = Turbo::AllocateProgramMemory(transientSize);

	Turbo::Engine engine = {};
	engine.Init(permanentMemory, permanentSize, transientMemory, transientSize);

	while (engine.Tick())
	{
	}

	engine.Shutdown();
}
