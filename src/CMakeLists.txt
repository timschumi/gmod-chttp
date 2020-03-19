file(GLOB SOURCES
	"*.h"
	"*.cpp"
)

target_sources(chttp PRIVATE ${SOURCES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
add_subdirectory(windows)
else()
add_subdirectory(linux)
endif()
