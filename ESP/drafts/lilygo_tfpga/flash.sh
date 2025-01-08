#! /bin/sh

. /home/sergzz/esp/esp-idf/export.sh

idf.py flash
#C:\Users\sergi\.espressif\python_env\idf5.0_py3.10_env\Scripts\python.exe C:\Espressif\frameworks\esp-idf-v5.0\components/esptool_py/esptool/esptool.py --no-stub --chip esp32s3 -p COM3 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size 2MB 0x0 build/bootloader/bootloader.bin 0x10000 build/main.bin 0x8000 build/partition_table/partition-table.bin
#C:\Espressif\python_env\idf5.0_py3.10_env\Scripts\python.exe C:\Espressif\frameworks\esp-idf-v5.0\components\esptool_py\esptool\esptool.py --no-stub -p COM4 -b 460800 --before default_reset --after hard_reset --chip esp32s3  write_flash --flash_mode dio --flash_size 2MB --flash_freq 80m 0x0 build\bootloader\bootloader.bin 0x8000 build\partition_table\partition-table.bin 0x10000 build\main.bin
