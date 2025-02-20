SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.14)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(LINKER_SCRIPT ../../stm32g031f6px_flash.ld)

SET(COMMON_FLAGS
        "-mcpu=cortex-m0plus -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs -nostartfiles")

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DSTM32G031xx)

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")

project(stm32g0_ad9833 C ASM)

file(GLOB SOURCES
        "src/hal.c"
        "src/handler.c"
        "src/main.c"
        "../cmisis/device/*.*"
        "../stm32g0lib/gpio.c"
        "../stm32g0lib/exti.c"
        "../stm32g0lib/nvic.c"
        "../stm32g0lib/rcc.c"
        "../stm32g0lib/spi.c"
        "../stm32g0lib/adc.c"
        "../stm32g0lib/timer.c"
        "../../common_lib/rf/ad9833.c"
        "../startup_stm32g031xx.s"
)

include_directories(src src/devices ../cmsis ../cmsis/device ../stm32g0lib ../../common_lib/rf)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
