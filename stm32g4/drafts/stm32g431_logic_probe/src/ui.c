#include "board.h"
#include "ui.h"
#include <i2c.h>
#include <lcd_ssd1306.h>

void UI_Init(void)
{
  I2C_Write(I2C2, SSD1306_I2C_ADDRESS, NULL, 0, I2C_TIMEOUT); // workaround
  LcdInit();
  LcdScreenFill(BLACK_COLOR);
  LcdUpdate();
}

void Process_Timer_Event(void)
{
  LcdUpdate();
}
