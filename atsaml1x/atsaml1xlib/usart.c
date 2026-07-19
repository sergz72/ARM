#include "board.h"
#include <usart.h>
#include <common_printf.h>
#include <stdio.h>

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

void usart_init(void)
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
