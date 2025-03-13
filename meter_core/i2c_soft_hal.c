#include "board.h"
#include <i2c_soft.h>
#include "devices.h"

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

int si5351_write(unsigned char device_address, int channel, const unsigned char *data, unsigned int length)
{
  return i2c_soft_command(channel, device_address, NULL, 0, data, length, NULL, 0, I2C_TIMEOUT);
}

int mcp9600Read16(int channel, unsigned char address, unsigned char reg, unsigned short *data)
{
  return inaReadRegister(channel, address, reg, data);
}

int mcp9600Read8(int channel, unsigned char address, unsigned char reg, unsigned char *data)
{
  return i2c_soft_command(channel, address, &reg, 1, NULL, 0, data, 1, I2C_TIMEOUT);
}

int mcp9600Write(int channel, unsigned char address, unsigned char reg, unsigned char data)
{
  return i2c_soft_command(channel, address, &reg, 1, &data, 1, NULL, 0, I2C_TIMEOUT);
}

int i2c_transfer(struct _DeviceObject *o, const void *txdata, unsigned int txdatalength, void *rxdata,
                        unsigned int rxdatalength)
{
  return i2c_soft_command(o->idx, o->device->device_id, (unsigned char*)&o->subdevice, 1,
                    txdata, txdatalength, rxdata, rxdatalength, I2C_TIMEOUT);
}
