#pragma once

namespace Turbo
{
	struct Texture;
	struct GPUDevice;

	struct Graphics
	{
		/* Data */
		THandle<Texture> depthTexture;
		THandle<Texture> sdrSceneColor;

		/* Layer API */
		static Graphics* Create(Engine* engine);
		void Init(GPUDevice* gpu, UVec2 frameBufferSize);
		void Shutdown(Engine* engine);

		void Render(Engine* engine);
		void ResizeFrameBuffer(GPUDevice* gpu, UVec2 newSize);
	};
}
