#include "board.h"
#include "ui.h"
#include "devices.h"
#include "dev_keyboard.h"
#include <getstring.h>
#include <pico/stdio.h>
#include <stdio.h>
#include <pico/time.h>
#include <keyboard.h>
#include "main_loop.h"

const unsigned char KeyboardMap[][1] = {
    {KEYBOARD_EVENT_F4},
    {KEYBOARD_EVENT_LEAVE},
    {KEYBOARD_EVENT_KEY8},
    {KEYBOARD_EVENT_KEY4},

    {KEYBOARD_EVENT_F3},
    {KEYBOARD_EVENT_ENTER},
    {KEYBOARD_EVENT_KEY7},
    {KEYBOARD_EVENT_KEY3},

    {KEYBOARD_EVENT_F2},
    {KEYBOARD_EVENT_KEY0},
    {KEYBOARD_EVENT_KEY6},
    {KEYBOARD_EVENT_KEY2},

    {KEYBOARD_EVENT_F1},
    {KEYBOARD_EVENT_KEY9},
    {KEYBOARD_EVENT_KEY5},
    {KEYBOARD_EVENT_KEY1},
};

static char command_line[200];

int getch_(void)
{
  int ch = getchar_timeout_us(0);
  if (ch == PICO_ERROR_TIMEOUT)
    return EOF;
  return ch;
}

char *gets_(void)
{
  getstring_next();
  return command_line;
}

void puts_(const char *s)
{
  while (*s)
    putchar(*s++);
}

int main(void)
{
  int rc;

  SystemInit();
  stdio_init_all();

  //sleep_ms(10000);

  UI_Init();
  KbdInit((const unsigned char*)KeyboardMap, NULL, 0, HALKbdHandler);

  shell_init(printf, gets_);
  //printf("shell_init\n");

  sleep_ms(1000); // time for peripheral devices to initialize

  BuildDeviceList();
  //printf("BuildDeviceList\n");

  BuildShellCommands();
  //printf("BuildShellCommands\n");

  dev_keyboard_init();
  //printf("dev_keyboard_init\n");

  getstring_init(command_line, sizeof(command_line), getch_, puts_);
  //printf("getstring_init\n");

  main_loop_init();
  while(1)
  {
    sleep_ms(100);
    if (!getstring_next())
    {
      switch (command_line[0])
      {
        case SHELL_UP_KEY:
          puts("\r\33[2K$ ");
          getstring_buffer_init(shell_get_prev_from_history());
          break;
        case SHELL_DOWN_KEY:
          puts("\r\33[2K$ ");
          getstring_buffer_init(shell_get_next_from_history());
          break;
        default:
          rc = shell_execute(command_line);
          if (rc == 0)
            printf("OK\n$ ");
          else if (rc < 0)
            printf("Invalid command line\n$ ");
          else
            printf("shell_execute returned %d\n$ ", rc);
          break;
      }
    }
    main_loop();
  }
}
