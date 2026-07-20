#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include <saml10e16a.h>

#define LED_TIMER_ON PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA00
#define LED_TIMER_OFF PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA00

#define SYSTICK_MULTIPLIER 4

#define USART_REGS           SERCOM0_REGS
#define USART_Handler        SERCOM0_2_Handler
#define USART_IRQn           SERCOM0_2_IRQn
#define USART_RX_PIN         25
#define USART_TX_PIN         24
#define USART_BAUD_4M_115200 35337
#define USART_BUFFER_SIZE    256
#define USART_RXPO           SERCOM_USART_INT_CTRLA_RXPO(3)
#define USART_TXPO           SERCOM_USART_INT_CTRLA_TXPO(1)

#define I2C_MASTER_SCL_PIN       17
#define I2C_MASTER_SDA_PIN       16
#define I2C_MASTER_REGS          SERCOM1_REGS
#define I2C_MASTER_BAUD          0x1F
#define I2C_MASTER_MB_Handler    SERCOM1_0_Handler
#define I2C_MASTER_SB_Handler    SERCOM1_1_Handler
#define I2C_MASTER_ERROR_Handler SERCOM1_OTHER_Handler
#define I2C_MASTER_MB_IRQn       SERCOM1_0_IRQn
#define I2C_MASTER_SB_IRQn       SERCOM1_1_IRQn
#define I2C_MASTER_ERROR_IRQn    SERCOM1_OTHER_IRQn
#define I2C_MASTER_SCLSM         0

#define USART_INTERRUPT_PRIORITY      1
#define I2C_MASTER_INTERRUPT_PRIORITY 1
#define SPI_MASTER_INTERRUPT_PRIORITY 1
#define EIC_INTERRUPT_PRIORITY        0

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 200
#define USE_MYVSPRINTF

#define cc1101_GD0_PIN    1
#define cc1101_GD2_PIN    1
#define cc1101_GD0        (PORT_REGS->GROUP[0].PORT_IN & (1 << cc1101_GD0_PIN))
#define cc1101_GD2        (PORT_REGS->GROUP[0].PORT_IN & (1 << cc1101_GD2_PIN))
#define cc1101_CSN_CLR(d)
#define cc1101_CSN_SET(d)
#define CC1101_TIMEOUT    0xFFFFF
#define RF_MODE           CC1101_MODE_GFSK_600
#define TX_POWER          CC1101_TX_POWER_M30_433
#define CC1101_RXOFF_MODE CC1101_RXOFF_MODE_IDLE
#define CC1101_TXOFF_MODE CC1101_TXOFF_MODE_IDLE

void SysInit(void);

#include <delay_systick.h>

#endif
