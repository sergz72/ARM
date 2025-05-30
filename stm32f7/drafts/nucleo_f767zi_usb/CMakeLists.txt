SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_C_STANDARD 23)
cmake_minimum_required(VERSION 3.14)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)

SET(LINKER_SCRIPT ../../STM32F767ZI_FLASH.ld)

SET(CMAKE_VERBOSE_MAKEFILE ON)

#Uncomment for hardware floating point
SET(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv5-sp-d16")
add_definitions(-DARM_MATH_CM7 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING)

SET(COMMON_FLAGS
        "-mcpu=cortex-m7 ${FPU_FLAGS} -mthumb -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs")

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og")

project(nucleo_f767zi_usb C ASM)

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DSTM32F767xx)

file(GLOB SOURCES
        "src/main.c"
        "src/hal.c"
        "src/system_init.c"
        "src/usb_device_otg.c"
        "src/usb_device.c"
        "src/class/cdc/usb_cdc.c"
        "../startup_stm32f767xx.s"
        "../stm32f7lib/*.*")

include_directories(src ../cmsis ../cmsis/device ../stm32f7lib)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
