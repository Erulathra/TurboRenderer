#pragma once

#include "DataStructures/Handle.h"
#include "Renderer/Resources.h"

namespace Turbo
{
	struct Engine;

	namespace Renderer
	{
		constexpr static u32 maxTextures = 1024;
		constexpr static u32 maxBuffers  = 16 * 1024;

		constexpr static u32 maxFramesInFlight = 3;

		constexpr static DeviceSize maxUniformBufferSize = 64 * TE_KIBI;
	}

	struct GPUDevice
	{
		/* Platform specific context. Hidden under abstraction. */
		void* context;

		/* Layer API */
		static GPUDevice* Create(Engine* engine);
		void Init(Engine* engine);
		void Shutdown(Engine* engine);

		/* Common API */
		void WaitGPUIdle();

		/* Swapchain */
		void CreateSwapchain(PlatformContext* platformCtx);
		void DestroySwapchain();
		UVec2 GetSurfaceSize();

		/* Resource management */
		THandle<Texture> CreateTexture(const TextureBuilder builder);
		THandle<Buffer> CreateBuffer(const BufferBuilder builder);

		Texture* Access(THandle<Texture> handle);
		Buffer* Access(THandle<Buffer> handle);

		void Destroy(THandle<Texture> handle);
		void Destroy(THandle<Buffer> handle);
	};
} // namespace Turbo
