#pragma once

#include "volk.h"

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
}
