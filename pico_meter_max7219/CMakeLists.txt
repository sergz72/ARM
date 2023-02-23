cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_meter_max7219 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        -DPICO_HEAP_SIZE=131072
        )

file(GLOB SOURCES
        "src/*.*"
        "src/devices/*.*"
        "../common_lib/sensor/ina226.c"
        "../common_lib/sensor/mcp3421.c"
        "../common_lib/rf/si5351.c"
        "../common_lib/display/max7219.c"
        "../common_lib/display/seven_seg.c"
        "../common_lib/keyboard/keyboard.c"
        "../common_lib/spi_4bit_soft.c"
        "../common_lib/shell/shell.c"
        "../common_lib/fixed_queue.c"
        "../common_lib/getstring.c"
        "../common_lib/memory/24c01_16.c")

add_executable(pico_meter_max7219 ${SOURCES})

include_directories(src src/devices ../common_lib ../common_lib/keyboard ../common_lib/display ../common_lib/sensor
                    ../common_lib/rf ../common_lib/shell ../common_lib/memory)

target_link_libraries(pico_meter_max7219 pico_stdlib hardware_pwm hardware_pio hardware_i2c hardware_adc)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_meter_max7219 1)
pico_enable_stdio_uart(pico_meter_max7219 0)

pico_add_extra_outputs(pico_meter_max7219)

pico_generate_pio_header(pico_meter_max7219 ${CMAKE_CURRENT_LIST_DIR}/src/devices/counter1.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)
pico_generate_pio_header(pico_meter_max7219 ${CMAKE_CURRENT_LIST_DIR}/src/devices/counter2.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)
pico_generate_pio_header(pico_meter_max7219 ${CMAKE_CURRENT_LIST_DIR}/src/devices/pwm.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)
pico_generate_pio_header(pico_meter_max7219 ${CMAKE_CURRENT_LIST_DIR}/src/devices/cap.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
