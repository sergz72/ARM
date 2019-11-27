#include "board.h"
#ifdef I2C_SOFT
#include <i2c_soft.h>

void i2c_soft_init(int channel)
{
  SCL_HIGH(channel);
  SDA_HIGH(channel);
}

void i2c_soft_start(int channel)
{
  SDA_HIGH(channel);             // i2c start bit sequence
  i2c_dly;
  SCL_HIGH(channel);
  i2c_dly;
  SDA_LOW(channel);
  i2c_dly;
  SCL_LOW(channel);
  i2c_dly;
}

void i2c_soft_stop(int channel)
{
  SDA_LOW(channel);             // i2c stop bit sequence
  i2c_dly;
  SCL_HIGH(channel);
  i2c_dly;
  SDA_HIGH(channel);
  i2c_dly;
}

unsigned char i2c_soft_rx(int channel, unsigned int ack)
{
  unsigned int x;
  unsigned char d = 0;

  SDA_HIGH(channel); 
  for(x = 0; x < 8; x++)
  {
    d <<= 1;
    i2c_dly;
    SCL_HIGH(channel);
    i2c_dly;
    if (SDA_IN(channel)) d |= 1;
    SCL_LOW(channel);
  } 
  if (ack)
    SDA_LOW(channel);
  else
    SDA_HIGH(channel);
  SCL_HIGH(channel);
  i2c_dly;             // send (N)ACK bit
  SCL_LOW(channel);
  SDA_HIGH(channel);

  return d;
}

unsigned int i2c_soft_tx(int channel, unsigned char d)
{
  unsigned int b, x;
  for(x = 8; x > 0; x--) {
    if (d & 0x80)
      SDA_HIGH(channel);
    else
      SDA_LOW(channel);
    i2c_dly;
    SCL_HIGH(channel);
    i2c_dly;
    SCL_LOW(channel);
    d <<= 1;
  }
  SDA_HIGH(channel);
  i2c_dly;
  b = SDA_IN(channel);          // possible ACK bit
  SCL_HIGH(channel);
  i2c_dly;
  SCL_LOW(channel);

  return b;
}

int i2c_soft_command(int channel, unsigned int address, unsigned char *commands, unsigned int commands_length,
                     unsigned char *out_data, unsigned int out_data_length,
                     unsigned char *in_data, unsigned int in_data_length)
{
  unsigned int i;

  i2c_soft_start(channel);              // send start sequence
  if (i2c_soft_tx(channel, address)) // no ack
	{
		i2c_soft_stop(channel);
    return 1;
	}

  if (commands_length > 0)
  {
    for (i = 0; i < commands_length; i++)
		{
		  if (i2c_soft_tx(channel, commands[i]))
			{
				i2c_soft_stop(channel);
				return 1;
			}
		}
  }

  if (out_data_length > 0)
  {
    for (i = 0; i < out_data_length; i++)
		{
      if (i2c_soft_tx(channel, out_data[i]))
			{
				i2c_soft_stop(channel);
				return 1;
			}
		}
  }

  if (in_data_length != 0)
  {
    i2c_soft_start(channel);              // send a restart sequence
    i2c_soft_tx(channel, address | 1);
    for (i = 0; i < in_data_length - 1; i++)
      i2c_soft_rx(channel, 1);
    i2c_soft_rx(channel, 0); //we don't acknowledge the last byte.
  }
  i2c_soft_stop(channel);               // send stop sequence

  return 0;
}
#endif
