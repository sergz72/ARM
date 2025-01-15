#include "board.h"
#include <i2c_soft.h>
#include <ina226.h>
#include <mcp3421.h>
#include <ads1115.h>

void SCL_HIGH(int channel)
{
  switch (channel)
  {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    default:
      break;
  }
}

void SCL_LOW(int channel)
{
  switch (channel)
  {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    default:
      break;
  }
}

void SDA_HIGH(int channel)
{
  switch (channel)
  {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    default:
      break;
  }
}

void SDA_LOW(int channel)
{
  switch (channel)
  {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    default:
      break;
  }
}

int SDA_IN(int channel)
{
  switch (channel)
  {
    case 0:
      return 0;
    case 1:
      return 0;
    case 2:
      return 0;
    default:
      return 0;
  }
}

int SCL_IN(int channel)
{
  switch (channel)
  {
    case 0:
      return 0;
    case 1:
      return 0;
    case 2:
      return 0;
    default:
      return 0;
  }
}

int I2C_SendAddress(int idx, int address)
{
  i2c_soft_start(idx);

  if (i2c_soft_tx(idx, address, I2C_TIMEOUT)) // no ack
  {
    i2c_soft_stop(idx);
    return 1;
  }

  return 0;
}

int I2CCheck(int idx, int device_id)
{
  int rc;

  rc = I2C_SendAddress(idx, device_id);
  if (!rc)
    i2c_soft_stop(idx);
  return rc == 0;
}

int inaReadRegister(int channel, unsigned char address, unsigned char reg, unsigned short *data)
{
  unsigned char d[2];
  int rc = i2c_soft_command(channel, address, &reg, 1, NULL, 0, d, 2, I2C_TIMEOUT);
  if (!rc)
    *data = (d[0] << 8) | d[1];
  return rc;
}

int ads1115ReadRegister(int channel, unsigned char address, unsigned char reg, unsigned short *data)
{
  return inaReadRegister(channel, address, reg, data);
}

int mcp3421Read(int channel, unsigned char address, unsigned char *data, unsigned int l)
{
  return i2c_soft_read(channel, address, data, l, I2C_TIMEOUT);
}

int inaWriteRegister(int channel, unsigned char address, unsigned char reg, unsigned short data)
{
  unsigned char d[2];
  d[0] = data >> 8;
  d[1] = data & 0xFF;
  return i2c_soft_command(channel, address, &reg, 1, d, 2, NULL, 0, I2C_TIMEOUT);
}

int ads1115WriteRegister(int channel, unsigned char address, unsigned char reg, unsigned short data)
{
  return inaWriteRegister(channel, address, reg, data);
}

int mcp3421Write(int channel, unsigned char address, unsigned char data)
{
  return i2c_soft_command(channel, address, NULL, 0, &data, 1, NULL, 0, I2C_TIMEOUT);
}
