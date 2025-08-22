cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_hyperram_test C CXX ASM)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 26)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(pico_hyperram_test
        src/hal.c
        src/main.c
        src/memory_commands.c
        src/hyperram.c
        ../../common_lib/getstring.c
        ../../common_lib/fixed_queue.c
        ../../common_lib/shell/shell.c
        ../../common_lib/read_hex_string.c
        ../../common_lib/crypto/chacha.c
)

include_directories(
        src 
        ../../common_lib
        ../../common_lib/shell
        ../../common_lib/crypto
)

target_link_libraries(pico_hyperram_test pico_stdlib)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_hyperram_test 1)
pico_enable_stdio_uart(pico_hyperram_test 0)

pico_add_extra_outputs(pico_hyperram_test)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
