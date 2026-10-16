#include "Engine.h"

#include "Platform.h"

i32 main(i32 argc, char* argv[])
{
	void* PermamentMemory = Turbo::Platform::AllocateProgramMemory(128 * Turbo::kMebi);
	void* TransientMemory = Turbo::Platform::AllocateProgramMemory(2 * Turbo::kGibi);

	Turbo::Engine engine = {};
	engine.Init(PermamentMemory, TransientMemory);
	engine.Shutdown();
}
