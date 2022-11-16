#include <queue.h>

#define NULL 0

void queue_init(Queue *q, unsigned int size, unsigned int buffer_size, unsigned char *buffer, unsigned int *message_size_array)
{
  q->size = size;
  q->buffer_size = buffer_size;
  q->buffer = buffer;
  q->message_size_array = message_size_array;
  q->rx_head = q->tx_head = 0;
}

unsigned char *queue_poll(Queue *q, unsigned int *message_size)
{
  unsigned char *result;
  unsigned int newHead;

  if (q->rx_head == q->tx_head)
    return NULL;

  newHead = q->rx_head;
  result = &q->buffer[newHead * q->buffer_size];
  *message_size = q->message_size_array[newHead];
  newHead++;
  if (newHead >= q->size)
    newHead = 0;
  q->rx_head = newHead;

  return result;
}

unsigned char *queue_peekTx(Queue *q)
{
  return &q->buffer[q->tx_head * q->buffer_size];
}

void queue_add(Queue *q, unsigned int message_size)
{
  unsigned int newHead = q->tx_head;
  q->message_size_array[newHead] = message_size;
  newHead++;
  if (newHead >= q->size)
    newHead = 0;
  q->tx_head = newHead;
}
