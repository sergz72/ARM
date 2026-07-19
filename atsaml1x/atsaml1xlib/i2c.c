#include "board.h"
#include <i2c.h>

typedef enum
{
  I2C_MASTER_STATE_IDLE,
  I2C_MASTER_STATE_WRITE_DATA,
  I2C_MASTER_STATE_REPEATED_START,
  I2C_MASTER_STATE_READ_DATA,
  I2C_MASTER_STATE_DONE,
  I2C_MASTER_STATE_ERROR
} i2c_master_state_t;

volatile i2c_master_state_t i2c_master_state = I2C_MASTER_STATE_IDLE;
volatile const unsigned char *txp;
volatile unsigned int tx_length;
volatile unsigned char *rxp;
volatile unsigned int rx_length;
unsigned int slave_address;

static void i2c_read_byte(void)
{
  if (rx_length == 1)
  {
    I2C_MASTER_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_ACKACT(1); // last byte = NACK
    I2C_MASTER_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP
    i2c_master_state = I2C_MASTER_STATE_DONE;
  }
  else
  {
    I2C_MASTER_REGS->I2CM.SERCOM_CTRLB &= ~SERCOM_I2CM_CTRLB_ACKACT(1); // ACK
    I2C_MASTER_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(2); // Issue read ack
  }
  *rxp++ = I2C_MASTER_REGS->I2CM.SERCOM_DATA;
  rx_length--;
}

#ifdef I2C_MASTER_MB_Handler
void __attribute__((used)) I2C_MASTER_MB_Handler(void)
{
  switch (i2c_master_state)
  {
    case I2C_MASTER_STATE_WRITE_DATA:
      // Check for NACK from slave
      if (I2C_MASTER_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK(1))
      {
        // Handle error: Slave did not acknowledge address or data
        I2C_MASTER_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP
        i2c_master_state = I2C_MASTER_STATE_ERROR;
        break;
      }
      I2C_MASTER_REGS->I2CM.SERCOM_DATA = *txp++;
      tx_length--;
      if (!tx_length)
      {
        if (!rx_length)
        {
          I2C_MASTER_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP
          i2c_master_state = I2C_MASTER_STATE_DONE;
        }
        else
          i2c_master_state = I2C_MASTER_STATE_REPEATED_START;
      }
      break;
    case I2C_MASTER_STATE_REPEATED_START:
      I2C_MASTER_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(1);// prepare for repeated start
      I2C_MASTER_REGS->I2CM.SERCOM_ADDR = slave_address;
      i2c_master_state = I2C_MASTER_STATE_READ_DATA;
      break;
    /*case I2C_MASTER_STATE_READ_DATA:
      // Check for NACK from slave
      if (I2C_MASTER_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK(1))
      {
        // Handle error: Slave did not acknowledge address
        I2C_MASTER_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP
        i2c_master_state = I2C_MASTER_STATE_ERROR;
        break;
      }
      i2c_read_byte();
      break;*/
    default:
      I2C_MASTER_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP
      i2c_master_state = I2C_MASTER_STATE_ERROR;
      break;
  }
}
#endif

#ifdef I2C_MASTER_SB_Handler
void __attribute__((used)) I2C_MASTER_SB_Handler(void)
{
  switch (i2c_master_state)
  {
    case I2C_MASTER_STATE_READ_DATA:
      if (I2C_MASTER_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK(1))
      {
        // Handle error: Slave did not acknowledge address
        I2C_MASTER_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP
        i2c_master_state = I2C_MASTER_STATE_ERROR;
        break;
      }
      i2c_read_byte();
      break;
    default:
      I2C_MASTER_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP
      i2c_master_state = I2C_MASTER_STATE_ERROR;
      break;
  }
}
#endif

#ifdef I2C_MASTER_ERROR_Handler
void __attribute__((used)) I2C_MASTER_ERROR_Handler(void)
{
  I2C_MASTER_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP
  i2c_master_state = I2C_MASTER_STATE_ERROR;
  I2C_MASTER_REGS->I2CM.SERCOM_INTFLAG = SERCOM_I2CM_INTFLAG_ERROR(1);
}
#endif

/*
 *PAD[1] = SCL
 *PAD[0] = SDA
 */
void i2c_master_init(void)
{
  PORT_REGS->GROUP[0].PORT_PINCFG[I2C_MASTER_SCL_PIN] = PORT_PINCFG_PMUXEN(1) | PORT_PINCFG_PULLEN(1);
  PORT_REGS->GROUP[0].PORT_PINCFG[I2C_MASTER_SDA_PIN] = PORT_PINCFG_PMUXEN(1) | PORT_PINCFG_PULLEN(1);
  PORT_REGS->GROUP[0].PORT_PMUX[I2C_MASTER_SDA_PIN / 2] |= PORT_PMUX_PMUXE_C | PORT_PMUX_PMUXO_C; // SERCOM

  /* Set Operation Mode to I2C Slave */
  I2C_MASTER_REGS->I2CS.SERCOM_CTRLA = SERCOM_I2CM_CTRLA_MODE_I2C_MASTER | SERCOM_I2CM_CTRLA_SDAHOLD_75NS |
    SERCOM_I2CM_CTRLA_SCLSM(0) | SERCOM_I2CM_CTRLA_SPEED_STANDARD_AND_FAST_MODE;
  /* Wait for synchronization */
  while ((I2C_MASTER_REGS->I2CS.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  I2C_MASTER_REGS->I2CS.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_SMEN(1);
  /* Wait for synchronization */
  while ((I2C_MASTER_REGS->I2CS.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  I2C_MASTER_REGS->I2CM.SERCOM_BAUD = I2C_MASTER_BAUD;
  /* Wait for synchronization */
  while ((I2C_MASTER_REGS->I2CM.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  /* Enable peripheral */
  I2C_MASTER_REGS->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_ENABLE_Msk;
  /* Wait for synchronization */
  while ((I2C_MASTER_REGS->I2CM.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  I2C_MASTER_REGS->I2CM.SERCOM_STATUS = 0x743 | SERCOM_I2CM_STATUS_BUSSTATE(1);
  /* Wait for synchronization */
  while ((I2C_MASTER_REGS->I2CM.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  I2C_MASTER_REGS->I2CM.SERCOM_INTENSET = SERCOM_I2CM_INTENSET_MB(1) | SERCOM_I2CM_INTENSET_SB(1) |
    SERCOM_I2CM_INTENSET_ERROR(1);

  NVIC_EnableIRQ(I2C_MASTER_MB_IRQn); // MB (Master On Bus) interrupt vector
  NVIC_EnableIRQ(I2C_MASTER_SB_IRQn); // SB (Slave On Bus) interrupt vector
  NVIC_EnableIRQ(I2C_MASTER_ERROR_IRQn); // Error interrupt vector
}

int i2c_write(unsigned char address, const unsigned char* data, unsigned int l)
{
  txp = data;
  tx_length = l;
  rx_length = 0;
  i2c_master_state = I2C_MASTER_STATE_WRITE_DATA;
  // 1. Send START condition + 7-bit Address + Write bit (0)
  I2C_MASTER_REGS->I2CM.SERCOM_ADDR = address;

  while (i2c_master_state != I2C_MASTER_STATE_DONE && i2c_master_state != I2C_MASTER_STATE_ERROR)
    __WFI();

  return i2c_master_state == I2C_MASTER_STATE_ERROR;
}

int i2c_read(unsigned char address, unsigned char* data, unsigned int l)
{
  rxp = data;
  rx_length = l;
  i2c_master_state = I2C_MASTER_STATE_READ_DATA;
  // 1. Send START condition + 7-bit Address + Read bit (1)
  I2C_MASTER_REGS->I2CM.SERCOM_ADDR = address | 1;

  while (i2c_master_state != I2C_MASTER_STATE_DONE && i2c_master_state != I2C_MASTER_STATE_ERROR)
    __WFI();

  return i2c_master_state == I2C_MASTER_STATE_ERROR;
}

int i2c_transfer(unsigned char address, const unsigned char *wdata, unsigned int wlen, unsigned char *rdata, unsigned int rlen)
{
  txp = wdata;
  tx_length = wlen;
  rxp = rdata;
  rx_length = rlen;
  slave_address = address | 1;
  i2c_master_state = I2C_MASTER_STATE_WRITE_DATA;
  // 1. Send START condition + 7-bit Address + Write bit (0)
  I2C_MASTER_REGS->I2CM.SERCOM_ADDR = address;

  while (i2c_master_state != I2C_MASTER_STATE_DONE && i2c_master_state != I2C_MASTER_STATE_ERROR)
    __WFI();

  return i2c_master_state == I2C_MASTER_STATE_ERROR;
}
