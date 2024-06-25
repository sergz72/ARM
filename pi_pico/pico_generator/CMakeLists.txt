cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_generator C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(pico_generator
        src/main.c
        src/hal.c
        src/ui.c
        src/keyboard.c
        ../common_lib/display/lcd_st7735.c
        ../common_lib/display/lcd.c
        ../common_lib/display/font.c
        ../common_lib/display/fonts/font14.c
)

include_directories(src ../common_lib ../common_lib/display ../common_lib/display/fonts)

target_link_libraries(pico_generator pico_stdlib hardware_spi hardware_pwm)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_generator 1)
pico_enable_stdio_uart(pico_generator 0)

pico_add_extra_outputs(pico_generator)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND size ${PROJECT_NAME}.elf)
