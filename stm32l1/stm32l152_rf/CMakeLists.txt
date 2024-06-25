SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.14)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)

SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32L152RB_FLASH.ld)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(COMMON_FLAGS
        "-mcpu=cortex-m3 -mthumb -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs")

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")
if (CMAKE_BUILD_TYPE MATCHES Debug)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og")
else()
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif()

project(STM32L152_rf C ASM)

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DSTM32L152xB)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

file(GLOB SOURCES
        "src/*.*"
        "cmisis/*.*"
        "cmisis/device/*.*"
        "../common_lib/st7066_8bit.c"
        "../common_lib/th02.c"
        "../common_lib/cc1101.c"
        "../common_lib/rtc_func.c"
        "../common_lib/aes128.c"
        "../stm32l1lib/*.*")

include_directories(src cmsis cmsis/device ../stm32l1lib ../common_lib)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

