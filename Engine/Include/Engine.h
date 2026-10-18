#pragma once

#include "DataStructures/Arena.h"

namespace Turbo
{
	enum struct EventType : u32
	{
		Sigint,
	};

	struct EventData
	{
	};

	struct Engine
	{
		/* Data */
		Arena mPermanentMemory = {};
		Arena mTransientMemory = {};

		Platform::Context* mPlatformContext = nullptr;

		bool bShouldTick = false;

		/* API */
		void Init(void* permanent, u64 permanentSize, void* transient, u64 transientSize);
		void Shutdown();

		bool Tick();

		void PushEvent(EventType type, const EventData& eventData);
		void OnEvent(EventType type, const EventData& eventData);
	};
} // namespace Turbo
