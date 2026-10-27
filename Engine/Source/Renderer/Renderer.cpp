#include "Renderer/Renderer.h"

#include "VulkanHelpers.h"
#include "DataStructures/Stack.h"
#include "volk.h"

#include "Engine.h"

#define WITH_DEBUG_RENDERING_FEATURES TURBO_DEVELOPMENT

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

#if WITH_DEBUG_RENDERING_FEATURES
		VkDebugUtilsMessengerEXT mVkDebugUtilsMessenger = nullptr;
#endif // WITH_DEBUG_RENDERING_FEATURES
	};

	VkBool32 DebugUtilsMessengerCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData
	)
	{
		// TODO(SS): add type flags to message after implementing string library.

		switch (severity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			TURBO_LOG(Warning, "[Vulkan Validation] %s", callbackData->pMessage);
			DEBUG_BREAK();
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			TURBO_LOG(Error, "[Vulkan Validation] %s", callbackData->pMessage);
			DEBUG_BREAK();
			break;
		invalidDefaultCase
		}

		return VK_FALSE;
	}

	GPUDevice* GPUDevice::Create(Engine* engine)
	{
		return engine->mPersistentMemory.Allocate<GPUDevice>();
	}

	void GPUDevice::Init(Engine* engine)
	{
		mContext = engine->mPersistentMemory.Allocate<PlatformRenderContext>();

		/* Create vulkan instance */
		{
			/* Extensions names */
			TStaticStack<const char*, 32> instanceExtensions;
			u32 numWindowExtensions = 0;
			const char** windowExtensionsNames = GetRequiredVulkanWindowExtensions(numWindowExtensions);
			for (u32 i = 0; i < numWindowExtensions; ++i)
			{
				instanceExtensions.Push(windowExtensionsNames[i]);
			}

#if WITH_DEBUG_RENDERING_FEATURES
			instanceExtensions.Push(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // WITH_DEBUG_RENDERING_FEATURES

			/* Layers names */
			const char* enabledLayers[] = {
#if WITH_DEBUG_RENDERING_FEATURES
				"VK_LAYER_KHRONOS_validation",
#endif // WITH_DEBUG_RENDERING_FEATURES
			};

			const VkApplicationInfo vkAppInfo = {
				.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
				.apiVersion = VK_API_VERSION_1_3,
			};

			const VkInstanceCreateInfo instanceCI = {
				.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.pApplicationInfo = &vkAppInfo,
				.enabledLayerCount = arrayLen(enabledLayers),
				.ppEnabledLayerNames = enabledLayers,
				.enabledExtensionCount = instanceExtensions.mSize,
				.ppEnabledExtensionNames = instanceExtensions.mData,
			};

			vkCheck(vkCreateInstance(&instanceCI, nullptr, &mCtx->mVkInstance));
			volkLoadInstance(mCtx->mVkInstance);

#if WITH_DEBUG_RENDERING_FEATURES
			/* Initialize debug messenger */
			const VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI = {
				.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
				.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
				                 | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
				.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
				             | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
				             | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
				             | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
				.pfnUserCallback = DebugUtilsMessengerCallback,
				.pUserData = nullptr,
			};

			vkCheck(vkCreateDebugUtilsMessengerEXT(
				mCtx->mVkInstance,
				&debugUtilsMessengerCI,
				nullptr,
				&mCtx->mVkDebugUtilsMessenger
			));
#endif // WITH_DEBUG_RENDERING_FEATURES
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

			for (u32 deviceId = 0; deviceId < deviceCount; ++deviceId)
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
#if WITH_DEBUG_RENDERING_FEATURES
		vkDestroyDebugUtilsMessengerEXT(mCtx->mVkInstance, mCtx->mVkDebugUtilsMessenger, nullptr);
#endif // WITH_DEBUG_RENDERING_FEATURES

		vkDestroyInstance(mCtx->mVkInstance, nullptr);
	}
}
