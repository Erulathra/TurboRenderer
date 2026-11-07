#pragma once

#include "String.h"

namespace Turbo
{
	enum class ETexFlags : u8
	{
		None         = 0,
		SampledImage = 1 << 1,
		StorageImage = 1 << 2,
		RenderTarget = 1 << 3,
		SwapChain    = 1 << 4,
	};
	DEFINE_ENUM_OPERATORS(ETexFlags);

	enum class ETexType : u8
	{
		Texture1D,
		Texture2D,
		Texture3D
	};

	enum class EPixFormat : u8
	{
		Undefined = 0,

		/* Color */
		RGBA8unorm,
		RGBA8srgb,
		BGRA8srgb,

		/* Depth-Stencil */
		D32sfloat,

	};

	struct TextureBuilder
	{
		u16 mWidth = 1;
		u16 mHeight = 1;
		u16 mDepth = 1;
		u8 mNumMips = 1;

		ETexFlags mFlags = ETexFlags::None;
		ETexType mType = ETexType::Texture2D;
		EPixFormat mPixelFormat = EPixFormat::BGRA8srgb;

		Name mName;

		inline static TextureBuilder Create2D(UVec2 size, EPixFormat format, ETexFlags flags, Name name)
		{
			return {
				.mWidth = TruncToU16(size.x),
				.mHeight = TruncToU16(size.y),
				.mFlags = flags,
				.mPixelFormat = format,
				.mName = name
			};
		}
	};

	struct Texture
	{
		ETexFlags mFlags = ETexFlags::None;
		EPixFormat mFormat = EPixFormat::RGBA8srgb;

		u16 mWidth = 1;
		u16 mHeight = 1;
		u16 mDepth = 1;
		u8 mNumMips = 1;

		Name mName = Name::None();
	};

}
