#include <i2c_func.h>

int i2c_wait_until_not_busy(i2c_type *i2cx, unsigned int timeout)
{
  while (i2c_flag_get(i2cx, I2C_BUSYF_FLAG) != RESET && timeout)
    timeout--;
  return timeout == 0;
}

int i2c_wait_flag_or_ackfail(i2c_type *i2cx, unsigned int flag, unsigned int timeout)
{
  while(i2c_flag_get(i2cx, flag) == RESET && timeout)
  {
    timeout--;
    /* check the ack fail flag */
    if(i2c_flag_get(i2cx, I2C_ACKFAIL_FLAG) != RESET)
    {
      /* generate stop condtion */
      i2c_stop_generate(i2cx);

      /* clear ack fail flag */
      i2c_flag_clear(i2cx, I2C_ACKFAIL_FLAG);

      return 1;
    }
  }

  return timeout == 0;
}

int i2c_master_write_addr(i2c_type *i2cx, unsigned short address, unsigned int timeout)
{
  /* generate start condtion */
  i2c_start_generate(i2cx);

  /* wait for the start flag to be set */
  while(i2c_flag_get(i2cx, I2C_STARTF_FLAG) == RESET && timeout)
    timeout--;
  if (!timeout)
    return 1;

  /* send slave address */
  i2c_7bit_address_send(i2cx, address, I2C_DIRECTION_TRANSMIT);

  return i2c_wait_flag_or_ackfail(i2cx, I2C_ADDR7F_FLAG, timeout);
}

int i2c_master_transmit(i2c_type *i2cx, unsigned short address, unsigned char *pdata, unsigned int size, unsigned int timeout)
{
  /* wait for the busy flag to be reset */
  if(i2c_wait_until_not_busy(i2cx, timeout))
  {
    return 1;
  }

  /* ack acts on the current byte */
  i2c_master_receive_ack_set(i2cx, I2C_MASTER_ACK_CURRENT);

  /* send slave address */
  if(i2c_master_write_addr(i2cx, address, timeout))
    return 2;

  /* clear addr flag */
  i2c_flag_clear(i2cx, I2C_ADDR7F_FLAG);

  while(size > 0)
  {
    /* wait for the tdbe flag to be set */
    if(i2c_wait_flag_or_ackfail(i2cx, I2C_TDBE_FLAG, timeout))
      return 3;

    /* write data */
    i2c_data_send(i2cx, *pdata++);
    size--;
  }

  /* wait for the tdc flag to be set */
  if(i2c_wait_flag_or_ackfail(i2cx, I2C_TDC_FLAG, timeout))
    return 4;

  /* generate stop condtion */
  i2c_stop_generate(i2cx);

  return 0;
}
