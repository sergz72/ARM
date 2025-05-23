#! /bin/sh
/opt/arm/bin/arm-none-eabi-gdb -q -iex "target remote tcp:127.0.0.1:2331" -iex "monitor reset" -iex "monitor go" -iex "disconnect" -iex "quit"
