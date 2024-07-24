#include "board.h"
#include <stdio.h>

void run_one_byte_command(char *command, char *buf)
{
	int i;
	
	switch (*command)
	{
		case 'I': // Device identification
			*buf++ = 'C';
			*buf++ = 'H';
			*buf++ = 'G';
			*buf++ = '1';
			*buf++ = '0';
		  break;
		case 'S': // Status
			for (i = 0; i < MAX_CHANNEL; i++)
		    buf += sprintf(buf, "%04d %03d %04d %03d %04d|", status.channel[i].set.U, status.channel[i].set.I,
		                   status.channel[i].fact.U, status.channel[i].fact.I, status.channel[i].duty);
		  break;
		case 'R': // Reset
			for (i = 0; i < MAX_CHANNEL; i++)
		  {
     	  status.channel[i].set.U = 0;
	      status.channel[i].set.I = 0;
	   	  setDuty(i, 0);
			}
		  break;
		default: *buf++ = 'e'; break;
	}
	
  *buf = 0;
}

void run_four_byte_command(char *command, char *buf)
{
//	int F;
	int channel, duty;
	char c;
	
	switch (*command)
	{
/*		case 'F': // PWM Frequency change
			command++;
			F = *command - '0';
			command++;
		  F *= 10;
			F += *command - '0';
			command++;
		  F *= 10;
			F += *command - '0';
		  TIM_PERIOD = 72000/F;
      TIM3->ARR = TIM_PERIOD;
			pwm_init();
		  sprintf(buf, "F=%d", F);
      return;*/
		case 'D': //PWM duty change
			*buf++ = 'k';
			command++;
		  c = *command;
  	  if (c < '0' || c > '2')
	  	{
				*buf++ = 'e';
				break;
			}
			channel = c - '0';
			command++;
		  c = *command;
  	  if (c < '0' || c > '4')
	  	{
				*buf++ = 'e';
				break;
			}
			duty = c - '0';
			command++;
		  duty *= 10;
		  c = *command;
  	  if (c < '0' || c > '9')
	  	{
				*buf++ = 'e';
				break;
			}
			duty += c - '0';
			setDuty(channel, _PWM_PERIOD_ * duty / 100);
			*buf++ = 'k';
  		break;
		default: *buf++ = 'e'; break;
	}
	
  *buf = 0;
}

int set_channel(int channel, char *command)
{
	int U = 0, I = 0, i;
	char c;
	
	for (i = 0; i < 3; i++)
	{
		c = *command++;
	  if (c < '0' || c > '9')
			return 0;
		U *= 10;
		U += c - '0';
	}

	for (i = 0; i < 2; i++)
	{
		c = *command++;
	  if (c < '0' || c > '9')
			return 0;
		I *= 10;
		I += c - '0';
	}
	
	status.channel[channel].set.U = U * 10;
	status.channel[channel].set.I = I * 10;
	
	if (U == 0 || I == 0)
		setDuty(channel, 0);
	
	return 1;
}

void run_six_byte_command(char *command, char *buf)
{
	char c = *command;
	
	switch (*command)
	{
		case '0': // set channel 0
		case '1': // set channel 1
		case '2': // set channel 2
			if (set_channel(c - '0', command+1))
			{
			  *buf++ = 'k';
		    break;
			}
		default: *buf++ = 'e'; break;
	}
	
  *buf = 0;
}

void process_command(char *command, char *buf, unsigned int command_size)
{
	switch (command_size)
	{
		case 1: run_one_byte_command(command, buf); break;
		case 4: run_four_byte_command(command, buf); break;
		case 6: run_six_byte_command(command, buf); break;
    default:
      *buf++ = 'e';
      *buf = 0;
      break;
  }
}
