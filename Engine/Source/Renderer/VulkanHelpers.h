#pragma once

#include "volk.h"

#include "String.h"
#include "DataStructures/Arena.h"

namespace Turbo
{
	inline LiteralString EnumToString(VkDebugUtilsMessageTypeFlagsEXT messageType)
	{
		switch (messageType) {
			case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
				return "General";
			case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
				return "Validation";
			case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
				return "Performance";
			case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
				return "Device Address Binding";
			invalidDefaultCase
		}

		return "Invalid";
	}

	inline StringView FlagsToString(Arena& arena, VkDebugUtilsMessageTypeFlagsEXT messageType)
	{
		u32 flags = messageType;
		StringView result = String::BeginString(arena);

		if (messageType != 0)
		{
			while (flags != 0) {
				const u32 trailingZeros = CountTrailingZeros(flags);
				const u32 flagValue = 1u << trailingZeros;

				flags &= ~flagValue;

				LiteralString FlagName = EnumToString((VkDebugUtilsMessageTypeFlagsEXT)flagValue);
				String::AppendString(arena, result, String::MakeStringView(FlagName));
				String::AppendString(arena, result, LiteralStringView("|"));
			}

			String::ChopRight(result, 1);
		}

		return result;
	}
}
