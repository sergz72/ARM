SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_C_STANDARD 23)
cmake_minimum_required(VERSION 3.14)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)

SET(LINKER_SCRIPT ../../STM32F411XX_FLASH.ld)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16")
add_definitions(-DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

SET(COMMON_FLAGS
        "-mcpu=cortex-m4 ${FPU_FLAGS} -mthumb -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs")

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")

project(STM32F411_uart_log_test C ASM)

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DSTM32F411xE)

file(GLOB SOURCES
        "src/main.c"
        "src/hal.c"
        "../cmsis/*.*"
        "../cmsis/device/*.*"
        "../stm32f4lib/dbgmcu.c"
        "../stm32f4lib/systick.c"
        "../stm32f4lib/pll.c"
        "../stm32f4lib/gpio.c"
        "../stm32f4lib/rcc.c"
        "../stm32f4lib/timer.c"
        "../stm32f4lib/dma.c"
        "../startup_stm32f411xe.s"
        "../../common_lib/delay_systick.c"
        "../../common_lib/uart_log.c"
)

include_directories(src src/devices ../cmsis ../cmsis/device ../stm32f4lib ../../common_lib)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
