SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_STANDARD 23)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(LINKER_SCRIPT ../../saml10e16a_flash.ld)

SET(COMMON_FLAGS
        "-mcpu=cortex-m23 -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs -nostartfiles")

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_ASM_FLAGS_INIT "${COMMON_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage,--defsym,STACK_SIZE=512,--defsym,HEAP_SIZE=0 -T ${LINKER_SCRIPT}")

project(blink C ASM)

file(GLOB SOURCES
        "src/main.c"
        "src/hal.c"
        "../startup_saml10e16a.c"
        "../atsaml1xlib/systick.c"
        "../../../common_lib/delay_systick.c"
)

include_directories(src ../atsaml1xlib ../cmsis ../../../common_lib)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
