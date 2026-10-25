cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
project(TurboRenderer VERSION 0.0.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(GlobalSettings)

turbo_message(STATUS "Turbo Renderer")

add_subdirectory(Engine)
add_subdirectory(Runtime)
add_subdirectory(Thirdparty)
