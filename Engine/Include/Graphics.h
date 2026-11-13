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
		void Init(Engine* engine);
		void Shutdown(Engine* engine);

		void Render(Engine* engine);
	};
}
