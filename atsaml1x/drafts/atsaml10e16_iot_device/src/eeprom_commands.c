#include "board.h"
#include "eeprom_commands.h"
#include <shell.h>
#include <read_hex_string.h>
#include <stdlib.h>

unsigned char memory_buffer[EEPROM_BUFFER_SIZE];

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
  {nullptr, param_handler, nullptr},
  {nullptr, param_handler, nullptr},
  {nullptr, nullptr, read_handler}
};
static const ShellCommand read_command = {
  read_command_items,
  "eeprom_read",
  "eeprom_read memory_address length",
  nullptr,
  nullptr
};

static int write_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem write_command_items[] = {
  {nullptr, param_handler, nullptr},
  {nullptr, param_handler, nullptr},
  {nullptr, nullptr, write_handler}
};
static const ShellCommand write_command = {
  write_command_items,
  "eeprom_write",
  "eeprom_write memory_address data",
  nullptr,
  nullptr
};

void print_hex_buffer(unsigned char *buffer, int length, printf_func pfunc)
{
  pfunc("   ");
  for (int i = 0; i < 16; i++)
    pfunc(" %02x", i);

  for (int i = 0; i < length; i++)
  {
    int row_no = i / 16;
    if (i % 16 == 0)
      pfunc("\n%02x:", row_no);
    pfunc(" %02x", *buffer++);
  }
  pfunc("\n");
}

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned long int memory_address = strtoul(argv[0], nullptr, 16);
  int length = atoi(argv[1]);
  if (length <= 0 || length > sizeof(memory_buffer))
    return 1;
  if (memory_address + length > 4096)
  {
    pfunc("Invalid address\n");
    return 2;
  }
  int rc = eeprom_read(memory_address, memory_buffer, length);
  if (rc)
    return rc;
  print_hex_buffer(memory_buffer, length, pfunc);
  return 0;
}

static int write_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned long int memory_address = strtoul(argv[0], nullptr, 16);
  int length = read_hex_string(argv[1], memory_buffer, sizeof(memory_buffer));
  if (length <= 0)
  {
    pfunc("Invalid data\n");
    return 1;
  }
  if (memory_address + length > 4096)
  {
    pfunc("Invalid address\n");
    return 2;
  }
  pfunc("Writing %d bytes to address 0x%04X\n", length, memory_address);
  return eeprom_write(memory_address, memory_buffer, length);
}

void register_eeprom_commands(void)
{
  shell_register_command(&write_command);
  shell_register_command(&read_command);
}
