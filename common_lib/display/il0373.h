#ifndef _IL0373_H
#define _IL0373_H

#include <board.h>

#define IL0373_DISPLAY_BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT / 8)

#define IL0373_COMMAND_PANEL_SETTING              0U
#define IL0373_COMMAND_POWER_SETTING              1U
#define IL0373_COMMAND_POWER_OFF                  2U
#define IL0373_COMMAND_POWER_OFF_SEQUENCE_SETTING 3U
#define IL0373_COMMAND_POWER_ON                   4U
#define IL0373_COMMAND_POWER_ON_MEASURE           5U
#define IL0373_COMMAND_BOOSTER_SOFT_START         6U
#define IL0373_COMMAND_DEEP_SLEEP                 7U
#define IL0373_COMMAND_START_DATA_TRANSMISSION1   0x10U
#define IL0373_COMMAND_DATA_STOP                  0x11U
#define IL0373_COMMAND_DISPLAY_REFRESH            0x12U
#define IL0373_COMMAND_START_DATA_TRANSMISSION2   0x13U
#define IL0373_COMMAND_VCOM_LUT                   0x20U
#define IL0373_COMMAND_W2W_LUT                    0x21U
#define IL0373_COMMAND_PLL_CONTROL                0x30U
#define IL0373_COMMAND_TEMPERATURE_SENSOR_CALIB   0x40U
#define IL0373_COMMAND_TEMPERATURE_SENSOR_ENABLE  0x41U
#define IL0373_COMMAND_TEMPERATURE_SENSOR_WRITE   0x42U
#define IL0373_COMMAND_TEMPERATURE_SENSOR_READ    0x43U
#define IL0373_COMMAND_VCOM_DATA_INTERVAL_SETTING 0x50U
#define IL0373_COMMAND_LOW_POWER_DETECTION        0x51U
#define IL0373_COMMAND_TCON_SETTING               0x60U
#define IL0373_COMMAND_RESOLUTION_SETTING         0x61U
#define IL0373_COMMAND_GET_REVISION               0x70U
#define IL0373_COMMAND_GET_STATUS                 0x71U
#define IL0373_COMMAND_AUTO_MEASURE_VCOM          0x80U
#define IL0373_COMMAND_VCOM_VALUE                 0x81U
#define IL0373_COMMAND_VCOM_DC_SETTING            0x82U
#define IL0373_COMMAND_PARTIAL_WINDOW             0x90U
#define IL0373_COMMAND_PARTIAL_IN                 0x91U
#define IL0373_COMMAND_PARTIAL_OUT                0x92U
#define IL0373_COMMAND_PROGRAM_MODE               0xA0U
#define IL0373_COMMAND_ACTIVE_PROGRAM             0xA1U
#define IL0373_COMMAND_READ_OTP_DATA              0xA2U
#define IL0373_COMMAND_CASCADE_SETTING            0xE0U
#define IL0373_COMMAND_FORCE_TEMPERATURE          0xE5U

#define IL0373_COLOR_WHITE 2U
#define IL0373_COLOR_RED   1U
#define IL0373_COLOR_BLACK 0U

unsigned int ili0373_writeCommand(unsigned char command);
unsigned int ili0373_writeData(unsigned char data);
void ili0373_waitWhileBusy(void);

#endif
