#include "Renderer/Renderer.h"

#include "DataStructures/Stack.h"
#include "VulkanHelpers.h"
#include "ResourcePool.h"

#define VOLK_IMPLEMENTATION
#include "volk.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include "vk_mem_alloc.h"
#pragma clang diagnostic pop

#include "Engine.h"

#define WITH_DEBUG_RENDERING_FEATURES TURBO_DEVELOPMENT

/* Macros */
#define ctx (static_cast<PlatformRenderContext*>(context))
#define vkCheck(EXPRESSION)                                                                                            \
	do                                                                                                                  \
	{                                                                                                                   \
		VkResult _result = (EXPRESSION);                                                                                 \
		verifyf(_result == VK_SUCCESS, "[%s] Vulkan error: %i", #EXPRESSION, _result);                                   \
	} while (false)

namespace Turbo
{
	/* Internal resource data */
	struct IntTexture
	{
		VkImage vkImage = nullptr;
		VkImageView vkImageView = nullptr;
		VmaAllocation vmaAllocation = nullptr;
	};

	/* Data structures */
	struct PlatformRenderContext
	{
		VkInstance vkInstance = nullptr;
		VkPhysicalDevice vkPhysicalDevice = nullptr;
		VkDevice vkDevice = nullptr;

		u32 graphicsQueueFamily = 0;
		VkQueue graphicsQueue = nullptr;

		VkSurfaceKHR vkSurface = nullptr;
		VkSwapchainKHR vkSwapchain = nullptr;

		TStaticStack<THandle<Texture>, Renderer::maxFramesInFlight> swapchainImages;

		VmaAllocator vmaAllocator = {};

#if WITH_DEBUG_RENDERING_FEATURES
		VkDebugUtilsMessengerEXT vkDebugUtilsMessenger = nullptr;
#endif // WITH_DEBUG_RENDERING_FEATURES

		/* Resources */
		TResourcePool<Texture, IntTexture, Renderer::maxTextures> texturePool;
	};

	/* Functions and methods */
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

	void SetResourceName(VkDevice device, VkObjectType type, u64 handle, const char* name)
	{
#if WITH_DEBUG_RENDERING_FEATURES
		const VkDebugUtilsObjectNameInfoEXT nameInfo = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.objectType = type,
			.objectHandle = handle,
			.pObjectName = name,
		};
		vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
#endif // WITH_DEBUG_RENDERING_FEATURES
	}

	GPUDevice* GPUDevice::Create(Engine* engine)
	{
		return engine->persistentMemory.Allocate<GPUDevice>();
	}

	void GPUDevice::Init(Engine* engine)
	{
		context = engine->persistentMemory.Allocate<PlatformRenderContext>();
		ctx->texturePool.Init();

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
				.enabledExtensionCount = instanceExtensions.size,
				.ppEnabledExtensionNames = instanceExtensions.data,
			};

			vkCheck(vkCreateInstance(&instanceCI, nullptr, &ctx->vkInstance));
			volkLoadInstance(ctx->vkInstance);
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
				ctx->vkInstance,
				&debugUtilsMessengerCI,
				nullptr,
				&ctx->vkDebugUtilsMessenger
			));

			TURBO_LOG(Log, "Debug messenger created.");
#endif // WITH_DEBUG_RENDERING_FEATURES
		}

		/* Select best physical device */
		{
			u32 deviceCount = 4;
			VkPhysicalDevice physicalDevices[deviceCount];
			vkCheck(vkEnumeratePhysicalDevices(ctx->vkInstance, &deviceCount, physicalDevices));
			verify(deviceCount > 0);

			ctx->vkPhysicalDevice = physicalDevices[0];

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
				ctx->vkPhysicalDevice = discreteGPU;
			}
			else if (integratedGPU)
			{
				ctx->vkPhysicalDevice = integratedGPU;
			}

			VkPhysicalDeviceProperties2 deviceProperties = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
			vkGetPhysicalDeviceProperties2(ctx->vkPhysicalDevice, &deviceProperties);

			TURBO_LOG(Log, "Selected device: %s", deviceProperties.properties.deviceName);
		}

		/* Select queue family */
		{
			u32 numQueueFamilies = 16;
			VkQueueFamilyProperties queueFamilyProperties[numQueueFamilies];
			vkGetPhysicalDeviceQueueFamilyProperties(ctx->vkPhysicalDevice, &numQueueFamilies, queueFamilyProperties);

			bool queueFamilyFound = false;

			for (u32 i = 0; i < numQueueFamilies; ++i)
			{
				const VkQueueFamilyProperties& queueFamilyProps = queueFamilyProperties[i];
				if (queueFamilyProps.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					ctx->graphicsQueueFamily = i;
					queueFamilyFound = true;
					break;
				}
			}
			verify(queueFamilyFound);
			verify(GetPhysicalDevicePresentationSupport(
				ctx->vkInstance,
				ctx->vkPhysicalDevice,
				ctx->graphicsQueueFamily
			));
		}

		/* Create device */
		{
			LiteralString deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

			const float queuePriorities[] = {1.f};
			const VkDeviceQueueCreateInfo queueCI = {
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = ctx->graphicsQueueFamily,
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

			vkCheck(vkCreateDevice(ctx->vkPhysicalDevice, &deviceCI, nullptr, &ctx->vkDevice));
			volkLoadDevice(ctx->vkDevice);

			vkGetDeviceQueue(ctx->vkDevice, ctx->graphicsQueueFamily, 0, &ctx->graphicsQueue);

			TURBO_LOG(Log, "Vulkan logical device created.");
		}

		/* Obtain Surface */
		{
			vkCheck((VkResult)GetVulkanSurface(engine->platformContext, ctx->vkInstance, ctx->vkSurface));
		}

		/* Init VMA */
		{
			VmaAllocatorCreateInfo createInfo = {
				.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
				.physicalDevice = ctx->vkPhysicalDevice,
				.device = ctx->vkDevice,
				.instance = ctx->vkInstance,
				.vulkanApiVersion = VK_API_VERSION_1_3,
			};

			VmaVulkanFunctions vmaVulkanFunctions = {};
			vmaImportVulkanFunctionsFromVolk(&createInfo, &vmaVulkanFunctions);
			createInfo.pVulkanFunctions = &vmaVulkanFunctions;

			vkCheck(vmaCreateAllocator(&createInfo, &ctx->vmaAllocator));
			TURBO_LOG(Log, "VMA Allocator created.");
		}
	}

	void GPUDevice::Shutdown(Engine* engine)
	{
		WaitGPUIdle();

		DestroySwapchain();

		vkDestroySurfaceKHR(ctx->vkInstance, ctx->vkSurface, nullptr);

		vmaDestroyAllocator(ctx->vmaAllocator);

		vkDestroyDevice(ctx->vkDevice, nullptr);

#if WITH_DEBUG_RENDERING_FEATURES
		vkDestroyDebugUtilsMessengerEXT(ctx->vkInstance, ctx->vkDebugUtilsMessenger, nullptr);
#endif // WITH_DEBUG_RENDERING_FEATURES

		vkDestroyInstance(ctx->vkInstance, nullptr);
	}

	void GPUDevice::WaitGPUIdle() { vkCheck(vkDeviceWaitIdle(ctx->vkDevice)); }

	void GPUDevice::CreateSwapchain(PlatformContext* platformCtx)
	{
		VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
		vkCheck(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			ctx->vkPhysicalDevice,
			ctx->vkSurface,
			&vkSurfaceCapabilities
		));

		/* Handle Wayland shenanigans */
		VkExtent2D swapchainExtent = vkSurfaceCapabilities.currentExtent;
		if (swapchainExtent.width == U32_MAX)
		{
			swapchainExtent = ToVkExtent2D(GetFrameBufferSize(platformCtx));
		}

		// TODO(SS): Replace with proper present mode and swapchain format handling
		const VkFormat swapchainFormat = VK_FORMAT_B8G8R8A8_SRGB;
		const VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

		const VkSwapchainCreateInfoKHR swapchainCI = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = ctx->vkSurface,
			.minImageCount = vkSurfaceCapabilities.minImageCount,
			.imageFormat = swapchainFormat,
			.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
			.imageExtent = swapchainExtent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = presentMode
		};
		vkCheck(vkCreateSwapchainKHR(ctx->vkDevice, &swapchainCI, nullptr, &ctx->vkSwapchain));

		/* Obtain swapchain images */
		u32 imagesCount = Renderer::maxFramesInFlight;
		VkImage swapchainImages[Renderer::maxFramesInFlight];
		vkCheck(vkGetSwapchainImagesKHR(ctx->vkDevice, ctx->vkSwapchain, &imagesCount, swapchainImages));

		for (u32 i = 0; i < imagesCount; ++i)
		{
			/* Create abstraction */
			const THandle<Texture> textureHandle = ctx->texturePool.Acquire();
			check(textureHandle);

			ctx->swapchainImages.Push(textureHandle);

			Texture* texture = ctx->texturePool.Get(textureHandle);
			*texture = {
				.flags = ETexFlags::SwapChain,
				.format = EPixFormat::BGRA8srgb,
				.width = TruncToU16(swapchainExtent.width),
				.height = TruncToU16(swapchainExtent.height),
				.name = Name("SwapchainImage"),
			};

			IntTexture* intTexture = ctx->texturePool.GetInternal(textureHandle);
			*intTexture = {
				.vkImage = swapchainImages[i],
			};
		}

		TURBO_LOG(Log, "Swaphchain created. [%ux%u] (Frames: %u)", swapchainExtent.width, swapchainExtent.height, imagesCount);
	}

	void GPUDevice::DestroySwapchain()
	{
		if (ctx->vkSwapchain)
		{
			vkDestroySwapchainKHR(ctx->vkDevice, ctx->vkSwapchain, nullptr);
			ctx->vkSwapchain = nullptr;
		}

		for (THandle<Texture> handle : ctx->swapchainImages)
		{
			IntTexture* intTexture = ctx->texturePool.GetInternal(handle);
			vkDestroyImageView(ctx->vkDevice, intTexture->vkImageView, nullptr);

			ctx->texturePool.Release(handle);
		}
		ctx->swapchainImages.Empty();
	}

	THandle<Texture> GPUDevice::CreateTexture(const TextureBuilder builder)
	{
		THandle<Texture> handle = ctx->texturePool.Acquire();
		Texture* texture = ctx->texturePool.Get(handle);
		IntTexture* intTexture = ctx->texturePool.GetInternal(handle);
		check(texture && intTexture);

		*texture = {
			.flags = builder.flags,
			.format = builder.format,
			.width = builder.width,
			.height = builder.height,
			.depth = builder.depth,
			.numMips = builder.numMips,
			.name = builder.name,
		};
		*intTexture = {};

		/* Create Image */
		VkImageUsageFlags imageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		imageFlags |= (builder.flags & ETexFlags::SampledImage) != 0 ? VK_IMAGE_USAGE_SAMPLED_BIT : 0;
		imageFlags |= (builder.flags & ETexFlags::StorageImage) != 0 ? VK_IMAGE_USAGE_STORAGE_BIT : 0;

		if ((builder.flags & ETexFlags::RenderTarget) != 0)
		{
			imageFlags |= HasDepthOrStencil(builder.format)
				? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
				: VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}

		VkImageCreateInfo imageCI = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = ToVkImageType(builder.type),
			.format = ToVkFormat(builder.format),
			.extent = {
				.width = builder.width,
				.height = builder.height,
				.depth = builder.depth,
			},
			.mipLevels = builder.numMips,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = imageFlags,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};

		VmaAllocationCreateInfo allocationCI = {
			.usage = VMA_MEMORY_USAGE_AUTO,
		};

		VmaAllocationInfo allocationInfo = {};
		vkCheck(vmaCreateImage(
			ctx->vmaAllocator,
			&imageCI,
			&allocationCI,
			&intTexture->vkImage,
			&intTexture->vmaAllocation,
			&allocationInfo
		));
		SetResourceName(ctx->vkDevice, VK_OBJECT_TYPE_IMAGE, (u64)intTexture->vkImage, texture->name.stringLookUp);

		/* CreateImageView */
		const VkImageViewCreateInfo imageViewCI = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = intTexture->vkImage,
			.viewType = ToVkImageViewType(builder.type),
			.format = ToVkFormat(builder.format),
			.subresourceRange = {
				.aspectMask = HasDepthOrStencil(builder.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT,
				.levelCount = builder.numMips,
				.layerCount = 1
			}
		};
		vkCheck(vkCreateImageView(ctx->vkDevice, &imageViewCI, nullptr, &intTexture->vkImageView));
		SetResourceName(ctx->vkDevice, VK_OBJECT_TYPE_IMAGE_VIEW, (u64)intTexture->vkImageView, texture->name.stringLookUp);

		return handle;
	}

	Texture* GPUDevice::Access(THandle<Texture> handle) { return ctx->texturePool.Get(handle); }

	void GPUDevice::Destroy(THandle<Texture> handle) { unimplemented(); }
} // namespace Turbo
