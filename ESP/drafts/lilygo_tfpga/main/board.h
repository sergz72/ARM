#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define WEMOS_S3

#define UART_BAUD_RATE 115200
#define UART_BUFFER_SIZE 1024

#ifdef T_FPGA
#define PIN_BTN      0

#define PMU_IIC_SDA  38
#define PMU_IIC_SCL  39
#define PIN_PMU_IRQ  40
#define PMU_I2C_MASTER_NUM 0

#define PIN_LED      46

#define PIN_FPGA_CS  1
#define PIN_FPGA_SCK 2
#define PIN_FPGA_D0  3
#define PIN_FPGA_D1  5
#define PIN_FPGA_D2  6
#define PIN_FPGA_D3  4

#define MAX_DEVICES 5

#endif
#ifdef WEMOS_S3
#define PIN_BTN      0

#define PIN_LED      38
#define LED_STRIP

#define UART_PORT 0
#define PIN_UART_TXD 43
#define PIN_UART_RXD 44

#define PIN_FPGA_CS  1
#define PIN_FPGA_SCK 2
#define PIN_FPGA_D0  3
#define PIN_FPGA_D1  5
#define PIN_FPGA_D2  6
#define PIN_FPGA_D3  4

#define MAX_DEVICES 5
#define SDA_PINS 1,2,3,4,5
#define SCL_PINS 6,7,8,9,0
#endif

#define I2C_SOFT
#define I2C_TIMEOUT 1000000
#define i2c_dly delayus(5)

#define SI5351_XTAL_FREQ                                        25000000
#define SI5351_CHANNELS 4

int I2CCheck(int idx, int device_id);
void SCL_HIGH(int);
void SCL_LOW(int);
void SDA_HIGH(int);
void SDA_LOW(int);
int SDA_IN(int);
int SCL_IN(int);
void configure_i2c(void);
void configure_led(void);
void blink_led(void);
void configure_uart(void);
void delayus(unsigned int us);

#endif