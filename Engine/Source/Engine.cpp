#include "Engine.h"
#include "Math/Vec2.h"

#include <stdio.h>

namespace Turbo
{
	void Engine::Init(void* permanent, u64 permanentSize, void* transient, u64 transientSize)
	{
		TURBO_LOG(Log, "Welcome to Turbo::Engine!");
		TURBO_LOG(Log, "Initializing memory. [P: %uMiB, T: %uMiB]", permanentSize / kMebi, transientSize / kMebi);
		mPermanentMemory.Init(permanent, permanentSize);
		mTransientMemory.Init(transient, transientSize);

		mPlatformContext = Platform::Init(mPermanentMemory);
		check(mPlatformContext);

		bShouldTick = true;
	}

	void Engine::Shutdown()
	{
		TURBO_LOG(Log, "Shutting down Turbo::Engine.");

		Platform::Shutdown(*mPlatformContext);
	}

	bool Engine::Tick()
	{
		Platform::TickPlatform(*mPlatformContext, *this);

		return bShouldTick;
	}

	void Engine::PushEvent(EventType type, const EventData& eventData)
	{
		// NOTE(SS): For now we don't need event queue. I will just fire event ad hoc.
		OnEvent(type, eventData);
	}

	void Engine::OnEvent(EventType type, const EventData& eventData)
	{
		switch (type)
		{
		case EventType::Sigint:
			TURBO_LOG(Warning, "SIGINT signaled. Requesting shutdown.");
			bShouldTick = false;
			break;
		default:
			break;
		}
	}
} // namespace Turbo
