#! /bin/sh
/opt/arm/bin/arm-none-eabi-gdb -q -iex "target remote tcp:127.0.0.1:2331" -iex "load /home/sergzz/serg/my/Programs/ARM_CLION/gd32f303/blink/cmake-build-debug-arm/GD32F303_blink.elf" -iex "monitor reset" -iex "disconnect" -iex "quit"
