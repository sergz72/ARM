SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)
SET(CMAKE_CXX_COMPILER_WORKS 1)

SET(CMAKE_VERBOSE_MAKEFILE ON)

if (CLANG)
    SET(LINKER_SCRIPT ../../STM32H503xx_FLASH_clang.ld)
else()
    SET(LINKER_SCRIPT ../../STM32H503xx_FLASH.ld)
endif()

SET(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16")

SET(COMMON_FLAGS
        "-mcpu=cortex-m33 ${FPU_FLAGS} -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs -nostartfiles")

if (CLANG)
    SET(COMMON_FLAGS "${COMMON_FLAGS} --target=arm-none-eabi")
endif()

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DSTM32H503xx)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_ASM_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")

if (CLANG)
    SET(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -nostdlib -L /opt/arm/arm-none-eabi/lib/thumb/v8-m.main+fp/hard -lstdc++_nano -lc_nano -lnosys")
    add_definitions(-DCLANG)
endif()

project(stm32h503_logic_probe C CXX ASM)

file(GLOB SOURCES
        "src/main.cpp"
        "src/hal.c"
        "../startup_stm32h503xx.s"
        "../stm32h5lib/pll.c"
        "../stm32h5lib/systick.c"
        "../stm32h5lib/gpio.c"
        "../stm32h5lib/nvic.c"
        "../stm32h5lib/spi.c"
        "../../common_lib/delay_systick.c"
        "../../common_lib/display/lcd_st7789.c"
        "../../../usblib/usb_device.cpp"
        "../../../usblib/usb_device_drd.cpp"
        "../../../usblib/cdc_class.cpp"
)

if (CLANG)
    include_directories(src ../cmsis ../cmsis/device ../stm32h5lib ../../common_lib ../../../usblib ../../common_lib/display
            /opt/arm/arm-none-eabi/include/c++/14.3.1 /opt/arm/arm-none-eabi/include
            /opt/arm/arm-none-eabi/include/c++/14.3.1/arm-none-eabi
    )
else()
    include_directories(src ../cmsis ../cmsis/device ../stm32h5lib ../../common_lib ../../../usblib ../../common_lib/display)
endif()

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
