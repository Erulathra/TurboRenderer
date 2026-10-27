#pragma once

#include "volk.h"

#include "DataStructures/Arena.h"

namespace Turbo
{
	inline LiteralString EnumToString( VkDebugUtilsMessageTypeFlagsEXT messageType)
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
}
