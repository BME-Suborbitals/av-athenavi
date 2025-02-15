# List directories containing header files
include_directories(
	app/src
)

# Automatically gather source files
file(GLOB_RECURSE SOURCES "app/src/*.cpp")