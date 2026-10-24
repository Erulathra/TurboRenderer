#include "Engine.h"
#include "Math/Quaternion.h"
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

		const Vec3 testPos = {1.f, 0.f, 0.f};
		const Vec3 testPos1 = Quat::Identity().RotateVector(testPos);

		Quat testQuat = Quat::FromAxisAngle(DegToRad(90.f), Vec3{0.f, 0.f, 1.f});
		const Vec3 testPos2 = testQuat.RotateVector(testPos);

		TURBO_LOG(Log, "--------------------");
		TURBO_LOG(Log, "OrgPos: (%f, %f, %f)", testPos.x, testPos.y, testPos.z);
		TURBO_LOG(Log, "Pos1:   (%f, %f, %f)", testPos1.x, testPos1.y, testPos1.z);
		TURBO_LOG(Log, "Pos2:   (%f, %f, %f)", testPos2.x, testPos2.y, testPos2.z);
		TURBO_LOG(Log, "--------------------");


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
