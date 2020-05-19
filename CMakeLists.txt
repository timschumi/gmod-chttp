cmake_minimum_required(VERSION 3.10)
project(gmod-chttp LANGUAGES CXX)

set(CMAKE_CONFIGURATION_TYPES Release Debug)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(gmod-module-base/include)
add_subdirectory(gmod-common-curl)

add_library(chttp SHARED)

# Add sources
add_subdirectory(src)

# Link against the GMod API
target_link_libraries(chttp gmod-module-base)

# Link against curl
target_link_libraries(chttp curl-phony)

# Set linker script for Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_options(chttp PRIVATE "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/chttp-linux.exp")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_definitions(-DWINDOWS_BUILD)
endif()

set_gmod_suffix_prefix(chttp)
