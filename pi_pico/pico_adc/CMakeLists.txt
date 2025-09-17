cmake_minimum_required(VERSION 3.12)

#set(PICO_PLATFORM rp2350-arm-s)
#set(PICO_BOARD pico2)
#set(PICO_SDK_PATH ../../../../RaspberryPiPico/pico-sdk)

#if (${PICO_PLATFORM} MATCHES "rp2350-arm-s")
#    set(PICO_TOOLCHAIN_PATH /opt/arm/bin)
#else()
#    set(PICO_TOOLCHAIN_PATH /opt2/gcc14-rp2350-no-zcmp/bin)
#endif()

include(pico_sdk_import.cmake)

project(pico_adc C CXX ASM)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
)

add_executable(pico_adc
        src/main.c
        src/ws2812.c
        src/hal.c
        src/ads1259_commands.c
        src/ads1232_commands.c
        src/ad7124_commands.c
        src/ad7705_commands.c
        ../../common_lib/getstring.c
        ../../common_lib/fixed_queue.c
        ../../common_lib/shell/shell.c
        ../../common_lib/adc/ads1259.c
        ../../common_lib/adc/ads1232.c
        ../../common_lib/adc/ad7124.c
        ../../common_lib/adc/ad7705.c
)

include_directories(
        src
        ../../common_lib
        ../../common_lib/shell
        ../../common_lib/adc
)

target_link_libraries(pico_adc pico_stdlib hardware_pio hardware_spi)

pico_generate_pio_header(pico_adc ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_adc 1)
pico_enable_stdio_uart(pico_adc 0)

pico_add_extra_outputs(pico_adc)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
