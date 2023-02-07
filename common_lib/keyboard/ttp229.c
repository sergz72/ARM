#include "board.h"
#include <ttp229.h>
#include <fixed_queue.h>

#define CLOCK_DELAY (500 / TTP229_SPEED)

static Queue q;
static unsigned char buffer[TTP229_QUEUE_SIZE];

void ttp229_interrupt(void)
{
  unsigned char c = 0;
  int i;

  wait_us(10);
  for (i = 1; i <= TTP229_KEYS; i++)
  {
    TTP_229_SCL_LOW;
    wait_us(CLOCK_DELAY);
    TTP_229_SCL_HIGH;
    if (!TTP_229_SDO)
      c = (unsigned char) i;
    wait_us(CLOCK_DELAY);
  }
  if (c)
  {
    ttp229_semaphore_acquire();
    queue_push(&q, &c);
    ttp229_semaphore_release();
  }
}

void ttp229_init(void)
{
  queue_init(&q, TTP229_QUEUE_SIZE, 1, (char*)buffer);
  ttp229_gpio_init();
  ttp229_semaphore_init();
}

int ttp229_get(void)
{
  ttp229_semaphore_acquire();
  unsigned char *v = (unsigned char *)queue_peek(&q);
  if (v)
  {
    queue_pop(&q);
    ttp229_semaphore_release();
    return *v;
  }
  ttp229_semaphore_release();
  return 0;
}