add_library(freertos_config INTERFACE)

target_include_directories(freertos_config SYSTEM 
    INTERFACE
    ${CMAKE_SOURCE_DIR}/app/src
)

target_compile_definitions(freertos_config
    INTERFACE
    projCOVERAGE_TEST=0
)

target_compile_definitions(freertos_config INTERFACE ${definitions})
target_compile_options(freertos_config INTERFACE ${options})

set(FREERTOS_PORT GCC_ARM_CM4F CACHE STRING "")
set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)

FetchContent_Declare(
    FreeRTOS
    GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
    GIT_TAG        V11.1.0
)

FetchContent_MakeAvailable(FreeRTOS)

target_link_libraries(${PROJECT_NAME}.elf PRIVATE freertos_kernel)