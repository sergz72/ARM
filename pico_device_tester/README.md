# Raspberry PI pico tester for various devices

Uses command line interface using USB CDC.

**Can test:**
- I2C devices
- LED boards with MAX7219 controler.
- LED/keyboard boards with TM1638 controller.
- DDS adapters for [STM32401 Blue pill universal measuring tool](../stm32f401_meter)

**Pins:**

**I2C:** 4, 5

**TM1638 (PLEASE USE 3.3v supply!)**
- CLK: 0
- DIO: 1
- CS: 2

**MAX7219:**
- CLK: 16
- CS: 17
- First module DIO: 18
- Second module DIO: 19
- Third module DIO: 20
- Fourth module DIO: 21
