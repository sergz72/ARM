SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.14)

# specify the cross compiler
SET(CMAKE_C_COMPILER_WORKS 1)

SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F767ZI_FLASH.ld)

SET(CMAKE_VERBOSE_MAKEFILE ON)

#Uncomment for hardware floating point
SET(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv5-sp-d16")
add_definitions(-DARM_MATH_CM7 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -D__FPU_PRESENT=1)

SET(COMMON_FLAGS
        "-mcpu=cortex-m7 ${FPU_FLAGS} -mthumb -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs")

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og")

project(nucleo_f767zi_ethernet C ASM)

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DSTM32F767xx)

file(GLOB SOURCES
        "src/*.*"
        "cmisis/*.*"
        "cmisis/device/*.*"
        "../stm32f7lib/*.*"
        "../stm32f7lib/eth/*.*"
        "../common_lib/queue.c")

include_directories(src cmsis cmsis/device ../stm32f7lib ../stm32f7lib/eth ../common_lib)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

#add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
#        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
#        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
#        COMMENT "Building ${HEX_FILE}
#Building ${BIN_FILE}")
