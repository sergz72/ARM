#! /bin/sh
/opt/arm/bin/arm-none-eabi-gdb -q -iex "target remote tcp:127.0.0.1:2331" -iex "load /home/sergzz/serg/my/Programs/ARM_CLION/at32f4/blink/cmake-build-debug-arm/AT32F403_blink.elf" -iex "monitor reset" -iex "monitor go" -iex "disconnect" -iex "quit"
