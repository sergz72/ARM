# Raspberry PI pico tester for various devices

Uses command line interface via USB CDC.

**Can test:**
- I2C devices
- LED boards with MAX7219 controler.
- LED/keyboard boards with TM1638 controller.
- DDS adapters for [STM32401 Blue pill universal measuring tool](../stm32f401_meter)

**Pins:**

**I2C:**
- SDA: GP4
- SCL: GP5

**TM1638 (PLEASE USE 3.3v supply!)**
- CLK: GP0
- DIO: GP1
- CS: GP2

**MAX7219:**
- CLK: GP16
- CS: GP17
- First module DIN: GP18
- Second module DIN: GP19
- Third module DIN: GP20
- Fourth module DIN: GP21