#include "board.h"
#include "systick.h"
#include "gd32f30x_gpio.h"
#include <tlv320dac3100.h>
#ifdef I2C_SOFT
#include <i2c_soft.h>
#else
#include "gd32f30x_i2c.h"
#endif

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

#ifdef I2C_SOFT
void SCL_HIGH(int channel)
{
  gpio_bit_write(GPIOB, GPIO_PIN_8, 1);
}

void SCL_LOW(int channel)
{
  gpio_bit_write(GPIOB, GPIO_PIN_8, 0);
}

void SDA_HIGH(int channel)
{
  gpio_bit_write(GPIOB, GPIO_PIN_9, 1);
}

void SDA_LOW(int channel)
{
  gpio_bit_write(GPIOB, GPIO_PIN_9, 0);
}

int SDA_IN(int channel)
{
  return gpio_input_bit_get(GPIOB, GPIO_PIN_9);
}

int SCL_IN(int channel)
{
  return gpio_input_bit_get(GPIOB, GPIO_PIN_8);
}

static void I2CSoftInit(void)
{
  // enable GPIOB clock
  rcu_periph_clock_enable(RCU_GPIOB);

  i2c_soft_init(0);

  // PB8 is I2C_SCL
  // PB9 is I2C_SDA
  gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_8);
  gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_9);
}
#else
static void I2C0Init(void)
{
  // enable GPIOB clock
  rcu_periph_clock_enable(RCU_GPIOB);

  // connect PB8 to I2C_SCL
  // connect PB9 to I2C_SDA
  gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
  gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
  gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);

  // enable I2C clock
  rcu_periph_clock_enable(RCU_I2C0);
  // configure I2C clock
  i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
  // configure I2C address
  i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0);
  // enable I2CX
  i2c_enable(I2C0);
  // enable acknowledge
  i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

static void I2C_Stop(uint32_t instance, unsigned int timeout)
{
  // send a stop condition to I2C bus
  i2c_stop_on_bus(instance);
  // i2c master sends STOP signal successfully
  while((I2C_CTL0(instance) & I2C_CTL0_STOP) && timeout)
    timeout--;
}

static int I2C_Transfer(uint32_t instance, unsigned short address, unsigned char *wdata, unsigned int wsize,
                 unsigned char *rdata, unsigned int rsize, unsigned int timeout)
{
  unsigned int t = timeout;
  // i2c master sends start signal only when the bus is idle
  while(i2c_flag_get(instance, I2C_FLAG_I2CBSY) && t)
    t--;
  if (!t)
    return 1;
  //start
  i2c_start_on_bus(instance);
  t = timeout;
  while(!i2c_flag_get(instance, I2C_FLAG_SBSEND) && t)
    t--;
  if (!t)
    return 2;
  // send address
  i2c_master_addressing(instance, address, wsize ? I2C_TRANSMITTER : I2C_RECEIVER);
  t = timeout;
  while(!i2c_flag_get(instance, I2C_FLAG_ADDSEND) && t)
    t--;
  if (!t)
  {
    I2C_Stop(instance, timeout);
    return 3;
  }
  i2c_flag_clear(instance, I2C_FLAG_ADDSEND);

  unsigned int cnt = wsize;
  // send data
  while (cnt--)
  {
    // wait until the transmit data buffer is empty
    t = timeout;
    while(!i2c_flag_get(instance, I2C_FLAG_TBE) && t)
      t--;
    if (!t)
    {
      I2C_Stop(instance, timeout);
      return 4;
    }
    i2c_data_transmit(instance, *wdata++);
    // wait until BTC bit is set
    while(!i2c_flag_get(instance, I2C_FLAG_BTC) && t)
      t--;
    if (!t)
    {
      I2C_Stop(instance, timeout);
      return 5;
    }
  }

  if (wsize && rsize) // repeated start & send address
  {
    i2c_start_on_bus(instance);
    t = timeout;
    while(!i2c_flag_get(instance, I2C_FLAG_SBSEND) && t)
      t--;
    if (!t)
      return 6;
    // send address
    i2c_master_addressing(instance, address, I2C_RECEIVER);
    t = timeout;
    while(!i2c_flag_get(instance, I2C_FLAG_ADDSEND) && t)
      t--;
    if (!t)
    {
      I2C_Stop(instance, timeout);
      return 7;
    }
    i2c_flag_clear(instance, I2C_FLAG_ADDSEND);
  }

  i2c_ack_config(instance, I2C_ACK_ENABLE);

  // read data
  while (rsize--)
  {
    if (!rsize)
      i2c_ack_config(instance, I2C_ACK_DISABLE);
    while(!i2c_flag_get(instance, I2C_FLAG_RBNE) && t)
      t--;
    if (!t)
    {
      I2C_Stop(instance, timeout);
      return 8;
    }
    *rdata++ = i2c_data_receive(instance);
  }

  I2C_Stop(instance, timeout);

  return 0;
}
#endif

static void TLVInit(void)
{
  // TLV reset pin
  gpio_bit_write(GPIOB, GPIO_PIN_7, 0);
  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_7);
}

int tlv320dac3100_read(unsigned char register_number, unsigned char *value)
{
#ifdef I2C_SOFT
  return i2c_soft_command(0, TLV320DAC3100_I2C_ADDRESS, &register_number,
    1, NULL, 0, value, 1, I2C_TIMEOUT);
#else
  return I2C_Transfer(I2C0, TLV320DAC3100_I2C_ADDRESS, &register_number, 1, value, 1, I2C_TIMEOUT);
#endif
}

int tlv320dac3100_write(unsigned char register_number, unsigned char value)
{
#ifdef I2C_SOFT
  return i2c_soft_command(0, TLV320DAC3100_I2C_ADDRESS, &register_number,
    1, &value, 1, NULL, 0, I2C_TIMEOUT);
#else
  unsigned char data[2];
  data[0] = register_number;
  data[1] = value;
  return I2C_Transfer(I2C0, TLV320DAC3100_I2C_ADDRESS, data, 2, NULL, 0, I2C_TIMEOUT);
#endif
}

void HALInit(void)
{
  rcu_periph_clock_enable(RCU_AF);
  gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

  GPIOInit();
#ifdef I2C_SOFT
  I2CSoftInit();
#else
  I2C0Init();
#endif
  TLVInit();
}
