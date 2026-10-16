#pragma once

namespace Turbo
{
	struct Engine
	{
		/* Data */
		void* mPernamentMemory = nullptr;
		void* mTransientMemory = nullptr;

		/* API */
		void Init(void* permament, void* transient);
		void Shutdown();

		void Tick();
	};
} // namespace Turbo
