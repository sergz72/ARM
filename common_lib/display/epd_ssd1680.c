#include "board.h"
#include <epd_ssd1680.h>

#define INTERNAL_TEMP_SENSOR       0x80
#define BORDER_WAVEFORM_FOLLOW_LUT 0b0100
#define BORDER_WAVEFORM_LUT1       0b0001
#define DISPLAY_MODE_1             0xF7

#define COMMAND_DRIVER_CONTROL          0x01
#define COMMAND_GATE_CONTROL            0x03
#define COMMAND_SOURCE_VOLTAGE_CONTROL  0x04
#define COMMAND_BOOSTER_SOFT_START      0x0C
#define COMMAND_DATA_ENTRY_MODE         0x11
#define COMMAND_SW_RESET                0x12
#define COMMAND_TEMP_CONTROL            0x18
#define COMMAND_MASTER_ACTIVATION       0x20
#define COMMAND_DISPLAY_UPDATE_CONTROL  0x21
#define COMMAND_DISPLAY_UPDATE_CONTROL2 0x22
#define COMMAND_WRITE_BW_RAM            0x24
#define COMMAND_WRITE_RED_RAM           0x26
#define COMMAND_VCOM_CONTROL            0x2C
#define COMMAND_BORDER_WAVEFORM_CONTROL 0x3C
#define COMMAND_SET_RAMXPOS             0x44
#define COMMAND_SET_RAMYPOS             0x45
#define COMMAND_WRITE_RED_RAM_PATTERN   0x46
#define COMMAND_WRITE_BW_RAM_PATTERN    0x47
#define COMMAND_SET_RAM_X_COUNTER       0x4E
#define COMMAND_SET_RAM_Y_COUNTER       0x4F

static void ssd1680_soft_reset(void)
{
  ssd1680_command(COMMAND_SW_RESET, NULL, 0);
}

static void ssd1680_set_data_entry_mode(unsigned char mode)
{
  ssd1680_command(COMMAND_DATA_ENTRY_MODE, &mode, 1);
}

static void ssd1680_set_border_waveform(unsigned char waveform)
{
  ssd1680_command(COMMAND_BORDER_WAVEFORM_CONTROL, &waveform, 1);
}

static void ssd1680_set_vcom_voltage(unsigned char voltage)
{
  ssd1680_command(COMMAND_VCOM_CONTROL, &voltage, 1);
}

static void ssd1680_set_gate_voltage(unsigned char voltage)
{
  ssd1680_command(COMMAND_GATE_CONTROL, &voltage, 1);
}

static void ssd1680_set_source_voltage(unsigned char voltage_a, unsigned char voltage_b, unsigned char voltage_c)
{
  unsigned char data[3] = {voltage_a, voltage_b, voltage_c};
  ssd1680_command(COMMAND_SOURCE_VOLTAGE_CONTROL, data, 3);
}

void ssd1680_set_ram_x_counter(unsigned char counter)
{
  ssd1680_command(COMMAND_SET_RAM_X_COUNTER, &counter, 1);
}

void ssd1680_set_ram_y_counter(unsigned short counter)
{
  ssd1680_command(COMMAND_SET_RAM_Y_COUNTER, (unsigned char*)&counter, 2);
}

static void ssd1680_set_driver_control(unsigned char scanning_sequence)
{
  unsigned char data[3] = {(unsigned char)(LCD_WIDTH-1), (unsigned char)((LCD_WIDTH-1)>>8), scanning_sequence};
  ssd1680_command(COMMAND_DRIVER_CONTROL, data, 3);
}

static void ssd1680_set_display_update_mode(unsigned char update_mode)
{
  ssd1680_command(COMMAND_DISPLAY_UPDATE_CONTROL2, &update_mode, 1);
}

static void ssd1680_display_update_control(unsigned char a, unsigned char b)
{
  unsigned char data[2] = {a, b};
  ssd1680_command(COMMAND_DISPLAY_UPDATE_CONTROL, data, 2);
}

static void ssd1680_select_temp_sensor(unsigned char sensor)
{
  ssd1680_command(COMMAND_TEMP_CONTROL, &sensor, 1);
}

static void ssd1680_hard_reset()
{
  SSD1680_RES_CLR;
  delayms(1);
  SSD1680_RES_SET;
  delayms(1);
}

static void ssd1680_wait(void)
{
  while (SSD1680_GET_BUSY)
    delayms(1);
}

void ssd1680_set_window(unsigned short x1, unsigned short x2, unsigned short y1, unsigned short y2)
{
  unsigned char data[4];
#ifdef LCD_SWAPXY
  data[0] = (unsigned char)(y1 >>= 3);
  data[1] = (unsigned char)(y2 >>= 3);
#else
  data[0] = (unsigned char)(x1 >>= 3);
  data[1] = (unsigned char)(x2 >>= 3);
#endif
  ssd1680_command(COMMAND_SET_RAMXPOS, data, 2); // set x range
#ifdef LCD_SWAPXY
  data[0] = (unsigned char)x1;
  data[1] = (unsigned char)(x1 >> 8);
  data[2] = (unsigned char)x2;
  data[3] = (unsigned char)(x2 >> 8);
#else
  data[0] = (unsigned char)y1;
  data[1] = (unsigned char)(y1 >> 8);
  data[2] = (unsigned char)y2;
  data[3] = (unsigned char)(y2 >> 8);
#endif
  ssd1680_command(COMMAND_SET_RAMYPOS, data, 4); // set y range
}

static void ssd1680_write_red_ram_with_pattern(unsigned char pattern)
{
  ssd1680_command(COMMAND_WRITE_RED_RAM_PATTERN, &pattern, 1);
  ssd1680_wait();
}

static void ssd1680_write_bw_ram_with_pattern(unsigned char pattern)
{
  ssd1680_command(COMMAND_WRITE_BW_RAM_PATTERN, &pattern, 1);
  ssd1680_wait();
}

void ssd1680_ram_fill(enum SSD1680_Pattern kx, enum SSD1680_Pattern ky, enum SSD1680_Pattern rx,
                      enum SSD1680_Pattern ry, enum SSD1680_Color color)
{
  uint8_t pattern = (ky << 4) | kx | ((color & 1) << 7);
  ssd1680_write_bw_ram_with_pattern(pattern);
  ssd1680_wait();
  pattern = (ry << 4) | rx | ((color & 2) << 6);
  ssd1680_write_red_ram_with_pattern(pattern);
  ssd1680_wait();
}

void ssd1680_fill_rect(unsigned short x1, unsigned short dx, unsigned short y1, unsigned short dy, enum SSD1680_Color color)
{
  ssd1680_set_window(x1, x1 + dx - 1, y1, y1 + dy - 1);
  ssd1680_ram_fill(PatternSolid, PatternSolid, PatternSolid, PatternSolid, color);
}

void ssd1680_fill_screen(enum SSD1680_Color color)
{
  ssd1680_set_window(0, LCD_WIDTH-1, 0, LCD_HEIGHT-1);
  ssd1680_ram_fill(PatternSolid, PatternSolid, PatternSolid, PatternSolid, color);
}

void ssd1680_write_red_ram(unsigned char *data, unsigned int length)
{
  ssd1680_command(COMMAND_WRITE_RED_RAM, data, length);
}

void ssd1680_write_bw_ram(unsigned char *data, unsigned int length)
{
  ssd1680_command(COMMAND_WRITE_BW_RAM, data, length);
}

static void ssd1680_booster_soft_start(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
  unsigned char data[4] = {a, b, c, d};
  ssd1680_command(COMMAND_BOOSTER_SOFT_START, data, 4);
}

static void ssd1680_master_activation(void)
{
  ssd1680_command(COMMAND_MASTER_ACTIVATION, NULL, 0);
}

void ssd1680_refresh(void)
{
  ssd1680_master_activation();
  ssd1680_wait();
}

void ssd1680_init(unsigned char data_entry_mode)
{
  ssd1680_hard_reset();
  ssd1680_soft_reset();
  ssd1680_wait();
  ssd1680_set_driver_control(0);
  ssd1680_set_data_entry_mode(data_entry_mode);
  ssd1680_set_border_waveform(BORDER_WAVEFORM_FOLLOW_LUT|BORDER_WAVEFORM_LUT1);
  ssd1680_select_temp_sensor(INTERNAL_TEMP_SENSOR);
  ssd1680_display_update_control(0x00, 0x80);
  ssd1680_set_display_update_mode(DISPLAY_MODE_1);

  //ssd1680_set_vcom_voltage(0x36);
  //ssd1680_set_gate_voltage(0x17);
  //ssd1680_set_source_voltage(0x41, 0, 0x32); // vsh2 = 0
  //ssd1680_set_ram_x_counter(1);
  //ssd1680_set_ram_y_counter(0);
  //ssd1680_booster_soft_start(0x80, 0x90, 0x90, 0x00);
}
