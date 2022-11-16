#include <board.h>
#include <eth.h>
#include <eth_udp.h>

volatile unsigned int led_counter;

void ETH_UDP_Handler(unsigned int sourceIP, ETH_UDP_Header *udp_hdr)
{
}

void ETH_UserRxCpltCallback(void)
{
  led_counter++;
  if (led_counter & 1)
    LD1_ON;
  else
    LD1_OFF;
  if (led_counter & 2)
    LD2_ON;
  else
    LD2_OFF;
  if (led_counter & 4)
    LD3_ON;
  else
    LD3_OFF;
}
