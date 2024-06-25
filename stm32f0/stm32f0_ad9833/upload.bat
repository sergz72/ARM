@echo off
cd C:\serg\my\Programs\ARM_CLION\stm32f0_ad9833
start c:\progra~1/segger/jlink/JLinkGDBServerCL -select USB -device STM32F051R8 -endian little -if SWD -speed 4000 -noir -LocalhostOnly  
c:\armgcc\bin\arm-none-eabi-gdb -q -iex "target remote tcp:127.0.0.1:2331" -iex "load C:/serg/my/Programs/ARM_CLION/stm32f0_ad9833/cmake-build-debug/stm32f0_ad9833.elf" -iex "monitor reset" -iex "disconnect" -iex "quit"
