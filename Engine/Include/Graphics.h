#pragma once

namespace Turbo
{
	struct Texture;

	struct Graphics
	{
		/* Data */
		THandle<Texture> DepthTexture;
		THandle<Texture> SDRSceneColor;

		/* Layer API */
		static Graphics* Create(Engine* engine);
		void Init(Engine* engine);
		void Shutdown(Engine* engine);
	};
}
