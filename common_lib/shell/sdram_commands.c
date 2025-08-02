#include "board.h"
#include <sdram_commands.h>
#include <shell.h>
#include <stdlib.h>
#include <string.h>
#include <chacha.h>
#include <stdio.h>

static int sdram_fill_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem sdram_fill_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, sdram_fill_handler}
};
static const ShellCommand sdram_fill_command = {
  sdram_fill_command_items,
  "sdram_fill",
  "sdram_fill data_width offset size"
};

static int sdram_check_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem sdram_check_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, sdram_check_handler}
};
static const ShellCommand sdram_check_command = {
  sdram_check_command_items,
  "sdram_check",
  "sdram_check data_width offset size"
};

static int sdram_fill_check_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem sdram_fill_check_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, sdram_fill_check_handler}
};
static const ShellCommand sdram_fill_check_command = {
  sdram_fill_check_command_items,
  "sdram_fill_check",
  "sdram_fill_check data_width offset size"
};

static unsigned char *sdram_address;
static unsigned int sdram_size;
static ChaCha rng;
static unsigned int tm;

static int parse_size(char *str, unsigned int *size)
{
  char *endptr;
  unsigned long value = strtoul(str, &endptr, 10);
  switch (*endptr)
  {
    case 0: break;
    case 'k': value *= 1024; break;
    case 'm': value *= 1024 * 1024; break;
    default: return 1;
  }
  *size = (unsigned int)value;
  return 0;
}

static int parse_params(char *str_dw, char *str_offset, char *str_size, unsigned int *dw, unsigned char **p, unsigned int *size)
{
  unsigned int w = atoi(str_dw);
  if (w != 8 && w != 16 && w != 32) return 10;
  *dw = w;
  unsigned int o, s;
  int rc = parse_size(str_offset, &o);
  if (rc) return rc;
  if (o >= sdram_size) return 20;
  rc = parse_size(str_size, &s);
  if (rc) return rc;
  if (o + s > sdram_size) return 30;
  *p = sdram_address + o;
  *size = s;
  return 0;
}

static void chacha_init(void)
{
  uint32_t key[8];
  uint32_t iv[3];

  key[0] = tm;
  key[1] = tm;
  key[2] = tm;
  key[3] = tm;
  key[4] = tm;
  key[5] = tm;
  key[6] = tm;
  key[7] = tm;
  iv[0] = tm;
  iv[1] = tm;
  iv[2] = tm;
  chacha20_init(&rng, key, iv);
}

static void sdram_fill(printf_func pfunc, unsigned int dw, unsigned char *p, unsigned int size)
{
  static unsigned char buffer[16];
  tm = get_time();
  chacha_init();
  chacha_fill_u8(&rng, buffer, sizeof(buffer));
  pfunc("%02x %02x %02x %02x %02x %02x %02x %02x\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
  pfunc("%02x %02x %02x %02x %02x %02x %02x %02x\n", buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]);
  chacha_init();
  switch (dw)
  {
    case 8:
      chacha_fill_u8(&rng, p, size);
      break;
    case 16:
      chacha_fill_u16(&rng, (unsigned short*)p, size / 2);
      break;
    default:
      chacha_fill_u32(&rng, (uint32_t*)p, size / 4);
      break;
  }
}

static int sdram_fill_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned int dw;
  unsigned int size;
  unsigned char *p;

  int rc = parse_params(argv[0], argv[1], argv[2], &dw, &p, &size);
  if (rc) return rc;
  sdram_fill(pfunc, dw, p, size);
  return 0;
}

static int check_failed(printf_func pfunc, void *p)
{
  pfunc("failure at address %p\n", p);
  return 100;
}

static int sdram_check(printf_func pfunc, unsigned int dw, unsigned char *p, unsigned int size)
{
  static unsigned char buffer[64];

  pfunc("%02x %02x %02x %02x %02x %02x %02x %02x\n", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
  pfunc("%02x %02x %02x %02x %02x %02x %02x %02x\n", p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);

  chacha_init();
  while (size > 0)
  {
    unsigned int s = size > 64 ? 64 : size;
    chacha_fill_u8(&rng, buffer, s);
    switch (dw)
    {
      case 8:
        unsigned char *b8 = buffer;
        unsigned char *p8 = p;
        for (int i = 0; i < s; i++)
        {
          if (*b8++ != *p8)
            return check_failed(pfunc, p8);
          p8++;
        }
        break;
      case 16:
        unsigned short *b16 = (unsigned short*)buffer;
        unsigned short *p16 = (unsigned short*)p;
        for (int i = 0; i < s / 2; i++)
        {
          if (*b16++ != *p16)
            return check_failed(pfunc, p16);
          p16++;
        }
        break;
      default:
        unsigned int *b32 = (unsigned int*)buffer;
        unsigned int *p32 = (unsigned int*)p;
        for (int i = 0; i < s / 4; i++)
        {
          if (*b32++ != *p32)
            return check_failed(pfunc, p32);
          p32++;
        }
        break;
    }
    p += s;
    size -= s;
  }
  return 0;
}

static int sdram_check_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned int dw;
  unsigned int size;
  unsigned char *p;

  int rc = parse_params(argv[0], argv[1], argv[2], &dw, &p, &size);
  if (rc) return rc;
  return sdram_check(pfunc, dw, p, size);
}

static int sdram_fill_check_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned int dw;
  unsigned int size;
  unsigned char *p;

  int rc = parse_params(argv[0], argv[1], argv[2], &dw, &p, &size);
  if (rc) return rc;
  sdram_fill(pfunc, dw, p, size);
  return sdram_check(pfunc, dw, p, size);
}

void register_sdram_commands(void *_sdram_address, unsigned int _sdram_size)
{
  sdram_address = _sdram_address;
  sdram_size = _sdram_size;
  shell_register_command(&sdram_fill_command);
  shell_register_command(&sdram_check_command);
  shell_register_command(&sdram_fill_check_command);
}

