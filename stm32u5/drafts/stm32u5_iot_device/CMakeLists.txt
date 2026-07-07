SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 26)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)
SET(CMAKE_CXX_COMPILER_WORKS 1)

SET(CMAKE_VERBOSE_MAKEFILE ON)

if (CLANG)
    SET(LINKER_SCRIPT ../../STM32U575xx_FLASH_clang.ld)
else()
    SET(LINKER_SCRIPT ../../STM32U575xx_FLASH.ld)
endif()

SET(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16")

SET(COMMON_FLAGS
        "-mcpu=cortex-m33 ${FPU_FLAGS} -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs -nostartfiles")

if (CLANG)
    SET(COMMON_FLAGS "${COMMON_FLAGS} --target=arm-none-eabi")
endif()

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DSTM32U575xx)

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_ASM_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")
string(REPLACE "-O3" "-O1" CMAKE_C_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
string(REPLACE "-O3" "-O1" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

if (CLANG)
    SET(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -nostdlib -L /opt/arm/arm-none-eabi/lib/thumb/v8-m.main+fp/hard -lc_nano -lnosys")
    add_definitions(-DCLANG)
endif()

project(stm32u5_iot_device C CXX ASM)

file(GLOB SOURCES
        "src/main.c"
#        "src/main.cpp"
        "src/hal.c"
        "src/scd41_commands.c"
        "src/date_commands.c"
        "../startup_stm32u575xx.s"
        "../stm32u5lib/rcc.c"
        "../stm32u5lib/rtc.c"
        "../stm32u5lib/usart.c"
        "../stm32u5lib/nvic.c"
        "../stm32u5lib/systick.c"
        "../stm32u5lib/gpio.c"
        "../../../common_lib/delay_systick.c"
        "../../../common_lib/spi_soft.c"
        "../../../common_lib/i2c_soft.c"
        "../../../common_lib/rtc/rtc_func.c"
#        "../../../usblib/usb_device.cpp"
#        "../../../usblib/usb_device_stm32f.cpp"
#        "../../../usblib/cdc_class.cpp"
        "../../../common_lib/shell/shell.c"
        "../../../common_lib/getstring.c"
        "../../../common_lib/fixed_queue.c"
        "../../../common_lib/common_printf.c"
        "../../../common_lib/read_hex_string.c"
        "../../../common_lib/myprintf.c"
        "../../../common_lib/rf/cc1101/cc1101.c"
        "../../../common_lib/rf/cc1101/cc1101_func.c"
        "../../../common_lib/rf/cc1101/cc1101_commands.c"
        "../../../common_lib/sensor/scd4x.c"
        "../../../common_lib/crc8.c"
)

if (CLANG)
    include_directories(src ../cmsis ../cmsis/device ../stm32u5lib ../../../common_lib ../../../common_lib/shell
            ../../../common_lib/rf/cc1101 ../../../common_lib/sensor ../../../common_lib/rtc
            /opt/arm/arm-none-eabi/include/c++/15.3.1 /opt/arm/arm-none-eabi/include
            /opt/arm/arm-none-eabi/include/c++/15.3.1/arm-none-eabi
    )
else()
include_directories(src ../cmsis ../cmsis/device ../stm32u5lib ../../../common_lib ../../../common_lib/shell
        ../../../common_lib/rf/cc1101 ../../../common_lib/sensor ../../../common_lib/rtc
    )
endif()

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
