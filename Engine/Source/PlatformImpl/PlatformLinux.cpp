#include "DataStructures/Stack.h"
#include "Platform.h"

#include <signal.h>

#include "DataStructures/Arena.h"
#include "Engine.h"

namespace Turbo
{
	struct LinuxPlatformCtx final : Platform::Context
	{
	};

	namespace Platform
	{
		TStaticStack<i32, 128> gSignalsStack;

		void SignalsHandler(i32 signalId)
		{
			// TODO(SS): Make it thread safe
			gSignalsStack.Push(signalId);
		}

		Context* Init(Arena& arena)
		{
			TURBO_LOG(Log, "Initializing Linux Platform.");

			/* Register signals handlers */
			signal(SIGINT, SignalsHandler);

			/* create context */
			Context* ctx = arena.Allocate<LinuxPlatformCtx>();
			return ctx;
		};

		void Shutdown(Context& ctx) { TURBO_LOG(Log, "Linux Platform Shutdown."); }

		void TickPlatform(Context& ctx, Engine& engine)
		{
			// TODO(SS): Make it thread safe

			/* Handle signals fired last frame */
			for (i32 signal : gSignalsStack)
			{
				switch (signal)
				{
				case SIGINT:
					engine.PushEvent(EventType::Sigint, {});
					break;

				default:
					break;
				}
			}
		}
	} // namespace Platform
} // namespace Turbo
