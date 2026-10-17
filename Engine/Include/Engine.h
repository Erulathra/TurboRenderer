#pragma once

#include "DataStructures/Arena.h"

namespace Turbo
{
	struct Engine
	{
		/* Data */
		Arena mPermanentMemory = {};
		Arena mTransientMemory = {};

		/* API */
		void Init(void* permanent, u64 permanentSize, void* transient, u64 transientSize);
		void Shutdown();

		void Tick();
	};
} // namespace Turbo
