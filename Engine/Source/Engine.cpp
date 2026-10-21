#include "Engine.h"
#include "Math/Vector.h"

namespace Turbo
{
	void Engine::Init(void* permanent, u64 permanentSize, void* transient, u64 transientSize)
	{
		TURBO_LOG(Log, "Welcome to Turbo::Engine!");
		TURBO_LOG(Log, "Initializing memory. [P: %luMiB, T: %luMiB]", permanentSize / kMebi, transientSize / kMebi);
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

		vec2 test = {1.f, 12.f};
		vec2 test2 = {12.f, 2.f};
		vec2 test3 = vec2::One;

		TURBO_LOG(Log, "(%f, %f)", test.x, test.y);
		TURBO_LOG(Log, "(%f, %f)", (test + test2).x, (test + test2).y);
		TURBO_LOG(Log, "(%f, %f)", (test - test2).x, (test - test2).y);
		TURBO_LOG(Log, "%f", Dot(test, test2));
		TURBO_LOG(Log, "%f", Cross(test, test2));
		TURBO_LOG(Log, "(%f, %f)", Normalize(test).x, Normalize(test).y);
		TURBO_LOG(Log, "One: (%f, %f)", test3.x, test3.y);

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
