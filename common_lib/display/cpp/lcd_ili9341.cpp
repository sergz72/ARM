extern "C" {
#include "board.h"
}
#include <lcd_lil9341.hpp>

#define DISPLAY_COMMAND_SWRESET                    1
#define DISPLAY_COMMAND_SLEEP                      0x10
#define DISPLAY_COMMAND_SLEEP_OUT                  0x11
#define DISPLAY_COMMAND_DISPLAY_INVERSION_OFF      0x20
#define DISPLAY_COMMAND_DISPLAY_INVERSION_ON       0x21
#define DISPLAY_COMMAND_DISPLAY_OFF                0x28
#define DISPLAY_COMMAND_DISPLAY_ON                 0x29
#define DISPLAY_COMMAND_COLUMN_ADDRESS_SET         0x2A
#define DISPLAY_COMMAND_PAGE_ADDRESS_SET           0x2B
#define DISPLAY_COMMAND_MEMORY_WRITE               0x2C
#define DISPLAY_COMMAND_VERTICAL_SCROLLING         0x33
#define DISPLAY_COMMAND_TEARING_EFFECT_LINE_ON     0x35
#define DISPLAY_COMMAND_MEMORY_ACCESS_CONTROL      0x36
#define DISPLAY_COMMAND_VERTICAL_SCROLLING_ADDRESS 0x37
#define DISPLAY_COMMAND_IDLE_MODE_OFF              0x38
#define DISPLAY_COMMAND_IDLE_MODE_ON               0x39
#define DISPLAY_COMMAND_PIXEL_FORMAT_SET           0x3A
#define DISPLAY_COMMAND_SET_BRIGHTNESS             0x51

#define DISPLAY_RESET_DELAY 15 // Reset delay is 10 us minimum

void LCD_ILI9341::Init()
{
  delay(DISPLAY_RESET_DELAY);
  ResetSet();
  delayms(120); // reset cancel according to datasheet
  SleepOut();
  unsigned char v = 0x55; // 16 bits/pixel
  interface->SendCommand(DISPLAY_COMMAND_PIXEL_FORMAT_SET, &v, 1);
  SetHWOrientation();
  On();
}

unsigned int LCD_ILI9341::InternalUpdate(bool all)
{
  interface->SendCommand(DISPLAY_COMMAND_MEMORY_WRITE, (unsigned char*)buffer1, bufferSize * 2);
  return 0;
}

void LCD_ILI9341::SleepOut()
{
  interface->SendCommand(DISPLAY_COMMAND_SLEEP_OUT, nullptr, 0);
  delayms(10);
}

void LCD_ILI9341::On()
{
  interface->SendCommand(DISPLAY_COMMAND_DISPLAY_ON, nullptr, 0);
}

void LCD_ILI9341::Off()
{
  interface->SendCommand(DISPLAY_COMMAND_DISPLAY_OFF, nullptr, 0);
}

unsigned int LCD_ILI9341::GetBufferOffset(unsigned int x, unsigned int y)
{
  switch (orientation)
  {
    case LCD_ORIENTATION_PORTRAIT:
    case LCD_ORIENTATION_PORTRAIT_REVERSED:
      return width * y + x;
    //LCD_ORIENTATION_LANDSCAPE
    //LCD_ORIENTATION_LANDSCAPE_REVERSED
    default:
      return height * x + y;
  }
}

void LCD_ILI9341::SetHWOrientation()
{
  unsigned char v;

  switch (orientation)
  {
    case LCD_ORIENTATION_PORTRAIT:
      v = 0x00;
      break;
    case LCD_ORIENTATION_LANDSCAPE:
      v = 0x40;
      break;
    case LCD_ORIENTATION_PORTRAIT_REVERSED:
      v = 0xC0;
      break;
    //LCD_ORIENTATION_LANDSCAPE_REVERSED
    default:
      v = 0x90;
      break;
  }
  interface->SendCommand(DISPLAY_COMMAND_MEMORY_ACCESS_CONTROL, &v, 1);
}