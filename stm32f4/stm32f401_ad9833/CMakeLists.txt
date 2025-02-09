SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.14)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)

SET(LINKER_SCRIPT ../../STM32F401CEUX_FLASH.ld)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16")
add_definitions(-DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING)

SET(COMMON_FLAGS
        "-mcpu=cortex-m4 ${FPU_FLAGS} -mthumb -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs")

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")

project(STM32F401_ad9833 C ASM)

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DSTM32F401xE)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

file(GLOB SOURCES
        "src/*.c"
        "src/devices/*.*"
        "../cmisis/*.*"
        "../cmisis/device/*.*"
        "../stm32f4lib/dbgmcu.c"
        "../stm32f4lib/pll.c"
        "../stm32f4lib/gpio.c"
        "../stm32f4lib/exti.c"
        "../stm32f4lib/nvic.c"
        "../stm32f4lib/rcc.c"
        "../stm32f4lib/spi.c"
        "../stm32f4lib/adc.c"
        "../stm32f4lib/timer.c"
        "../../common_lib/rf/ad9833.c"
        "../startup_stm32f401ceux.s"
)

include_directories(src src/devices ../cmsis ../cmsis/device ../stm32f4lib ../../common_lib/rf)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
