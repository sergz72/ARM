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

project(pico_memory_tester C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
)

add_executable(pico_memory_tester
        src/main.c
        src/hal.c
        src/spi_commands.c
        src/spi_memory_commands.c
        ../../common_lib/getstring.c
        ../../common_lib/fixed_queue.c
        ../../common_lib/shell/shell.c
        ../../common_lib/read_hex_string.c
        ../../common_lib/crypto/chacha.c
        ../../common_lib/memory/spi_memory.c
        ../../common_lib/memory/93cXX_16.c
)

include_directories(
        src
        ../../common_lib
        ../../common_lib/shell
        ../../common_lib/crypto
        ../../common_lib/memory
)

target_link_libraries(pico_memory_tester pico_stdlib hardware_spi)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_memory_tester 1)
pico_enable_stdio_uart(pico_memory_tester 0)

pico_add_extra_outputs(pico_memory_tester)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
