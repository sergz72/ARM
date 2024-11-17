#ifndef PICO_OVEN_CONTROL_CONTROLLER_H
#define PICO_OVEN_CONTROL_CONTROLLER_H

enum OvenStatus
{
  Idle,
  Off,
  On
};

typedef struct {
  int temperature;
  unsigned int time;
} ProgramItem;

void set_status(enum OvenStatus status);
void controller_init(void);
enum OvenStatus update_status(int temperature);
void set_program(const ProgramItem *program);
const ProgramItem *get_program(void);
enum OvenStatus start_program(void);
void stop_program(void);
const ProgramItem *get_current_step(void);
unsigned int get_current_step_time(void);

#endif
