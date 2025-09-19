SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.14)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(LINKER_SCRIPT ../../mspm0g3507.lds)

SET(COMMON_FLAGS
        "-mcpu=cortex-m0plus -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs -nostartfiles")

add_definitions(-D__MSPM0G3507__)

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_ASM_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")

project(mspm0g3507_multimeter C ASM)

file(GLOB SOURCES
        "src/main.c"
        "src/hal.c"
        "src/ui.c"
        "src/dac_commands.c"
        "src/adc_commands.c"
        "../startup_mspm0g350x_gcc.c"
        "../systick.c"
        "../ti/driverlib/dl_gpio.c"
        "../ti/driverlib/dl_common.c"
        "../ti/driverlib/dl_uart.c"
        "../ti/driverlib/dl_uart_main.c"
        "../ti/driverlib/dl_vref.c"
        "../ti/driverlib/dl_dac12.c"
        "../ti/driverlib/dl_opa.c"
        "../ti/driverlib/dl_i2c.c"
        "../ti/driverlib/m0p/sysctl/dl_sysctl_mspm0g1x0x_g3x0x.c"
        "../../common_lib/shell/shell.c"
        "../../common_lib/getstring.c"
        "../../common_lib/fixed_queue.c"
        "../../common_lib/common_printf.c"
        "../../common_lib/myprintf.c"
        "../../common_lib/ultoa.c"
        "../../common_lib/adc/mcp3421.c"
)

include_directories(src .. ../cmsis/core ../ti/driverlib ../../common_lib ../../common_lib/shell ../../common_lib/adc)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
