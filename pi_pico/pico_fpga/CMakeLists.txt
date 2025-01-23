cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_fpga C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(pico_fpga
        src/hal.c
        src/main.c
        src/core/devices.c
        src/core/device_list.c
        src/core/device_config.c
        src/core/core_main.c
        src/hal.c
        ../../common_lib/i2c_soft.c
        ../../common_lib/spi_3bit_soft.c
        ../../common_lib/sensor/ads1115.c
        ../../common_lib/sensor/mcp3421.c
        ../../common_lib/sensor/ina226.c
        ../../common_lib/sensor/ina3221.c
        ../../common_lib/sensor/mcp9600.c
        ../../common_lib/rf/si5351.c
        ../../common_lib/memory/24c01_16.c
        src/core/devices/dev_ad9833.c
        src/core/devices/dev_ads1115.c
        src/core/devices/dev_dds.c
        src/core/devices/dev_ina226.c
        src/core/devices/dev_ina3221.c
        src/core/devices/dev_mcp3421.c
        src/core/devices/dev_mcp9600.c
        src/core/devices/dev_si5351.c
)

include_directories(
        src 
        src/core
        src/core/devices
        ../../common_lib
        ../../common_lib/rf
        ../../common_lib/memory
        ../../common_lib/sensor
)

target_link_libraries(pico_fpga pico_stdlib)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_fpga 1)
pico_enable_stdio_uart(pico_fpga 0)

pico_add_extra_outputs(pico_fpga)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
