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
		DepthTexture = gpu->CreateTexture(TextureBuilder::Create2D(
				surfaceSize,
				EPixFormat::D32sfloat,
				ETexFlags::SampledImage | ETexFlags::RenderTarget,
				Name("SceneDepth")));

		SDRSceneColor = gpu->CreateTexture(TextureBuilder::Create2D(
				surfaceSize,
				EPixFormat::RGBA8unorm,
				ETexFlags::SampledImage | ETexFlags::StorageImage | ETexFlags::RenderTarget,
				Name("SDRSceneColor")));
	}

	void Graphics::Shutdown(Engine* engine)
	{
		GPUDevice* gpu = engine->gpu;
		gpu->Destroy(DepthTexture);
		gpu->Destroy(SDRSceneColor);
	}
}
