@echo off
cd C:\serg\my\Programs\ARM_CLION\stm32l152_rf
start c:\progra~2/segger/jlink/JLinkGDBServerCL -select USB -device STM32L152CB -endian little -if SWD -speed 4000 -noir -LocalhostOnly  
c:\armgcc\bin\arm-none-eabi-gdb -q -iex "target remote tcp:127.0.0.1:2331" -iex "load C:/serg/my/Programs/ARM_CLION/stm32l152_rf/cmake-build-debug/stm32l152_rf.elf" -iex "monitor reset" -iex "disconnect" -iex "quit"
