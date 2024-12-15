#include <string.h>
#include "board.h"

#define SIZE (LCD_HWHEIGHT * LCD_HWWIDTH / 8)

extern int keyboard_state;
static unsigned int temperature = 20;
static unsigned int coef = 0;
static int isOn = 0;
static unsigned char lcd_buffer[SIZE], *lcd_buffer_p = lcd_buffer;

void delayms(unsigned int ms)
{
}

int SSD1306_I2C_Write(int num_bytes, unsigned char control_byte, unsigned char *buffer)
{
  if (control_byte == 0x40)
  {
    memcpy(lcd_buffer_p, buffer, num_bytes);
    lcd_buffer_p += num_bytes;
    if (lcd_buffer_p - lcd_buffer >= SIZE)
      lcd_buffer_p = lcd_buffer;
  }
  return 0;
}

unsigned int get_keyboard_status(void)
{
  unsigned int state = keyboard_state;
  keyboard_state = 0xF;
  return state;
}

void off(void)
{
  isOn = 0;
}

void on(void)
{
  isOn = 1;
}

unsigned int get_temperature(void)
{
  if (coef > 5)
    temperature++;
  else if (temperature > 20)
    temperature--;
  if (isOn)
  {
    if (coef < 10)
      coef++;
  }
  else
  {
    if (coef > 0)
      coef--;
  }

  return temperature;
}

int get_lcd_buffer_bit(int x, int y)
{
  unsigned char *p = lcd_buffer + x + (y >> 3U) * LCD_WIDTH;
  int bit = y & 7;
  return *p & (1 << bit);
}
