cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico2_adf4351 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(pico2_adf4351
        src/hal.c
        src/main.c
        src/adf4351_commands.c
        ../../common_lib/getstring.c
        ../../common_lib/shell/shell.c
        ../../common_lib/read_hex_string.c
        ../../common_lib/fixed_queue.c
        ../../common_lib/rf/adf4351.c
)

include_directories(src ../../common_lib ../../common_lib/shell ../../common_lib/rf)

target_link_libraries(pico2_adf4351 pico_stdlib hardware_spi)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico2_adf4351 1)
pico_enable_stdio_uart(pico2_adf4351 0)

pico_add_extra_outputs(pico2_adf4351)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
