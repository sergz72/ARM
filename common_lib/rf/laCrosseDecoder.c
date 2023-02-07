#include "laCrosseDecoder.h"
#include "board.h"

static unsigned char crc8(const unsigned char *message, unsigned int nBytes, unsigned char polynomial, unsigned char init)
{
  unsigned char remainder = init;
  unsigned int byte, bit;

  for (byte = 0; byte < nBytes; ++byte) {
    remainder ^= message[byte];
    for (bit = 0; bit < 8; ++bit) {
      if (remainder & 0x80) {
        remainder = (remainder << 1) ^ polynomial;
      } else {
        remainder = (remainder << 1);
      }
    }
  }
  return remainder;
}

const char * laCrosseDecode(int sensorId, unsigned char *packet, short int *t, unsigned short int *h)
{
  int sensor_id;

  if (packet[4] != crc8(packet, 4, 0x31, 0))
    return "bad CRC";
  if ((packet[0] & 0xF0) != 0x90)
    return "unknown packet header";
  sensor_id = ((packet[0] & 0x0f) << 2) | (packet[1] >> 6);
  if (sensor_id != sensorId)
    return "unknown sensor id";

  *t = (short int)((packet[1] & 0x0F) * 100 + ((packet[2] & 0xF0) >> 4) * 10 + (packet[2] & 0x0F) - 400);

  return NULL;
}
