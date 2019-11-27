#ifndef __CC1101_H
#define __CC1101_H

typedef struct
{
  unsigned char rxAttenuation;
  unsigned char packetLength;
  unsigned char pqt;
  unsigned char crcAutoflush;
  unsigned char appendStatus;
  unsigned char addressCheck;
  unsigned char whiteData;
  unsigned char crcEnabled;
  unsigned char packetLengthConfig;
  unsigned char address;
  unsigned char channel;
  unsigned char freqoffset;
  unsigned int  freq; // in KHz
  unsigned char demDcFiltOff;
  unsigned char mode;
  unsigned char manchesterEn;
  unsigned char syncMode;
  unsigned char enableFec;
  unsigned char numPreamble;
  unsigned char rxTimeRSSI;
  unsigned char rxTimeQual;
  unsigned char rxTime;
  unsigned char ccaMode;
  unsigned char rxoffMode;
  unsigned char txoffMode;
  unsigned char autocal;
  unsigned char poTimeout;
  unsigned char enablePinControl;
  unsigned char forceXOSCOn;
  unsigned char txPower;
} cc1101_cfg;

#define CC1101_READ  0x80U
#define CC1101_BURST 0x40U

#define CC1101_RX_ATTENUATION_0DB  0U
#define CC1101_RX_ATTENUATION_6DB  0x10U
#define CC1101_RX_ATTENUATION_12DB 0x20U
#define CC1101_RX_ATTENUATION_18DB 0x30U

#define CC1101_NO_ADDRESS_CHECK                 0U
#define CC1101_ADDRESS_CHECK                    1U
#define CC1101_ADDRESS_CHECK_AND_0_BCAST        2U
#define CC1101_ADDRESS_CHECK_AND_0_255_BCAST    3U

#define CC1101_PQT0 0U
#define CC1101_PQT1 0x20U
#define CC1101_PQT2 0x40U
#define CC1101_PQT3 0x60U
#define CC1101_PQT4 0x80U
#define CC1101_PQT5 0xA0U
#define CC1101_PQT6 0xC0U
#define CC1101_PQT7 0xE0U

#define CC1101_PACKET_LENGTH_FIXED    0U
#define CC1101_PACKET_LENGTH_VARIABLE 1U
#define CC1101_PACKET_LENGTH_INFINITE 2U

#define CC1101_DRATE_E_0 0U
#define CC1101_DRATE_E_1 1U
#define CC1101_DRATE_E_2 2U
#define CC1101_DRATE_E_3 3U
#define CC1101_DRATE_E_4 4U
#define CC1101_DRATE_E_5 5U
#define CC1101_DRATE_E_6 6U
#define CC1101_DRATE_E_7 7U
#define CC1101_DRATE_E_8 8U
#define CC1101_DRATE_E_9 9U
#define CC1101_DRATE_E_10 10U
#define CC1101_DRATE_E_11 11U
#define CC1101_DRATE_E_12 12U
#define CC1101_DRATE_E_13 13U
#define CC1101_DRATE_E_14 14U
#define CC1101_DRATE_E_15 15U

#define CC1101_MODE_GFSK_1200 0U

#define CC1101_SYNC_MODE_OFF           0U
#define CC1101_SYNC_MODE_1516          1U
#define CC1101_SYNC_MODE_1616          2U
#define CC1101_SYNC_MODE_3032          3U
#define CC1101_SYNC_MODE_CSENSE        4U
#define CC1101_SYNC_MODE_1516_CSENSE   5U
#define CC1101_SYNC_MODE_1616_CSENSE   6U
#define CC1101_SYNC_MODE_3032_CSENSE   7U

#define CC1101_PREAMBLE_2  0U
#define CC1101_PREAMBLE_3  0x10U
#define CC1101_PREAMBLE_4  0x20U
#define CC1101_PREAMBLE_6  0x30U
#define CC1101_PREAMBLE_8  0x40U
#define CC1101_PREAMBLE_12 0x50U
#define CC1101_PREAMBLE_16 0x60U
#define CC1101_PREAMBLE_24 0x70U

#define CC1101_RX_TIME_0 0U
#define CC1101_RX_TIME_1 1U
#define CC1101_RX_TIME_2 2U
#define CC1101_RX_TIME_3 3U
#define CC1101_RX_TIME_4 4U
#define CC1101_RX_TIME_5 5U
#define CC1101_RX_TIME_6 6U
#define CC1101_RX_TIME_7 7U

#define CC1101_CCA_MODE_ALWAYS      0U
#define CC1101_CCA_MODE_RSSI        0x10U
#define CC1101_CCA_MODE_PACKET      0x20U
#define CC1101_CCA_MODE_RSSI_PACKET 0x30U

#define CC1101_RXOFF_MODE_IDLE   0U
#define CC1101_RXOFF_MODE_FSTXON 4U
#define CC1101_RXOFF_MODE_TX     8U
#define CC1101_RXOFF_MODE_RX     0xCU

#define CC1101_TXOFF_MODE_IDLE   0U
#define CC1101_TXOFF_MODE_FSTXON 1U
#define CC1101_TXOFF_MODE_TX     2U
#define CC1101_TXOFF_MODE_RX     3U

#define CC1101_AUTOCAL_NEVER     0U
#define CC1101_AUTOCAL_FROM_IDLE 0x10U
#define CC1101_AUTOCAL_TO_IDLE   0x20U
#define CC1101_AUTOCAL_4TIME     0x30U

#define CC1101_PO_TIMEOUT_2US   0U
#define CC1101_PO_TIMEOUT_40US  4U
#define CC1101_PO_TIMEOUT_150US 8U
#define CC1101_PO_TIMEOUT_600US 0xCU

#define CC1101_TX_POWER_M30_315 0x12U
#define CC1101_TX_POWER_M20_315 0x0DU
#define CC1101_TX_POWER_M15_315 0x1CU
#define CC1101_TX_POWER_M10_315 0x34U
#define CC1101_TX_POWER_0_315   0x51U
#define CC1101_TX_POWER_5_315   0x85U
#define CC1101_TX_POWER_7_315   0xCBU
#define CC1101_TX_POWER_10_315  0xC2U

#define CC1101_TX_POWER_M30_433 0x12U
#define CC1101_TX_POWER_M20_433 0x0EU
#define CC1101_TX_POWER_M15_433 0x1DU
#define CC1101_TX_POWER_M10_433 0x34U
#define CC1101_TX_POWER_0_433   0x60U
#define CC1101_TX_POWER_5_433   0x84U
#define CC1101_TX_POWER_7_433   0xC8U
#define CC1101_TX_POWER_10_433  0xC0U

#define CC1101_TX_POWER_M30_868 0x03U
#define CC1101_TX_POWER_M20_868 0x17U
#define CC1101_TX_POWER_M15_868 0x1DU
#define CC1101_TX_POWER_M10_868 0x26U
#define CC1101_TX_POWER_M6_868  0x37U
#define CC1101_TX_POWER_0_868   0x50U
#define CC1101_TX_POWER_5_868   0x86U
#define CC1101_TX_POWER_7_868   0xCDU
#define CC1101_TX_POWER_10_868  0xC5U
#define CC1101_TX_POWER_12_868  0xC0U

#define CC1101_TX_POWER_M30_915 0x03U
#define CC1101_TX_POWER_M20_915 0x0EU
#define CC1101_TX_POWER_M15_915 0x1EU
#define CC1101_TX_POWER_M10_915 0x27U
#define CC1101_TX_POWER_M6_915  0x38U
#define CC1101_TX_POWER_0_915   0x8EU
#define CC1101_TX_POWER_5_915   0x84U
#define CC1101_TX_POWER_7_915   0xCCU
#define CC1101_TX_POWER_10_915  0xC3U
#define CC1101_TX_POWER_12_915  0xC0U

#define CC1101_STROBE_SRES 0x30U
#define CC1101_STROBE_SFSTXON  0x31U
#define CC1101_STROBE_SRX  0x34U
#define CC1101_STROBE_STX  0x35U
#define CC1101_STROBE_SIDLE 0x36U
#define CC1101_STROBE_SPWD 0x39U
#define CC1101_STROBE_SFRX 0x3AU
#define CC1101_STROBE_SFTX 0x3BU
#define CC1101_STROBE_SNOP 0x3DU

#define CC1101_STATUS_RDY 0x80U
#define CC1101_STATUS_FIFO_MASK 0x0FU
#define CC1101_MODE_MASK 0x70U
#define CC1101_MODE_IDLE 0U
#define CC1101_MODE_RX   0x10U
#define CC1101_MODE_TX   0x20U
#define CC1101_MODE_FSTXON 0x30U
#define CC1101_MODE_CALIBRATE 0x40U
#define CC1101_MODE_SETTLING 0x50U
#define CC1101_MODE_RXFIFO_UNDERFLOW 0x60U
#define CC1101_MODE_TXFIFO_UNDERFLOW 0x70U

#define CC1101_FIFO 0x3FU

#define CC1101_PARTNUM 0xF0U
#define CC1101_VERSION 0xF1U
#define CC1101_FREQEST 0xF2U
#define CC1101_LQI 0xF3U
#define CC1101_RSSI 0xF4U
#define CC1101_TXBYTES 0xFAU
#define CC1101_RXBYTES 0xFBU
#define CC1101_MARCSTATE 0xF5U

unsigned int cc1101_Init(unsigned int device_num, const cc1101_cfg *cfg);
unsigned int cc1101_Check(unsigned int device_num);
unsigned int cc1101_RW(unsigned int device_num, unsigned char *txdata, unsigned char *rxdata, unsigned int size);
unsigned int cc1101_setTxPower(unsigned int device_num, unsigned char power);
unsigned int cc1101_strobe(unsigned int device_num, unsigned char data, unsigned char *status);
unsigned int cc1101_powerOn(unsigned int device_num);
short cc1101_calculateRSSI(unsigned char rssiIn);

#endif
