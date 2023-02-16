#include <board.h>
#include <max7219.h>

void max7219_init(unsigned short intensity)
{
  max7219_scan_limit(7);
  max7219_decode_mode(0);
  max7219_intensity(intensity);
  max7219_clear_screen();
  max7219_on();
}

void max7219_clear_screen(void)
{
  int i;
  unsigned char zero[MAX7219_NUM_PARALLEL * MAX7219_NUM_SERIES * 8];
  for (i = 0; i < MAX7219_NUM_PARALLEL * MAX7219_NUM_SERIES * 8; i++)
    zero[i] = 0;
  max7219_out(zero);
}

static void max7219_command(unsigned short command)
{
  int i, j;
  unsigned short buffer[MAX7219_NUM_PARALLEL][MAX7219_NUM_SERIES];
  unsigned short *d[MAX7219_NUM_PARALLEL];

  for (i = 0; i < MAX7219_NUM_PARALLEL; i++)
  {
    d[i] = buffer[i];
    for (j = 0; j < MAX7219_NUM_SERIES; j++)
      buffer[i][j] = command;
  }
  max7219_send(d);
}

static unsigned char data_convert(unsigned char data)
{
  unsigned char v = data & (128 + 8);
  if (data & 1)
    v |= 64;
  if (data & 2)
    v |= 32;
  if (data & 4)
    v |= 16;
  if (data & 16)
    v |= 4;
  if (data & 32)
    v |= 2;
  if (data & 64)
    v |= 1;
  return v;
}

void max7219_out(const unsigned char *data)
{
  int i, j, k;
  unsigned short buffer[MAX7219_NUM_PARALLEL][MAX7219_NUM_SERIES];
  unsigned short *d[MAX7219_NUM_PARALLEL];
  //for each digit
  for (i = 0; i < 8; i++)
  {
    // buffer init
    for (j = 0; j < MAX7219_NUM_PARALLEL; j++)
      d[j] = buffer[j];
    for (j = 0; j < MAX7219_NUM_PARALLEL; j++)
    {
      for (k = 0; k < MAX7219_NUM_SERIES; k++)
        buffer[j][k] = ((8 - i) << 8) | data_convert(data[i + k * 8 + (j * 8 * MAX7219_NUM_SERIES)]);
    }
    max7219_send(d);
  }
}

void max7219_decode_mode(unsigned short mode)
{
  max7219_command(0x0900 | (mode & 0xFF));
}

void max7219_scan_limit(unsigned short limit)
{
  max7219_command(0x0B00 | (limit & 0x07));
}

void max7219_intensity(unsigned short intensity)
{
  max7219_command(0x0A00 | (intensity & 0x0F));
}

void max7219_on(void)
{
  max7219_command(0x0C01);
}

void max7219_off(void)
{
  max7219_command(0x0C00);
}

void max7219_display_test_on(void)
{
  max7219_command(0x0F01);
}

void max7219_display_test_off(void)
{
  max7219_command(0x0F00);
}
