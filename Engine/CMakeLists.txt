set(ENGINE_TARGET_NAME TurboEngine)

set(SOURCE_FILES
	Source/Engine.cpp
)

add_library(${ENGINE_TARGET_NAME} SHARED ${SOURCE_FILES})
target_include_directories(${ENGINE_TARGET_NAME} PUBLIC Include)

target_precompile_headers(${ENGINE_TARGET_NAME} PUBLIC pch.h)
