#include <eth.h>
#include <queue.h>
#include <memory.h>

Queue eth_irq_queue __attribute__((section(".ETHQueueSection")));
unsigned char eth_irq_buffer[ETH_IRQ_QUEUE_SIZE * ETH_MAX_PACKET_SIZE] __attribute__((section(".ETHQueueSection")));
unsigned int eth_irq_message_sizes[ETH_IRQ_QUEUE_SIZE] __attribute__((section(".ETHQueueSection")));

Queue eth_user_queue __attribute__((section(".ETHQueueSection")));
unsigned char eth_user_buffer[ETH_USER_QUEUE_SIZE * ETH_MAX_PACKET_SIZE] __attribute__((section(".ETHQueueSection")));
unsigned int eth_user_message_sizes[ETH_USER_QUEUE_SIZE] __attribute__((section(".ETHQueueSection")));

void ETH_IRQ_QueueInit(void)
{
  queue_init(&eth_irq_queue, ETH_IRQ_QUEUE_SIZE, ETH_MAX_PACKET_SIZE, eth_irq_buffer, eth_irq_message_sizes);
}

unsigned char *ETH_IRQ_QueuePoll(unsigned int *message_size)
{
  return queue_poll(&eth_irq_queue, message_size);
}

void ETH_USER_QueueInit(void)
{
  queue_init(&eth_user_queue, ETH_USER_QUEUE_SIZE, ETH_MAX_PACKET_SIZE, eth_user_buffer, eth_user_message_sizes);
}

unsigned char *ETH_USER_QueuePoll(unsigned int *message_size)
{
  return queue_poll(&eth_user_queue, message_size);
}

void ETH_QueueInit(void)
{
  ETH_IRQ_QueueInit();
  ETH_USER_QueueInit();
}

void ETH_ProcessQueue(void)
{
  unsigned char *buffer;
  unsigned int message_size;

  while (buffer = ETH_IRQ_QueuePoll(&message_size))
  {
    memcpy((void*)ETHInstance.TxDesc->Buffer1Addr, buffer, message_size);
    while (ETH_TransmitFrame(message_size))
      ;
  }

  while (buffer = ETH_USER_QueuePoll(&message_size))
  {
    memcpy((void*)ETHInstance.TxDesc->Buffer1Addr, buffer, message_size);
    while (ETH_TransmitFrame(message_size))
      ;
  }
}
