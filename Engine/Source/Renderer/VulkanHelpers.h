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
}
