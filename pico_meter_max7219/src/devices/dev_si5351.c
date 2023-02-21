#include <malloc.h>
#include "dev_si5351.h"
#include <si5351.h>
#include "ui.h"

void* si5351_initializer(void)
{
  si5351_init();

  si5351_write(0, SI5351_CRYSTAL_LOAD, SI5351_CRYSTAL_LOAD_8PF);

  return NULL;
}

void si5351_ui_init_handler(void* config)
{
  LED_ClearScreen();
  LED_Write_String(3, 3, "F1F2F3  ");
}

int si5351_ui_keyboard_handler(void *config, unsigned int event)
{
  return 0;
}
