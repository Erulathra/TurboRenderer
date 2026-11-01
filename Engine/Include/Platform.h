#pragma once

/* Real platform implementation */
#if PLATFORM_LINUX
#include "PlatformImpl/PlatformLinux.h"
#endif // PLATFORM_LINUX

#include "Math/Vector.h"

struct VkInstance_T;
struct VkPhysicalDevice_T;
struct VkSurfaceKHR_T;

namespace Turbo
{
	// ##################################################################################
	//                                 Platform handling
	// ##################################################################################

	struct Arena;
	struct Engine;

	// NOTE(SS): Used for type safety, not to make this struct abstract.
	struct PlatformContext { };

	/* Returns pointer to allocated context */
	PlatformContext* PlatformInit(Arena& arena);
	void PlatformShutdown(PlatformContext* ctx);
	void PlatformTick(PlatformContext* ctx, Engine* engine);

	/* Window */
	IVec2 GetFrameBufferSize(PlatformContext* ctx);

	/* Vulkan */
	const char** GetRequiredVulkanWindowExtensions(u32& extensionsCount);
	bool GetPhysicalDevicePresentationSupport(VkInstance_T* vkInstance, VkPhysicalDevice_T* vkPhysicalDevice, u32 queueFamilyIndex);
	i32 GetVulkanSurface(PlatformContext* ctx, VkInstance_T* vkInstance, VkSurfaceKHR_T*& outVkSurface);

	// ##################################################################################
	//                                   Common memory
	// ##################################################################################

	template <typename T>
	constexpr T Align4(T value)
	{
		return (T)(((iPtr)value + 3) & ~3);
	}

	template <typename T>
	constexpr T Align16(T value)
	{
		return (T)(((iPtr)value + 15) & ~15);
	}

	// ##################################################################################
	//                                        Time
	// ##################################################################################

	/* Returns time in nanoseconds */
	u64 GetRealTimeClockPrecision();
	/* Returns time in nanoseconds */
	u64 GetRelativeClockPrecision();
	/* Returns time in nanoseconds */
	u64 GetRealTime();
	/* Returns time in nanoseconds */
	u64 GetRelativeTime();
} // namespace Turbo
