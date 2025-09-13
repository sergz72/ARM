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

void queue_move_to_top(Queue* q, int offset, void *buffer)
{
  if (offset <= 0 || offset > queue_size(q) - 1)
    return;
  int top = q->current - 1;
  if (top < 0)
    top += q->size;
  int idx = top - offset;
  if (idx < 0)
    idx += q->size;
  memcpy(buffer, q->buffer + idx * q->item_length, q->item_length);
  while (idx != top)
  {
    int next_idx = idx + 1;
    if (next_idx >= q->size)
      next_idx = 0;
    memcpy(q->buffer + idx * q->item_length, q->buffer + next_idx * q->item_length, q->item_length);
    idx = next_idx;
  }
  memcpy(q->buffer + top * q->item_length, buffer, q->item_length);
}

int queue_get_index(Queue* q, void *data, int (*cmp)(void*, void*))
{
  int size = queue_size(q);
  int idx = q->current - 1;
  if (idx < 0)
    idx += q->size;
  char *p = q->buffer + q->item_length * idx;
  idx = 0;
  for (int i = 0; i < size; i++)
  {
    if (!cmp(p, data))
      return idx;
    p -= q->item_length;
    idx++;
    if (p < q->buffer)
      p += q->size * q->item_length;
  }
  return -1;
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
