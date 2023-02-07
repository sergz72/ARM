#include <string.h>
#include <fixed_queue.h>

void queue_init(Queue* q, int size, int item_length, char* buffer)
{
  q->size = size;
  q->item_length = item_length;
  q->buffer = buffer;
  q->current = q->first = 0;
}

void queue_reset(Queue* q)
{
  q->current = q->first = 0;
}

void *queue_peek(Queue* q)
{
  if (q->current != q->first)
    return q->buffer + q->first * q->item_length;
  return NULL;
}

void queue_pop(Queue* q)
{
  if (q->first != q->current)
  {
    q->first++;
    if (q->first >= q->size)
      q->first = 0;
  }
}

void queue_push(Queue* q, void *data)
{
  memcpy(q->buffer + q->item_length * q->current++, data, q->item_length);
  if (q->current >= q->size)
    q->current = 0;
  if (q->current == q->first)
  {
    q->first++;
    if (q->first >= q->size)
      q->first = 0;
  }
}

int queue_size(Queue* q)
{
  if (q->current >= q->first)
    return q->current - q->first;
  return q->size - q->first + q->current;
}

void *queue_peekn(Queue* q, int offset)
{
  if (offset < 0 || offset > queue_size(q) - 1)
    return NULL;
  offset = q->current - offset - 1;
  if (offset < 0)
    offset += q->size;
  return q->buffer + offset * q->item_length;
}
