#include "mh_z19b.h"
#ifdef MH_Z19b_DEBUG
#include <stdio.h>
#endif

static unsigned char checksum(unsigned char *data)
{
  unsigned char checksum = *data++;
  for (int i = 1; i < 7; i++)
    checksum += *data++;
  return ~checksum + 1;
}

static void mh_z19b_send_command(unsigned char b1, unsigned char b2, unsigned char b3)
{
  unsigned char data[9];
  data[0] = 0xFF;
  data[1] = 0x01;
  data[2] = b1;
  data[3] = b2;
  data[4] = b3;
  data[5] = 0;
  data[6] = 0;
  data[7] = 0;
  data[8] = checksum(&data[1]);
#ifdef MH_Z19b_DEBUG
  printf("checksum = %02x\n", data[8]);
#endif
  mh_z19b_send(data, 9);
}

void mh_z19b_set_range(int hi_range)
{
  mh_z19b_send_command(0x99, hi_range ? 5000 >> 8 : 2000 >> 8, hi_range ? 5000 & 0xFF : 2000 & 0xFF);
}

void mh_z19b_send_read_command(void)
{
  mh_z19b_send_command(0x86, 0, 0);
}

int mh_z19b_get_result(unsigned short *result)
{
  unsigned char data[9];
  int length = mh_z19b_read(data, 9);
#ifdef MH_Z19b_DEBUG
  printf("length = %d\n", length);
  printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
#endif
  if (length != 9)
    return 1;
  if (data[0] != 0xFF)
    return 2;
  if (data[1] != 0x86)
    return 3;
  if (data[8] != checksum(&data[1]))
    return 4;
  *result = ((unsigned short)data[2] << 8) | data[3];
  return 0;
}