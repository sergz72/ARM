#include "cc1101_commands.h"
#include <shell.h>
#include "cc1101_func.h"
#include <stdlib.h>
#include <read_hex_string.h>
#include <string.h>

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem init_command_items[] = {
  {nullptr, param_handler, nullptr}, // address
  {nullptr, param_handler, nullptr}, // packet length
  {nullptr, param_handler, nullptr}, // frequency offset
  {nullptr, nullptr, init_handler}
};
static const ShellCommand init_command = {
  init_command_items,
  "cc1101_init",
  "cc1101_init host_address packet_length freq_offset",
  nullptr,
  nullptr
};

static int receive_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem receive_command_items[] = {
  {nullptr, nullptr, receive_handler}
};
static const ShellCommand receive_command = {
  receive_command_items,
  "cc1101_receive",
  "cc1101_receive",
  nullptr,
  nullptr
};

static int receive_start_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem receive_start_command_items[] = {
  {nullptr, nullptr, receive_start_handler}
};
static const ShellCommand receive_start_command = {
  receive_start_command_items,
  "cc1101_receive_start",
  "cc1101_receive_start",
  nullptr,
  nullptr
};

static int receive_stop_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem receive_stop_command_items[] = {
  {nullptr, nullptr, receive_stop_handler}
};
static const ShellCommand receive_stop_command = {
  receive_stop_command_items,
  "cc1101_receive_stop",
  "cc1101_receive_stop",
  nullptr,
  nullptr
};

static int send_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem send_command_items[] = {
  {nullptr, param_handler, nullptr}, // address
  {nullptr, param_handler, nullptr}, // data
  {nullptr, nullptr, send_handler}
};
static const ShellCommand send_command = {
  send_command_items,
  "cc1101_send",
  "cc1101_send address data",
  nullptr,
  nullptr
};

static int packet_length = 0;

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int host_address = atoi(argv[0]);
  if (host_address < 0 || host_address > 255)
  {
    pfunc("Invalid host address\n");
    return 1;
  }
  packet_length = atoi(argv[1]);
  if (packet_length <= 0 || packet_length > 255)
  {
    pfunc("Invalid packet_length\n");
    return 2;
  }
  int frequency = atoi(argv[2]);
  if (frequency < 430000 || frequency > 439999)
  {
    pfunc("Invalid frequency\n");
    return 2;
  }
  return cc1101Init(host_address, packet_length, frequency);
}

static int receive_start_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  return cc1101ReceiveStart();
}

static int receive_stop_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  return cc1101ReceiveStop();
}

static int receive_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned char *buffer;
  unsigned char length;
  int rc = cc1101Receive(&buffer, &length);
  if (rc)
    return rc;
  pfunc("Received %d bytes:\n", length);
  for (int i = 0; i < 16; i++)
    pfunc("%02X ", i);
  for (int i = 0; i < length; i++)
  {
    if (!(i & 0x0F))
      pfunc("\r\n");
    pfunc("%02X ", buffer[i]);
  }
  return 0;
}

static int send_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  static unsigned char data_to_send[64];

  const int address = atoi(argv[0]);
  if (address < 0 || address > 255)
  {
    pfunc("Invalid address\n");
    return 1;
  }

  int l = strlen(argv[1]);
  if (l != packet_length * 2 - 2)
  {
    pfunc("Invalid data length\n");
    return 2;
  }
  const int rc = read_hex_string(argv[1], data_to_send, packet_length);
  if (rc <= 0)
  {
    pfunc("Invalid data\r\n");
    return 3;
  }

  return cc1101Send(address, data_to_send, packet_length);
}

void register_cc1101_commands(void)
{
  shell_register_command(&init_command);
  shell_register_command(&send_command);
  shell_register_command(&receive_command);
  shell_register_command(&receive_start_command);
  shell_register_command(&receive_stop_command);
}
