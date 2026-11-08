#include "Engine.h"

#include "Graphics.h"
#include "Renderer/Renderer.h"

#include "String.h"

namespace Turbo
{
	void Engine::Init(void* permanent, u64 permanentSize, void* transient, u64 transientSize)
	{
		TURBO_LOG(Log, "Welcome to Turbo::Engine!");
		TURBO_LOG(Log, "Initializing memory. [P: %luMiB, T: %luMiB]", permanentSize / TE_MEBI, transientSize / TE_MEBI);
		persistentMemory.Init(permanent, permanentSize);
		transientMemory.Init(transient, transientSize);

		TURBO_LOG(Log, "Initializing clocks. [RT: %luns, M: %luns]", GetRealTimeClockPrecision(), GetRelativeClockPrecision());
		NameDB = String::InitNameDB(persistentMemory);

		static Name TestName = {"TestName"};
		static Name TestName2 = {"OtherTestName"};
		static Name TestName_TheSame = {"TestName"};

		check(TestName == TestName_TheSame);
		check(TestName != TestName2);

		platformContext = PlatformInit(persistentMemory);
		check(platformContext);

		gpu = GPUDevice::Create(this);
		gpu->Init(this);
		gpu->CreateSwapchain(platformContext);

		graphics = Graphics::Create(this);
		graphics->Init(this);

		shouldTick = true;
	}

	void Engine::Shutdown()
	{
		TURBO_LOG(Log, "Shutting down Turbo::Engine.");

		graphics->Shutdown(this);

		gpu->Shutdown(this);

		PlatformShutdown(platformContext);
	}

	bool Engine::Tick()
	{
		PlatformTick(platformContext, this);

		return shouldTick;
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
			shouldTick = false;
			break;
		default:
			break;
		}
	}
} // namespace Turbo
