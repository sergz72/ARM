@echo off
cd d:\serg\my\Programs\ARM_CLION\stm32f401_meter
start c:\progra~1/segger/jlink/JLinkGDBServerCL -select USB -device STM32F401CE -endian little -if SWD -speed 4000 -noir -LocalhostOnly  
c:\armgcc\bin\arm-none-eabi-gdb -q -iex "target remote tcp:127.0.0.1:2331" -iex "load D:/serg/my/Programs/ARM_CLION/stm32f401_meter/cmake-build-debug/stm32f401_meter.elf" -iex "monitor reset" -iex "monitor go" -iex "disconnect" -iex "quit"
