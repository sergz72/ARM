SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.14)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(LINKER_SCRIPT ../../STM32H503xx_FLASH.ld)

SET(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16")

SET(COMMON_FLAGS
        "-mcpu=cortex-m33 ${FPU_FLAGS} -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs -nostartfiles")

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DSTM32H503xx)

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_ASM_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")

project(stm32h503_meter C ASM)

file(GLOB SOURCES
        "src/main.c"
        "src/hal.c"
        "src/usb_device.c"
        "src/usb_device_drd.c"
        "src/class/cdc/usb_cdc.c"
        "../startup_stm32h503xx.s"
        "../stm32h5lib/pll.c"
        "../stm32h5lib/systick.c"
        "../stm32h5lib/gpio.c"
        "../stm32h5lib/nvic.c"
)

include_directories(src ../cmsis ../cmsis/device ../stm32h5lib)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
