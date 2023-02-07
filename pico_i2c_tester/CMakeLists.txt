cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_i2c_tester C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(pico_i2c_tester
        src/hal.c
        src/main.c
        src/i2c_commands.c
        src/tm1638_commands.c
        ../common_lib/getstring.c
        ../common_lib/shell/shell.c
        ../common_lib/read_hex_string.c
        ../common_lib/keyboard_display/tm1638.c
        ../common_lib/display/seven_seg.c
        ../common_lib/fixed_queue.c
        ../common_lib/spi_3wire_soft.c
)

include_directories(src ../common_lib ../common_lib/shell ../common_lib/keyboard_display ../common_lib/display)

target_link_libraries(pico_i2c_tester pico_stdlib hardware_i2c)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_i2c_tester 1)
pico_enable_stdio_uart(pico_i2c_tester 0)

pico_add_extra_outputs(pico_i2c_tester)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
