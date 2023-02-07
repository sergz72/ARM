#include "board.h"
#include <tm1638.h>
#include <spi_3wire_soft.h>

#define TM_ACTIVATE 0x8F // display on
#define TM_DEACTIVATE 0x80 // display off
#define TM_BUTTONS_MODE 0x42 // Buttons mode
#define TM_WRITE_LOC 0x44 // Write to a location
#define TM_WRITE_INC 0x40 // Incremental write
#define TM_SEG_ADR 0xC0  // leftmost segment Address C0 C2 C4 C6 C8 CA CC CE
#define TM_LEDS_ADR 0xC1  // Leftmost LED address C1 C3 C5 C7 C9 CB CD CF
#define TM_BRIGHT_ADR 0x88 // Brightness address
#define TM_BRIGHT_MAX 0x07 // Brightness maximum
#define TM_DEFAULT_BRIGHTNESS 0x02 //can be 0x00 to 0x07 
#define TM_DISPLAY_SIZE 8 //size of display

static void sendCommand(unsigned char command)
{
  SPI3_CS_CLR; // clear CS
  SPI3_CLK_CLR;
  spi3_byte_out(command, 0);
  SPI3_CS_SET; // set CS
}

static void reset(void)
{
  int i;

  sendCommand(TM_WRITE_INC); // set auto increment mode
  SPI3_CS_CLR; // clear CS
  SPI3_CLK_CLR;
  spi3_byte_out(TM_SEG_ADR, 1);
  for (i = 15; i >= 0; i--)
    spi3_byte_out(0, i);
  SPI3_CS_SET; // set CS
}

void tm1638_brightness(unsigned char brightness)
{
  if (brightness > TM_BRIGHT_MAX)
    brightness = TM_BRIGHT_MAX;
  sendCommand(TM_BRIGHT_ADR + brightness);
}

void tm1638_display_on(void)
{
  sendCommand(TM_ACTIVATE);
}

void tm1638_display_off(void)
{
  sendCommand(TM_DEACTIVATE);
}

void tm1638_init(void)
{
  tm1638_display_on();
  tm1638_brightness(TM_DEFAULT_BRIGHTNESS);
  reset();
}

void tm1638_set_led(unsigned char position, unsigned char state)
{
  sendCommand(TM_WRITE_LOC);
  spi3_command(TM_LEDS_ADR + (position << 1), &state, 1, NULL, 0, 1, 0);
}

void tm1638_set_leds(unsigned char state)
{
  int i;

  for (i = 0; i < 8; i++)
  {
    tm1638_set_led(i, state & 1);
    state >>= 1;
  }
}

unsigned int tm1638_in(int num_bytes)
{
  union {
    unsigned char b[4];
    unsigned int i;
  } data;

  if (num_bytes > 0 && num_bytes <= 4)
  {
    data.i = 0;
    spi3_command(TM_BUTTONS_MODE, NULL, 0, data.b, num_bytes, 1, 2);
    return data.i;
  }

  return 0;
}

unsigned int tm1638_key_state(void)
{
  int rc = 0;
  int state = tm1638_in(4);
  if (state & 0x0F)
    rc |= 1;
  if (state & 0xF0)
    rc |= 0x10;
  if (state & 0xF00)
    rc |= 2;
  if (state & 0xF000)
    rc |= 0x20;
  if (state & 0xF0000)
    rc |= 4;
  if (state & 0xF00000)
    rc |= 0x40;
  if (state & 0xF000000)
    rc |= 8;
  if (state & 0xF0000000)
    rc |= 0x80;
  return rc;
}

void tm1638_out(int pos, int length, unsigned char *values)
{
  while (pos < TM_DISPLAY_SIZE && length)
  {
    sendCommand(TM_WRITE_LOC);
    spi3_command(TM_SEG_ADR + (pos << 1), values++, 1, NULL, 0, 1, 0);
    pos++;
    length--;
  }
}
