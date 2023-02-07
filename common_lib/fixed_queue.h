#ifndef MAIN_FIXED_QUEUE_H
#define MAIN_FIXED_QUEUE_H

typedef struct {
  int size, item_length, current, first;
  char* buffer;
} Queue;

void queue_init(Queue *q, int size, int item_length, char *buffer);
void *queue_peek(Queue* q);
void queue_pop(Queue* q);
void queue_push(Queue* q, void *data);
void queue_reset(Queue* q);
int queue_size(Queue* q);
void *queue_peekn(Queue* q, int offset);

#endif //MAIN_FIXED_QUEUE_H
