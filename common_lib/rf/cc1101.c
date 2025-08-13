#include <board.h>
#include <cc1101.h>

typedef struct {
  unsigned char adcRetention;
  unsigned char fifoThr;
  unsigned char sYNCH;
  unsigned char sYNCL;
  unsigned char freqIf;
  unsigned char mode;
  unsigned char chanSpcE;
  unsigned char chanSpcM;
  unsigned char MDMCFG4;
  unsigned char drateM;
  unsigned char DEVIATN;
  unsigned char FOCCFG;
  unsigned char aGCCTRL2;
  unsigned char aGCCTRL1;
  unsigned char WORCTRL;
  unsigned char FSCAL3;
  unsigned char FSCAL2;
  unsigned char FSCAL1;
  unsigned char FSCAL0;
  unsigned char TEST2;
  unsigned char TEST1;
  unsigned char TEST0;
  unsigned char pktFormat;
} BaudRateAndModeParameters;

#define CC1101_IOCFG2  0U
#define CC1101_AGCCTRL2 0x1BU
#define CC1101_IOCFG0  2U
#define CC1101_WORCTRL 0x20U
#define CC1101_FSCAL3  0x23U
#define CC1101_TEST2   0x2CU

#define CC1101_PKTLEN  6U

#define CC1101_PACKET_RECEIVED 7U
#define CC1101_RX_FIFO_FULL_OR_END_OF_THE_PACKET 1U

#define CC1101_APPEND_STATUS 4U
#define CC1101_CRC_AUTOFLUSH 8U

#define CC1101_WHITE_DATA 0x40U
#define CC1101_CRC_EN 4U

#define CC1101_MANCHESTER_EN  8U
#define CC1101_DEM_DCFILT_OFF 0x80U
#define CC1101_FEC_EN 0x80U
#define CC1101_RX_TIME_RSSI 0x10U
#define CC1101_RX_TIME_QUAL 8U
#define CC1101_PINCTRL_EN 2U
#define CC1101_XOSC_FORCE_ON 1U

#define CC1101_PATABLE 0x3EU

#define FOSC 26000

BaudRateAndModeParameters baudRateAndModeParameters[] = {
  //GFSK 1200
  {
    0x40U,
    7,
    0xD3,
    0x91,
    6,
    0x10, // GFSK
    2,
    0xF8,
    0xF5,
    0x83,
    0x15,
    0x16,
    3,
    0x40,
    0xFB,
    0xE9,
    0x2A,
    0,
    0x1F,
    0x81,
    0x35,
    0x09,
    0 // Normal mode, use FIFOs for RX and TX
  },
  // Infinite RX, RSSI 5 Khz
  {
      0x40U,
      7,
      0xD3,
      0x91,
      6,
      0x10, // GFSK
      2,
      0xF8,
      0xF5,
      0x83,
      0x15,
      0x16,
      3,
      0x40,
      0xFB,
      0xE9,
      0x2A,
      0,
      0x1F,
      0x81,
      0x35,
      0x09,
      0x30 // Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins
  },
    // LaCrosse TX29IT
  {
      0x40U,
      1,
      0x2D,
      0xD4,
      0x0F,
      0, // 2-FSK
      2,
      0xF8,
      0x89,
      0x5C,
      0x56,
      0x16,
      0x43,
      0x68,
      0xF8,
      0xE9,
      0x2A,
      0,
      0x11,
      0x81,
      0x35,
      0x09,
      0 // Normal mode, use FIFOs for RX and TX
  }
};

int cc1101_Init(unsigned int device_num, const cc1101_cfg *cfg)
{
  unsigned char txdata[21];
  unsigned char rxdata[21];
  unsigned int freq;
  int rc;
  BaudRateAndModeParameters p;

  p = baudRateAndModeParameters[cfg->mode];

  // disable the clock output on GDO0
  txdata[0] = CC1101_IOCFG0;
  txdata[1] = cfg->crcEnabled ? CC1101_PACKET_RECEIVED : CC1101_RX_FIFO_FULL_OR_END_OF_THE_PACKET;
  txdata[2] = p.adcRetention | cfg->rxAttenuation | p.fifoThr;
  txdata[3] = p.sYNCH;
  txdata[4] = p.sYNCL;
  rc = cc1101_RW(device_num, txdata, rxdata, 5);
  if (rc)
    return rc;

  freq = (unsigned int)((unsigned long long int)cfg->freq * 65536 / FOSC);

  // packet length
  txdata[0] = CC1101_PKTLEN;
  txdata[1] = cfg->packetLength;
  txdata[2] = cfg->addressCheck | cfg->pqt | (cfg->crcAutoflush ? CC1101_CRC_AUTOFLUSH : 0) | (cfg->appendStatus ? CC1101_APPEND_STATUS : 0);
  txdata[3] = (cfg->whiteData ? CC1101_WHITE_DATA : 0) | (cfg->crcEnabled ? CC1101_CRC_EN : 0) | cfg->packetLengthConfig | p.pktFormat;
  txdata[4] = cfg->address;
  txdata[5] = cfg->channel;
  txdata[6] = p.freqIf;
  txdata[7] = cfg->freqoffset;
  txdata[8] = freq >> 16;
  txdata[9] = (freq >> 8) & 0xFF;
  txdata[10] = freq & 0xFF;
  txdata[11] = p.MDMCFG4;
  txdata[12] = p.drateM;
  txdata[13] = (cfg->demDcFiltOff ? CC1101_DEM_DCFILT_OFF : 0) | p.mode | (cfg->manchesterEn ? CC1101_MANCHESTER_EN : 0) | cfg->syncMode;
  txdata[14] = (cfg->enableFec ? CC1101_FEC_EN : 0) | cfg->numPreamble | p.chanSpcE;
  txdata[15] = p.chanSpcM;
  txdata[16] = p.DEVIATN;
  txdata[17] = (cfg->rxTimeRSSI ? CC1101_RX_TIME_RSSI : 0) | (cfg->rxTimeQual ? CC1101_RX_TIME_QUAL : 0) | cfg->rxTime;
  txdata[18] = cfg->ccaMode | cfg->rxoffMode | cfg->txoffMode;
  txdata[19] = (cfg->enablePinControl ? CC1101_PINCTRL_EN : 0) | (cfg->forceXOSCOn ? CC1101_XOSC_FORCE_ON : 0) | cfg->autocal | cfg->poTimeout;
  txdata[20] = p.FOCCFG;
  rc = cc1101_RW(device_num, txdata, rxdata, 21);
  if (rc)
    return rc;

  txdata[0] = CC1101_AGCCTRL2;
  txdata[1] = p.aGCCTRL2;
  txdata[2] = p.aGCCTRL1;
  rc = cc1101_RW(device_num, txdata, rxdata, 3);
  if (rc)
    return rc;

  txdata[0] = CC1101_WORCTRL;
  txdata[1] = p.WORCTRL;
  rc = cc1101_RW(device_num, txdata, rxdata, 2);
  if (rc)
    return rc;

  txdata[0] = CC1101_FSCAL3;
  txdata[1] = p.FSCAL3;
  txdata[2] = p.FSCAL2;
  txdata[3] = p.FSCAL1;
  txdata[4] = p.FSCAL0;
  rc = cc1101_RW(device_num, txdata, rxdata, 5);
  if (!rc)
    return 0;

  txdata[0] = CC1101_TEST2;
  txdata[1] = p.TEST2;
  txdata[2] = p.TEST1;
  txdata[3] = p.TEST0;
  rc = cc1101_RW(device_num, txdata, rxdata, 4);
  if (!rc)
    return 0;

  return cc1101_setTxPower(device_num, cfg->txPower);
}

int cc1101_setTxPower(unsigned int device_num, unsigned char power)
{
  unsigned char txdata[2];
  unsigned char rxdata[2];

  txdata[0] = CC1101_PATABLE;
  txdata[1] = power;
  return cc1101_RW(device_num, txdata, rxdata, 2);
}

int cc1101_Check(unsigned int device_num)
{
  unsigned char txdata[2];
  unsigned char rxdata[2];

  txdata[0] = CC1101_PARTNUM;
  int rc = cc1101_RW(device_num, txdata, rxdata, 2);
  if (rc)
    return rc;
  if (rxdata[1])
    return 100;

  txdata[0] = CC1101_VERSION;
  rc = cc1101_RW(device_num, txdata, rxdata, 2);
  if (rc)
    return rc;
  return rxdata[1] == 0x14 ? 0 : 101;
}

void cc1101_powerOn(unsigned int device_num)
{
  cc1101_CSN_CLR(device_num);
  cc1101_CSN_SET(device_num);
  cc1101_CSN_CLR(device_num);
  delay(50);
  cc1101_CSN_SET(device_num);
  delay(50);
}

short cc1101_calculateRSSI(unsigned char rssiIn)
{
  short rssi = rssiIn;
  if (rssi >= 128)
    return (rssi - 256) / 2 - 74;
  return rssi / 2 - 74;
}
