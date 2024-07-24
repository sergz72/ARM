#include "board.h"
#include "drill.h"
#include <timer.h>

int _drill_power;
int _drill_max_power;
int _drill_step;
int _drill_temp;

int calc_step(int rise_time)
{
  int steps = rise_time * 10;
  int step = _drill_max_power / steps;
	return step > 0 ? step : 1;
}

int drill_number1_handler(char c)
{
	if (c < '0' || c > '9')
		return 0;
	else
	{
		_drill_temp += c - '0';
		if (command == 'd')
		  _drill_max_power = _drill_temp * 10;
		else
			_drill_step = calc_step(_drill_temp);
		OK();
		return 1;
	}
}

int drill_number2_handler(char c)
{
	if (c < '0' || c > '9')
		return 0;
	else
	{
		_drill_temp = (c - '0') * 10;
	  serial_handler = drill_number1_handler;
		return 1;
	}
}

void drill_on(void)
{
  if (_drill_power < _drill_max_power)
  {
    _drill_power += _drill_step;
    // adjust the PWM
    TIM_SetCompare1(TIM2, TIM_PERIOD * _drill_power / 1000);
  }
  else if (_drill_power > _drill_max_power)
  {
    _drill_power = _drill_max_power;
    // adjust the PWM
    TIM_SetCompare1(TIM2, TIM_PERIOD * _drill_power / 1000);
  }
}

void drill_off(void)
{
  if (_drill_power)
  {
    _drill_power = 0;
    TIM_SetCompare1(TIM2, 0);
  }
}

int drill_handler(char c)
{
	switch (c)
	{
    case 'd':
    case 'r':
			_drill_temp = 0;
			serial_handler = drill_number2_handler;
			return 1;
		default:
			return 0;
	}
}

void drill_init(void)
{
  _drill_power = 0;
  _drill_max_power = 1000;
  _drill_step = 10;
}

void drill_timer_proc(void)
{
  if (DRILL_BUTTON_PRESSED)
    drill_on();
  else
    drill_off();
}
