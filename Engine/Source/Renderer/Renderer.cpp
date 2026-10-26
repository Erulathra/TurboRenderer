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
			volkLoadInstance(mCtx->mVkInstance);
		}

		/* Select best physical device */
		{
			u32 deviceCount = 4;
			VkPhysicalDevice physicalDevices[deviceCount];
			vkCheck(vkEnumeratePhysicalDevices(mCtx->mVkInstance, &deviceCount, physicalDevices));
			verify(deviceCount > 0);

			mCtx->mVkPhysicalDevice = physicalDevices[0];

			VkPhysicalDevice discreteGPU = nullptr;
			VkPhysicalDevice integratedGPU = nullptr;

			for (u32 deviceId; deviceId < deviceCount; ++deviceId)
			{
				VkPhysicalDeviceProperties2 deviceProperties = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
				vkGetPhysicalDeviceProperties2(physicalDevices[deviceId], &deviceProperties);

				switch (deviceProperties.properties.deviceType)
				{
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					discreteGPU = physicalDevices[deviceId];
					break;
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					integratedGPU = physicalDevices[deviceId];
					break;
				default:
					break;
				}
			}

			if (discreteGPU)
			{
				mCtx->mVkPhysicalDevice = discreteGPU;
			}
			else if (integratedGPU)
			{
				mCtx->mVkPhysicalDevice = integratedGPU;
			}

			VkPhysicalDeviceProperties2 deviceProperties = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
			vkGetPhysicalDeviceProperties2(mCtx->mVkPhysicalDevice, &deviceProperties);

			TURBO_LOG(Log, "Selected device: %s", deviceProperties.properties.deviceName);
		}
	}

	void GPUDevice::Shutdown(Engine* engine)
	{
		vkDestroyInstance(mCtx->mVkInstance, nullptr);
	}
}
