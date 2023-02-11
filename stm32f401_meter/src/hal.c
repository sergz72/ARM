#include "board.h"
#include "devices.h"
#include <systick.h>
#include <usb.h>
#include <i2c_soft.h>
#include <ina226.h>
#include <mcp3421.h>
#include <ads1115.h>
#include "dev_dds.h"
#include <si5351.h>
#include "dev_si5351.h"

volatile int Sys_Tick;
int keyboard_present;

void SysTick_Handler(void)
{
  Sys_Tick++;
}

void systick_start(unsigned int us)
{
	systick_set_reload(us * SYSTICK_MULTIPLIER);

	systick_interrupt_enable();
	/* start counting */
	systick_counter_enable();
}

void systick_stop(void)
{
	/* stop counting */
	systick_counter_disable();
	systick_interrupt_disable();
}

void systick_wait(void)
{
  Sys_Tick = 0;
  while (!Sys_Tick)
    __WFI();
}

void delay(unsigned int us)
{
  systick_start(us);
  systick_wait();
  systick_stop();
}

void delayms(unsigned int ms)
{
  while (ms--)
    delay(1000);
}

void LCD_CONTROL_SET(int pins)
{
  if (pins & LCD_DI)
  {
    // set PC14
    GPIOC->BSRR = 0x4000;
  }
  if (pins & LCD_E)
  {
    // set PC15
    GPIOC->BSRR = 0x8000;
  }
  if (pins & LCD_CS1)
  {
    // set PB0
    GPIOB->BSRR = 0x0001;
  }
  if (pins & LCD_CS2)
  {
    // set PB1
    GPIOB->BSRR = 0x0002;
  }
  if (pins & LCD_RST)
  {
    // set PB2
    GPIOB->BSRR = 0x0004;
  }
}

void LCD_CONTROL_CLR(int pins)
{
  if (pins & LCD_DI)
  {
    // clr PC14
    GPIOC->BSRR = 0x40000000;
  }
  if (pins & LCD_E)
  {
    // clr PC15
    GPIOC->BSRR = 0x80000000;
  }
  if (pins & LCD_CS1)
  {
    // clr PB0
    GPIOB->BSRR = 0x00010000;
  }
  if (pins & LCD_CS2)
  {
    // clr PB1
    GPIOB->BSRR = 0x00020000;
  }
  if (pins & LCD_RST)
  {
    // clr PB2
    GPIOB->BSRR = 0x00040000;
  }
}

// all low
void lcd_init(void)
{
  // clr PB0-PB2
  GPIOB->BSRR = 0x00070000;
  // clr PC14,15
  GPIOC->BSRR = 0xC0000000;
}

void SCL_HIGH(int channel)
{
  switch (channel)
  {
    case 0:
      //set PB3
      GPIOB->BSRR = 8;
      break;
    case 1:
      //set PB4
      GPIOB->BSRR = 0x10;
      break;
    case 2:
      // set PB7
      GPIOB->BSRR = 0x80;
      break;
    default:
      // set PB9
      GPIOB->BSRR = 0x200;
      break;
  }
}

void SCL_LOW(int channel)
{
  switch (channel)
  {
    case 0:
      //clr PB3
      GPIOB->BSRR = 0x80000;
      break;
    case 1:
      //clr PB4
      GPIOB->BSRR = 0x100000;
      break;
    case 2:
      // clr PB7
      GPIOB->BSRR = 0x800000;
      break;
    default:
      // clr PB9
      GPIOB->BSRR = 0x2000000;
      break;
  }
}

void SDA_HIGH(int channel)
{
  switch (channel)
  {
    case 0:
      //set PB10
      GPIOB->BSRR = 0x400;
      break;
    case 1:
      //set PA8
      GPIOA->BSRR = 0x100;
      break;
    case 2:
      // set PB6
      GPIOB->BSRR = 0x40;
      break;
    default:
      // set PB8
      GPIOB->BSRR = 0x100;
      break;
  }
}

void SDA_LOW(int channel)
{
  switch (channel)
  {
    case 0:
      //clr PB10
      GPIOB->BSRR = 0x4000000;
      break;
    case 1:
      //clr PA8
      GPIOA->BSRR = 0x1000000;
      break;
    case 2:
      //clr PB6
      GPIOB->BSRR = 0x400000;
      break;
    default:
      //clr PB8
      GPIOB->BSRR = 0x1000000;
      break;
  }
}

int SDA_IN(int channel)
{
  switch (channel)
  {
    case 0:
      //PB10
      return GPIOB->IDR & 0x400;
    case 1:
      //PA8
      return GPIOA->IDR & 0x100;
    case 2:
      //PB6
      return GPIOB->IDR & 0x40;
    default:
      //PB8
      return GPIOB->IDR & 0x100;
  }
}

int SCL_IN(int channel)
{
  switch (channel)
  {
    case 0:
      // PB3
      return GPIOB->IDR & 8;
    case 1:
      // PB4
      return GPIOB->IDR & 0x10;
    case 2:
      // PB7
      return GPIOB->IDR & 0x80;
    default:
      // PB9
      return GPIOB->IDR & 0x200;
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

int dds_command(unsigned char deviceId, unsigned char cmd, dds_cmd *data, int idx)
{
  dds_i2c_command c;
  switch (cmd)
  {
    case DDS_COMMAND_ENABLE_OUTPUT:
      c.c3.command = cmd;
      c.c3.channel = data->channel;
      c.c3.parameter = data->enable_command.enable;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 3,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    case DDS_COMMAND_SET_ATTENUATOR:
      c.c4.command = cmd;
      c.c4.channel = data->channel;
      c.c4.parameter = data->set_attenuator_command.attenuator_value;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 4,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    case DDS_COMMAND_SET_FREQUENCY:
      c.c8.command = cmd;
      c.c8.channel = data->channel;
      c.c8.freq = data->set_frequency_command.frequency;
      c.c8.div = data->set_frequency_command.divider;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 8,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    case DDS_COMMAND_SET_FREQUENCY_CODE:
      c.c8.command = cmd;
      c.c8.channel = data->channel;
      c.c8.freq = data->set_frequency_code_command.frequency_code;
      c.c8.div = data->set_frequency_code_command.divider;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 8,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    case DDS_COMMAND_SET_MODE:
      c.c3.command = cmd;
      c.c3.channel = data->channel;
      c.c3.parameter = data->set_mode_command.mode;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 3,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    default:
      return 1;
  }
}

int si5351_write_bulk(int channel, unsigned char addr, unsigned char bytes, unsigned char *data)
{
  return i2c_soft_command(channel, SI5351_DEVICE_ID, &addr, 1, data, bytes, NULL, 0, I2C_TIMEOUT);
}

int si5351_write(int channel, unsigned char addr, unsigned char data)
{
  return i2c_soft_command(channel, SI5351_DEVICE_ID, &addr, 1, &data, 1, NULL, 0, I2C_TIMEOUT);
}

int dds_get_config(dds_config *cfg, unsigned char deviceId, int idx)
{
  return i2c_soft_read(idx, deviceId, (unsigned char*)cfg, sizeof(dds_config), I2C_TIMEOUT);
}

void OTG_FS_IRQHandler(void)
{
  USBInterruptHandler();
}
