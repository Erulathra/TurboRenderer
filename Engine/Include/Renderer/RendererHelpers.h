#pragma once

#include "String.h"

namespace Turbo
{
	// ##################################################################################
	//                                       Enums
	// ##################################################################################

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

	enum struct EBufState : u32
	{
		None                              = 0,

		UniformGraphics                   = 1 << 0,
		UniformCompute                    = 1 << 1,
		UniformTraceRays                  = 1 << 2,

		SRVGraphics                       = 1 << 3,
		SRVCompute                        = 1 << 4,
		SRVTraceRays                      = 1 << 5,

		UAVGraphics                       = 1 << 6,
		UAVCompute                        = 1 << 7,
		UAVTraceRays                      = 1 << 8,

		VertexOrIndex                     = 1 << 9,

		IndirectDraw                      = 1 << 10,
		IndirectCompute                   = 1 << 11,
		IndirectTraceRays                 = 1 << 12,

		TransferSrc                       = 1 << 13,
		TransferDst                       = 1 << 14,

		AccelerationStructureBuild        = 1 << 15,
		AccelerationStructureBuildScratch = 1 << 16,
		ShaderBindingTable                = 1 << 17,

		/* Derived */
		AllUniform = UniformGraphics | UniformCompute | UniformTraceRays,
		AllSRV = SRVGraphics | SRVCompute | SRVTraceRays,
		AllUAV = UAVGraphics | UAVCompute | UAVTraceRays,
		AllIndirect = IndirectDraw | IndirectCompute | IndirectTraceRays,
		AllTransfer = TransferSrc | TransferDst,

		AllGraphics = UniformGraphics | SRVGraphics | UAVGraphics | VertexOrIndex | IndirectDraw,
		AllCompute = UniformCompute | SRVCompute | UAVCompute | IndirectCompute,
		AllTraceRays = UniformTraceRays | SRVTraceRays | UAVTraceRays | IndirectTraceRays,

		AllRayTracing = AllTraceRays | AccelerationStructureBuild | AccelerationStructureBuildScratch,
		AllRead = AllUniform | AllSRV | AllUAV | VertexOrIndex | AllIndirect | TransferSrc | AccelerationStructureBuild | ShaderBindingTable,
		AllWrite = AllUAV | TransferDst | AccelerationStructureBuildScratch,

		AllShaderResource = AllUniform | AllSRV | AllUAV,

		All = AllRead | AllWrite
	};
	DEFINE_ENUM_OPERATORS(EBufState);

	enum struct ETexState : u32
	{
		None = 0,

		SRVGraphics  = 1 << 0,
		SRVCompute   = 1 << 1,
		SRVTraceRays = 1 << 2,

		UAVGraphics  = 1 << 3,
		UAVCompute   = 1 << 4,
		UAVTraceRays = 1 << 5,

		RTVDSVRead   = 1 << 6,
		RTVDSVWrite  = 1 << 7,

		TransferSrc  = 1 << 8,
		TransferDst  = 1 << 9,

		Present      = 1 << 10,

		/* Derived */
		AllSRV = SRVGraphics | SRVCompute | SRVTraceRays,
		AllUAV = UAVGraphics | UAVCompute | UAVTraceRays,
		AllRTVDSV = RTVDSVRead | RTVDSVWrite,

		AllGraphics = SRVGraphics | UAVGraphics | RTVDSVRead | RTVDSVWrite,
		AllCompute = SRVCompute | UAVCompute,
		AllTransfer = TransferSrc | TransferDst,

		AllRead = AllSRV | AllUAV | RTVDSVRead | TransferSrc | Present,
		AllWrite = AllUAV | RTVDSVWrite | TransferDst,
		All = AllRead | AllWrite,
		AllShaderResource = AllSRV | AllUAV,
	};
	DEFINE_ENUM_OPERATORS(ETexState);

	// ##################################################################################
	//                                    Resources
	// ##################################################################################

	/*  */
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

	// ##################################################################################
	//                             Command buffer helpers
	// ##################################################################################

	struct BufferBarrierInfo
	{
		THandle<Buffer> handle = {};
		EBufState oldUsage = EBufState::None;
		EBufState newUsage = EBufState::None;
	};

	struct TextureBarrierInfo
	{
		THandle<Texture> handle = {};
		ETexState oldUsage = ETexState::None;
		ETexState newUsage = ETexState::None;
	};

	// ##################################################################################
	//                                 Helper functions
	// ##################################################################################

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
