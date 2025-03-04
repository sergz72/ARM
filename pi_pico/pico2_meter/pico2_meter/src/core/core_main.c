#include <string.h>
#include "board.h"
#include "devices.h"
#include <24c01_16.h>

#define COMM_BUFFER_SIZE 1024

typedef struct
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
static DeviceObject *current_channel;
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
  DeviceObject *d = device_list;
  int idx = 0;
  for (int i = 0; i < MAX_TOTAL_DEVICES; i++)
  {
    if (d->device == NULL)
    {
      comm_buffer[idx++] = 0;
      comm_buffer[idx++] = 0;
    }
    else
    {
      comm_buffer[idx+2] = (unsigned char)d->device->public_id;
      int len = d->device->save_config ? d->device->save_config(d, &comm_buffer[idx+3]) + 1 : 1;
      comm_buffer[idx] = len & 0xff;
      comm_buffer[idx+1] = (len >> 8) & 0xff;
      idx += len + 2;
    }
    d++;
  }
  main_comm_port_write_bytes(comm_buffer, idx);
}

static void timer_event(void)
{
  comm_buffer[0] = 'k';
  int len = 1;
  unsigned char *p = comm_buffer + 1;

  DeviceObject *d = device_list;
  int id = timer_event_id;
  for (int i = 0; i < MAX_TOTAL_DEVICES; i++)
  {
    if (d->device)
    {
      if (d->device->timer_event)
      {
        change_channel(i);
        int elen = d->device->timer_event(d, id, p + 3);
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
        main_comm_port_write_bytes((const unsigned char*)"we", 2);
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
      main_comm_port_write_bytes((const unsigned char*)"re", 2);
    else
      main_comm_port_write_bytes(&eeprom_command_buffer.command.data, eeprom_command_buffer.command.size);
    eeprom_command_buffer_p = NULL;
  }
}

void core_main(void)
{
  configure_hal();

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
        int response_length = current_channel->device->message_processor(current_channel, comm_buffer, len);
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
        else if (c < MAX_TOTAL_DEVICES) // channel message
        {
          current_channel = &device_list[c];
          if (!current_channel->device)
            error_response();
          else
          {
            if (!current_channel->device->message_processor)
            {
              current_channel = NULL;
              error_response();
            }
            else
            {
              change_channel(current_channel->idx);
              if (len > 1)
              {
                int response_length = current_channel->device->message_processor(current_channel, comm_buffer + 1, len - 1);
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
