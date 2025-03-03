add_library(littlefs STATIC)

FetchContent_Declare(
	LittleFS
    GIT_REPOSITORY https://github.com/littlefs-project/littlefs
    GIT_TAG        v2.10.1
)

FetchContent_MakeAvailable(LittleFS)

target_sources(littlefs PRIVATE
	${littlefs_SOURCE_DIR}/lfs.c
	${littlefs_SOURCE_DIR}/lfs_util.c
)

target_include_directories(littlefs PUBLIC
	${littlefs_SOURCE_DIR}/
)