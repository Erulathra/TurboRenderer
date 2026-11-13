#pragma once

#include "volk.h"

#include "Renderer/RendererHelpers.h"

#include "String.h"
#include "DataStructures/Arena.h"

namespace Turbo
{
	/* Math types conversions */
	FORCEINLINE VkExtent2D ToVkExtent2D(const IVec2& extent)
	{
		return {.width = TruncToU32(extent.x), .height = TruncToU32(extent.y)};
	}

	FORCEINLINE VkOffset3D ToVkOffset3D(const IVec3& offset)
	{
		return {.x = offset.x, .y = offset.y, .z = offset.z};
	}

	/* Enum to string */
	LiteralString EnumToString(VkDebugUtilsMessageTypeFlagsEXT messageType);
	StringView FlagsToString(Arena& arena, VkDebugUtilsMessageTypeFlagsEXT messageType);

	/* Enum conversions */
	FORCEINLINE VkFormat ToVkFormat(EPixFormat format)
	{
		switch (format)
		{
		case EPixFormat::Undefined: return VK_FORMAT_UNDEFINED;
		case EPixFormat::RGBA8unorm: return VK_FORMAT_R8G8B8A8_UNORM;
		case EPixFormat::RGBA8srgb: return VK_FORMAT_R8G8B8A8_SRGB;
		case EPixFormat::BGRA8srgb: return VK_FORMAT_B8G8R8A8_SRGB;
		case EPixFormat::D32sfloat: return VK_FORMAT_D32_SFLOAT;
		invalidDefaultCase;
		}

		return VK_FORMAT_UNDEFINED;
	}

	FORCEINLINE VkImageType ToVkImageType(ETexType type)
	{
		switch (type)
		{
		case ETexType::Texture1D: return VK_IMAGE_TYPE_1D;
		case ETexType::Texture2D: return VK_IMAGE_TYPE_2D;
		case ETexType::Texture3D: return VK_IMAGE_TYPE_3D;
		invalidDefaultCase;
		}

		return VK_IMAGE_TYPE_2D;
	}

	FORCEINLINE VkImageViewType ToVkImageViewType(ETexType type)
	{
		switch (type)
		{
		case ETexType::Texture1D: return VK_IMAGE_VIEW_TYPE_1D;
		case ETexType::Texture2D: return VK_IMAGE_VIEW_TYPE_2D;
		case ETexType::Texture3D: return VK_IMAGE_VIEW_TYPE_3D;
		invalidDefaultCase;
		}

		return VK_IMAGE_VIEW_TYPE_2D;
	}

	FORCEINLINE VkFilter ToVkFilter(EFilter filter)
	{
		switch (filter)
		{
		case EFilter::Nearest: return VK_FILTER_NEAREST;
		case EFilter::Linear: return VK_FILTER_LINEAR;
		invalidDefaultCase;
		}

		return VK_FILTER_NEAREST;
	}

	void FindStageAccess(EBufState state, VkPipelineStageFlagBits2& outStage, VkAccessFlagBits2& outAccess);
	void FindStageAccess(ETexState state, EPixFormat format, VkPipelineStageFlagBits2& outStage, VkAccessFlagBits2& outAccess);
	VkImageLayout FindLayout(ETexState state, EPixFormat format);
}
