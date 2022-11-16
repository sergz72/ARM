#include <board.h>
#include <max31865.h>

unsigned int max31865Read(void)
{
  unsigned char bufOut, bufIn[2];
  unsigned int result, tries;

  bufOut = 0x81; // turn bias and 50 hz filter on

  if (!MAX31865_TRANSFER(0x80, &bufOut, bufIn))
    return 1;

  delayms(10);

  bufOut = 0xA1; // start conversion

  if (!MAX31865_TRANSFER(0x80, &bufOut, bufIn))
    return 1;

  delayms(50);
  tries = 30; // till 200ms
  while (tries && MAX31865_DRDY_GET)
  {
    tries--;
    delayms(5);
  }
  if (!tries) // timeout
    return 1;

  if (!MAX31865_TRANSFER(1, NULL, bufIn))
    return 1;

  result = bufIn[1];
  result <<= 8;

  if (!MAX31865_TRANSFER(2, NULL, bufIn))
    return 1;

  result |= bufIn[1];

  bufOut = 0; // turn off bias

  if (!MAX31865_TRANSFER(0x80, &bufOut, bufIn))
    return 1;

  return result;
}
