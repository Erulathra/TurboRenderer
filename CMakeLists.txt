cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
project(TurboRenderer VERSION 0.0.1)

include(CMake/GlobalSettings.cmake)

turbo_message(STATUS "Turbo Renderer")

add_subdirectory(Engine)
add_subdirectory(Runtime)
