SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_STANDARD 23)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(LINKER_SCRIPT ../../saml10e16a_flash.ld)

SET(COMMON_FLAGS
        "-mcpu=cortex-m23 -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs -nostartfiles")

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_ASM_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage,--defsym,STACK_SIZE=512,--defsym,HEAP_SIZE=0 -T ${LINKER_SCRIPT}")

project(atsaml10e16_iot_device C ASM)

file(GLOB SOURCES
        "src/main.c"
        "src/hal.c"
        "src/scd41_commands.c"
        "../startup_saml10e16a.c"
        "../atsaml1xlib/systick.c"
        "../atsaml1xlib/usart.c"
        "../atsaml1xlib/i2c.c"
        "../atsaml1xlib/spi.c"
        "../atsaml1xlib/pwr.c"
        "../../../common_lib/delay_systick.c"
        "../../../common_lib/shell/shell.c"
        "../../../common_lib/getstring.c"
        "../../../common_lib/fixed_queue.c"
        "../../../common_lib/read_hex_string.c"
        "../../../common_lib/common_printf.c"
        "../../../common_lib/myprintf.c"
        "../../../common_lib/ultoa.c"
        "../../../common_lib/rf/cc1101/cc1101.c"
        "../../../common_lib/rf/cc1101/cc1101_func.c"
        "../../../common_lib/rf/cc1101/cc1101_commands.c"
        "../../../common_lib/sensor/scd4x.c"
        "../../../common_lib/crc8.c"
        "../../../common_lib/rtc/rtc_func.c"
        "../../../common_lib/rtc/rtc_ds1307.c"
        "../../../common_lib/rtc/rtc_ds3231.c"
        "../../../common_lib/rtc/rtc_pcf8563.c"
        "../../../common_lib/rtc/ds1307_commands.c"
        "../../../common_lib/rtc/ds3231_commands.c"
        "../../../common_lib/rtc/pcf8563_commands.c"
)

include_directories(src ../atsaml1xlib ../cmsis ../../../common_lib ../../../common_lib/shell
        ../../../common_lib/rf/cc1101 ../../../common_lib/sensor ../../../common_lib/rtc)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
