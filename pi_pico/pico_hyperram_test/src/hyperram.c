#include "board.h"
#include "hyperram.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define HYPERRAM_CS_SET gpio_put(HYPERRAM_CS, true)
#define HYPERRAM_CS_CLR gpio_put(HYPERRAM_CS, false)
#define HYPERRAM_RESET_SET gpio_put(HYPERRAM_RESET, true)
#define HYPERRAM_RESET_CLR gpio_put(HYPERRAM_RESET, false)
#define HYPERRAM_CK_TOGGLE gpio_xor_mask(1<<HYPERRAM_CK)
#define HYPERRAM_CK_CLR gpio_put(HYPERRAM_CK, false)
#define HYPERRAM_CK_SET gpio_put(HYPERRAM_CK, true)
#define HYPERRAM_RWDS_SET gpio_put(HYPERRAM_RWDS, true)
#define HYPERRAM_RWDS_CLR gpio_put(HYPERRAM_RWDS, false)
#define HYPERRAM_RWDS_GET gpio_get(HYPERRAM_RWDS)
#define HYPERRAM_DATA_SET(v) gpio_put_masked(0xFF, (v) & 0xFF)
#define HYPERRAM_IO_DIR_SET gpio_put(HYPERRAM_IO_DIR, true)
#define HYPERRAM_IO_DIR_CLR gpio_put(HYPERRAM_IO_DIR, false)
#define HYPERRAM_RWDS_DIR_SET gpio_put(HYPERRAM_RWDS_DIR, true)
#define HYPERRAM_RWDS_DIR_CLR gpio_put(HYPERRAM_RWDS_DIR, false)

static const unsigned int io_pins[] = {HYPERRAM_DQ0, HYPERRAM_DQ1, HYPERRAM_DQ2, HYPERRAM_DQ3, HYPERRAM_DQ4, HYPERRAM_DQ5,
                                        HYPERRAM_DQ6, HYPERRAM_DQ7};
static const unsigned int output_pins[] = {HYPERRAM_CK, HYPERRAM_CS, HYPERRAM_RESET, HYPERRAM_IO_DIR, HYPERRAM_RWDS_DIR};

static int latency;

static void io_set_dir(bool out)
{
  if (out)
    HYPERRAM_IO_DIR_CLR;
  else
    HYPERRAM_IO_DIR_SET;
  for (int i = 0; i < sizeof(io_pins) / sizeof(io_pins[0]); i++)
    gpio_set_dir(io_pins[i], out);
}

static void rwds_set_dir(bool out)
{
  if (out)
    HYPERRAM_RWDS_DIR_CLR;
  else
    HYPERRAM_RWDS_DIR_SET;
  gpio_set_dir(HYPERRAM_RWDS, out);
}

void hyperram_init(int _latency)
{
  latency = _latency;
  HYPERRAM_CS_SET;
  HYPERRAM_CK_CLR;
  HYPERRAM_RESET_CLR;
  for (int i = 0; i < sizeof(output_pins) / sizeof(output_pins[0]); i++)
  {
    unsigned int pin = output_pins[i];
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
  }
  HYPERRAM_CS_SET;
  for (int i = 0; i < sizeof(io_pins) / sizeof(io_pins[0]); i++)
  {
    unsigned int pin = io_pins[i];
    gpio_init(pin);
    gpio_pull_down(pin);
  }
  io_set_dir(GPIO_IN);
  rwds_set_dir(GPIO_IN);
  HYPERRAM_RESET_SET;
}

static void ck_toggle(void)
{
  busy_wait_us_32(1);
  HYPERRAM_CK_TOGGLE;
  busy_wait_us_32(1);
}

static int send_address(unsigned int address, unsigned int command)
{
  io_set_dir(GPIO_OUT);
  HYPERRAM_CS_CLR;
  int latency2x = HYPERRAM_RWDS_GET ? 1 : 0;
  HYPERRAM_DATA_SET((address >> 27) | command);
  ck_toggle();
  HYPERRAM_DATA_SET(address >> 19);
  ck_toggle();
  HYPERRAM_DATA_SET(address >> 11);
  ck_toggle();
  HYPERRAM_DATA_SET(address >> 3);
  ck_toggle();
  HYPERRAM_DATA_SET(0);
  ck_toggle();
  HYPERRAM_DATA_SET(address & 7);
  ck_toggle();
  return latency2x;
}

static void wait(int latency2x, int adder)
{
  int counter = (latency2x ? latency * 4 : latency * 2) + adder;
  while (counter--)
    ck_toggle();
}

void hyperram_read(unsigned int address, unsigned char *data, unsigned int nbytes)
{
  HYPERRAM_CK_CLR;
  int latency2x = send_address(address, 0xA0); // read transaction, linear burst
  io_set_dir(GPIO_IN);
  wait(latency2x, -2);
  bool start = false;
  //nbytes >>= 1;
  while (nbytes)
  {
    if (HYPERRAM_RWDS_GET != 0)
      start = true;
    if (start)
    {
      //*data++ = HYPERRAM_RWDS_GET ? 1 : 0;
      *data++ = (unsigned char)gpio_get_all();
      nbytes--;
    }
    ck_toggle();
  }
  HYPERRAM_CS_SET;
}

void hyperram_write(unsigned int address, unsigned char *data, unsigned int nbytes)
{
  HYPERRAM_CK_CLR;
  int latency2x = send_address(address, 0x20); // write transaction, linear burst
  wait(latency2x, -2);
  rwds_set_dir(GPIO_OUT);
  HYPERRAM_RWDS_CLR;
  nbytes >>= 1;
  while (nbytes--)
  {
    HYPERRAM_DATA_SET(*data++);
    ck_toggle();
    HYPERRAM_DATA_SET(*data++);
    ck_toggle();
  }
  HYPERRAM_CS_SET;
  io_set_dir(GPIO_IN);
  rwds_set_dir(GPIO_IN);
}

int hyperram_read_callback(unsigned int address, int (*set_byte)(unsigned char c), unsigned int nbytes)
{
  return 1;
}

void hyperram_write_callback(unsigned int address, unsigned char (*next_byte)(void), unsigned int nbytes)
{

}
