include(CPM)

CPMAddPackage(
  NAME glfw
  GITHUB_REPOSITORY glfw/glfw
  GIT_TAG 3.5.1
  OPTIONS "GLFW_BUILD_EXAMPLES OFF"
          "GLFW_BUILD_TESTS OFF"
          "GLFW_BUILD_DOCS OFF"
)

CPMAddPackage(
  NAME volk
  GITHUB_REPOSITORY zeux/volk
  GIT_TAG vulkan-sdk-1.4.363.0
)

CPMAddPackage(
  NAME VulkanMemoryAllocator
  GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
  GIT_TAG v3.5.0
  OPTIONS "VMA_BUILD_SAMPLES OFF"
          "VMA_BUILD_DOCUMENTATION OFF"
)

set_target_properties(
	glfw
	volk
	VulkanMemoryAllocator
	PROPERTIES FOLDER "thirdparty"
)
