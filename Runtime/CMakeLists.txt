set(RUNTIME_TARGET_NAME Turbo)

add_executable(${RUNTIME_TARGET_NAME} main.cpp)
target_link_libraries(${RUNTIME_TARGET_NAME} TurboEngine)
