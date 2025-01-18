#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "board.h"
#include "devices.h"
#include "driver/uart.h"

extern esp_err_t pmu_init();
extern esp_err_t pmu_i2c_init(void);

static const char *TAG = "main";
static const char error_ = 'e';

static unsigned char uart_buffer[UART_BUFFER_SIZE];
static Device *current_channel;
static int timer_event_id;

static void error(unsigned long ms)
{
  unsigned long v = ms / portTICK_PERIOD_MS;

  for (;;)
  {
    vTaskDelay(v);
    blink_led();
  }
}

static void error_response(void)
{
  uart_write_bytes(UART_PORT, &error_, 1);
}

static void device_list_response()
{
  Device **d = device_list;
  for (int i = 0; i < MAX_DEVICES; i++)
  {
    Device *dd = *d;
    uart_buffer[i] = dd ? (unsigned char)dd->public_id : 0;
    d++;
  }
  uart_write_bytes(UART_PORT, uart_buffer, MAX_DEVICES);
}

static void timer_event(void)
{
  uart_buffer[0] = 'k';
  int len = 1;
  unsigned char *p = uart_buffer + 1;

  Device **d = device_list;
  int id = timer_event_id;
  for (int i = 0; i < MAX_DEVICES; i++)
  {
    Device *dd = *d;
    if (dd)
    {
      if (dd->timer_event)
      {
        len += dd->timer_event(i, id, device_config[i], &device_data[i], p + 1);
        if (len)
        {
          *p++ = (unsigned char)i;
          p += len;
        }
      }
    }
    if (id == 9)
      id = 0;
    else
      id++;
    d++;
  }
  uart_write_bytes(UART_PORT, uart_buffer, len);
  if (timer_event_id == 9)
    timer_event_id = 0;
  else
    timer_event_id++;
}

void app_main(void)
{
  //vTaskDelay(5000 / portTICK_PERIOD_MS);

  configure_led();

#ifdef PMU_I2C_MASTER_NUM
  if (pmu_i2c_init() != ESP_OK)
    error(50);
  ESP_LOGI(TAG, "PMU I2C initialized successfully");
  if (pmu_init() != ESP_OK)
    error(200);
#endif

  configure_i2c();
  configure_uart();

  InitDeviceLists();

  current_channel = NULL;

  unsigned long start_time = xTaskGetTickCount();

  timer_event_id = 0;

  unsigned char c = 0;

  for (;;)
  {
    int len = uart_read_bytes(UART_PORT, uart_buffer, UART_BUFFER_SIZE, 10 / portTICK_PERIOD_MS);
    if (len > 0)
    {
      if (current_channel)
      {
        int response_length = current_channel->message_processor(c, device_config[c], device_data[c],
                                                                 uart_buffer, len);
        if (response_length)
        {
          uart_write_bytes(UART_PORT, uart_buffer, response_length);
          current_channel = NULL;
        }
      }
      else
      {
        c = uart_buffer[0];
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
        else if (c < MAX_DEVICES) // channel message
        {
          if (len < 2)
            error_response();
          else
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
                int response_length = current_channel->message_processor(c, device_config[c], device_data[c],
                                                                         uart_buffer + 1, len - 1);
                if (response_length)
                {
                  uart_write_bytes(UART_PORT, uart_buffer + 1, response_length);
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
    unsigned long new_time = xTaskGetTickCount();
    if (new_time - start_time >= 1000 / portTICK_PERIOD_MS)
    {
      blink_led();
      start_time = new_time;
    }
  }
}
