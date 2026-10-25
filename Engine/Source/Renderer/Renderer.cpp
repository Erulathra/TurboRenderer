#include "Renderer/Renderer.h"

#include "volk.h"

#include "Engine.h"

/* Macros */
#define mCtx (static_cast<PlatformRenderContext*>(mContext))
#define vkCheck(EXPRESSION)                                                                                            \
	do                                                                                                                  \
	{                                                                                                                   \
		VkResult _result = (EXPRESSION);                                                                                 \
		verifyf(_result == VK_SUCCESS, "[%s] Vulkan error: %i", #EXPRESSION, _result);                                   \
	} while (false)

namespace Turbo
{
	struct PlatformRenderContext
	{
		VkInstance mVkInstance = nullptr;
		VkPhysicalDevice mVkPhysicalDevice = nullptr;
		VkDevice mVkDevice = nullptr;
	};

	GPUDevice* GPUDevice::Create(Engine* engine) { return engine->mPersistentMemory.Allocate<GPUDevice>(); }

	void GPUDevice::Init(Engine* engine)
	{
		mContext = engine->mPersistentMemory.Allocate<PlatformRenderContext>();

		/* Create vulkan instance */
		{
			u32 numInstanceExtensions = 0;
			const char** instanceExtensionsNames = GetRequiredVulkanWindowExtensions(numInstanceExtensions);

			const VkApplicationInfo vkAppInfo = {
				.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
				.apiVersion = VK_API_VERSION_1_3,
			};

			const VkInstanceCreateInfo instanceCI = {
				.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.pApplicationInfo = &vkAppInfo,
				.enabledExtensionCount = numInstanceExtensions,
				.ppEnabledExtensionNames = instanceExtensionsNames
			};

			vkCheck(vkCreateInstance(&instanceCI, nullptr, &mCtx->mVkInstance));
		}
	}

	void GPUDevice::Shutdown(Engine* engine) {}
}
