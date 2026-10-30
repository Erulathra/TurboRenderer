#include "Platform.h"

#include <signal.h>

#define VOLK_IMPLEMENTATION
#include "volk.h"
#include "GLFW/glfw3.h"

#include "DataStructures/Arena.h"
#include "DataStructures/Stack.h"
#include "Engine.h"

namespace Turbo
{
	struct LinuxPlatformCtx final : Platform::Context
	{
	};

	void* AllocateProgramMemory(u64 size)
	{
		return mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	}

	namespace Platform
	{
		TStaticStack<i32, 128> gSignalsStack;

		void SignalsHandler(i32 signalId)
		{
			// TODO(SS): Make it thread safe
			gSignalsStack.Push(signalId);
		}

		Context* Init(Arena& arena)
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

			/* create context */
			Context* ctx = arena.Allocate<LinuxPlatformCtx>();
			return ctx;
		};

		void Shutdown(Context& ctx) { TURBO_LOG(Log, "Linux Platform Shutdown."); }

		void TickPlatform(Context& ctx, Engine& engine)
		{
			// TODO(SS): Make it thread safe

			/* Handle signals fired last frame */
			for (i32 signal : gSignalsStack)
			{
				switch (signal)
				{
				case SIGINT:
					engine.PushEvent(EventType::Sigint, {});
					break;

				default:
					break;
				}
			}
		}
	} // namespace Platform

	const char** GetRequiredVulkanWindowExtensions(u32& extensionsCount)
	{
		return glfwGetRequiredInstanceExtensions(&extensionsCount);
	}

	bool GetPhysicalDevicePresentationSupport(VkInstance_T* vkInstance, VkPhysicalDevice_T* vkPhysicalDevice, u32 queueFamilyIndex)
	{
		return glfwGetPhysicalDevicePresentationSupport(vkInstance, vkPhysicalDevice, queueFamilyIndex);
	}

} // namespace Turbo
