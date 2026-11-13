#include "Graphics.h"
#include "Engine.h"
#include "Renderer/Renderer.h"

namespace Turbo
{
	Graphics* Graphics::Create(Engine* engine) { return engine->persistentMemory.Allocate<Graphics>(); }

	void Graphics::Init(Engine* engine)
	{
		GPUDevice* gpu = engine->gpu;

		// TODO(SS): Handle window resize.
		const UVec2 surfaceSize = gpu->GetSurfaceSize();
		depthTexture = gpu->CreateTexture(TextureBuilder::Create2D(
				surfaceSize,
				EPixFormat::D32sfloat,
				ETexFlags::SampledImage | ETexFlags::RenderTarget,
				Name("SceneDepth")));

		sdrSceneColor = gpu->CreateTexture(TextureBuilder::Create2D(
				surfaceSize,
				EPixFormat::RGBA8unorm,
				ETexFlags::SampledImage | ETexFlags::StorageImage | ETexFlags::RenderTarget,
				Name("SDRSceneColor")));
	}

	void Graphics::Shutdown(Engine* engine)
	{
		GPUDevice* gpu = engine->gpu;
		gpu->Destroy(depthTexture);
		gpu->Destroy(sdrSceneColor);
	}

	void Graphics::Render(Engine* engine)
	{
		GPUDevice* gpu = engine->gpu;
		CommandBuffer* commandBuffer = gpu->GetCommandBuffer();
		THandle<Texture> presentTexture = gpu->GetPresentTexture();

		const Color bgColor = (gpu->GetFrameIdx() / 100) % 2 == 0 ? Color::Red() : Color::Blue();

		commandBuffer->TextureBarrier(sdrSceneColor, ETexState::Undefined, ETexState::TransferDst);
		commandBuffer->ClearColorTexture({sdrSceneColor, bgColor});
		commandBuffer->TextureBarrier(sdrSceneColor, ETexState::TransferDst, ETexState::TransferSrc);
		commandBuffer->TextureBarrier(presentTexture, ETexState::Undefined, ETexState::TransferDst);
		commandBuffer->BlitWholeTexture(sdrSceneColor, presentTexture, EFilter::Nearest);
		commandBuffer->TextureBarrier(presentTexture, ETexState::TransferDst, ETexState::Present);
	}
}
