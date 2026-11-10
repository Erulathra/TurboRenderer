#pragma once

#include "String.h"

namespace Turbo
{
	/* Enums */
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

	enum class EBufFlags : u8
	{
		None = 0,

		UniformBuffer  = 1 << 0,
		StorageBuffer  = 1 << 1,
		IndexBuffer    = 1 << 2,
		IndirectBuffer = 1 << 3,

		CreateMapped   = 1 << 4
	};
	DEFINE_ENUM_OPERATORS(EBufFlags);

	/* Structures */
	using DeviceSize = u64;
	using BindlessIndex = u64; // NOTE(SS): DeviceAddress on Vulkan

	struct TextureBuilder
	{
		u16 width = 1;
		u16 height = 1;
		u16 depth = 1;
		u8 numMips = 1;

		ETexFlags flags = ETexFlags::None;
		ETexType type = ETexType::Texture2D;
		EPixFormat format = EPixFormat::BGRA8srgb;

		Name name;

		/* Constructors */
		inline static TextureBuilder Create2D(UVec2 size, EPixFormat format, ETexFlags flags, Name name)
		{
			return {
				.width = TruncToU16(size.x),
				.height = TruncToU16(size.y),
				.flags = flags,
				.format = format,
				.name = name
			};
		}
	};

	struct Texture
	{
		ETexFlags flags = ETexFlags::None;
		EPixFormat format = EPixFormat::RGBA8srgb;

		u16 width = 1;
		u16 height = 1;
		u16 depth = 1;
		u8 numMips = 1;

		Name name = Name::None();
	};

	struct BufferBuilder
	{
		EBufFlags flags = EBufFlags::None;
		DeviceSize size = 0;
		Name name = Name::None();
	};

	struct Buffer
	{
		EBufFlags flags = EBufFlags::None;
		DeviceSize size = {};
		BindlessIndex bindlessIndex = {};
		void* mappedAddress = nullptr;

		Name name = Name::None();
	};

	/* Helpers */
	constexpr bool IsColor(EPixFormat format)
	{
		switch (format)
		{
		case EPixFormat::RGBA8unorm:
		case EPixFormat::RGBA8srgb:
		case EPixFormat::BGRA8srgb:
			return true;
		default:
			break;
		}

		return false;
	}

	constexpr bool HasDepth(EPixFormat format)
	{
		switch (format)
		{
		case EPixFormat::D32sfloat:
			return true;
		default:
			break;
		}

		return false;
	}

	constexpr bool HasStencil(EPixFormat format) { return false; }
	constexpr bool HasDepthOrStencil(EPixFormat format) { return HasDepth(format) || HasStencil(format); }
} // namespace Turbo
