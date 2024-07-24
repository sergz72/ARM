#include "board.h"
#include <stdio.h>
#include <adc.h>
#include <itoa.h>

int get_temp(void)
{
  int t;

  ADC_On(ADC1);
  t = ADC_Read(ADC1, TEMP_SENSOR_ADC_CHANNEL, ADC_SampleTime_239Cycles5);
  ADC_Off(ADC1);

  return t * 3300 / 4095;
}

void run_one_byte_command(char *command, char *buf)
{
	int t;
	
	switch (*command)
	{
		case 'I': // Device identification
			*buf++ = 'L';
			*buf++ = 'H';
			*buf++ = 'C';
			*buf++ = '1';
			*buf++ = '0';
		  break;
		case 'r': //read ADC value
			t = get_temp();
			if (t < 1000)
				*buf++ = '0';
			if (t < 100)
				*buf++ = '0';
			if (t < 10)
				*buf++ = '0';
      itoa(t, buf, 10);
		  return;
		default: *buf++ = 'e'; break;
	}
	
  *buf = 0;
}

void run_two_byte_command(char *command, char *buf)
{
	char c;
	
	switch (*command)
	{
		case 'd': //disable heater
			command++;
		  c = *command;
  	  if (c == '0')
	  	{
  			HEATER0_OFF;
				*buf++ = 'k';
				break;
			}
  	  if (c == '1')
	  	{
  			HEATER1_OFF;
				*buf++ = 'k';
				break;
			}
			*buf++ = 'e';
		  break;
		case 'e': //enable heater
			command++;
		  c = *command;
  	  if (c == '0')
	  	{
  			HEATER0_ON;
				*buf++ = 'k';
				break;
			}
  	  if (c == '1')
	  	{
  			HEATER1_ON;
				*buf++ = 'k';
				break;
			}
			*buf++ = 'e';
		  break;
	}

  *buf = 0;
}

void process_command(char *command, char *buf, unsigned int command_size)
{
	switch (command_size)
	{
		case 1: run_one_byte_command(command, buf); break;
		case 2: run_two_byte_command(command, buf); break;
    default:
      *buf++ = 'e';
      *buf = 0;
      break;
  }
}
