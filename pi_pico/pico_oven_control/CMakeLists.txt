cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_oven_control C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(pico_oven_control
        src/hal.c
        src/ui.c
        src/main.c
        src/controller.c
        src/ws2812.c
        ../../common_lib/getstring.c
        ../../common_lib/shell/shell.c
        ../../common_lib/read_hex_string.c
        ../../common_lib/fixed_queue.c
        ../../common_lib/display/lcd_ssd1306.c
        ../../common_lib/display/lcd.c
        ../../common_lib/display/font.c
        ../../common_lib/display/fonts/font36.c
        ../../common_lib/display/fonts/font18.c
        ../../common_lib/sensor/mcp9600.c
)

include_directories(src ../../common_lib ../../common_lib/shell ../../common_lib/display ../../common_lib/sensor)

target_link_libraries(pico_oven_control pico_stdlib hardware_i2c hardware_pio)

pico_generate_pio_header(pico_oven_control ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_oven_control 1)
pico_enable_stdio_uart(pico_oven_control 0)

pico_add_extra_outputs(pico_oven_control)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
