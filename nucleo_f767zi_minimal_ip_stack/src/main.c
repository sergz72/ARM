#include "board.h"
#include <uart.h>
#include <eth.h>
#include <eth_ipv4.h>
#include <eth_udp.h>

extern volatile unsigned int led_counter;

const UART_InitTypeDef UARTCfg = {
  115200,
  UART_WORDLENGTH_8B,
  UART_STOPBITS_1,
  UART_PARITY_NONE,
  UART_MODE_TX_RX,
  UART_HWCONTROL_NONE,
  UART_OVERSAMPLING_16,
  UART_ONE_BIT_SAMPLE_DISABLE
};

ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __attribute__((section(".DMARxSection")));
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __attribute__((section(".DMATxSection")));
unsigned char Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __attribute__((section(".ETHRxSection")));
unsigned char Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __attribute__((section(".ETHRxSection")));

const unsigned char macaddress[6]= { MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5 };

int main(void)
{
  led_counter = 0;

  UART_Init(USART3, &UARTCfg);

  ETH_QueueInit();
  ETHInstance.Instance = ETH;
  ETHInstance.Init.MACAddr = (unsigned char*)macaddress;
  ETHInstance.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
  ETHInstance.Init.Speed = ETH_SPEED_100M;
  ETHInstance.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
  ETHInstance.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
  ETHInstance.Init.RxMode = ETH_RXINTERRUPT_MODE;
  ETHInstance.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
  ETHInstance.Init.PhyAddress = LAN8742A_PHY_ADDRESS;
  ETHInstance.Init.ip_addr = IP4_ADDR(0, 0, 0, 0); //DHCP
//  ETHInstance.Init.ip_addr = IP4_ADDR(192, 168, 1, 101);
  do
  {
    while (ETH_Init())
      delayms(100);

    /* Initialize Tx Descriptors list: Chain Mode */
    ETH_DMATxDescListInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);

    /* Initialize Rx Descriptors list: Chain Mode  */
    ETH_DMARxDescListInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

    ETH_Start();
    delayms(5000);
  } while (led_counter == 0);

  ETH_DHCP_Send("nucleo_f767zi");

  while (1)
	{
    ETH_ProcessQueue();
	}
}
