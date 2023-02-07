cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_meter_tm1638 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(pico_meter_tm1638
        src/main.c
        src/hal.c
        src/ui.c
        src/cap_meter.c
        ../common_lib/keyboard_display/tm1638.c
        ../common_lib/display/seven_seg.c
        ../common_lib/spi_3wire_soft.c
)

include_directories(src ../common_lib ../common_lib/keyboard_display ../common_lib/display)

target_link_libraries(pico_meter_tm1638 pico_stdlib hardware_pwm hardware_pio)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_meter_tm1638 1)
pico_enable_stdio_uart(pico_meter_tm1638 0)

pico_add_extra_outputs(pico_meter_tm1638)

pico_generate_pio_header(pico_meter_tm1638 ${CMAKE_CURRENT_LIST_DIR}/src/counter.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)
pico_generate_pio_header(pico_meter_tm1638 ${CMAKE_CURRENT_LIST_DIR}/src/pwm.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)
pico_generate_pio_header(pico_meter_tm1638 ${CMAKE_CURRENT_LIST_DIR}/src/cap.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
