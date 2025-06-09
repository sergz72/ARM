SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.14)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)

SET(LINKER_SCRIPT ../../AT32F403AxG_FLASH.ld)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16")
add_definitions(-DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING)

SET(COMMON_FLAGS
        "-mcpu=cortex-m4 ${FPU_FLAGS} -mthumb -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs")

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")

project(AT32F403_usb_audio C ASM)

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DAT32F403ACGU7)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

file(GLOB SOURCES
        "src/hal.c"
        "src/main.c"
        "src/audio_codec.c"
        "src/tlv.c"
        "../lib/i2c_func.c"
        "../lib/systick.c"
        "../lib/usbd_drivers/src/*.c"
        "../lib/usbd_class/audio/*.c"
        "../../common_lib/delay_systick.c"
        "../lib/src/at32f403a_407_gpio.c"
        "../lib/src/at32f403a_407_crm.c"
        "../lib/src/at32f403a_407_usb.c"
        "../lib/src/at32f403a_407_misc.c"
        "../lib/src/at32f403a_407_dma.c"
        "../lib/src/at32f403a_407_i2c.c"
        "../lib/src/at32f403a_407_spi.c"
        "../lib/system_at32f403a_407.c"
        "../startup_at32f403a_407.s"
        "../../common_lib/delay_systick.c"
        "../../common_lib/audio/tlv320dac3100.c"
)

include_directories(src ../cmsis ../cmsis/device ../lib/inc ../lib ../lib/usbd_drivers/inc ../lib/usbd_class/audio
        ../../common_lib ../../common_lib/audio)

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
