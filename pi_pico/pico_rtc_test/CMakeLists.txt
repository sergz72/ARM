cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_rtc_test C CXX ASM)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 26)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(${PROJECT_NAME}
        src/hal.c
        src/main.c
        src/ds3231_commands.c
        src/ds1307_commands.c
        src/pcf8563_commands.c
        src/i2c_commands.c
        ../lib/ws2812.c
        ../../../common_lib/getstring.c
        ../../../common_lib/fixed_queue.c
        ../../../common_lib/read_hex_string.c
        ../../../common_lib/shell/shell.c
        ../../../common_lib/rtc/rtc_ds1307.c
        ../../../common_lib/rtc/rtc_ds3231.c
        ../../../common_lib/rtc/rtc_pcf8563.c
        ../../../common_lib/rtc/rtc_func.c
)

include_directories(
        src
        ../lib
        ../../../common_lib
        ../../../common_lib/shell
        ../../../common_lib/rtc
)

target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_i2c hardware_pio)

pico_generate_pio_header(${PROJECT_NAME} ../../lib/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)

# enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
