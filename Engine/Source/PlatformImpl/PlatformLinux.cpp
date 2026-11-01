#include "Platform.h"

#include <time.h>
#include <signal.h>

#include "volk.h"
#include "GLFW/glfw3.h"

#include "DataStructures/Arena.h"
#include "Engine.h"

#define ToLinuxCtx(CTX) static_cast<LinuxPlatformCtx*>((CTX))

namespace Turbo
{
	struct LinuxPlatformCtx final : public PlatformContext
	{
		GLFWwindow* mGlfwWindow = nullptr;
	};

	void* AllocateProgramMemory(u64 size)
	{
		return mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	}

	u64 GetRealTimeClockPrecision()
	{
		timespec ts;
		if (clock_getres(CLOCK_REALTIME, &ts) == 0)
		{
			return ts.tv_sec * (u64)TE_INV_NANO + ts.tv_nsec;
		}

		checkNoEntry();
		return 0;
	}

	u64 GetRelativeClockPrecision()
	{
		timespec ts;
		if (clock_getres(CLOCK_MONOTONIC, &ts) == 0)
		{
			return ts.tv_sec * (u64)TE_INV_NANO + ts.tv_nsec;
		}

		checkNoEntry();
		return 0;
	}

	u64 GetRealTime()
	{
		timespec ts;
		if (clock_gettime(CLOCK_REALTIME, &ts) == 0)
		{
			return ts.tv_sec * (u64)TE_INV_NANO + ts.tv_nsec;
		}

		checkNoEntry();
		return 0;
	}

	u64 GetRelativeTime()
	{
		timespec ts;
		if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
		{
			return ts.tv_sec * (u64)TE_INV_NANO + ts.tv_nsec;
		}

		checkNoEntry();
		return 0;
	}

	// NOTE(SS): this is not thread safe. It only handles main thread signals interrupts.
	static i32 gSignalsStack[128];
	static sig_atomic_t gSignalsStackTop = 0;

	void SignalsHandler(i32 signalId)
	{
		check(gSignalsStackTop + 1 < arrayLen(gSignalsStack));
		gSignalsStack[gSignalsStackTop] = signalId;
		gSignalsStackTop += 1;
	}

	PlatformContext* PlatformInit(Arena& arena)
	{
		TURBO_LOG(Log, "Initializing Linux Platform.");

		/* Init VOLK */
		TURBO_LOG(Log, "Initializing Volk");
		const VkResult volkInitResult = volkInitialize();
		verifyf(volkInitResult == VK_SUCCESS, "Volk initialization Error: %i", volkInitResult);

		/* Init GLFW */
		TURBO_LOG(Log, "Initializing GLFW");
		const i32 glfwInitResult = glfwInit();
		verifyf(glfwInitResult == GLFW_TRUE, "GLFW Init Error: %i", glfwInitResult);
		verifyf(glfwVulkanSupported() == GLFW_TRUE, "Vulkan not supported.");

		/* Register signals handlers */
		signal(SIGINT, SignalsHandler);

		/* Create context */
		LinuxPlatformCtx* ctx = arena.Allocate<LinuxPlatformCtx>();

		/* Create window */
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		ctx->mGlfwWindow = glfwCreateWindow(1280, 720, "TurboEngine", nullptr, nullptr);
		verify(ctx->mGlfwWindow != nullptr);

		return ctx;
	};

	void PlatformShutdown(PlatformContext* ctx)
	{
		TURBO_LOG(Log, "Linux Platform Shutdown.");
		LinuxPlatformCtx* linuxCtx = ToLinuxCtx(ctx);

		/* Destroy window */
		glfwDestroyWindow(linuxCtx->mGlfwWindow);

		/* GLFW teardown */
		glfwTerminate();
	}

	void PlatformTick(PlatformContext* ctx, Engine* engine)
	{
		// TODO(SS): Handle window events.

		/* Handle signals fired last frame */
		while (gSignalsStackTop > 0)
		{
			--gSignalsStackTop;
			const i32 signal = gSignalsStack[gSignalsStackTop];

			switch (signal)
			{
			case SIGINT:
				engine->PushEvent(EventType::Sigint, {});
				break;

			default:
				break;
			}
		}
	}

	IVec2 GetFrameBufferSize(PlatformContext* ctx)
	{
		LinuxPlatformCtx* linuxCtx = ToLinuxCtx(ctx);

		IVec2 result = {};
		glfwGetFramebufferSize(linuxCtx->mGlfwWindow, &result.x, &result.y);

		return result;
	}

	i32 GetVulkanSurface(PlatformContext* ctx, VkInstance vkInstance, VkSurfaceKHR& outVkSurface)
	{
		LinuxPlatformCtx* linuxCtx = ToLinuxCtx(ctx);
		return glfwCreateWindowSurface(vkInstance, linuxCtx->mGlfwWindow, nullptr, &outVkSurface);
	}

	const char** GetRequiredVulkanWindowExtensions(u32& extensionsCount)
	{
		return glfwGetRequiredInstanceExtensions(&extensionsCount);
	}

	bool GetPhysicalDevicePresentationSupport(VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice, u32 queueFamilyIndex)
	{
		return glfwGetPhysicalDevicePresentationSupport(vkInstance, vkPhysicalDevice, queueFamilyIndex);
	}

} // namespace Turbo
