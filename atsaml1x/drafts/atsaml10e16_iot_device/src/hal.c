#include "board.h"
#include <stdio.h>
#include <common_printf.h>
#include <cc1101.h>
#include <scd4x.h>
#include <rtc_ds1307.h>
#include <rtc_ds3231.h>

static char usart_buffer[USART_BUFFER_SIZE];
static char *usart_buffer_write_p, *usart_buffer_read_p;

void __attribute__((used)) USART_Handler(void)
{
  unsigned char flags = USART_REGS->USART_INT.SERCOM_INTFLAG;
  if (flags & SERCOM_USART_INT_INTFLAG_ERROR(1))
    // clear error flag
    USART_REGS->USART_INT.SERCOM_INTFLAG = SERCOM_USART_INT_INTFLAG_ERROR(1);
  if (flags & SERCOM_USART_INT_INTFLAG_RXC(1))
  {
    unsigned char c = USART_REGS->USART_INT.SERCOM_DATA;
    *usart_buffer_write_p++ = c;
    if (usart_buffer_write_p == usart_buffer + USART_BUFFER_SIZE)
      usart_buffer_write_p = usart_buffer;
  }
}

/*
 * PA00(1) = LED
 */
static void ports_init(void)
{
  LED_TIMER_OFF;
  PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA00;
}

static void clock_init(void)
{
  // EIC
  GCLK_REGS->GCLK_PCHCTRL[3] = GCLK_PCHCTRL_CHEN(1);
  // SERCOM0_CORE - USART
  GCLK_REGS->GCLK_PCHCTRL[11] = GCLK_PCHCTRL_CHEN(1);
  // SERCOM1_CORE - I2C
  GCLK_REGS->GCLK_PCHCTRL[12] = GCLK_PCHCTRL_CHEN(1);
  // SERCOM2_CORE - SPI
  GCLK_REGS->GCLK_PCHCTRL[13] = GCLK_PCHCTRL_CHEN(1);
  // ADC
  GCLK_REGS->GCLK_PCHCTRL[16] = GCLK_PCHCTRL_CHEN(1);
}

static void usart_init(void)
{
  usart_buffer_write_p = usart_buffer_read_p = usart_buffer;

  PORT_REGS->GROUP[0].PORT_PINCFG[USART_TX_PIN] = PORT_PINCFG_PMUXEN(1);
  PORT_REGS->GROUP[0].PORT_PINCFG[USART_RX_PIN] = PORT_PINCFG_PMUXEN(1) | PORT_PINCFG_INEN(1) | PORT_PINCFG_PULLEN(1);
  PORT_REGS->GROUP[0].PORT_PMUX[USART_TX_PIN/2] = PORT_PMUX_PMUXE_C | PORT_PMUX_PMUXO_C;

  /* Selection of the Baud Value */
  USART_REGS->USART_INT.SERCOM_BAUD = SERCOM_USART_INT_BAUD_BAUD(USART_BAUD_4M_115200);

  USART_REGS->USART_INT.SERCOM_CTRLB = SERCOM_USART_INT_CTRLB_CHSIZE_8_BIT | SERCOM_USART_EXT_CTRLB_RXEN(1) |
                                       SERCOM_USART_EXT_CTRLB_TXEN(1) | SERCOM_USART_EXT_CTRLB_SBMODE_1_BIT |
                                       SERCOM_USART_EXT_CTRLB_SFDE(1);

  /* Wait for synchronization */
  while((USART_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  USART_REGS->USART_INT.SERCOM_CTRLA = SERCOM_USART_INT_CTRLA_DORD_LSB | SERCOM_USART_INT_CTRLA_CMODE_ASYNC |
      SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_NO_PARITY | USART_RXPO | USART_TXPO |
          SERCOM_USART_INT_CTRLA_SAMPR_16X_ARITHMETIC | SERCOM_USART_INT_CTRLA_RUNSTDBY(1) |
              SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK | SERCOM_USART_INT_CTRLA_ENABLE(1);


  /* Wait for synchronization */
  while((USART_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  USART_REGS->USART_INT.SERCOM_INTENSET = SERCOM_USART_INT_INTENSET_ERROR(1) | SERCOM_USART_INT_INTENSET_RXC(1);

  NVIC_SetPriority(USART_IRQn, USART_INTERRUPT_PRIORITY);
  NVIC_EnableIRQ(USART_IRQn);
}

/*
 *PAD[1] = SCL
 *PAD[0] = SDA
 */
static void i2c_init(void)
{
  //PORT_REGS->GROUP[0].PORT_OUTSET = (1<<I2C_SCL_PIN) | (1<<I2C_SDA_PIN);
  PORT_REGS->GROUP[0].PORT_PINCFG[I2C_SCL_PIN] = PORT_PINCFG_PMUXEN(1) | PORT_PINCFG_PULLEN(1);
  PORT_REGS->GROUP[0].PORT_PINCFG[I2C_SDA_PIN] = PORT_PINCFG_PMUXEN(1) | PORT_PINCFG_PULLEN(1);
  PORT_REGS->GROUP[0].PORT_PMUX[I2C_SDA_PIN/2] |= PORT_PMUX_PMUXE_C | PORT_PMUX_PMUXO_C; // SERCOM

  /* Set Operation Mode to I2C Slave */
  I2C_REGS->I2CS.SERCOM_CTRLA = SERCOM_I2CS_CTRLA_MODE_I2C_MASTER | SERCOM_I2CS_CTRLA_SDAHOLD_75NS |
      SERCOM_I2CS_CTRLA_SCLSM(1) | SERCOM_I2CS_CTRLA_SPEED_STANDARD_AND_FAST_MODE;
  /* Wait for synchronization */
  while((I2C_REGS->I2CS.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  I2C_REGS->I2CS.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_SMEN(1);
  /* Wait for synchronization */
  while((I2C_REGS->I2CS.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  I2C_REGS->I2CM.SERCOM_BAUD = I2C_BAUD;
  /* Wait for synchronization */
  while((I2C_REGS->I2CM.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  /* Enable peripheral */
  I2C_REGS->I2CS.SERCOM_CTRLA |= SERCOM_I2CS_CTRLA_ENABLE_Msk;
  /* Wait for synchronization */
  while((I2C_REGS->I2CM.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  I2C_REGS->I2CM.SERCOM_STATUS = 0x743 | SERCOM_I2CM_STATUS_BUSSTATE(1);
  /* Wait for synchronization */
  while((I2C_REGS->I2CM.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }
}

void SysInit(void)
{
  clock_init();
  ports_init();
  usart_init();
  i2c_init();
}

void _init(void)
{
}

void usart_transmit(char c)
{
  while (!(USART_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE(1)))
    ;
  USART_REGS->USART_INT.SERCOM_DATA = c;
}

void puts_(const char *s)
{
  while (*s)
    usart_transmit(*s++);
}

int getch_(void)
{
  if (usart_buffer_write_p != usart_buffer_read_p)
  {
    char c = *usart_buffer_read_p++;
    if (usart_buffer_read_p == usart_buffer + USART_BUFFER_SIZE)
      usart_buffer_read_p = usart_buffer;
    return c;
  }
  return EOF;
}

int cc1101_RW(unsigned int device_num, unsigned char *txdata, unsigned char *rxdata, unsigned int size)
{
  int rc;

  if (size < 2)
    return 1;

  cc1101_CSN_CLR(device_num);

  rc = CC1101_TIMEOUT;
  while (--rc && cc1101_GD2) // waiting for chip ready
    ;
  if (!rc)
  {
    cc1101_CSN_SET(device_num);
    return 2; // timeout
  }

  if (size > 2)
    txdata[0] |= CC1101_BURST;

  //rc = SPI_TransmitReceive(SPI_INST, txdata, rxdata, size, SPI_TIMEOUT);

  cc1101_CSN_SET(device_num);

  return 0;
}

int cc1101_strobe(unsigned int device_num, unsigned char data, unsigned char *status)
{
  int rc;

  cc1101_CSN_CLR(device_num);

  rc = CC1101_TIMEOUT;
  while (--rc && cc1101_GD2) // waiting for chip ready
    ;
  if (!rc)
  {
    cc1101_CSN_SET(device_num);
    return 1; // timeout
  }

  //rc = SPI_TransmitReceive(SPI_INST, &data, status, 1, SPI_TIMEOUT);

  cc1101_CSN_SET(device_num);

  return 0;
}

int i2c_write(unsigned char address, const unsigned char *data, unsigned int l, unsigned int timeout, bool stop)
{
    // 1. Send START condition + 7-bit Address + Write bit (0)
    I2C_REGS->I2CM.SERCOM_ADDR = address;
    // Wait for transmit complete flag (MB: Master On-Bus)
    unsigned int t = timeout;
    while ((I2C_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB(1)) && t != 0)
        t--;
    if (!t)
        return 1;

    // Check for NACK from slave
    if (I2C_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK(1))
    {
        // Handle error: Slave did not acknowledge the address
        I2C_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP
        return 2;
    }

    while (l--)
    {
        I2C_REGS->I2CM.SERCOM_DATA = *data++;
        t = timeout;
        while ((I2C_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB(1)) && t != 0)
            t--;
        if (!t)
        {
            I2C_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP
            return 3;
        }

        if (I2C_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK(1))
        {
            // Handle error: Slave did not acknowledge the address
            I2C_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP
            return 4;
        }
    }

    if (stop)
        I2C_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP

    return 0;
}

int i2c_read(unsigned char address, unsigned char *data, unsigned int l, unsigned int timeout)
{
    I2C_REGS->I2CM.SERCOM_CTRLB &= ~SERCOM_I2CM_CTRLB_ACKACT(1); // sending ACK
    /* Wait for synchronization */
    while((I2C_REGS->I2CM.SERCOM_SYNCBUSY) != 0U)
    {
        /* Do nothing */
    }

    // 1. Send START condition + 7-bit Address + Read bit (1)
    I2C_REGS->I2CM.SERCOM_ADDR = address | 1;
    // Wait for transmit complete flag (MB: Master On-Bus)
    unsigned int t = timeout;
    while ((I2C_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB(1)) && t != 0)
        t--;
    if (!t)
        return 5;

    // Check for NACK from slave
    if (I2C_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK(1))
    {
        // Handle error: Slave did not acknowledge the address
        I2C_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP
        return 6;
    }

    while (l)
    {
        if (l == 1)
        {
            I2C_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_ACKACT(1); // last byte = NACK
            /* Wait for synchronization */
            while((I2C_REGS->I2CM.SERCOM_SYNCBUSY) != 0U)
            {
                /* Do nothing */
            }
        }
        // Read the incoming byte out of the hardware data buffer
        *data++ = I2C_REGS->I2CM.SERCOM_DATA;
        l--;
    }

    I2C_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP

    return 0;
}

int scd4x_write(const unsigned char *data, unsigned int len, bool no_ack_expected)
{
  return i2c_write(SCD4X_SENSOR_ADDR << 1, data, len, I2C_TIMEOUT, true);
}

int scd4x_read(unsigned char *data, unsigned int len)
{
  return i2c_read(SCD4X_SENSOR_ADDR << 1, data, len, I2C_TIMEOUT);
}

int i2c_ds1307_write(const unsigned char *data, int data_length)
{
  return i2c_write(DS1307_I2C_ADDRESS, data, data_length, I2C_TIMEOUT, true);
}

int i2c_ds1307_transfer(const unsigned char *wdata, int wdata_length, unsigned char *rdata, int rdata_length)
{
  int rc = i2c_write(DS1307_I2C_ADDRESS, wdata, wdata_length, I2C_TIMEOUT, false);
  if (rc)
    return rc;
  return i2c_read(DS1307_I2C_ADDRESS, rdata, rdata_length, I2C_TIMEOUT);
}

int i2c_ds3231_write(const unsigned char *data, int data_length)
{
  return i2c_write(DS3231_I2C_ADDRESS, data, data_length, I2C_TIMEOUT, true);
}

int i2c_ds3231_transfer(const unsigned char *wdata, int wdata_length, unsigned char *rdata, int rdata_length)
{
  int rc = i2c_write(DS3231_I2C_ADDRESS, wdata, wdata_length, I2C_TIMEOUT, false);
  if (rc)
    return -rc;
  return i2c_read(DS3231_I2C_ADDRESS, rdata, rdata_length, I2C_TIMEOUT);
}
