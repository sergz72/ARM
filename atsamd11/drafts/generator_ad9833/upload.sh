#! /bin/sh
/opt/arm/bin/arm-none-eabi-gdb -q -iex "target remote tcp:127.0.0.1:2331" -iex "load /home/sergzz/serg/my/Programs/ARM_CLION/atsamd11/generator_ad9833/cmake-build-debug-arm/generator_ad9833.elf" -iex "monitor reset" -iex "disconnect" -iex "quit"
