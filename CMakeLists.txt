#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.28.1)


# project settings
project(athenavi C CXX ASM)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# hardware specific options
include(cmake/stm32f446xx.cmake)

file(GLOB_RECURSE GENERATED_SOURCES "Core/*.*" "Drivers/*.*" "Middlewares/*.*" "USB_DEVICE/*.*")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
    add_definitions(-DDEBUG)

    # Replace default syscalls.c with semihosting to allow debug prints
    list(FILTER GENERATED_SOURCES EXCLUDE REGEX ".*/Core/Src/syscalls.c")
    add_link_options(-lc -lrdimon -specs=rdimon.specs)
endif ()

if (${DUMP_LOGS})
    add_compile_options(-DDUMP_LOGS)
endif()

if(${FORMAT_FLASH}) 
    add_compile_options(-DFORMAT_FLASH)
endif()

include_directories(Core/Inc Drivers/STM32F4xx_HAL_Driver/Inc Drivers/STM32F4xx_HAL_Driver/Inc/Legacy Drivers/CMSIS/Device/ST/STM32F4xx/Include Drivers/CMSIS/Include Middlewares/ST/STM32_USB_Device_Library/Core/Inc Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc USB_DEVICE/App USB_DEVICE/Target)

add_definitions(-DUSE_HAL_DRIVER -DSTM32F446xx)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F446RETX_FLASH.ld)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m4 -mthumb -mthumb-interwork)
add_link_options(-T ${LINKER_SCRIPT})

include(cmake/sources.cmake)
add_executable(${PROJECT_NAME}.elf ${GENERATED_SOURCES} ${SOURCES} ${LINKER_SCRIPT})

include(FetchContent)
include(cmake/freertos.cmake)
include(cmake/littlefs.cmake)

add_subdirectory(app/lib/drivers)
add_subdirectory(app/lib/utils)
target_link_libraries(${PROJECT_NAME}.elf PRIVATE 
    drivers
    utils
    littlefs
)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
