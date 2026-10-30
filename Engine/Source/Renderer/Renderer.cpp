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
		VkDevice mVkDevice = nullptr;

		u32 mGraphicsQueueFamily = 0;
		VkQueue mGraphicsQueue = nullptr;

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

		MakeStaticArena(stringArena, 1024);
		StringView typeString = FlagsToString(stringArena, type);

		switch (severity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			TURBO_LOG(Warning, "[Vulkan Validation][" SV_FMT "] %s", SV_ARG(typeString), callbackData->pMessage);
			DEBUG_BREAK();
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			TURBO_LOG(Error, "[Vulkan Validation][" SV_FMT "] %s", SV_ARG(typeString), callbackData->pMessage);
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
			TURBO_LOG(Log, "Vulkan instance created.");

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

			TURBO_LOG(Log, "Debug messenger created.");
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

		/* Select queue family */
		{
			u32 numQueueFamilies = 16;
			VkQueueFamilyProperties queueFamilyProperties[numQueueFamilies];
			vkGetPhysicalDeviceQueueFamilyProperties(mCtx->mVkPhysicalDevice, &numQueueFamilies, queueFamilyProperties);

			bool bQueueFamilyFound = false;

			for (u32 i = 0; i < numQueueFamilies; ++i)
			{
				const VkQueueFamilyProperties& queueFamilyProps = queueFamilyProperties[i];
				if (queueFamilyProps.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					mCtx->mGraphicsQueueFamily = i;
					bQueueFamilyFound = true;
					break;
				}
			}
			verify(bQueueFamilyFound);
			verify(GetPhysicalDevicePresentationSupport(mCtx->mVkInstance, mCtx->mVkPhysicalDevice, mCtx->mGraphicsQueueFamily));
		}

		/* Create device */
		{
			LiteralString deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

			const float queuePriorities[] = {1.f};
			const VkDeviceQueueCreateInfo queueCI = {
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = mCtx->mGraphicsQueueFamily,
				.queueCount = arrayLen(queuePriorities),
				.pQueuePriorities = queuePriorities,
			};

			VkPhysicalDeviceFeatures vk10Features = {
				.samplerAnisotropy = true
			};

			VkPhysicalDeviceVulkan12Features vk12Features = {
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
				.descriptorIndexing = true,
				.shaderSampledImageArrayNonUniformIndexing = true,
				.descriptorBindingVariableDescriptorCount = true,
				.runtimeDescriptorArray = true,
				.bufferDeviceAddress = true,
			};

			VkPhysicalDeviceVulkan13Features vk13Features = {
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
				.pNext = &vk12Features,
				.synchronization2 = true,
				.dynamicRendering = true,
			};

			const VkDeviceCreateInfo deviceCI = {
				.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
				.pNext = &vk13Features,
				.queueCreateInfoCount = 1,
				.pQueueCreateInfos = &queueCI,
				.enabledExtensionCount = arrayLen(deviceExtensions),
				.ppEnabledExtensionNames = deviceExtensions,
				.pEnabledFeatures = &vk10Features
			};

			vkCheck(vkCreateDevice(mCtx->mVkPhysicalDevice, &deviceCI, nullptr, &mCtx->mVkDevice));
			volkLoadDevice(mCtx->mVkDevice);

			vkGetDeviceQueue(mCtx->mVkDevice, mCtx->mGraphicsQueueFamily, 0, &mCtx->mGraphicsQueue);

			TURBO_LOG(Log, "Vulkan logical device created.");
		}
	}

	void GPUDevice::Shutdown(Engine* engine)
	{
		vkDestroyDevice(mCtx->mVkDevice, nullptr);

#if WITH_DEBUG_RENDERING_FEATURES
		vkDestroyDebugUtilsMessengerEXT(mCtx->mVkInstance, mCtx->mVkDebugUtilsMessenger, nullptr);
#endif // WITH_DEBUG_RENDERING_FEATURES

		vkDestroyInstance(mCtx->mVkInstance, nullptr);
	}
}
