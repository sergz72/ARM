#include <board.h>
#include <dht22.h>
#include <systick.h>

#define DHT22_RESPONSE_LENGTH 5
#define WAIT_TIMEOUT 200
#define ZERO_ONE_THRESHOLD 50

static unsigned int wait_for0(void)
{
  unsigned int start = systick_get_value();
  unsigned int time = 0;

  while (DHT22_DATA_GET) {
    time = systick_get_ms(start);
    if (time > WAIT_TIMEOUT)
      break;
  }

  return time;
}

static unsigned int wait_for1(void)
{
  unsigned int start = systick_get_value();
  unsigned int time;

  while (!DHT22_DATA_GET) {
    time = systick_get_ms(start);
    if (time > WAIT_TIMEOUT)
      return time;
  }

  return 0;
}

static unsigned char receive_bit(void)
{
  unsigned int time;

  if (!wait_for1())
  {
     time = wait_for0();
     if (time <= WAIT_TIMEOUT)
       return time > ZERO_ONE_THRESHOLD ? 1 : 0;
     return 3;
  }

  return 2;
}

static unsigned char receive_byte(unsigned char *data)
{
  unsigned char bit, byte = 0;
  int i;

  for (i = 0; i < 8; i++) {
    byte <<= 1;
    bit = receive_bit();
    if (bit > 1)
      return bit;
    byte |= bit;
  }

  *data = byte;

  return 0;
}

int DHT22_Read(DHT22_Data *dht_data)
{
  unsigned char data[DHT22_RESPONSE_LENGTH];
  unsigned char bit;
  int i;

  // start signal to dht22
  DHT22_DATA_SET;
  delayms(10);
  DHT22_DATA_CLR;
  delayms(3);
  DHT22_DATA_SET;

  systick_start_noint();

  if (wait_for0() > WAIT_TIMEOUT)
  {
    systick_counter_disable();
    return 1;
  }

  bit = receive_bit();
  if (bit > 1)
  {
    systick_counter_disable();
    return bit;
  }

  for (i = 0; i < DHT22_RESPONSE_LENGTH; i++) {
    bit = receive_byte(&data[i]);
    if (bit > 1)
    {
      systick_counter_disable();
      return i + 10;
    }
  }

  bit = data[0] + data[1] + data[2] + data[3];
  if (data[4] != bit)
  {
    systick_counter_disable();
    return 4;
  }

  dht_data->humidity = ((unsigned short)data[0] << 8) + data[1];
  dht_data->temperature = (((unsigned short)(data[2] & 0x7F)) << 8) + data[3];
  if (data[2] & 0x80)
    dht_data->temperature = -dht_data->temperature;

  systick_counter_disable();

  return 0;
}