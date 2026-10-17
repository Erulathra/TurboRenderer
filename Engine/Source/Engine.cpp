#include "Engine.h"

#include <stdio.h>

namespace Turbo
{
	void Engine::Init(void* permanent, u64 permanentSize, void* transient, u64 transientSize)
	{
		TURBO_LOG(Log, "Welcome to Turbo::Engine!");
		TURBO_LOG(Log, "Initializing memory. [P: %uMiB, T: %uMiB]", permanentSize / kMebi, transientSize / kMebi);
		mPermanentMemory.Init(permanent, permanentSize);
		mTransientMemory.Init(transient, transientSize);
	}

	void Engine::Shutdown() { TURBO_LOG(Log, "Shutting down Turbo::Engine."); }

	void Engine::Tick()
	{

	}
}
