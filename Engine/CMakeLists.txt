set(ENGINE_TARGET_NAME TurboEngine)

set(SOURCE_FILES
	Source/Engine.cpp
)

add_library(${ENGINE_TARGET_NAME} ${SOURCE_FILES})
target_include_directories(${ENGINE_TARGET_NAME} PUBLIC Include)
