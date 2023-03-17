#include <swim.h>
#include "board.h"
//#include <stdio.h>

#define SWIM_ENTER_DELAY1_US 20
#define SWIM_ENTER_DELAY2_US 500
#define SWIM_ENTER_DELAY3_US 250
#define SWIM_ENTER_PULSES    4

#define SWIM_COMMAND_SRST  0
#define SWIM_COMMAND_ROTF  1
#define SWIM_COMMAND_WOTF  2

#define SWIM_PACKET_ITEMS 2

#define SWIM_TICK_MHZ 8

#define SWIM_ACTION_SET   0
#define SWIM_ACTION_CLR   4
#define SWIM_ACTION_GET   8
#define SWIM_ACTION_WAIT  12
#define SWIM_ACTION_WAIT0 16

unsigned int *swim_packet_end, swim_packet[SWIM_MAX_PACKET * SWIM_PACKET_ITEMS];

static unsigned int *swim_ack[SWIM_MAX_PACKET], parity, clock128, encode_counter_v, base_clock;
static unsigned int short_delay, long_delay, half_delay, full_delay;

/*void swim_packet_handler(unsigned int clr)
{
  unsigned int *p = swim_packet, next_counter, op;
  //printf("clr = %u\n", clr);
  do
  {
    next_counter = *p++;
    op = *p++;
    //printf("next_counter %u, setv %X clrv %X\n", SWIM_TIME_COUNTER_MAX - next_counter, setv, clrv);
  } while (p < swim_packet_end);
}*/

static void set_low_speed(void)
{
  short_delay = 4 * base_clock / SWIM_TICK_MHZ;
  long_delay = 18 * base_clock / SWIM_TICK_MHZ;
  half_delay = 11 * base_clock / SWIM_TICK_MHZ;
  full_delay = 22 * base_clock / SWIM_TICK_MHZ;
}

static void set_high_speed(void)
{
  short_delay = 2 * base_clock / SWIM_TICK_MHZ;
  long_delay = 8 * base_clock / SWIM_TICK_MHZ;
  half_delay = 5 * base_clock / SWIM_TICK_MHZ;
  full_delay = 10 * base_clock / SWIM_TICK_MHZ;
}

static unsigned int *swim_encode(unsigned int delay, unsigned int action)
{
  encode_counter_v -= delay;
  *swim_packet_end++ = encode_counter_v;
  *swim_packet_end = action;
  return swim_packet_end++;
}

static void swim_pulses(unsigned int delay_us)
{
  int i;
  delay_us *= SWIM_COUNTER_MHZ;
  for (i = 0; i < SWIM_ENTER_PULSES; i++)
  {
    swim_encode(delay_us, SWIM_ACTION_CLR);
    swim_encode(delay_us, SWIM_ACTION_SET);
  }
}

void swim_init(unsigned int bclock)
{
  clock128 = 0;
  base_clock = bclock;
  set_low_speed();
}

unsigned int swim_get_base_clock(void)
{
  return base_clock;
}

static void swim_encode_bit(unsigned int bit, int last_bit)
{
  parity = parity ^ (bit ? 1 : 0);
  if (bit)
  {
    swim_encode(short_delay, SWIM_ACTION_SET);
    swim_encode(long_delay, last_bit ? SWIM_ACTION_WAIT : SWIM_ACTION_CLR);
  }
  else
  {
    swim_encode(long_delay, SWIM_ACTION_SET);
    swim_encode(short_delay, last_bit ? SWIM_ACTION_WAIT : SWIM_ACTION_CLR);
  }
}

static unsigned int *swim_encode_read(int clr)
{
  unsigned int *p = swim_encode(half_delay, SWIM_ACTION_GET);
  swim_encode(half_delay, clr ? SWIM_ACTION_CLR : SWIM_ACTION_WAIT);
  return p;
}

static void swim_encode_start(void)
{
  encode_counter_v = SWIM_TIME_COUNTER_MAX;
  swim_packet_end = swim_packet;
}

static unsigned int *swim_encode_command(unsigned int cmd, int clr)
{
  unsigned int *rc;

  swim_encode_bit(0, 0);
  parity = 0;
  swim_encode_bit(cmd & 4, 0);
  swim_encode_bit(cmd & 2, 0);
  swim_encode_bit(cmd & 1, 0);
  swim_encode_bit(parity, 1);
  rc = swim_encode_read(0);
  swim_encode(full_delay, clr ? SWIM_ACTION_CLR : SWIM_ACTION_WAIT);
  return rc;
}

static unsigned int *swim_encode_data_write(unsigned char data, int clr, int wait)
{
  unsigned int *rc;
  swim_encode_bit(0, 0);
  parity = 0;
  swim_encode_bit(data & 0x80, 0);
  swim_encode_bit(data & 0x40, 0);
  swim_encode_bit(data & 0x20, 0);
  swim_encode_bit(data & 0x10, 0);
  swim_encode_bit(data & 8, 0);
  swim_encode_bit(data & 4, 0);
  swim_encode_bit(data & 2, 0);
  swim_encode_bit(data & 1, 0);
  swim_encode_bit(parity, 1);
  if (wait)
  {
    rc = swim_encode_read(0);
    swim_encode(full_delay, clr ? SWIM_ACTION_CLR : SWIM_ACTION_WAIT);
  }
  else
    rc = swim_encode_read(clr);
  return rc;
}

static unsigned int *swim_encode_data_read(void)
{
  unsigned int *p = swim_encode(0, SWIM_ACTION_WAIT0);
  encode_counter_v = SWIM_TIME_COUNTER_MAX;
  swim_encode_read(0);
  swim_encode_read(0);
  swim_encode_read(0);
  swim_encode_read(0);
  swim_encode_read(0);
  swim_encode_read(0);
  swim_encode_read(0);
  swim_encode_read(0);
  swim_encode_read(0);
  swim_encode_read(1);
  //ack
  swim_encode_bit(1, 1);
  return p;
}

unsigned int swim_srst(void)
{
  unsigned int *p;
  swim_encode_start();
  p = swim_encode_command(SWIM_COMMAND_SRST, 0);
  swim_packet_handler(SWIM_SET_VALUE);
  return SWIM_GET_BIT(*p);
}

void swim_reset(void)
{
  swim_encode_start();
  swim_encode(SWIM_ENTER_DELAY1_US * SWIM_COUNTER_MHZ, SWIM_ACTION_SET);
  swim_packet_handler(SWIM_SET_VALUE);
}

unsigned int swim_enter(unsigned int *clock)
{
  unsigned char v;
  unsigned int port_v, port_vv, change_cnt, change_counter[2];

  if (!clock128)
  {
    swim_encode_start();
    swim_encode(SWIM_ENTER_DELAY1_US * SWIM_COUNTER_MHZ, SWIM_ACTION_SET);
    swim_pulses(SWIM_ENTER_DELAY2_US);
    swim_pulses(SWIM_ENTER_DELAY3_US);
    swim_packet_handler(SWIM_SET_VALUE);
    swim_enter_critical_section();
    SWIM_TIME_COUNTER_START;
    port_v = 1<<SWIM_IN_PIN;
    change_cnt = 0;
    while (!SWIM_TIME_COUNTER_OVERLOAD)
    {
      port_vv = SWIM_GET_BIT(SWIM_GET);
      if (port_vv != port_v)
      {
        change_counter[change_cnt++] = SWIM_TIME_COUNTER;
        if (change_cnt == 2)
          break;
        port_v = port_vv;
      }
    }
    SWIM_TIME_COUNTER_STOP;
    swim_leave_critical_section();
    if (change_cnt == 2)
    {
      clock128 = change_counter[0] - change_counter[1];
      if (clock)
        *clock = clock128;
      set_low_speed();
      v = SWIM_CSR_SAFE_MASK | SWIM_CSR_DM;
      SWIM_SLEEP(1);
      return swim_write(SWIM_CSR, 1, &v);
    }
    return SWIM_ENTER_NO_SYNC_FRAME;
  }
  return SWIM_OK;
}

unsigned int swim_write(unsigned int address, unsigned char length, unsigned char *values)
{
  unsigned int i, **ack_p = swim_ack, **p = swim_ack, *ack;
  swim_encode_start();
  *ack_p++ = swim_encode_command(SWIM_COMMAND_WOTF, 1);
  *ack_p++ = swim_encode_data_write(length, 1, 1);
  *ack_p++ = swim_encode_data_write(address >> 16, 1, 1);
  *ack_p++ = swim_encode_data_write((address >> 8) & 0xFF, 1, 1);
  *ack_p++ = swim_encode_data_write(address & 0xFF, 1, 1);
  while (length--)
    *ack_p++ = swim_encode_data_write(*values++, length, length);
  swim_packet_handler(SWIM_SET_VALUE);
  i = SWIM_COMMAND_NO_ACK;
  while (p < ack_p)
  {
    ack = *p++;
    if (!SWIM_GET_BIT(*ack))
      return i;
    i++;
  }
  return SWIM_OK;
}

static int swim_check_parity(unsigned int parity_bit, unsigned char v)
{
  int parity_v = 0, vv;
  for (vv = 0x80; vv != 0; vv>>=1)
    parity_v = parity_v ^ (v & vv ? 1 : 0);
  return (parity_bit ? 1 : 0) != parity_v;
}

static unsigned int swim_byte_read(unsigned char *value, unsigned int **p)
{
  unsigned int *data_p = *p;
  unsigned char v = 0;
  //timeout flag
  if (!*data_p)
    return 1000;
  //stm8 always sets first bit to 1
  data_p += SWIM_PACKET_ITEMS;
  if (!SWIM_GET_BIT(*data_p))
    return 2000;
  data_p += SWIM_PACKET_ITEMS * 2;
  if (SWIM_GET_BIT(*data_p))
    v |= 0x80;
  data_p += SWIM_PACKET_ITEMS * 2;
  if (SWIM_GET_BIT(*data_p))
    v |= 0x40;
  data_p += SWIM_PACKET_ITEMS * 2;
  if (SWIM_GET_BIT(*data_p))
    v |= 0x20;
  data_p += SWIM_PACKET_ITEMS * 2;
  if (SWIM_GET_BIT(*data_p))
    v |= 0x10;
  data_p += SWIM_PACKET_ITEMS * 2;
  if (SWIM_GET_BIT(*data_p))
    v |= 8;
  data_p += SWIM_PACKET_ITEMS * 2;
  if (SWIM_GET_BIT(*data_p))
    v |= 4;
  data_p += SWIM_PACKET_ITEMS * 2;
  if (SWIM_GET_BIT(*data_p))
    v |= 2;
  data_p += SWIM_PACKET_ITEMS * 2;
  if (SWIM_GET_BIT(*data_p))
    v |= 1;
  data_p += SWIM_PACKET_ITEMS * 2;
  if (swim_check_parity(SWIM_GET_BIT(*data_p), v))
    return 3000;
  *value = v;
  //skipping sent acknowledge
  *p = data_p + SWIM_PACKET_ITEMS * 4;
  return 0;
}

unsigned int swim_read(unsigned int address, unsigned char length, unsigned char *values)
{
  unsigned int i, *data_p, **ack_p = swim_ack, **p = swim_ack, *ack, l = length, rc;
  swim_encode_start();
  *ack_p++ = swim_encode_command(SWIM_COMMAND_ROTF, 1);
  *ack_p++ = swim_encode_data_write(length, 1, 1);
  *ack_p++ = swim_encode_data_write(address >> 16, 1, 1);
  *ack_p++ = swim_encode_data_write((address >> 8) & 0xFF, 1, 1);
  *ack_p++ = swim_encode_data_write(address & 0xFF, 0, 0);
  l--;
  data_p = swim_encode_data_read();
  while (l--)
    swim_encode_data_read();
  swim_packet_handler(SWIM_SET_VALUE);
  i = SWIM_COMMAND_NO_ACK;
  while (p < ack_p)
  {
    ack = *p++;
    if (!SWIM_GET_BIT(*ack))
      return i;
    i++;
  }
  while (length--)
  {
    rc = swim_byte_read(values++, &data_p);
    if (rc)
      return rc + i;
    i++;
  }
  return SWIM_OK;
}

