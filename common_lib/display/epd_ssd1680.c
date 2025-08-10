#include "board.h"
#include <epd_ssd1680.h>
#include <string.h>

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

#ifndef SSD1680_RAM_SIZE
#define SSD1680_RAM_SIZE 400
#endif
static unsigned char bw_ram[SSD1680_RAM_SIZE];
static unsigned char red_ram[SSD1680_RAM_SIZE];
static unsigned char *bw_buffer_pointer;
static unsigned char *red_buffer_pointer;

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

static unsigned char calculate_x(unsigned short x, unsigned short y)
{
#if (SSD1680_DATA_ENTRY_MODE & 1) == 0 // X decrement
#ifdef LCD_SWAPXY
  return (LCD_HEIGHT - y - 1) >> 3;
#else
  return (LCD_WIDTH - x - 1) >> 3;
#endif
#else
#ifdef LCD_SWAPXY
  return y >> 3;
#else
  return x >> 3;
#endif
#endif
}

static unsigned short calculate_y(unsigned short x, unsigned short y)
{
#if (SSD1680_DATA_ENTRY_MODE & 2) == 0 // Y decrement
#ifdef LCD_SWAPXY
  return LCD_WIDTH - x - 1;
#else
  return LCD_HEIGHT - y - 1;
#endif
#else
#ifdef LCD_SWAPXY
  return x;
#else
  return y;
#endif
#endif
}

void ssd1680_set_ram_x_counter(unsigned short x, unsigned short y)
{
  x = calculate_x(x, y);
  ssd1680_command(COMMAND_SET_RAM_X_COUNTER, (unsigned char*)&x, 1);
}

void ssd1680_set_ram_y_counter(unsigned short x, unsigned short y)
{
  y = calculate_y(x, y);
  ssd1680_command(COMMAND_SET_RAM_Y_COUNTER, (unsigned char*)&y, 2);
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
  unsigned char xx1 = calculate_x(x1, y1);
  unsigned char xx2 = calculate_x(x2, y2);
  unsigned short yy1 = calculate_y(x1, y1);
  unsigned short yy2 = calculate_y(x2, y2);
  unsigned char data[4];
  data[0] = xx1;
  data[1] = xx2;
  ssd1680_command(COMMAND_SET_RAMXPOS, data, 2); // set x range
  data[0] = (unsigned char)yy1;
  data[1] = (unsigned char)(yy1 >> 8);
  data[2] = (unsigned char)yy2;
  data[3] = (unsigned char)(yy2 >> 8);
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

void ssd1680_clear_screen_buffers(void)
{
  memset(bw_ram, 0xFF, sizeof(bw_ram));
  memset(red_ram, 0, sizeof(red_ram));
}

void ssd1680_init(void)
{
  ssd1680_hard_reset();
  ssd1680_soft_reset();
  ssd1680_wait();
  ssd1680_set_driver_control(0);
  ssd1680_set_data_entry_mode(SSD1680_DATA_ENTRY_MODE);
  ssd1680_set_border_waveform(BORDER_WAVEFORM_FOLLOW_LUT|BORDER_WAVEFORM_LUT1);
  ssd1680_select_temp_sensor(INTERNAL_TEMP_SENSOR);
  ssd1680_display_update_control(0x00, 0x80);
  ssd1680_set_display_update_mode(DISPLAY_MODE_1);

  //ssd1680_set_vcom_voltage(0x36);
  //ssd1680_set_gate_voltage(0x17);
  //ssd1680_set_source_voltage(0x41, 0, 0x32); // vsh2 = 0
  //ssd1680_booster_soft_start(0x80, 0x90, 0x90, 0x00);

  ssd1680_clear_screen_buffers();
}

static void process_transformed_byte(unsigned char data, enum SSD1680_Color textColor, enum SSD1680_Color bkColor)
{
  unsigned char red_data = 0;
  switch (textColor)
  {
    case ColorBlack:
      data ^= 0xFF;
      if (bkColor == ColorRed)
        red_data = data;
      break;
    case ColorRed:
      red_data = data;
      data = bkColor == ColorBlack ? 0 : 0xFF;
      break;
    default: // white
      break;
  }
  *red_buffer_pointer++ = red_data;
  *bw_buffer_pointer++ = data;
}

void ssd1680_draw_char(unsigned short x, unsigned short y, char c, const FONT_INFO *f, enum SSD1680_Color textColor,
                        enum SSD1680_Color bkColor)
{
  unsigned int font_width = (f->character_max_width + f->character_spacing) & ~0x7;
  unsigned int font_width_bytes = font_width >> 3;
  unsigned int character_bytes = font_width_bytes * f->char_height;
  const unsigned char *character_pointer = f->char_bitmaps + (c - f->start_character) * character_bytes;
  bw_buffer_pointer = bw_ram;
  red_buffer_pointer = red_ram;
  const unsigned char *p = character_pointer;
  unsigned int h = f->char_height & ~0x7;
  for (unsigned int i = 0; i < h; i += 8)
  {
    const unsigned char *ppp = p;
    for (unsigned int b = 0; b < font_width_bytes; b++)
    {
      unsigned char mask = 0x80;
      for (int xx = 0; xx < 8; xx++)
      {
        const unsigned char *pp = ppp;
        unsigned char transformed = 0;
        for (int yy = 0; yy < 8; yy++)
        {
          transformed <<= 1;
          unsigned char data = *pp;
          if (data & mask)
            transformed |= 1;
          pp += font_width_bytes;
        }
        process_transformed_byte(transformed, textColor, bkColor);
        mask >>= 1;
      }
      ppp++;
    }
    p += font_width_bytes * 8;
  }
  unsigned short x2 = x + font_width - 1;
  unsigned short y2 = y + h - 1;
  ssd1680_set_window(x, x2, y, y2);
//  Log("x", x);
//  Log("y", y);
  ssd1680_set_ram_x_counter(x, y);
  ssd1680_set_ram_y_counter(x, y);
  character_bytes = font_width_bytes * h;
  ssd1680_write_red_ram(red_ram, character_bytes);
  ssd1680_set_ram_x_counter(x, y);
  ssd1680_set_ram_y_counter(x, y);
  ssd1680_write_bw_ram(bw_ram, character_bytes);
}
