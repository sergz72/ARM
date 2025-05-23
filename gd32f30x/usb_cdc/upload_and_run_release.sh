#! /bin/sh
/opt/arm/bin/arm-none-eabi-gdb -q -iex "target remote tcp:127.0.0.1:2331" -iex "load /home/sergzz/serg/my/Programs/ARM_CLION/gd32f30x/usb_cdc/cmake-build-release-arm/GD32F303_usb_cdc.elf" -iex "monitor reset" -iex "monitor go" -iex "disconnect" -iex "quit"
