#include "board.h"
#include <bh1750.h>

int bh1750_measure(unsigned char mode, unsigned short *result)
{
  unsigned char data[2];

  int rc = bh1750_write(mode);
  if (rc)
    return rc;
  delayms(500);
  rc = bh1750_read(data);
  if (rc)
    return rc;
  *result = ((unsigned short)data[0] << 8) | (unsigned short)data[1];
  return 0;
}

int bh1750_set_measurement_time(unsigned char mtreg)
{
  int rc = bh1750_write((0b01000 << 3) | (mtreg >> 5));
  if (rc)
    return rc;
  return bh1750_write((0b011 << 5) | (mtreg & 0b11111));
}