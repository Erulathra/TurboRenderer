#pragma once

namespace Turbo
{
	struct Engine;

	struct GPUDevice
	{
		/* Data */
		/* Platform specific context. Hidden under abstraction. */
		void* mContext;

		/* Layer API */
		static GPUDevice* Create(Engine* engine);
		void Init(Engine* engine);
		void Shutdown(Engine* engine);

		/* Common API */
		void WaitGPUIdle();

		/* Swapchain */
		void CreateSwapchain(PlatformContext* platformCtx);
		void DestroySwapchain();
	};
} // namespace Turbo
