cmake_minimum_required(VERSION 3.10)
project(gmod-chttp LANGUAGES CXX)

set(CHTTP_VERSION "dev" CACHE STRING "The user-visible CHTTP version")

set(CMAKE_CONFIGURATION_TYPES Release Debug)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use curl builds that only have HTTP
set(CURL_HTTPONLY ON)

add_subdirectory(gmod-module-base/include)
add_subdirectory(curl-static-bin)

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

target_compile_definitions(chttp PRIVATE "CHTTP_VERSION=\"${CHTTP_VERSION}\"")

set_gmod_suffix_prefix(chttp)

if("${MORE_STATIC_LINKS}" AND ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_options(chttp PRIVATE "-static-libstdc++" "-static-libgcc")
endif()
