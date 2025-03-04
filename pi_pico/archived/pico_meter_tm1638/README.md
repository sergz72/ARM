# Raspberry PI pico frequency/capacitance meter

Uses [TM1638 LED/keyboard](https://www.amazon.de/gp/product/B07Y9QB74M)

**Pins:**

**TM1638 (PLEASE USE 3.3v supply!):**
- SCK: GP0
- DIO: GP1
- CS: GP2

**Capacitance meter:**
- CX input: between GP28 and GND
- Resistor 220 Ohm between GP28 and GP26
- Resistor 100 kOhm between GP28 and GP27

**Frequency meter:**
- Input pin: GP18

**Generator 1.25 MHZ for testing:**
- Output pin: GP16
