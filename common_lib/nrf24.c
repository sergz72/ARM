#include <nrf24.h>
#include <board.h>
#include <memory.h>

unsigned char nrf24_configByte[NRF24_MAX_DEVICES];
nrf24_dataRate nrf24_dataRateByte[NRF24_MAX_DEVICES];
unsigned char txData[33];
unsigned char rxData[33];

unsigned int nrf24_readRegister(unsigned int device_num, unsigned char registerNo, unsigned char *buffer, unsigned int dataBytes)
{
  txData[0] = registerNo;
  return nrf24_RW(device_num, txData, buffer, dataBytes+1);
}

unsigned int nrf24_command(unsigned int device_num, unsigned char registerNo, unsigned char *status)
{
  return nrf24_RW(device_num, &registerNo, status, 1);
}

static unsigned int nrf24_initConfigRegister(unsigned int device_num, unsigned int twoCRCBytes)
{
  unsigned int rc;
  unsigned char localTxData[2];
  unsigned char config;

  config = nrf24_configByte[device_num] = 0x38U;

  if (twoCRCBytes)
    config |= 4U;

  localTxData[0] = nRF24_REG_CONFIG | nRF24_CMD_W_REGISTER;
  localTxData[1] = config;
  rc = nrf24_RW(device_num, localTxData, rxData, 2);
  if (rc)
    nrf24_configByte[device_num] = config;
  return rc;
}

static unsigned int nrf24_setAddressWidth(unsigned int device_num, unsigned char width)
{
  unsigned char localTxData[2];
  if (width < 3 || width > 5)
    return 0;
  localTxData[0] = nRF24_REG_SETUP_AW | nRF24_CMD_W_REGISTER;
  localTxData[1] = width - 2;
  return nrf24_RW(device_num, localTxData, rxData, 2);
}

unsigned int nrf24_Init(unsigned int device_num, const nrf24_config *config)
{
  unsigned int rc;

  memset(txData, 0xFF, sizeof(txData));

  rc = nrf24_initConfigRegister(device_num, config->twoCRCBytes);
  if (!rc)
    return 0;
  // no AutoAck
  rc = nrf24_setAA(device_num, 0);
  if (!rc)
    return 0;
  //only pipe 1
  rc = nrf24_rXAddrControl(device_num, 2);
  if (!rc)
    return 0;
  rc = nrf24_setAddressWidth(device_num, config->addressWidth);
  if (!rc)
    return 0;
  // no retransmit
  rc = nrf24_setRetransmit(device_num, 0);
  if (!rc)
    return 0;
  rc = nrf24_setChannel(device_num, config->channel);
  if (!rc)
    return 0;
  rc = nrf24_setDataRateAndTXPower(device_num, config->dataRate, config->txPower);
  if (!rc)
    return 0;
  rc = nrf24_clearIRQFlags(device_num);
  if (!rc)
    return 0;
  rc = nrf24_setRXAddress(device_num, nRF24_PIPE1, config->rxAddress, config->addressWidth);
  if (!rc)
    return 0;
  rc = nrf24_setTXAddress(device_num, config->txAddress, config->addressWidth);
  if (!rc)
    return 0;
  rc = nrf24_setRXPayloadLength(device_num, nRF24_PIPE0, 0);
  if (!rc)
    return 0;
  rc = nrf24_setRXPayloadLength(device_num, nRF24_PIPE1, config->rxPacketSize);
  if (!rc)
    return 0;
  rc = nrf24_setRXPayloadLength(device_num, nRF24_PIPE2, 0);
  if (!rc)
    return 0;
  rc = nrf24_setRXPayloadLength(device_num, nRF24_PIPE3, 0);
  if (!rc)
    return 0;
  rc = nrf24_setRXPayloadLength(device_num, nRF24_PIPE4, 0);
  if (!rc)
    return 0;
  rc = nrf24_setRXPayloadLength(device_num, nRF24_PIPE5, 0);
  if (!rc)
    return 0;
  rc = nrf24_setDynPD(device_num, 0);
  if (!rc)
    return 0;
  rc = nrf24_setFeature(device_num, 0);
  if (!rc)
    return 0;
  rc = nrf24_flushTX(device_num);
  if (!rc)
    return 0;
  return nrf24_flushRX(device_num);
}

unsigned int nrf24_powerControl(unsigned int device_num, int powerUp)
{
  unsigned char localTxData[2];
  unsigned char config = nrf24_configByte[device_num];
  unsigned int rc;

  if (powerUp)
    config |= 2U;
  else
    config &= 0xFDU;

  localTxData[0] = nRF24_REG_CONFIG | nRF24_CMD_W_REGISTER;
  localTxData[1] = config;
  rc = nrf24_RW(device_num, localTxData, rxData, 2);
  if (rc)
  {
    nrf24_configByte[device_num] = config;
    if (powerUp)
      delayms(2);
    else
      nRF24_CE_CLR(device_num);
  }
  return rc;
}

unsigned int nrf24_setAA(unsigned int device_num, unsigned char enAA)
{
  unsigned char localTxData[2];
  localTxData[0] = nRF24_REG_EN_AA | nRF24_CMD_W_REGISTER;
  localTxData[1] = enAA & 0x3FU;
  return nrf24_RW(device_num, localTxData, rxData, 2);
}

unsigned int nrf24_RX(unsigned int device_num)
{
  unsigned char localTxData[2];
  unsigned char config = nrf24_configByte[device_num];
  unsigned int rc;

  config |= 1U;

  localTxData[0] = nRF24_REG_CONFIG | nRF24_CMD_W_REGISTER;
  localTxData[1] = config;
  rc = nrf24_RW(device_num, localTxData, rxData, 2);
  if (rc)
  {
    nrf24_configByte[device_num] = config;
    nRF24_CE_SET(device_num);
  }
  return rc;
}

unsigned int nrf24_TX(unsigned int device_num)
{
  unsigned char localTxData[2];
  unsigned char config = nrf24_configByte[device_num];
  unsigned int rc;

  config &= 0xFEU;

  localTxData[0] = nRF24_REG_CONFIG | nRF24_CMD_W_REGISTER;
  localTxData[1] = config;
  rc = nrf24_RW(device_num, localTxData, rxData, 2);
  if (rc)
    nrf24_configByte[device_num] = config;
  return rc;
}

unsigned int nrf24_rXAddrControl(unsigned int device_num, unsigned char enRXAddr)
{
  unsigned char localTxData[2];
  localTxData[0] = nRF24_REG_EN_RXADDR | nRF24_CMD_W_REGISTER;
  localTxData[1] = enRXAddr & 0x3FU;
  return nrf24_RW(device_num, localTxData, rxData, 2);
}

unsigned int nrf24_setRetransmit(unsigned int device_num, unsigned char retransmit)
{
  unsigned char localTxData[2];
  localTxData[0] = nRF24_REG_SETUP_RETR | nRF24_CMD_W_REGISTER;
  localTxData[1] = retransmit;
  return nrf24_RW(device_num, localTxData, rxData, 2);
}

unsigned int nrf24_setChannel(unsigned int device_num, unsigned char channel)
{
  unsigned char localTxData[2];

  if (channel > 125)
    return 0;

  localTxData[0] = nRF24_REG_RF_CH | nRF24_CMD_W_REGISTER;
  localTxData[1] = channel;
  return nrf24_RW(device_num, localTxData, rxData, 2);
}

unsigned int nrf24_setDataRateAndTXPower(unsigned int device_num, nrf24_dataRate dataRate, nrf24_txPower txPower)
{
  unsigned char localTxData[2];

  nrf24_dataRateByte[device_num] = dataRate;
  localTxData[0] = nRF24_REG_RF_SETUP | nRF24_CMD_W_REGISTER;
  localTxData[1] = dataRate | txPower;
  return nrf24_RW(device_num, localTxData, rxData, 2);
}

unsigned int nrf24_setTXPower(unsigned int device_num, nrf24_txPower txPower)
{
  unsigned char localTxData[2];
  localTxData[0] = nRF24_REG_RF_SETUP | nRF24_CMD_W_REGISTER;
  localTxData[1] = nrf24_dataRateByte[device_num] | txPower;
  return nrf24_RW(device_num, localTxData, rxData, 2);
}

unsigned int nrf24_clearIRQFlags(unsigned int device_num)
{
  unsigned char localTxData[2];
  localTxData[0] = nRF24_REG_STATUS | nRF24_CMD_W_REGISTER;
  localTxData[1] = 0x70U;
  return nrf24_RW(device_num, localTxData, rxData, 2);
}

unsigned int nrf24_setRXAddress(unsigned int device_num, nrf24rxPipe pipe, const unsigned char *address, unsigned int addressWidth)
{
  unsigned int i;
  unsigned char localTxData[6];

  if ((pipe <= nRF24_PIPE1 && (addressWidth < 3 || addressWidth > 5)) || (pipe > nRF24_PIPE1 && addressWidth > 1))
    return 0;
  localTxData[0] = (nRF24_REG_RX_ADDR_P0 + pipe) | nRF24_CMD_W_REGISTER;
  for (i = 1; i <= addressWidth; i++)
    localTxData[i] = *address++;
  return nrf24_RW(device_num, localTxData, rxData, addressWidth + 1);
}

unsigned int nrf24_setRXPayloadLength(unsigned int device_num, nrf24rxPipe pipe, unsigned char length)
{
  unsigned char localTxData[2];

  if (length > 32)
    return 0;

  localTxData[0] = (nRF24_REG_RX_PW_P0 + pipe) | nRF24_CMD_W_REGISTER;
  localTxData[1] = length;
  return nrf24_RW(device_num, localTxData, rxData, 2);
}

unsigned int nrf24_setTXAddress(unsigned int device_num, const unsigned char *address, unsigned int addressWidth)
{
  unsigned int i;
  unsigned char localTxData[6];

  if (addressWidth < 3 || addressWidth > 5)
    return 0;
  localTxData[0] = nRF24_REG_TX_ADDR | nRF24_CMD_W_REGISTER;
  for (i = 1; i <= addressWidth; i++)
    localTxData[i] = *address++;
  return nrf24_RW(device_num, localTxData, rxData, addressWidth + 1);
}

unsigned int nrf24_setDynPD(unsigned int device_num, unsigned char value)
{
  unsigned char localTxData[2];
  localTxData[0] = nRF24_REG_DYNPD | nRF24_CMD_W_REGISTER;
  localTxData[1] = value & 0x3FU;
  return nrf24_RW(device_num, localTxData, rxData, 2);
}

unsigned int nrf24_setFeature(unsigned int device_num, unsigned char value)
{
  unsigned char localTxData[2];
  localTxData[0] = nRF24_REG_FEATURE | nRF24_CMD_W_REGISTER;
  localTxData[1] = value;
  return nrf24_RW(device_num, localTxData, rxData, 2);
}

unsigned int nrf24_flushTX(unsigned int device_num)
{
  unsigned char status;
  return nrf24_command(device_num, nRF24_CMD_FLUSH_TX, &status);
}

unsigned int nrf24_flushRX(unsigned int device_num)
{
  unsigned char status;
  return nrf24_command(device_num, nRF24_CMD_FLUSH_RX, &status);
}

unsigned int nrf24_NOP(unsigned int device_num, unsigned char *status)
{
  return nrf24_command(device_num, nRF24_CMD_NOP, status);
}

unsigned int nrf24_getStatus(unsigned int device_num, unsigned char *status)
{
  unsigned int rc = nrf24_readRegister(device_num, nRF24_REG_STATUS, rxData, 1);
  if (!rc)
    return 0;
  *status = rxData[1];
  return rc;
}

int nrf24_readRXFifo(unsigned int device_num, unsigned char *buffer)
{
  unsigned char status;
  unsigned int rc = nrf24_getStatus(device_num, &status);
  if (!rc)
    return 0;

  status = (status & 0x0EU) >> 1U;
  if (status > 5)
    return -1; // no data
  rc = nrf24_readRegister(device_num, nRF24_REG_RX_PW_P0 + status, rxData, 1);
  if (!rc)
    return 0;
  status = rxData[1];
  if (!status)
    return -2; //zero length response
  rc = nrf24_readRegister(device_num, nRF24_CMD_R_RX_PAYLOAD, buffer, status);
  if (!rc)
    return 0;
  return status; // number of received bytes
}

static unsigned int nrf24_writeTXFifo(unsigned int device_num, unsigned char *buffer, unsigned int length)
{
  unsigned int i;
  if (length > 32)
    return 0;
  buffer[0] = nRF24_CMD_W_TX_PAYLOAD;
  return nrf24_RW(device_num, buffer, rxData, length + 1);
}

static void nrf24_transmitCleanup(unsigned int device_num)
{
  nRF24_CE_CLR(device_num);
  nrf24_clearIRQFlags(device_num);
  nrf24_RX(device_num);
}

int nrf24_transmit(unsigned int device_num, unsigned char *buffer, unsigned int length)
{
  unsigned int rc;
  unsigned int counter;
  unsigned char status;

  nRF24_CE_CLR(device_num);
  rc = nrf24_TX(device_num);
  if (!rc)
    return 0;
  rc = nrf24_writeTXFifo(device_num, buffer, length);
  if (!rc)
    return 0;
  nRF24_CE_SET(device_num);
  counter = 200U; // 20 ms timeout
  while (--counter)
  {
    delay(100);
    rc = nrf24_getStatus(device_num, &status);
    if (!rc)
    {
      nrf24_transmitCleanup(device_num);
      nrf24_flushTX(device_num);
      return 0;
    }
    if (status & 0x10U) //MAX_RT
    {
      nrf24_transmitCleanup(device_num);
      nrf24_flushTX(device_num);
      return -1;
    }
    if (status & 0x20U) //TX_DS
      break;
  }
  nrf24_transmitCleanup(device_num);
  if (!counter) { // timeout
    nrf24_flushTX(device_num);
    return -2;
  }

  return 1;
}

// Check if the nRF24L01 present
// return:
//   1 - nRF24L01 is online and responding
//   0 - received sequence differs from original
unsigned int nrf24_Check(unsigned int device_num)
{
  unsigned int i;
  unsigned char *ptr = nRF24_TEST_ADDR;
  unsigned int rc = nrf24_setTXAddress(device_num, ptr, 5);

  if (!rc)
    return 0;

  rc = nrf24_readRegister(device_num, nRF24_REG_TX_ADDR, rxData, 5);
  if (!rc)
    return 0;

  // Compare buffers, return error on first mismatch
  for (i = 1; i <= 5; i++)
  {
    if (rxData[i] != *ptr++)
      return 0;
  }

  return 1;
}
