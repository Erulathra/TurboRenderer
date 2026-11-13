#pragma once

#include "DataStructures/Handle.h"
#include "Renderer/RendererHelpers.h"

namespace Turbo
{
	struct Engine;
	struct CommandBuffer;

	namespace Renderer
	{
		constexpr static u32 maxTextures = 1024;
		constexpr static u32 maxBuffers  = 16 * 1024;

		constexpr static u32 maxFramesInFlight = 3;

		constexpr static DeviceSize maxUniformBufferSize = 64 * TE_KIBI;

		constexpr static u64 gpuTimeOut = 1 * TE_INV_NANO;
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

		/* Frame to frame rendering */
		void BeginRenderFrame();
		void EndRenderFrame();

		u64 GetFrameIdx();
		CommandBuffer* GetCommandBuffer(); // Do not cache this cmd.
		THandle<Texture> GetPresentTexture();

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

	struct CommandBuffer
	{
		/* Platform specific context. Hidden under abstraction. */
		GPUDevice* gpu;
		void* context;

		/* API */
		void BufferBarrier(THandle<Buffer> handle, EBufState oldState, EBufState newState);
		void TextureBarrier(THandle<Texture> handle, ETexState oldState, ETexState newState);

		void BlitWholeTexture(THandle<Texture> srcHandle, THandle<Texture> dstHandle, EFilter filter = EFilter::Linear);
		void BlitTexture(const BlitTextureInfo& info);
		void ClearColorTexture(const ClearTextureInfo& info);
	};
} // namespace Turbo
