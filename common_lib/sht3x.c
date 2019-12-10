#include <board.h>
#include <sht3x.h>

#define SHT3X_ADDRESS 0x88

#define SHT31_COMMAND_START_MEASUREMENT 0x0024
#define SHT31_COMMAND_SOFT_RESET        0xA230
#define SHT31_COMMAND_HEATER_ENABLE     0x6D30
#define SHT31_COMMAND_HEATER_DISABLE    0x6630
#define SHT31_COMMAND_READ_STATUS       0x2DF3
#define SHT31_COMMAND_CLEAR_STATUS      0x4130

#define SHT31_GENERAL_CALL_RESET        0x0006

#define SHT_CRC_POLYNOMIAL 0x31

static unsigned int validateData(const unsigned char *buf, unsigned short *data)
{
  unsigned char crc = 0xFFU;

  crc ^= buf[0];
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;

  crc ^= buf[1];
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;
  crc = crc & 0x80U ? (crc << 1U) ^ SHT_CRC_POLYNOMIAL : crc << 1U;

  if (crc != buf[2])
    return 0;

  *data = ((unsigned short)buf[0] << 8U) | buf[1];
  return 1;
}

unsigned int sht3x_read_status(unsigned short *status)
{
  unsigned char buf[3];
  if (!sht3xRead(SHT3X_ADDRESS, SHT31_COMMAND_READ_STATUS, buf, 3))
    return 0;

  return validateData(buf, status);
}

unsigned int sht3x_get_data(short *temperature, unsigned char *humidity)
{
  unsigned char buf[6];
  unsigned short t, h;

  if (!sht3xCommand(SHT3X_ADDRESS, SHT31_COMMAND_START_MEASUREMENT))
    return 0;
  delayms(30);
  if (!sht3xReadData(SHT3X_ADDRESS, buf, 6))
    return 0;

  if (!validateData(buf, &t))
    return 0;
  if (!validateData(&buf[3], &h))
    return 0;

  *temperature = (1750 * (int)t) / 65535 - 450;
  *humidity = (100 * (unsigned int)h) / 65535;

  return 1;
}
