#include "board.h"
#include <i2c_soft.h>
#include <ina226.h>
#include <mcp3421.h>
#include <ads1115.h>
#include <generic_dds.h>
#include <stdio.h>
#include <hardware/clocks.h>
#include <hardware/pio.h>
#include <pico/stdio.h>
#include "dev_si5351.h"
#include <pico/time.h>
#include <pwm.pio.h>
#include <counter.pio.h>
#include <spi.pio.h>
#include <string.h>

#include "dev_dds.h"

#define PIN_SDA 0
#define PIN_SCL 1

#define PIN_MOSI 0
#define PIN_SCK  1
#define PIN_MISO 2
#define PIN_NCS  3

#define GATE_PIO pio0
#define GATE_SM 3

typedef struct
{
  PIO spi_pio;
  unsigned int spi_sm;
  int pins[6];
} ModulePredefinedInfo;

typedef struct
{
  PIO pio;
  unsigned int pwm_frequency[2];
  unsigned int pwm_duty[2];
} ModuleInfo;

static const ModulePredefinedInfo module_predefined_info[MAX_DEVICES] =
{
  {.spi_pio = pio0, .spi_sm = 2, .pins = {PIN_SDA1, PIN_SCL1, PIN_1_0, PIN_1_1, PIN_1_2, PIN_1_3}},
  {.spi_pio = pio1, .spi_sm = 2, .pins = {PIN_SDA2, PIN_SCL2, PIN_2_0, PIN_2_1, PIN_2_2, PIN_2_3}},
  {.spi_pio = pio1, .spi_sm = 3, .pins = {PIN_SDA3, PIN_SCL3, PIN_3_0, PIN_3_1, PIN_3_2, PIN_3_3}},
  {.spi_pio = pio2, .spi_sm = 2, .pins = {PIN_SDA4, PIN_SCL4, PIN_4_0, PIN_4_1, PIN_4_2, PIN_4_3}},
  {.spi_pio = pio2, .spi_sm = 3, .pins = {PIN_SDA5, PIN_SCL5, PIN_5_0, PIN_5_1, PIN_5_2, PIN_5_3}}
};

static bool led_state;
static int pwm_program_offset, counter_program_offset, spi_program_offset;
static ModuleInfo module_info[MAX_DEVICES];
static int last_allocated_pio;

// Write `period` to the input shift register
void pio_pwm_set_params(PIO pio, uint sm, unsigned int period, unsigned int level) {
  pio_sm_set_enabled(pio, sm, false);
  pio_sm_put_blocking(pio, sm, period-1);
  pio_sm_exec(pio, sm, pio_encode_pull(false, false));
  pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));
  pio_sm_exec(pio, sm, pio_encode_jmp(pwm_program_offset));
  pio_sm_set_enabled(pio, sm, true);
  pio_sm_put_blocking(pio, sm, level);
}

void counter_pio_init(int module_id, unsigned int sm)
{
  PIO pio = module_info[module_id].pio;
  if (!pio)
    return;
  if (sm > 1)
    return;
  int pin = module_predefined_info[module_id].pins[2 + sm];
  counter_program_init(pio, sm, counter_program_offset, pin, PIN_GATE);
  pio_sm_put_blocking(pio, sm, UINT32_MAX);
  pio_sm_exec(pio, sm, pio_encode_pull(false, false));
  pio_sm_set_enabled(pio, sm, true);
}

int counter_get(int module_id, int pin_id)
{
  if (pin_id < 0 || pin_id > 1)
    return -1;
  PIO pio = module_info[module_id].pio;
  if (!pio)
    return -1;

  int value = -1;

  while (pio_sm_get_rx_fifo_level(pio, pin_id))
    value = UINT32_MAX - pio_sm_get(pio, pin_id);
  return value;
}

int pwm_enable(int module_id, int pin_id, int enable)
{
  if (pin_id < 0 || pin_id > 1)
    return 1;
  PIO pio = module_info[module_id].pio;
  if (!pio)
    return 1;

  int pin = module_predefined_info[module_id].pins[2 + pin_id];
  if (enable)
  {
    pwm_program_init(pio, pin_id, pwm_program_offset, pin);
    pwm_set_frequency_and_duty(module_id, pin_id, module_info[module_id].pwm_frequency[pin_id], module_info[module_id].pwm_duty[pin_id]);
  }
  else
  {
    pio_sm_set_enabled(pio, pin_id, false);
    gpio_init(pin);
  }
  return 0;
}

int pwm_set_frequency_and_duty(int module_id, int pin_id, unsigned int frequency, unsigned int duty)
{
  if (pin_id < 0 || pin_id > 1)
    return 1;
  PIO pio = module_info[module_id].pio;
  if (!pio)
    return 1;
  pio_pwm_set_params(pio, pin_id, frequency, duty);
  return 0;
}

static void configure_gate_pin(void)
{
  pwm_program_init(GATE_PIO, GATE_SM, pwm_program_offset, PIN_GATE);
  unsigned int period = clock_get_hz(clk_sys) / 3;
  // level = 1% of period
  unsigned int l = period / 100;
  // 1 second gate low (3 cycles pio program length) period = 150000000 / 3
  pio_pwm_set_params(GATE_PIO, GATE_SM, period + l, l);
}

static void configure_ports(void)
{
  gpio_init(PIN_RESET);
  gpio_set_dir(PIN_RESET, GPIO_OUT);
  gpio_init(PIN_LED);
  gpio_set_dir(PIN_LED, GPIO_OUT);

  for (int i = 0; i < 3; i++)
  {
    PIO pio = i == 0 ? pio0 : (i == 1 ? pio1 : pio2);
    pwm_program_offset = pio_add_program(pio, &pwm_program);
    counter_program_offset = pio_add_program(pio, &counter_program);
    spi_program_offset = pio_add_program(pio, &spi_cpha0_program);
  }

  configure_gate_pin();

  led_state = false;
}

void blink_led(void)
{
  led_state = !led_state;
  gpio_put(PIN_LED, led_state);
}

static void configure_i2c(void)
{
  for (int i = 0; i < MAX_DEVICES; i++)
  {
    int pin_scl = module_predefined_info[i].pins[PIN_SCL];
    int pin_sda = module_predefined_info[i].pins[PIN_SDA];
    gpio_init(pin_scl);
    gpio_init(pin_sda);
    i2c_soft_init(i);
    gpio_put(pin_scl, false);
    gpio_put(pin_sda, false);
    gpio_pull_up(pin_scl);
    gpio_pull_up(pin_sda);
  }
}

void configure_hal(void)
{
  memset(&module_info, 0, sizeof(module_info));
  last_allocated_pio = -1;
  configure_i2c();
  configure_ports();
}

void change_channel(int channel)
{
}

void SCL_HIGH(int channel)
{
  gpio_set_dir(module_predefined_info[channel].pins[PIN_SCL], GPIO_IN);
}

void SCL_LOW(int channel)
{
  gpio_set_dir(module_predefined_info[channel].pins[PIN_SCL], GPIO_OUT);
}

void SDA_HIGH(int channel)
{
  gpio_set_dir(module_predefined_info[channel].pins[PIN_SDA], GPIO_IN);
}

void SDA_LOW(int channel)
{
  gpio_set_dir(module_predefined_info[channel].pins[PIN_SDA], GPIO_OUT);
}

int SDA_IN(int channel)
{
  return gpio_get(module_predefined_info[channel].pins[PIN_SDA]);
}

int SCL_IN(int channel)
{
  return gpio_get(module_predefined_info[channel].pins[PIN_SCL]);
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

unsigned long long int time_us(void)
{
  return time_us_64();
}

void delay_us(unsigned int us)
{
  sleep_us(us);
}

int dds_command(DeviceObject *o, unsigned char cmd, dds_cmd *data)
{
  dds_i2c_command c;
  c.device_command = DEVICE_COMMAND_DDS_COMMAND;
  c.command = cmd;
  c.channel = data->channel;
  switch (cmd)
  {
    case DDS_COMMAND_ENABLE_OUTPUT:
      c.c1.parameter = data->enable_command.enable;
      return o->transfer(o, (unsigned char*)&c, 4, NULL, 0);
    case DDS_COMMAND_SET_ATTENUATOR:
      c.c1.parameter = data->set_attenuator_command.attenuator_value;
      return o->transfer(o, (unsigned char*)&c, 4, NULL, 0);
    case DDS_COMMAND_SET_FREQUENCY:
    case DDS_COMMAND_SET_FREQUENCY_CODE:
      c.c10.freq = data->set_frequency_command.frequency;
      c.c10.div = data->set_frequency_command.divider;
      return o->transfer(o, (unsigned char*)&c, 13, NULL, 0);
    case DDS_COMMAND_SET_MODE:
      c.c1.parameter = data->set_mode_command.mode;
      return o->transfer(o, (unsigned char*)&c, 4, NULL, 0);
    case DDS_COMMAND_SWEEP:
    case DDS_COMMAND_SWEEP_CODES:
      c.c18.freq = data->sweep_command.frequency;
      c.c18.step = data->sweep_command.step;
      c.c18.points = data->sweep_command.points;
      c.c18.div = data->sweep_command.divider;
      return o->transfer(o, (unsigned char*)&c, 21, NULL, 0);
    default:
      return 1;
  }
}

int si5351_write(unsigned char device_address, int channel, const unsigned char *data, unsigned int length)
{
  return i2c_soft_command(channel, device_address, NULL, 0, data, length, NULL, 0, I2C_TIMEOUT);
}

int dds_get_config(DdsConfig *cfg, DeviceObject *o)
{
  unsigned char command = DEVICE_COMMAND_GET_CONFIGURATION;
  return o->transfer(o, &command, 1, (unsigned char*)cfg, sizeof(DdsConfig));
}

int mcp9600Read16(int channel, unsigned char address, unsigned char reg, unsigned short *data)
{
  return inaReadRegister(channel, address, reg, data);
}

int mcp9600Read8(int channel, unsigned char address, unsigned char reg, unsigned char *data)
{
  return i2c_soft_command(channel, address, &reg, 1, NULL, 0, data, 1, I2C_TIMEOUT);
}

int mcp9600Write(int channel, unsigned char address, unsigned char reg, unsigned char data)
{
  return i2c_soft_command(channel, address, &reg, 1, &data, 1, NULL, 0, I2C_TIMEOUT);
}

void main_comm_port_write_bytes(const unsigned char *data, int len)
{
  while (len--)
    putchar_raw(*data++);
}

int main_comm_port_read_bytes(unsigned char *buffer, int buffer_size)
{
  int count = 0;
  while (1)
  {
    int ch = getchar_timeout_us(0);
    if (ch == PICO_ERROR_TIMEOUT)
      return count;
    *buffer++ = (unsigned char)ch;
    count++;
  }
}

void release_reset(void)
{
  gpio_put(PIN_RESET, true);
}

int get_interrupt_pin_level(void)
{
  return 0;
}

int alloc_pio(int module_id)
{
  if (last_allocated_pio == 2)
    return 1;
  last_allocated_pio++;
  module_info[module_id].pio = last_allocated_pio == 0 ? pio0 : (last_allocated_pio == 1 ? pio1 : pio2);
  return 0;
}

int i2c_transfer(struct _DeviceObject *o, const void *txdata, unsigned int txdatalength, void *rxdata,
                        unsigned int rxdatalength)
{
  return i2c_soft_command(o->idx, o->device->device_id, (unsigned char*)&o->subdevice, 1,
                    txdata, txdatalength, rxdata, rxdatalength, I2C_TIMEOUT);
}

static void __time_critical_func(spi_rx)(PIO spi_pio, unsigned int spi_sm, int pin_ncs,
                                            unsigned char *rxdata, unsigned int num_bytes)
{
  gpio_put(pin_ncs, false);
  unsigned int tx_remain = num_bytes, rx_remain = num_bytes;
  volatile unsigned char *txfifo = (volatile unsigned char *) &spi_pio->txf[spi_sm];
  volatile unsigned char *rxfifo = (volatile unsigned char *) &spi_pio->rxf[spi_sm];
  while (tx_remain || rx_remain) {
    if (tx_remain && !pio_sm_is_tx_fifo_full(spi_pio, spi_sm)) {
      *txfifo = 0;
      tx_remain--;
    }
    if (rx_remain && !pio_sm_is_rx_fifo_empty(spi_pio, spi_sm)) {
      *rxdata++ = *rxfifo;
      rx_remain--;
    }
  }
  gpio_put(pin_ncs, true);
}

static void __time_critical_func(spi_tx)(PIO spi_pio, unsigned int spi_sm, int pin_ncs, unsigned char subdevice_id,
                                          const unsigned char *txdata, unsigned int num_bytes)
{
  gpio_put(pin_ncs, false);
  unsigned int tx_remain = num_bytes, rx_remain = num_bytes + 1;
  volatile unsigned char *txfifo = (volatile unsigned char *) &spi_pio->txf[spi_sm];
  volatile unsigned char *rxfifo = (volatile unsigned char *) &spi_pio->rxf[spi_sm];
  *txfifo = subdevice_id;
  while (tx_remain || rx_remain) {
    if (tx_remain && !pio_sm_is_tx_fifo_full(spi_pio, spi_sm)) {
      *txfifo = *txdata++;
      tx_remain--;
    }
    if (rx_remain && !pio_sm_is_rx_fifo_empty(spi_pio, spi_sm)) {
      volatile unsigned char dummy = *rxfifo;
      (void)dummy;
      rx_remain--;
    }
  }
  gpio_put(pin_ncs, true);
}

int __time_critical_func(spi_transfer)(struct _DeviceObject *o, const void *txdata, unsigned int txdatalength, void *rxdata,
                                       unsigned int rxdatalength)
{
  int pin_ncs = module_predefined_info[o->idx].pins[PIN_NCS];
  PIO spi_pio = module_predefined_info[o->idx].spi_pio;
  unsigned int spi_sm = module_predefined_info[o->idx].spi_sm;
  int tx = txdatalength && txdata;
  int rx = rxdatalength && rxdata;
  if (tx || rx)
  {
    if (tx)
      spi_tx(spi_pio, spi_sm, pin_ncs, o->subdevice, txdata, txdatalength);
    if (tx && rx)
      delay_us(10);
    if (rx)
      spi_rx(spi_pio, spi_sm, pin_ncs, rxdata, rxdatalength);
  }
  return 0;
}

void init_spi(int module_id)
{
  int pin_ncs = module_predefined_info[module_id].pins[PIN_NCS];
  int pin_sck = module_predefined_info[module_id].pins[PIN_SCK];
  int pin_mosi = module_predefined_info[module_id].pins[PIN_MOSI];
  int pin_miso = module_predefined_info[module_id].pins[PIN_MISO];
  PIO spi_pio = module_predefined_info[module_id].spi_pio;
  unsigned int spi_sm = module_predefined_info[module_id].spi_sm;
  gpio_init(pin_ncs);
  gpio_put(pin_ncs, true);
  gpio_set_dir(pin_ncs, GPIO_OUT);
  pio_spi_init(spi_pio, spi_sm, spi_program_offset, 8, 64, pin_sck, pin_mosi, pin_miso);
}

void init_device_pin(const struct _DeviceObject *o, int pin_no, enum gpio_dir dir)
{
  if (pin_no < 4 || pin_no > 5)
    return;
  int pin = module_predefined_info[o->idx].pins[pin_no];
  gpio_init(pin);
  gpio_set_dir(pin, dir);
}

bool get_device_pin_level(const struct _DeviceObject *o, int pin_no)
{
  if (pin_no < 4 || pin_no > 5)
    return false;
  int pin = module_predefined_info[o->idx].pins[pin_no];
  return gpio_get(pin);
}