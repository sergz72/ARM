cmake_minimum_required(VERSION 3.31)
project(usb_device_library_test C)

set(CMAKE_C_STANDARD 23)

add_executable(usb_device_library_test
        src/main.c
        src/usb_device.c
        src/usb_test_device.c
        src/class/cdc/usb_cdc.c
)

include_directories(src)