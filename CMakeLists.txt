cmake_minimum_required(VERSION 3.10)
project(gmod-chttp LANGUAGES CXX)

set(CMAKE_CONFIGURATION_TYPES Release Debug)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(gmod-module-base/include)

add_library(chttp SHARED)

# Add sources
add_subdirectory(src)

# Link against the GMod API
target_link_libraries(chttp gmod-module-base)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_definitions(-DWINDOWS_BUILD)
endif()

# Add the curl include directory
target_include_directories(chttp PRIVATE curl/include)

# Link against libcurl
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_directories(chttp PRIVATE curl/lib)
	if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64")
		target_link_libraries(chttp libcurl-x64)
	else()
		target_link_libraries(chttp libcurl)
	endif()
else()
	target_link_libraries(chttp curl)
endif()

set_gmod_suffix_prefix(chttp)
