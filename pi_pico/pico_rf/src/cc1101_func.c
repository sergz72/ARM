#include <board.h>
#include <cc1101.h>
#include <cc1101_func.h>
#include <string.h>

static unsigned char rxdata[64];
static unsigned char txdata[64];

static cc1101_cfg cfgDevice = {
  CC1101_RX_ATTENUATION_0DB,
  0,
  CC1101_PQT0,
  1,
  1,
  CC1101_ADDRESS_CHECK,
  0,
  1,
  CC1101_PACKET_LENGTH_FIXED,
  0,
  0,
  0,
  0,
  0,
  RF_MODE,
  0,
  CC1101_SYNC_MODE_3032,
  0,
  CC1101_PREAMBLE_4,
  0,
  0,
  CC1101_RX_TIME_7,
  CC1101_CCA_MODE_ALWAYS,
  CC1101_RXOFF_MODE,
  CC1101_TXOFF_MODE,
  CC1101_AUTOCAL_FROM_IDLE,
  CC1101_PO_TIMEOUT_150US,
  0,
  0,
  TX_POWER
};

int cc1101Init(unsigned char device_address, unsigned char packet_length, unsigned int frequency)
{
  unsigned char status;

  int rc = cc1101_strobe(0, CC1101_STROBE_SRES, &status);
  if (rc)
    return rc;

  rc = cc1101_Check(0);
  if (rc)
    return rc;

  cfgDevice.address = device_address;
  cfgDevice.packetLength = packet_length;
  cfgDevice.freq = frequency;
  rc = cc1101_Init(0, &cfgDevice);
  if (rc)
    return rc;

  return cc1101_strobe(0, CC1101_STROBE_SIDLE, &status);
}

int cc1101ReceiveStart(void)
{
  unsigned char status;
  int rc = cc1101_strobe(0, CC1101_STROBE_SFRX, &status);
  if (rc)
    return rc;
  return cc1101_strobe(0, CC1101_STROBE_SRX, &status);
}

int cc1101ReceiveStop(void)
{
  unsigned char status;
  return cc1101_strobe(0, CC1101_STROBE_SIDLE, &status);
}

int cc1101Receive(unsigned char **buffer, unsigned char *size)
{
  unsigned char status;

  if (!cc1101_GD0)
    return 1;

  txdata[0] = CC1101_RXBYTES;
  int rc = cc1101_RW(0, txdata, rxdata, 2);
  if (rc)
  {
    cc1101_strobe(0, CC1101_STROBE_SFRX, &status);
    cc1101_strobe(0, CC1101_STROBE_SIDLE, &status);
    return rc;
  }
  unsigned char sz = rxdata[1];
  *size = sz - 1;

  txdata[0] = CC1101_FIFO | CC1101_READ;
  rc = cc1101_RW(0, txdata, rxdata, sz + 1);
  if (rc)
  {
    cc1101_strobe(0, CC1101_STROBE_SIDLE, &status);
    return rc;
  }

  *buffer = &rxdata[2];

  return 0;
}

int cc1101Send(unsigned char host_address, unsigned char *data, unsigned char length)
{
  unsigned char status;

  int rc = cc1101_strobe(0, CC1101_STROBE_SFTX, &status);
  if (rc)
    return rc;

  memcpy(&txdata[2], data, length);
  txdata[0] = CC1101_FIFO;
  txdata[1] = host_address;

  rc = cc1101_RW(0, txdata, rxdata, length + 2);
  if (rc)
  {
    cc1101_strobe(0, CC1101_STROBE_SIDLE, &status);
    return rc;
  }

  rc = cc1101_strobe(0, CC1101_STROBE_STX, &status);
  if (rc)
  {
    cc1101_strobe(0, CC1101_STROBE_SIDLE, &status);
    return rc;
  }

  return 0;
}
