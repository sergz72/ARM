#include "board.h"
#include "heater.h"
#include <timer.h>
#include <adc.h>
#include <stdio.h>

unsigned int _heater_value[3];
unsigned int _heater_real_value[3];
unsigned int _heater_temp, _heater_id;

int heater_number1_handler(char c)
{
	if (c < '0' || c > '9')
		return 0;
	else
	{
		_heater_temp += c - '0';
    if (_heater_temp >= 4096)
      return 0;
		if (!_heater_temp)
		  heater_off(_heater_id + 1);
    _heater_value[_heater_id] = _heater_temp;
		OK();
		return 1;
	}
}

int heater_number2_handler(char c)
{
	if (c < '0' || c > '9')
		return 0;
	else
	{
		_heater_temp += (c - '0') * 10;
	  serial_handler = heater_number1_handler;
		return 1;
	}
}

int heater_number3_handler(char c)
{
	if (c < '0' || c > '9')
		return 0;
	else
	{
		_heater_temp += (c - '0') * 100;
	  serial_handler = heater_number2_handler;
		return 1;
	}
}

int heater_number4_handler(char c)
{
	if (c < '0' || c > '4')
		return 0;
	else
	{
		_heater_temp = (c - '0') * 1000;
	  serial_handler = heater_number3_handler;
		return 1;
	}
}

int heater_number5_handler(char c)
{
	if (c < '1' || c > '3')
		return 0;
	else
	{
		_heater_id = c - '1';
	  serial_handler = heater_number4_handler;
		return 1;
	}
}

void heater_off(unsigned int heater_no)
{
	if (heater_no < 1 || heater_no > 3)
    return;	
  GPIOA->BSRR = 1<<heater_no;
}

void heater_on(unsigned int heater_no)
{
	if (heater_no < 1 || heater_no > 3)
    return;	
  GPIOA->BSRR = 1<<(16 + heater_no);
}

int heater_handler(char c)
{
	switch (c)
	{
    case 'h': // change heater temp
			_heater_temp = 0;
			serial_handler = heater_number5_handler;
			return 1;
    case 's': // status
			*write_buffer_p++ = 's';
			END();
			return 1;
		default:
			return 0;
	}
}

void heater_send_status(void)
{
	char buffer[20];

	sprintf(buffer, "%04d%04d%04d%d", _heater_real_value[0], _heater_real_value[1], _heater_real_value[2],
	        IRON_BUTTON_PRESSED ? 1 : 0);
  str_send(buffer);
}

void heater_init(void)
{
  _heater_value[0] = 0;
  _heater_value[1] = 0;
  _heater_value[2] = 0;
  _heater_real_value[0] = 0;
  _heater_real_value[1] = 0;
  _heater_real_value[2] = 0;
}

void heater_timer_proc(void)
{
 	heater_off(1);
	heater_off(2);
	heater_off(3);
	delay(500); // 500 us delay
	ADC_StartConversion(ADC1, ADC_Channel_7, ADC_SampleTime_1Cycles5);
	_heater_real_value[0] = ADC_Read(ADC1) & 0xFFF;
	ADC_StartConversion(ADC1, ADC_Channel_6, ADC_SampleTime_1Cycles5);
	_heater_real_value[1] = ADC_Read(ADC1) & 0xFFF;
	ADC_StartConversion(ADC1, ADC_Channel_5, ADC_SampleTime_1Cycles5);
	_heater_real_value[2] = ADC_Read(ADC1) & 0xFFF;
	if (_heater_value[0] != 0 && _heater_real_value[0] < _heater_value[0])
		heater_on(1);
	if (_heater_value[1] != 0 && _heater_real_value[1] < _heater_value[1])
		heater_on(2);
	if (_heater_value[2] != 0 && _heater_real_value[2] < _heater_value[2])
		heater_on(3);
}
