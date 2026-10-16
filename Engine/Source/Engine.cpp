#include "Engine.h"

#include <stdio.h>

namespace Turbo
{
	void Engine::Init(void* pernament, void* transient)
	{
		mPernamentMemory = pernament;
		mTransientMemory = transient;

		TURBO_LOG(Log, "Hello World!");
		TURBO_LOG(Log, "Test Log");
		TURBO_LOG(Warning, "Test Warning");
		TURBO_LOG(Error, "Test Error");
	}

	void Engine::Shutdown() { TURBO_LOG(Log, "Goodbye World!"); }

	void Engine::Tick()
	{

	}
}
