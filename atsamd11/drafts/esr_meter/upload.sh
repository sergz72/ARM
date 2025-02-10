#! /bin/sh
/opt/arm/bin/arm-none-eabi-gdb -q -iex "target remote tcp:127.0.0.1:2331" -iex "load /home/sergzz/serg/my/Programs/ARM_CLION/atsamd11/esr_meter/cmake-build-debug-arm/esr_meter.elf" -iex "monitor reset" -iex "disconnect" -iex "quit"
