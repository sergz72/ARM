SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.14)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)

SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F051R8TX_FLASH.ld)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(COMMON_FLAGS
        "-mcpu=cortex-m0 -mthumb -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs")

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")
if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif()

project(STM32F0_AD9833 C ASM)

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DSTM32F051x8)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

file(GLOB SOURCES
        "src/*.*"
        "cmisis/*.*"
        "../common_lib/rf/ad9833.c"
        "../stm32f0lib/dbgmcu.c"
        "../stm32f0lib/gpio.c"
        "../stm32f0lib/i2c.c"
        "../stm32f0lib/spi.c"
        "../stm32f0lib/nvic.c"
        "../stm32f0lib/systick.c"
        )

include_directories(src cmsis cmsis/device ../stm32f0lib ../common_lib/rf ../common_lib/potentiometer)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

