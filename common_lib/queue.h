#ifndef _QUEUE_H
#define _QUEUE_H

typedef struct
{
  unsigned int size;
  unsigned int buffer_size;
  unsigned int rx_head;
  unsigned int tx_head;
  unsigned char *buffer;
  unsigned int *message_size_array;
} Queue;

void queue_init(Queue *q, unsigned int size, unsigned int buffer_size, unsigned char *buffer, unsigned int *message_size_array);
unsigned char *queue_poll(Queue *q, unsigned int *message_size);
unsigned char *queue_peekTx(Queue *q);
void queue_add(Queue *q, unsigned int message_size);

#endif
