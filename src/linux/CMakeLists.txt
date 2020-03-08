file(GLOB SOURCES
	"*.h"
	"*.cpp"
)

target_sources(chttp PRIVATE ${SOURCES})
