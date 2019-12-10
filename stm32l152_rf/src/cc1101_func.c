#include <board.h>
#include <cc1101.h>
#include <packet_func.h>

extern volatile int packet_received;
extern volatile int communication_counter;

static const cc1101_cfg cfgDevice = {
  CC1101_RX_ATTENUATION_0DB,
  17,
  CC1101_PQT0,
  1,
  1,
  CC1101_ADDRESS_CHECK,
  0,
  1,
  CC1101_PACKET_LENGTH_FIXED,
  DEVICE_ADDRESS,
  0,
  0,
  RF_FREQUENCY,
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
  CC1101_RXOFF_MODE_IDLE,
  CC1101_TXOFF_MODE_RX,
  CC1101_AUTOCAL_FROM_IDLE,
  CC1101_PO_TIMEOUT_150US,
  0,
  0,
  TX_POWER
};

static unsigned int cc1101Init(void)
{
  unsigned char status;

  packet_received = 0;

  if (!cc1101_strobe(0, CC1101_STROBE_SRES, &status))
    return 0;

  if (!cc1101_Check(0))
    return 0;

  if (!cc1101_Init(0, &cfgDevice))
    return 0;

  if (!cc1101_strobe(0, CC1101_STROBE_SIDLE, &status))
    return 0;

  delay(100);

  return cc1101_strobe(0, CC1101_STROBE_SRX, &status);
}

unsigned int cc1101Start(void)
{
  unsigned int rc = cc1101Init();

  packetInit(rc);
  return rc;
}

void cc1101Loop(void)
{
  unsigned char status;
  static unsigned char rxdata[64];
  static unsigned char txdata[64];

  if (!packet_received)
    return;

  packet_received = 0;

  txdata[0] = CC1101_RXBYTES;
  if (!cc1101_RW(0, txdata, rxdata, 2) || rxdata[1] != 19)
  {
    cc1101_strobe(0, CC1101_STROBE_SFRX, &status);
    cc1101_strobe(0, CC1101_STROBE_SRX, &status);
    return;
  }

  txdata[0] = CC1101_FIFO | CC1101_READ;
  if (!cc1101_RW(0, txdata, rxdata, 20))
    return;

  if (packetProcess(&rxdata[2], &txdata[2]))
  {
    txdata[0] = CC1101_FIFO;
    txdata[1] = 0; // host address

    if (!cc1101_RW(0, txdata, rxdata, 18))
    {
      cc1101_strobe(0, CC1101_STROBE_SFTX, &status);
      cc1101_strobe(0, CC1101_STROBE_SRX, &status);
      return;
    }

    if (!cc1101_strobe(0, CC1101_STROBE_STX, &status))
    {
      cc1101_strobe(0, CC1101_STROBE_SFTX, &status);
      cc1101_strobe(0, CC1101_STROBE_SRX, &status);
      return;
    }
    communication_counter = 0;
  }
}
