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
#define gpuCtx (static_cast<DeviceContext*>(context))
#define GetGpuCtx(CONTEXT) (static_cast<DeviceContext*>(CONTEXT))
#define cmdCtx (static_cast<CommandBufferContext*>(context))
#define GetCmdCtx(CONTEXT) (static_cast<CommandBufferContext*>(CONTEXT))

#define vkCheck(EXPRESSION)                                                                                            \
	do                                                                                                                  \
	{                                                                                                                   \
		VkResult _result = (EXPRESSION);                                                                                 \
		verifyf(_result == VK_SUCCESS, "[%s] Vulkan error: %i", #EXPRESSION, _result);                                   \
	} while (false)

namespace Turbo
{
	// ##################################################################################
	//                              Internal Resource Data
	// ##################################################################################

	struct IntTexture
	{
		VkImage vkImage = nullptr;
		VkImageView vkImageView = nullptr;
		VmaAllocation vmaAllocation = nullptr;
	};

	struct IntBuffer
	{
		VkBuffer vkBuffer = nullptr;
		VmaAllocation vmaAllocation = nullptr;
	};

	// ##################################################################################
	//                             Internal Data structures
	// ##################################################################################

	struct DeviceContext
	{
		VkInstance vkInstance = nullptr;
		VkPhysicalDevice vkPhysicalDevice = nullptr;
		VkDevice vkDevice = nullptr;

		u32 graphicsQueueFamily = 0;
		VkQueue vkGraphicsQueue = nullptr;

		VkSurfaceKHR vkSurface = nullptr;
		VkSwapchainKHR vkSwapchain = nullptr;

		TStaticStack<THandle<Texture>, Renderer::maxFramesInFlight> swapchainImages;

		VmaAllocator vmaAllocator = {};

#if WITH_DEBUG_RENDERING_FEATURES
		VkDebugUtilsMessengerEXT vkDebugUtilsMessenger = nullptr;
#endif // WITH_DEBUG_RENDERING_FEATURES

		/* Frame in flight */
		u64 frameIdx = 0;
		u64 frameInFlightIdx = 0;
		u32 swapChainImageIdx = 0;

		VkCommandPool vkCommandPool = nullptr;
		CommandBuffer commandBuffers[Renderer::maxFramesInFlight];
		VkFence vkFences[Renderer::maxFramesInFlight];
		VkSemaphore vkImageAcquireSemaphores[Renderer::maxFramesInFlight];
		VkSemaphore vkRenderCompleteSemaphores[Renderer::maxFramesInFlight];

		/* Resources */
		TResourcePool<Texture, IntTexture, Renderer::maxTextures> texturePool;
		TResourcePool<Buffer, IntBuffer, Renderer::maxBuffers> bufferPool;
	};

	struct CommandBufferContext
	{
		VkCommandBuffer vkCmd = nullptr;
	};

	// ##################################################################################
	//                                 Helper Functions
	// ##################################################################################

	VkBool32 DebugUtilsMessengerCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData
	)
	{
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

	CommandBuffer CreateCommandBuffer(Arena& arena, GPUDevice* gpu)
	{
		CommandBuffer result = {};
		result.gpu = gpu;
		result.context = arena.Allocate<CommandBufferContext>();

		DeviceContext* gpuContext = GetGpuCtx(gpu->context);
		CommandBufferContext* cmdContext = GetCmdCtx(result.context);

		const VkCommandBufferAllocateInfo commandBufferAI = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = gpuContext->vkCommandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};

		vkCheck(vkAllocateCommandBuffers(gpuContext->vkDevice, &commandBufferAI, &cmdContext->vkCmd));

		return result;
	}

	void BeginCommandBuffer(CommandBuffer& commandBuffer)
	{
		CommandBufferContext* cmdContext = GetCmdCtx(commandBuffer.context);
		vkCheck(vkResetCommandBuffer(cmdContext->vkCmd, 0));

		const VkCommandBufferBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		};
		vkCheck(vkBeginCommandBuffer(cmdContext->vkCmd, &beginInfo));
	}

	void EndCommandBuffer(CommandBuffer& commandBuffer)
	{
		CommandBufferContext* cmdContext = GetCmdCtx(commandBuffer.context);
		vkCheck(vkEndCommandBuffer(cmdContext->vkCmd));
	}

	// ##################################################################################
	//                                    GPUDevice
	// ##################################################################################

	GPUDevice* GPUDevice::Create(Engine* engine)
	{
		return engine->persistentMemory.Allocate<GPUDevice>();
	}

	void GPUDevice::Init(Engine* engine)
	{
		context = engine->persistentMemory.Allocate<DeviceContext>();
		gpuCtx->texturePool.Init();

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

			vkCheck(vkCreateInstance(&instanceCI, nullptr, &gpuCtx->vkInstance));
			volkLoadInstance(gpuCtx->vkInstance);
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
				gpuCtx->vkInstance,
				&debugUtilsMessengerCI,
				nullptr,
				&gpuCtx->vkDebugUtilsMessenger
			));

			TURBO_LOG(Log, "Debug messenger created.");
#endif // WITH_DEBUG_RENDERING_FEATURES
		}

		/* Select best physical device */
		{
			u32 deviceCount = 4;
			VkPhysicalDevice physicalDevices[deviceCount];
			vkCheck(vkEnumeratePhysicalDevices(gpuCtx->vkInstance, &deviceCount, physicalDevices));
			verify(deviceCount > 0);

			gpuCtx->vkPhysicalDevice = physicalDevices[0];

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
				gpuCtx->vkPhysicalDevice = discreteGPU;
			}
			else if (integratedGPU)
			{
				gpuCtx->vkPhysicalDevice = integratedGPU;
			}

			VkPhysicalDeviceProperties2 deviceProperties = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
			vkGetPhysicalDeviceProperties2(gpuCtx->vkPhysicalDevice, &deviceProperties);

			TURBO_LOG(Log, "Selected device: %s", deviceProperties.properties.deviceName);
		}

		/* Select queue family */
		{
			u32 numQueueFamilies = 16;
			VkQueueFamilyProperties queueFamilyProperties[numQueueFamilies];
			vkGetPhysicalDeviceQueueFamilyProperties(gpuCtx->vkPhysicalDevice, &numQueueFamilies, queueFamilyProperties);

			bool queueFamilyFound = false;

			for (u32 i = 0; i < numQueueFamilies; ++i)
			{
				const VkQueueFamilyProperties& queueFamilyProps = queueFamilyProperties[i];
				if (queueFamilyProps.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					gpuCtx->graphicsQueueFamily = i;
					queueFamilyFound = true;
					break;
				}
			}
			verify(queueFamilyFound);
			verify(GetPhysicalDevicePresentationSupport(
				gpuCtx->vkInstance,
				gpuCtx->vkPhysicalDevice,
				gpuCtx->graphicsQueueFamily
			));
		}

		/* Create device */
		{
			LiteralString deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

			const float queuePriorities[] = {1.f};
			const VkDeviceQueueCreateInfo queueCI = {
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = gpuCtx->graphicsQueueFamily,
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

			vkCheck(vkCreateDevice(gpuCtx->vkPhysicalDevice, &deviceCI, nullptr, &gpuCtx->vkDevice));
			volkLoadDevice(gpuCtx->vkDevice);

			vkGetDeviceQueue(gpuCtx->vkDevice, gpuCtx->graphicsQueueFamily, 0, &gpuCtx->vkGraphicsQueue);

			TURBO_LOG(Log, "Vulkan logical device created.");
		}

		/* Obtain Surface */
		{
			vkCheck((VkResult)GetVulkanSurface(engine->platformContext, gpuCtx->vkInstance, gpuCtx->vkSurface));
		}

		/* Init VMA */
		{
			VmaAllocatorCreateInfo createInfo = {
				.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
				.physicalDevice = gpuCtx->vkPhysicalDevice,
				.device = gpuCtx->vkDevice,
				.instance = gpuCtx->vkInstance,
				.vulkanApiVersion = VK_API_VERSION_1_3,
			};

			VmaVulkanFunctions vmaVulkanFunctions = {};
			vmaImportVulkanFunctionsFromVolk(&createInfo, &vmaVulkanFunctions);
			createInfo.pVulkanFunctions = &vmaVulkanFunctions;

			vkCheck(vmaCreateAllocator(&createInfo, &gpuCtx->vmaAllocator));
			TURBO_LOG(Log, "VMA Allocator created.");
		}

		/* Init per frame in flight data */
		{
			const VkCommandPoolCreateInfo commandPoolCI = {
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			};

			vkCheck(vkCreateCommandPool(gpuCtx->vkDevice, &commandPoolCI, nullptr, &gpuCtx->vkCommandPool));

			const VkFenceCreateInfo fenceCI = {
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.flags = VK_FENCE_CREATE_SIGNALED_BIT,
			};

			const VkSemaphoreCreateInfo semaphoreCI = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			};

			for (u32 i = 0; i < Renderer::maxFramesInFlight; ++i)
			{
				vkCheck(vkCreateFence(gpuCtx->vkDevice, &fenceCI, nullptr, &gpuCtx->vkFences[i]));
				vkCheck(vkCreateSemaphore(gpuCtx->vkDevice, &semaphoreCI, nullptr, &gpuCtx->vkImageAcquireSemaphores[i]));
				vkCheck(vkCreateSemaphore(gpuCtx->vkDevice, &semaphoreCI, nullptr, &gpuCtx->vkRenderCompleteSemaphores[i]));

				gpuCtx->commandBuffers[i] = CreateCommandBuffer(engine->persistentMemory, this);
			}
		}
	}

	void GPUDevice::Shutdown(Engine* engine)
	{
		WaitGPUIdle();

		/* Destroy per frame data */
		for (u32 i = 0; i < Renderer::maxFramesInFlight; ++i)
		{
			vkDestroyFence(gpuCtx->vkDevice, gpuCtx->vkFences[i], nullptr);
			vkDestroySemaphore(gpuCtx->vkDevice, gpuCtx->vkImageAcquireSemaphores[i], nullptr);
			vkDestroySemaphore(gpuCtx->vkDevice, gpuCtx->vkRenderCompleteSemaphores[i], nullptr);
		}
		vkDestroyCommandPool(gpuCtx->vkDevice, gpuCtx->vkCommandPool, nullptr);

		DestroySwapchain();

		vkDestroySurfaceKHR(gpuCtx->vkInstance, gpuCtx->vkSurface, nullptr);

		vmaDestroyAllocator(gpuCtx->vmaAllocator);

		vkDestroyDevice(gpuCtx->vkDevice, nullptr);

#if WITH_DEBUG_RENDERING_FEATURES
		vkDestroyDebugUtilsMessengerEXT(gpuCtx->vkInstance, gpuCtx->vkDebugUtilsMessenger, nullptr);
#endif // WITH_DEBUG_RENDERING_FEATURES

		vkDestroyInstance(gpuCtx->vkInstance, nullptr);
	}

	void GPUDevice::WaitGPUIdle() { vkCheck(vkDeviceWaitIdle(gpuCtx->vkDevice)); }

	void GPUDevice::BeginRenderFrame()
	{
		/* Wait for and reset gpu fence */
		vkCheck(vkWaitForFences(
			gpuCtx->vkDevice,
			1, &gpuCtx->vkFences[gpuCtx->frameInFlightIdx],
			VK_TRUE,
			Renderer::gpuTimeOut
		));
		vkCheck(vkResetFences(gpuCtx->vkDevice, 1, &gpuCtx->vkFences[gpuCtx->frameInFlightIdx]));

		/* Acquire swapchain image */
		// TODO(SS): Handle VK_ERROR_OUT_OF_DATE_KHR
		vkCheck(vkAcquireNextImageKHR(
			gpuCtx->vkDevice,
			gpuCtx->vkSwapchain,
			Renderer::gpuTimeOut,
			gpuCtx->vkImageAcquireSemaphores[gpuCtx->frameInFlightIdx],
			VK_NULL_HANDLE,
			&gpuCtx->swapChainImageIdx
		));

		BeginCommandBuffer(gpuCtx->commandBuffers[gpuCtx->frameInFlightIdx]);
	}

	void GPUDevice::EndRenderFrame()
	{
		EndCommandBuffer(gpuCtx->commandBuffers[gpuCtx->frameInFlightIdx]);

		/* Submit frame's command buffer */
		const VkSemaphoreSubmitInfo waitSemaphoreInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = gpuCtx->vkImageAcquireSemaphores[gpuCtx->frameInFlightIdx],
			.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		};

		CommandBufferContext* cmdContext = GetCmdCtx(gpuCtx->commandBuffers[gpuCtx->frameInFlightIdx].context);
		const VkCommandBufferSubmitInfo cmdSubmitInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
			.commandBuffer = cmdContext->vkCmd,
		};

		const VkSemaphoreSubmitInfo signalSemaphoreInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = gpuCtx->vkRenderCompleteSemaphores[gpuCtx->swapChainImageIdx],
			.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		};

		const VkSubmitInfo2 submitInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
			.waitSemaphoreInfoCount = 1,
			.pWaitSemaphoreInfos = &waitSemaphoreInfo,
			.commandBufferInfoCount = 1,
			.pCommandBufferInfos = &cmdSubmitInfo,
			.signalSemaphoreInfoCount = 1,
			.pSignalSemaphoreInfos = &signalSemaphoreInfo,
		};
		vkCheck(vkQueueSubmit2(gpuCtx->vkGraphicsQueue, 1, &submitInfo, gpuCtx->vkFences[gpuCtx->frameInFlightIdx]));

		/* Present Image */
		const VkPresentInfoKHR presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &gpuCtx->vkRenderCompleteSemaphores[gpuCtx->swapChainImageIdx],
			.swapchainCount = 1,
			.pSwapchains = &gpuCtx->vkSwapchain,
			.pImageIndices = &gpuCtx->swapChainImageIdx,
		};
		vkCheck(vkQueuePresentKHR(gpuCtx->vkGraphicsQueue, &presentInfo));

		/* Bump frame idx */
		++gpuCtx->frameIdx;
		gpuCtx->frameInFlightIdx = gpuCtx->frameIdx % Renderer::maxFramesInFlight;
	}

	void GPUDevice::CreateSwapchain(PlatformContext* platformCtx)
	{
		VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
		vkCheck(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			gpuCtx->vkPhysicalDevice,
			gpuCtx->vkSurface,
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
			.surface = gpuCtx->vkSurface,
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
		vkCheck(vkCreateSwapchainKHR(gpuCtx->vkDevice, &swapchainCI, nullptr, &gpuCtx->vkSwapchain));

		/* Obtain swapchain images */
		u32 imagesCount = Renderer::maxFramesInFlight;
		VkImage swapchainImages[Renderer::maxFramesInFlight];
		vkCheck(vkGetSwapchainImagesKHR(gpuCtx->vkDevice, gpuCtx->vkSwapchain, &imagesCount, swapchainImages));

		for (u32 i = 0; i < imagesCount; ++i)
		{
			/* Create abstraction */
			const THandle<Texture> textureHandle = gpuCtx->texturePool.Acquire();
			check(textureHandle);

			gpuCtx->swapchainImages.Push(textureHandle);

			Texture* texture = gpuCtx->texturePool.Get(textureHandle);
			*texture = {
				.flags = ETexFlags::SwapChain,
				.format = EPixFormat::BGRA8srgb,
				.width = TruncToU16(swapchainExtent.width),
				.height = TruncToU16(swapchainExtent.height),
				.name = Name("SwapchainImage"),
			};

			IntTexture* intTexture = gpuCtx->texturePool.GetInternal(textureHandle);
			*intTexture = {
				.vkImage = swapchainImages[i],
			};
		}

		TURBO_LOG(Log, "Swaphchain created. [%ux%u] (Frames: %u)", swapchainExtent.width, swapchainExtent.height, imagesCount);
	}

	void GPUDevice::DestroySwapchain()
	{
		if (gpuCtx->vkSwapchain)
		{
			vkDestroySwapchainKHR(gpuCtx->vkDevice, gpuCtx->vkSwapchain, nullptr);
			gpuCtx->vkSwapchain = nullptr;
		}

		for (THandle<Texture> handle : gpuCtx->swapchainImages)
		{
			IntTexture* intTexture = gpuCtx->texturePool.GetInternal(handle);
			vkDestroyImageView(gpuCtx->vkDevice, intTexture->vkImageView, nullptr);

			gpuCtx->texturePool.Release(handle);
		}
		gpuCtx->swapchainImages.Empty();
	}

	UVec2 GPUDevice::GetSurfaceSize()
	{
		check(gpuCtx->swapchainImages.size > 0);
		Texture* texture = Access(gpuCtx->swapchainImages.First());

		return {texture->width, texture->height};
	}

	THandle<Texture> GPUDevice::CreateTexture(const TextureBuilder builder)
	{
		THandle<Texture> handle = gpuCtx->texturePool.Acquire();
		Texture* texture = gpuCtx->texturePool.Get(handle);
		IntTexture* intTexture = gpuCtx->texturePool.GetInternal(handle);
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
			gpuCtx->vmaAllocator,
			&imageCI,
			&allocationCI,
			&intTexture->vkImage,
			&intTexture->vmaAllocation,
			&allocationInfo
		));
		SetResourceName(gpuCtx->vkDevice, VK_OBJECT_TYPE_IMAGE, (u64)intTexture->vkImage, texture->name.stringLookUp);

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
		vkCheck(vkCreateImageView(gpuCtx->vkDevice, &imageViewCI, nullptr, &intTexture->vkImageView));
		SetResourceName(gpuCtx->vkDevice, VK_OBJECT_TYPE_IMAGE_VIEW, (u64)intTexture->vkImageView, texture->name.stringLookUp);

		return handle;
	}

	THandle<Buffer> GPUDevice::CreateBuffer(const BufferBuilder builder)
	{
		THandle<Buffer> handle = gpuCtx->bufferPool.Acquire();
		Buffer* buffer = gpuCtx->bufferPool.Get(handle);
		IntBuffer* intBuffer = gpuCtx->bufferPool.GetInternal(handle);
		check(buffer && intBuffer);

		*buffer = {
			.flags = builder.flags,
			.size = builder.size,
			.name = builder.name,
		};
		*intBuffer = {};

		VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
		                          | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		usage |= (builder.flags & EBufFlags::UniformBuffer)  != 0 ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT  : 0;
		usage |= (builder.flags & EBufFlags::StorageBuffer)  != 0 ? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT  : 0;
		usage |= (builder.flags & EBufFlags::IndexBuffer)    != 0 ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT    : 0;
		usage |= (builder.flags & EBufFlags::IndirectBuffer) != 0 ? VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT : 0;

		VkBufferCreateInfo bufferCI = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = builder.size,
			.usage = usage,
		};

		VmaAllocationCreateInfo allocationCI = {
			.usage = VMA_MEMORY_USAGE_AUTO,
		};
		allocationCI.flags |=
			(builder.flags & EBufFlags::CreateMapped) != 0
			? (VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT)
			: 0;

		VmaAllocationInfo allocationInfo = {};
		vkCheck(vmaCreateBuffer(
			gpuCtx->vmaAllocator,
			&bufferCI,
			&allocationCI,
			&intBuffer->vkBuffer,
			&intBuffer->vmaAllocation,
			&allocationInfo
		));
		SetResourceName(gpuCtx->vkDevice, VK_OBJECT_TYPE_BUFFER, (u64)intBuffer->vkBuffer, builder.name.stringLookUp);

		buffer->mappedAddress = allocationInfo.pMappedData;

		const VkBufferDeviceAddressInfo deviceAddressInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
			.buffer = intBuffer->vkBuffer,
		};
		buffer->bindlessIndex = vkGetBufferDeviceAddress(gpuCtx->vkDevice, &deviceAddressInfo);

		return handle;
	}

	Texture* GPUDevice::Access(THandle<Texture> handle) { return gpuCtx->texturePool.Get(handle); }
	Buffer* GPUDevice::Access(THandle<Buffer> handle) { return gpuCtx->bufferPool.Get(handle); }

	void GPUDevice::Destroy(THandle<Texture> handle) { unimplemented(); }
	void GPUDevice::Destroy(THandle<Buffer> handle) { unimplemented(); }

	// ##################################################################################
	//                                Command buffer
	// ##################################################################################

	void CommandBuffer::BufferBarrier(const BufferBarrierInfo& barrier)
	{
		VkPipelineStageFlags2 srcStageMask = {};
		VkAccessFlags2 srcAccessMask = {};
		VkPipelineStageFlags2 dstStageMask = {};
		VkAccessFlags2 dstAccessMask = {};

		FindStageAccess(barrier.oldUsage, srcStageMask, srcAccessMask);
		FindStageAccess(barrier.newUsage, dstStageMask, dstAccessMask);

		const VkMemoryBarrier2 memoryBarrier = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
			.srcStageMask = srcStageMask,
			.srcAccessMask = srcAccessMask,
			.dstStageMask = dstStageMask,
			.dstAccessMask = dstAccessMask,
		};

		const VkDependencyInfo dependencyInfo = {
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.memoryBarrierCount = 1,
			.pMemoryBarriers = &memoryBarrier,
		};

		vkCmdPipelineBarrier2(cmdCtx->vkCmd, &dependencyInfo);
	}

	void CommandBuffer::TextureBarrier(const TextureBarrierInfo& barrier)
	{
		VkPipelineStageFlags2 srcStageMask = {};
		VkAccessFlags2 srcAccessMask = {};
		VkPipelineStageFlags2 dstStageMask = {};
		VkAccessFlags2 dstAccessMask = {};

		const DeviceContext* gpuContext = GetGpuCtx(gpu->context);
		const Texture* texture = gpuContext->texturePool.Get(barrier.handle);
		const IntTexture* intTexture = gpuContext->texturePool.GetInternal(barrier.handle);

		FindStageAccess(barrier.oldUsage, texture->format, srcStageMask, srcAccessMask);
		FindStageAccess(barrier.newUsage, texture->format, dstStageMask, dstAccessMask);
		VkImageLayout srcLayout = FindLayout(barrier.oldUsage, texture->format);
		VkImageLayout dstLayout = FindLayout(barrier.newUsage, texture->format);

		const bool bDepthStencil = HasDepthOrStencil(texture->format);
		VkImageAspectFlags imageAspect
			= bDepthStencil ? (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT) : VK_IMAGE_ASPECT_COLOR_BIT;

		const VkImageMemoryBarrier2 imageBarrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = srcStageMask,
			.srcAccessMask = srcAccessMask,
			.dstStageMask = dstStageMask,
			.dstAccessMask = dstAccessMask,
			.oldLayout = srcLayout,
			.newLayout = dstLayout,
			.image = intTexture->vkImage,
			.subresourceRange = {.aspectMask = imageAspect, .levelCount = texture->numMips, .layerCount = 1},
		};

		const VkDependencyInfo dependencyInfo = {
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &imageBarrier,
		};

		vkCmdPipelineBarrier2(cmdCtx->vkCmd, &dependencyInfo);
	}

	void CommandBuffer::BlitTexture(const BlitTextureInfo& info)
	{
		const DeviceContext* gpuContext = GetGpuCtx(gpu->context);

		const Texture* srcTexture = gpuContext->texturePool.Get(info.srcHandle);
		const IntTexture* srcIntTexture = gpuContext->texturePool.GetInternal(info.srcHandle);
		const Texture* dstTexture = gpuContext->texturePool.Get(info.dstHandle);
		const IntTexture* dstIntTexture = gpuContext->texturePool.GetInternal(info.dstHandle);

		check(srcTexture && dstTexture && srcIntTexture && dstIntTexture);

		// NOTE(SS): bliting depth or stencil doesn't make sense
		check(HasDepthOrStencil(srcTexture->format) == false && HasDepthOrStencil(dstTexture->format) == false);

		VkImageBlit2 region = {.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2};
		region.srcOffsets[0] = ToVkOffset3D(IVec3(info.srcRect.min, 0));
		region.srcOffsets[1] = ToVkOffset3D(IVec3(info.srcRect.max, 1));
		region.dstOffsets[0] = ToVkOffset3D(IVec3(info.dstRect.min, 0));
		region.dstOffsets[1] = ToVkOffset3D(IVec3(info.dstRect.max, 1));

		const VkImageSubresourceLayers layers = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,
		};

		const VkBlitImageInfo2 blitImageInfo = {
			.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
			.srcImage = srcIntTexture->vkImage,
			.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			.dstImage = dstIntTexture->vkImage,
			.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.regionCount = 1,
			.pRegions = &region,
			.filter = ToVkFilter(info.filter),
		};

		vkCmdBlitImage2(cmdCtx->vkCmd, &blitImageInfo);
	}
} // namespace Turbo
