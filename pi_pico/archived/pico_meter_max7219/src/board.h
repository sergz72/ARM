#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include <hardware/gpio.h>

#define FREQ1_PIN 10
#define FREQ2_PIN 9

#define PWM1_PIN  15
#define PWM2_PIN  13

#define CAP_IN_PIN    11
#define CAP_RHIGH_PIN 12
#define CAP_RLOW_PIN  14

#define SPI_4BIT_DIO0_PIN 18
#define SPI_4BIT_DIO1_PIN 19
#define SPI_4BIT_DIO2_PIN 20
#define SPI_4BIT_DIO3_PIN 21
#define SPI_4BIT_CLK_PIN  16
#define SPI_4BIT_CS_PIN   17

#define KB1_PIN 22
#define KB2_PIN 2
#define KB3_PIN 3
#define KB4_PIN 4

#define SCL_PIN 1
#define SDA_PIN 0

#define ADC0_PIN 26
#define ADC1_PIN 27
#define ADC2_PIN 28

#define SPI_4BIT_CLK_CLR gpio_put(SPI_4BIT_CLK_PIN, 0)
#define SPI_4BIT_CLK_SET gpio_put(SPI_4BIT_CLK_PIN, 1)
#define SPI_4BIT_CS_CLR gpio_put(SPI_4BIT_CS_PIN, 0)
#define SPI_4BIT_CS_SET gpio_put(SPI_4BIT_CS_PIN, 1)

#define MAX7219_NUM_PARALLEL 4
#define MAX7219_NUM_SERIES 1

#define SI5351_XTAL_FREQ 25000000
#define SI5351_CHANNELS 1

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define I2C_TIMEOUT 1000000

#define KBD_COUNTX 4

#define DEFAULT_CAP_MAX 60000000
#define DEFAULT_CAP_OFFSET 121
#define DEFAULT_CAP_VALUE_MUL 2300
#define CAP_VALUE_DIV 1000
#define DEFAULT_MCLK 125000000
#define DEFAULT_SETTING_UMUL 6000
#define DEFAULT_SETTING_UOFFSET 0
#define DEFAULT_SETTING_L_CAP 3400 //pF
#define U_DIV 1000

#define ADC_MAX 4095

#define DEFAULT_PWM_FREQ 1000 // Hz
#define DEFAULT_PWM_DUTY 500  // 50.0
#define MINIMUM_PWM_FREQ 10

#define DEFAULT_SI5351_FREQ 1000000

#define PWM_SM 0
#define PWM_PIO pio1
#define COUNTER1_SM 0
#define COUNTER2_SM 1
#define COUNTER_PIO pio0
#define CAP_SM 1
#define CAP_PIO pio1

#define ENCODER_A_PIN      8
#define ENCODER_B_PIN      7
#define ENCODER_BUTTON_PIN 6

extern volatile unsigned int cap_value, cap_value_updated;

void SystemInit(void);
void SPI_4BIT_DATA_SET(unsigned char data);
unsigned char HALKbdHandler(void);
void sleepms(unsigned int ms);

#endif
