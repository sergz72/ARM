#include "board.h"
#include "systick.h"
#include "gd32f30x_gpio.h"
#include "gd32f30x_i2c.h"
#include <tlv320dac3100.h>

volatile int Sys_Tick;

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

static void GPIOInit(void)
{
  rcu_periph_clock_enable(LED_CLOCK);
  gpio_init(LED_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ,LED_PIN);
}


static void I2C0Init(void)
{
  /* enable GPIOB clock */
  rcu_periph_clock_enable(RCU_GPIOB);

  gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);
  /* connect PB8 to I2C_SCL */
  /* connect PB9 to I2C_SDA */
  gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
  gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

  /* enable I2C clock */
  rcu_periph_clock_enable(RCU_I2C0);
  /* configure I2C clock */
  i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
  /* configure I2C address */
  i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0);
  /* enable I2CX */
  i2c_enable(I2C0);
  /* enable acknowledge */
  i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

static int I2C_Transfer(uint32_t instance, unsigned short address, unsigned char *wdata, unsigned int wsize,
                 unsigned char *rdata, unsigned int rsize, unsigned int timeout)
{
  //todo
  return 1;
}

int tlv320dac3100_read(unsigned char register_number, unsigned char *value)
{
  return I2C_Transfer(I2C0, TLV320DAC3100_I2C_ADDRESS, &register_number, 1, value, 1, I2C_TIMEOUT);
}

int tlv320dac3100_write(unsigned char register_number, unsigned char value)
{
  unsigned char data[2];
  data[0] = register_number;
  data[1] = value;
  return I2C_Transfer(I2C0, TLV320DAC3100_I2C_ADDRESS, data, 2, NULL, 0, I2C_TIMEOUT);
}

void HALInit(void)
{
  rcu_periph_clock_enable(RCU_AF);
  gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

  GPIOInit();
  I2C0Init();
}
