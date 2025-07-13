SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)
SET(CMAKE_CXX_COMPILER_WORKS 1)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(LINKER_SCRIPT ../../STM32G431XX_FLASH.ld)

SET(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16")
add_definitions(-DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING)

SET(COMMON_FLAGS
        "-mcpu=cortex-m4 ${FPU_FLAGS} -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs -nostartfiles")

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DSTM32G431xx)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_ASM_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O0")

project(stm32g431_logic_probe C CXX ASM)

file(GLOB SOURCES
        "../../logic_probe_core/main.c"
        "src/hal.c"
        "src/main.cpp"
        "../../logic_probe_core/ui_128x32.c"
        "../../logic_probe_core/ui_common.c"
        "../../logic_probe_core/dac_commands.c"
        "../../logic_probe_core/pwm_commands.c"
        "../../logic_probe_core/counters_commands.c"
        "../../logic_probe_core/custom_main_init_usb.cpp"
        "../startup_stm32g431xx.s"
        "../stm32g4lib/gpio.c"
        "../stm32g4lib/systick.c"
        "../stm32g4lib/nvic.c"
        "../stm32g4lib/usart.c"
        "../stm32g4lib/i2c.c"
        "../stm32g4lib/spi.c"
        "../../common_lib/delay_systick.c"
        "../../common_lib/shell/shell.c"
        "../../common_lib/getstring.c"
        "../../common_lib/fixed_queue.c"
        "../../common_lib/common_printf.c"
        "../../common_lib/myprintf.c"
        "../../common_lib/ultoa.c"
        "../../common_lib/display/lcd_ssd1306.c"
        "../../common_lib/display/font.c"
        "../../common_lib/display/fonts/font8_2.c"
        "../../common_lib/display/fonts/font10.c"
        "../../common_lib/display/lcd.c"
        "../../common_lib/ws2812_spi.c"
        "../../../usblib/usb_device.cpp"
        "../../../usblib/usb_device_stm32g4.cpp"
        "../../../usblib/cdc_class.cpp"
)

include_directories(src .. ../cmsis ../cmsis/device ../stm32g4lib ../../common_lib ../../common_lib/shell
        ../../common_lib/display ../../logic_probe_core ../../../usblib)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
