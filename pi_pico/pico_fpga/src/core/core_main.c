#include <string.h>
#include "board.h"
#include "devices.h"
#include <24c01_16.h>

#define COMM_BUFFER_SIZE 1024

typedef union
{
  unsigned short size;
  unsigned char device_id;
  unsigned char data;
} eeprom_command;

static const unsigned char error_ = 'e';
static const unsigned char ok_ = 'k';

static unsigned char comm_buffer[COMM_BUFFER_SIZE];

static union
{
  eeprom_command command;
  unsigned char buffer[COMM_BUFFER_SIZE];
} eeprom_command_buffer;

static unsigned char *eeprom_command_buffer_p;
static Device *current_channel;
static int timer_event_id;

static void error_response(void)
{
  main_comm_port_write_bytes(&error_, 1);
}

static void ok_response(void)
{
  main_comm_port_write_bytes(&ok_, 1);
}

static void device_list_response()
{
  Device **d = device_list;
  int idx = 0;
  for (int i = 0; i < MAX_DEVICES; i++)
  {
    Device *dd = *d++;
    if (dd == NULL)
    {
      comm_buffer[idx++] = 0;
      comm_buffer[idx++] = 0;
      continue;
    }
    comm_buffer[idx+2] = (unsigned char)dd->public_id;
    int len = dd->save_config ? dd->save_config(i, &comm_buffer[idx+3]) + 1 : 1;
    comm_buffer[idx] = len & 0xff;
    comm_buffer[idx+1] = (len >> 8) & 0xff;
    idx += len + 2;
  }
  main_comm_port_write_bytes(comm_buffer, idx);
}

static int read_interrupt_flags(void)
{
  if (get_interrupt_pin_level())
    return 0;
  //todo
  return 0;
}

static void timer_event(void)
{
  comm_buffer[0] = 'k';
  int len = 1;
  unsigned char *p = comm_buffer + 1;

  Device **d = device_list;
  int id = timer_event_id;
  int mask = 1;
  int interrupt_flags = read_interrupt_flags();
  for (int i = 0; i < MAX_DEVICES; i++)
  {
    Device *dd = *d;
    if (dd)
    {
      if (dd->timer_event)
      {
        change_channel(i);
        int elen = dd->timer_event(i, id, device_config[i], device_data[i], interrupt_flags & mask, p + 3);
        if (elen)
        {
          *p++ = (unsigned char)i;
          *p++ = elen & 0xFF;
          *p++ = (elen >> 8) & 0xFF;
          p += elen;
          len += elen + 3;
        }
      }
    }
    if (id == 9)
      id = 0;
    else
      id++;
    d++;
    mask <<= 1;
  }
  main_comm_port_write_bytes(comm_buffer, len);
  if (timer_event_id == 9)
    timer_event_id = 0;
  else
    timer_event_id++;
}

void build_eeprom_write_command(unsigned char *buffer, int len)
{
  memcpy(eeprom_command_buffer_p, buffer, len);
  eeprom_command_buffer_p += len;
  int l = eeprom_command_buffer_p - eeprom_command_buffer.buffer;
  if (l >= 3)
  {
    if (eeprom_command_buffer.command.device_id >= MAX_DEVICES)
    {
      eeprom_command_buffer_p = NULL;
      error_response();
      return;
    }
    if (l >= eeprom_command_buffer.command.size)
    {
      if (_24C01_16_write(eeprom_command_buffer.command.device_id, _24C01_16_address(0), 0,
                      &eeprom_command_buffer.command.data, eeprom_command_buffer.command.size - 1, I2C_TIMEOUT))
        error_response();
      else
        ok_response();
      eeprom_command_buffer_p = NULL;
    }
  }
}

void build_eeprom_read_command(unsigned char *buffer, int len)
{
  memcpy(eeprom_command_buffer_p, buffer, len);
  eeprom_command_buffer_p += len;
  int l = eeprom_command_buffer_p - eeprom_command_buffer.buffer;
  if (l >= 3)
  {
    if (eeprom_command_buffer.command.device_id >= MAX_DEVICES)
    {
      eeprom_command_buffer_p = NULL;
      error_response();
      return;
    }
    if (_24C01_16_read(eeprom_command_buffer.command.device_id, _24C01_16_address(0), 0,
                    &eeprom_command_buffer.command.data, eeprom_command_buffer.command.size, I2C_TIMEOUT))
      error_response();
    else
      main_comm_port_write_bytes(&eeprom_command_buffer.command.data, eeprom_command_buffer.command.size);
    eeprom_command_buffer_p = NULL;
  }
}

void core_main(void)
{
  configure_i2c();
  configure_ports();
  configure_logger();
  configure_spi();

  delay_us(1000000); // 1 second delay
  release_reset();

  InitDeviceLists();

  current_channel = NULL;

  timer_event_id = 0;

  unsigned char c = 0;

  unsigned int delay_id = 0;

  eeprom_command_buffer_p = NULL;

  for (;;)
  {
    unsigned long long int start_time = time_us();
    int len = main_comm_port_read_bytes(comm_buffer, COMM_BUFFER_SIZE);
    if (len > 0)
    {
      if (eeprom_command_buffer_p)
      {
        if (c == 's')
          build_eeprom_write_command(comm_buffer, len);
        else
          build_eeprom_read_command(comm_buffer, len);
      }
      else if (current_channel)
      {
        int response_length = current_channel->message_processor(c, device_config[c], device_data[c],
                                                                 comm_buffer, len);
        if (response_length)
        {
          main_comm_port_write_bytes(comm_buffer, response_length);
          current_channel = NULL;
        }
      }
      else
      {
        c = comm_buffer[0];
        if (c == 'n')
        {
          if (len == 1)
          {
            BuildDeviceList();
            device_list_response();
          }
          else
            error_response();
        }
        else if (c == 't') // timer event
        {
          if (len != 1)
            error_response();
          else
            timer_event();
        }
        else if (c == 'w') // eeprom write
        {
          eeprom_command_buffer_p = eeprom_command_buffer.buffer;
          build_eeprom_write_command(comm_buffer + 1, len - 1);
        }
        else if (c == 'r') // eeprom read
        {
          eeprom_command_buffer_p = eeprom_command_buffer.buffer;
          build_eeprom_read_command(comm_buffer + 1, len - 1);
        }
        else if (c < MAX_DEVICES) // channel message
        {
          current_channel = device_list[c];
          if (!current_channel)
            error_response();
          else
          {
            if (!current_channel->message_processor)
            {
              current_channel = NULL;
              error_response();
            }
            else
            {
              change_channel(c);
              if (len > 1)
              {
                int response_length = current_channel->message_processor(c, device_config[c], device_data[c],
                                                                         comm_buffer + 1, len - 1);
                if (response_length)
                {
                  main_comm_port_write_bytes(comm_buffer + 1, response_length);
                  current_channel = NULL;
                }
              }
            }
          }
        }
        else
          error_response();
      }
    }
    else
    {
      unsigned long long int diff = time_us() - start_time;
      if (diff < 10000)
        delay_us(10000 - diff);
      delay_id++;
      if (delay_id == 100)
      {
        delay_id = 0;
        blink_led();
      }
    }
  }
}
