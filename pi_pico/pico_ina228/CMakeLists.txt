cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_ina228 C CXX ASM)
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
        src/ina226_commands.c
        src/ina228_commands.c
        src/i2c_commands.c
        src/ws2812.c
        ../../common_lib/getstring.c
        ../../common_lib/fixed_queue.c
        ../../common_lib/shell/shell.c
        ../../common_lib/adc/ina228.c
        ../../common_lib/adc/ina226.c
)

include_directories(
        src
        ../../common_lib
        ../../common_lib/shell
        ../../common_lib/adc
)

target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_spi hardware_i2c hardware_pio)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)

# enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(pico_ina228)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
