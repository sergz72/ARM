cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_pio_spi C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(pico_pio_spi
        src/hal.c
        src/main.c
        src/spi_commands.c
        ../../common_lib/getstring.c
        ../../common_lib/shell/shell.c
        ../../common_lib/read_hex_string.c
        ../../common_lib/fixed_queue.c
)

include_directories(src ../../common_lib ../../common_lib/shell)

target_link_libraries(pico_pio_spi pico_stdlib hardware_pio)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_pio_spi 1)
pico_enable_stdio_uart(pico_pio_spi 0)

pico_add_extra_outputs(pico_pio_spi)

pico_generate_pio_header(pico_pio_spi ${CMAKE_CURRENT_LIST_DIR}/src/spi.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
