#include <board.h>
#include <th02.h>

#define TH02_REGISTER_STATUS 0
#define TH02_REGISTER_DATAH 1
#define TH02_REGISTER_DATAL 2
#define TH02_REGISTER_CONFIG 3
#define TH02_REGISTER_ID 0x11

unsigned int th02_check(void)
{
  unsigned char id;
  unsigned int rc = th02Read(TH02_REGISTER_ID, &id, 1);
  if (!rc)
    return 0;
  return id == 0x50;
}

unsigned int th02_get_temperature(int *result)
{
  unsigned char status;
  unsigned short v;

  unsigned int rc = th02Write(TH02_REGISTER_CONFIG, 0x11);
  if (!rc)
    return 0;

  do
  {
    delayms(10);
    rc = th02Read(TH02_REGISTER_STATUS, &status, 1);
    if (!rc)
      return 0;
  } while (status);

  rc = th02Read(TH02_REGISTER_DATAH, (unsigned char*)&v, 2);
  if (!rc)
    return 0;
  v = (((v & 0xFFU) << 8U) | (v >> 8U)) >> 2;

  *result = (((int)v * 10) >> 5) - 500;

  return 1;
}

unsigned int th02_get_humidity(unsigned int *result)
{
  unsigned char status;
  unsigned short v;

  unsigned int rc = th02Write(TH02_REGISTER_CONFIG, 0x01);
  if (!rc)
    return 0;

  do
  {
    delayms(10);
    rc = th02Read(TH02_REGISTER_STATUS, &status, 1);
    if (!rc)
      return 0;
  } while (status);

  rc = th02Read(TH02_REGISTER_DATAH, (unsigned char*)&v, 2);
  if (!rc)
    return 0;
  v = (((v & 0xFFU) << 8U) | (v >> 8U)) >> 4;

  *result = ((int)v >> 4) - 24;

  return 1;
}