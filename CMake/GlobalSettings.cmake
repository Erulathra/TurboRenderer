################################################################################
#                              Helper functions                                #
################################################################################

function(turbo_message VERBOSITY MESSAGE)
	message(${VERBOSITY} "[TURBO] ${MESSAGE}")
endfunction()

################################################################################
#                         Compiler and linker settings                         #
################################################################################

# Global settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# CLANG flags
add_compile_options(-fno-rtti)
add_compile_options(-fno-exceptions)
add_compile_options(-mavx2)

# enable mold
if (EXISTS "/usr/bin/mold")
	add_link_options("-fuse-ld=mold")
	turbo_message(STATUS "Mold linker enabled.")
endif ()

# Enable colored output
set(CMAKE_COLOR_DIAGNOSTICS ON)

################################################################################
#                             Compile Definitions                              #
################################################################################

# Platform definitions
set(PLATFORM_LINUX 0)
set(PLATFORM_WIN64 0)

if (UNIX)
	turbo_message(STATUS "Platform Linux")
	set(PLATFORM_LINUX 1)
elseif (WIN32)
	turbo_message(STATUS "Platform Windows")
	set(PLATFORM_WINDOWS 1)
endif ()

add_compile_definitions(
	PLATFORM_WINDOWS=${PLATFORM_WINDOWS}
	PLATFORM_LINUX=${PLATFORM_LINUX}
)

# Build type definitions
set(BUILD_TYPE "Development" CACHE STRING "Build type (Development or Shipping)")

set(TURBO_DEVELOPMENT 0)
set(TURBO_SHIPPING 0)

if (BUILD_TYPE STREQUAL "Development")
	turbo_message(STATUS "Build Development")
	set(TURBO_DEVELOPMENT 1)
elseif (BUILD_TYPE STREQUAL "Shipping")
	turbo_message(STATUS "Build Shipping")
	set(TURBO_SHIPPING 1)
else ()
	turbo_message(FATAL_ERROR, "Invalid build type")
endif()

add_compile_definitions(
	TURBO_DEVELOPMENT=${TURBO_DEVELOPMENT}
	TURBO_SHIPPING=${TURBO_SHIPPING}
)
