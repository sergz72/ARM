#include "board.h"
#include "controller.h"

static const ProgramItem default_program[] = {
    { 100, 30},
    { 210, 5},
    { 0, 0}
};

static const ProgramItem *current_program, *current_program_step;
static enum OvenStatus current_status;
static unsigned int current_step_time;
static int count_down;

void controller_init(void)
{
  set_program(default_program);
  stop_program();
}

void set_status(enum OvenStatus status)
{
  current_status = status;
  switch (status)
  {
    case Idle:
      off();
      put_pixel(urgb_u32(0, 0x10, 0)); // green
      break;
    case Off:
      off();
      put_pixel(urgb_u32(0x10, 0x10, 0)); // yellow
      break;
    case On:
      on();
      put_pixel(urgb_u32(0x10, 0, 0)); // red
      break;
  }
}

enum OvenStatus update_status(int temperature)
{
  if (current_program_step != NULL)
  {
    if (count_down)
    {
      current_step_time--;
      if (current_step_time == 0)
      {
        current_program_step++;
        if (current_program_step->temperature == 0)
        {
          stop_program();
          return current_status;
        }
        current_step_time = current_program_step->time;
        count_down = 0;
      }
    }
    if (temperature >= current_program_step->temperature && current_status == On)
    {
      count_down = 1;
      set_status(Off);
    }
    else if (temperature < current_program_step->temperature && current_status == Off)
      set_status(On);
  }
  return current_status;
}

void set_program(const ProgramItem *program)
{
  stop_program();
  current_program = program;
}

const ProgramItem *get_program(void)
{
  return current_program;
}

const ProgramItem *get_current_step(void)
{
  return current_program_step;
}

unsigned int get_current_step_time(void)
{
  return current_step_time;
}

enum OvenStatus start_program(void)
{
  current_step_time = current_program->time;
  current_program_step = current_program;
  count_down = 0;
  set_status(Off);
  return current_status;
}

void stop_program(void)
{
  set_status(Idle);
  current_program_step = NULL;
  current_step_time = 0;
  count_down = 0;
}
