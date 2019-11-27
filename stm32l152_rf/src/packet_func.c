#include <board.h>
#include <packet_func.h>
#include <st7066.h>
#include <aes128.h>
#include <temperature_func.h>
#include <adc_func.h>
#include <th02_func.h>
#include <cc1101.h>
#include <clock_func.h>

static const unsigned char aes_key[] = {???};

typedef struct __attribute__((__packed__))
{
  unsigned int timestamp;
  short extTemperatureW;
  short extTemperatureN;
  unsigned char magic[8];
} InPacket;

typedef struct __attribute__((__packed__))
{
  unsigned int timestamp;
  short temperature;
  unsigned short batteryVoltage;
  short rssi;
  unsigned char lqi;
  unsigned char humidity;
  unsigned char magic[4];
} OutPacket;

static InPacket inPacket;
static OutPacket outPacket;
static unsigned int savedTimestamp;

static void showError(void)
{
  st7066_write_data(' ');
  st7066_write_data('E');
  st7066_write_data('r');
  st7066_write_data('r');
  st7066_write_data(' ');
}

void packetInit(unsigned int rc)
{
  st7066_set_ddram_address(ST066_LINE2_ADDRESS);
  st7066_write_data('O');
  st7066_write_data('u');
  st7066_write_data('N');
  if (!rc)
    showError();
  st7066_set_ddram_address(ST066_LINE2_ADDRESS + 11);
  st7066_write_data('O');
  st7066_write_data('u');
  st7066_write_data('W');
  if (!rc)
    showError();
  else
  {
    savedTimestamp = 0;
    outPacket.magic[0] = 0x55;
    outPacket.magic[1] = 0xAA;
    outPacket.magic[2] = 0x11;
    outPacket.magic[3] = 0x22;
    aes128_set_key(aes_key);
  }
}

static void showTemperatures(void)
{
  st7066_set_ddram_address(ST066_LINE2_ADDRESS + 4);
  showTemperature(inPacket.extTemperatureN);
  st7066_set_ddram_address(ST066_LINE2_ADDRESS + 15);
  showTemperature(inPacket.extTemperatureW);
}

unsigned int packetProcess(unsigned char *inData, unsigned char *outData)
{
  aes128_decrypt((unsigned char*)&inPacket, inData);

  if (inPacket.timestamp >= savedTimestamp && inPacket.magic[0] == 0x55U && inPacket.magic[1] == 0xAAU)
  {
    savedTimestamp = inPacket.timestamp;
    ClockUpdate(inPacket.timestamp);
    showTemperatures();
    outPacket.timestamp = inPacket.timestamp;
    outPacket.batteryVoltage = battery_voltage;
    outPacket.temperature = (short)prev_temperature;
    outPacket.humidity = prev_humi;
    outPacket.rssi = cc1101_calculateRSSI(inData[16]);
    outPacket.lqi = inData[17];
    aes128_encrypt(outData, (unsigned char*)&outPacket);
    return 1;
  }

  return 0;
}
