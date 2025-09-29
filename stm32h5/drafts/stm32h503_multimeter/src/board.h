#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include <stm32h5xx.h>
#include <gpio.h>

#define SYSTICK_MULTIPLIER 30
#define SPI_CLOCK 48000000
#define BOARD_PCLK1 240000000

#define LED_PIN  GPIO_Pin_13
#define LED_PORT GPIOC
// clr PC13
#define LED_ON GPIOC->BSRR = 0x20000000
// set PC13
#define LED_OFF GPIOC->BSRR = 0x2000

#define BUTTONS_PORT     GPIOB
//PB8
#define BUTTON_OFF_PIN GPIO_Pin_8
#define BUTTON_OFF_PRESSED (GPIOB->IDR & 0x100)
//PB2
#define BUTTON1_PIN     GPIO_Pin_10
#define BUTTON1_PRESSED (GPIOB->IDR & 4)
//PB1
#define BUTTON2_PIN     GPIO_Pin_2
#define BUTTON2_PRESSED (GPIOB->IDR & 2)
//PB0
#define BUTTON3_PIN     GPIO_Pin_1
#define BUTTON3_PRESSED (GPIOB->IDR & 1)

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 200
#define USE_MYVSPRINTF

#define CAPTURE_PIN_CHARGE1 GPIO_Pin_1
#define CAPTURE_PIN_CHARGE2 GPIO_Pin_3
#define CAPTURE_PIN_CX      GPIO_Pin_5
#define CAPTURE_PORT        GPIOA

#define R_CHARGE1_PIN    GPIO_Pin_2
#define R_CHARGE2_PIN    GPIO_Pin_4
#define R_DISCHARGE_PIN  GPIO_Pin_6
#define R_PORT           GPIOA

#define FREQUENCY_METER_INPUT_PIN  GPIO_Pin_0
#define FREQUENCY_METER_INPUT_PORT GPIOA

// LCD - SH1107 128x128
#define LCD_SPI_INSTANCE SPI2
#define LCD_PORT     GPIOB
#define LCD_RST_PIN  GPIO_Pin_0
#define LCD_CS_PIN   GPIO_Pin_12
#define LCD_SCK_PIN  GPIO_Pin_13
#define LCD_DC_PIN   GPIO_Pin_14
#define LCD_DIN_PIN  GPIO_Pin_15

// clr PB0
#define LCD_RST_CLR GPIOB->BSRR = 0x10000
// set PB0
#define LCD_RST_SET GPIOB->BSRR = 1

// clr PB14
#define LCD_DC_CLR GPIOB->BSRR = 0x40000000
// set PB14
#define LCD_DC_SET GPIOB->BSRR = 0x4000

#define DRAW_TEXT_MAX 20

#define ADS1220_DRDY_PIN  GPIO_Pin_3
#define ADS1220_DRDY_PORT GPIOB
#define ADS1220_DRDY_GET(channel) (GPIOB->IDR & ADS1220_DRDY_PIN)
#define ADS1220_PORT      GPIOA
#define ADS1220_MISO_PIN  GPIO_Pin_15
#define ADS1220_MOSI_PIN  GPIO_Pin_10
#define ADS1220_SCK_PIN   GPIO_Pin_9
#define ADS1220_CS_PIN    GPIO_Pin_8
#define ADS1220_MAX_CHANNELS 2
#define ADS1220_TIMEOUT      1000
#define ADS1220_CHANNEL 0

#define AD7793_PORT      GPIOB
#define AD7793_CS_PIN    GPIO_Pin_7
#define AD7793_SCK_PIN   GPIO_Pin_6
#define AD7793_DIN_PIN   GPIO_Pin_5
#define AD7793_DOUT_PIN  GPIO_Pin_4
#define AD7793_RDY_GET(channel) (GPIOB->IDR & AD7793_DOUT_PIN)
#define AD7793_TIMEOUT      1000
#define AD7793_MAX_CHANNELS 2
#define AD7793_CHANNEL 1

#define SPI_CHANNELS 2

#define ON_PORT GPIOA
#define ON_PIN  GPIO_Pin_7
// set ON pin
#define POWER_ON ON_PORT->BSRR = ON_PIN
// clr ON pin
#define POWER_OFF ON_PORT->BSRR = ON_PIN << 16

#define SPI_TIMEOUT 1000000

#define SPI_DELAY(channel) 100

#define TIMER_INTERRUPT_PRIORITY 0

#define CAPACITANCE_COEFFICIENT_100K 1962851UL
#define CAPACITANCE_COEFFICIENT_1K 19628UL
#define CAPACITANCE_OFFSET_1K 23
#define CAPACITANCE_OFFSET_100K 23
#define CAPACITANCE_MIN_VALUE_1K 200000

extern volatile int capacity_measurement_done;

#ifdef __cplusplus
extern "C" {
#endif

void CDCInit(void);
int getch_(void);
void charge_off(void);
void charge_on(int channel);
void discharge_off(void);
void discharge_on(void);
void capacity_measurement_start(int channel);
unsigned int get_capacity_measurement_start_time(void);
unsigned int get_capacity_measurement_end_time(void);

void SPI_MOSI_SET(int channel);
void SPI_MOSI_CLR(int channel);
int SPI_CHECK_MISO(int channel);
void SPI_CLK_IDLE(int channel);
void SPI_CLK_ACTIVE(int channel);
void SPI_CS_SET(int channel);
void SPI_CS_CLR(int channel);

#ifdef __cplusplus
}
#endif

#include <delay_systick.h>
#include <lcd_sh1107.h>

#endif
