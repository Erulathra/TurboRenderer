set(ENGINE_TARGET_NAME TurboEngine)

set(SOURCE_FILES
	Source/Engine.cpp
)

# Linux platform implementation
if (${PLATFORM_LINUX})
	list(APPEND SOURCE_FILES Source/PlatformImpl/PlatformLinux.cpp)
endif ()

add_library(${ENGINE_TARGET_NAME} SHARED ${SOURCE_FILES})
target_include_directories(${ENGINE_TARGET_NAME} PUBLIC Include)

target_precompile_headers(${ENGINE_TARGET_NAME} PUBLIC pch.h)

target_link_libraries(${ENGINE_TARGET_NAME} PRIVATE glfw)
