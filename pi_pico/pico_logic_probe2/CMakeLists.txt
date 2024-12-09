cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_logic_probe2 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(pico_logic_probe2
        src/main.c
        src/hal.c
)

include_directories(src)

target_link_libraries(pico_logic_probe2 pico_stdlib hardware_pio)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_logic_probe2 1)
pico_enable_stdio_uart(pico_logic_probe2 0)

pico_add_extra_outputs(pico_logic_probe2)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
