#include "board.h"
#include <generic_dds.h>
#include <ad9833.h>
#include <mcp41_42.h>

const dds_config config =
{
    .max_frequency = 12500000,
    .min_frequency = 1,
    .mclk_MHz = 25,
    .max_vout_mV = 4000,
    .max_attenuator_value = 160,
    .channels = 1,
    .accumulator_bits = 28,
    .out_square_divider_bits = 1,
    .supported_modes = (1 << DDS_MODE_SINE) | (1 << DDS_MODE_TRIANGLE) | (1 << DDS_MODE_SQUARE)
};

static volatile unsigned char *rx_data_p;
static volatile char *tx_data_p;
static volatile int rx_cnt, tx_cnt, command_to_process, current_command;
static dds_i2c_command command[DDS_COMMAND_QUEUE_LENGTH];

void TXISHandler(I2C_TypeDef* i2c)
{
  if (tx_cnt < sizeof(dds_config))
  {
    i2c->TXDR = *tx_data_p++;
    tx_cnt++;
  }
  else
    i2c->TXDR = 0;
}

void RXNEHandler(I2C_TypeDef* i2c)
{
  volatile unsigned int dummy;
  if (rx_cnt < sizeof(command))
  {
    *rx_data_p++ = i2c->RXDR;
    rx_cnt++;
  }
  else
    dummy = i2c->RXDR;
}

void STOPHandler(I2C_TypeDef* i2c)
{
  LED_GREEN_OFF;
  if (rx_cnt)
  {
    current_command++;
    if (current_command >= DDS_COMMAND_QUEUE_LENGTH)
      current_command = 0;
  }
  i2c->ISR = I2C_ISR_TXE;
  i2c->ICR = I2C_ICR_STOPCF;
}

void ADDRHandler(I2C_TypeDef* i2c)
{
  LED_GREEN_ON;
  rx_data_p = command[current_command].bytes;
  tx_data_p = (char*)&config;
  rx_cnt = tx_cnt = 0;
  i2c->ICR = I2C_ICR_ADDRCF;
}

int main(void)
{
  unsigned short ad9833_cfg, ad9833_divider;
  dds_i2c_command *cmd;

  LED_BLUE_ON;

  command_to_process = current_command = 0;

  ad9833_cfg = AD9833_CFG_SLEEP1|AD9833_CFG_SLEEP12;
  ad9833_set_config(0, ad9833_cfg);
  ad9833_divider = 1;

  while (1)
  {
    while (current_command != command_to_process)
    {
      cmd = &command[command_to_process];
      switch (cmd->c3.command)
      {
        case DDS_COMMAND_SET_FREQUENCY_CODE:
          ad9833_set_freq0_word(0, cmd->c8.freq);
          if (ad9833_cfg & AD9833_CFG_MSBDIV2)
          {
            ad9833_divider = cmd->c8.div;
            if (ad9833_divider == 1)
            {
              if ((ad9833_cfg & AD9833_CFG_MSB) == AD9833_CFG_MSBDIV2)
                ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_MSB);
            }
            else if ((ad9833_cfg & AD9833_CFG_MSB) == AD9833_CFG_MSB)
              ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_MSBDIV2);
          }
          break;
        case DDS_COMMAND_SET_MODE:
          switch (cmd->c3.parameter)
          {
            case DDS_MODE_SINE:
              ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_SINUS);
              break;
            case DDS_MODE_TRIANGLE:
              ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_TRIANGLE);
              break;
            case DDS_MODE_SQUARE:
              if (ad9833_divider == 1)
                ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_MSB);
              else
                ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_MSBDIV2);
              break;
          }
          break;
        case DDS_COMMAND_SET_ATTENUATOR:
          mcp41_42_set(0, MCP41_42_SET_DATA0, cmd->c4.parameter);
          break;
        case DDS_COMMAND_ENABLE_OUTPUT:
          if (cmd->c3.parameter)
            ad9833_cfg &= ~(AD9833_CFG_SLEEP1|AD9833_CFG_SLEEP12);
          else
            ad9833_cfg |= AD9833_CFG_SLEEP1|AD9833_CFG_SLEEP12;
          ad9833_set_config(0, ad9833_cfg);
          break;
      }
      command_to_process++;
      if (command_to_process >= DDS_COMMAND_QUEUE_LENGTH)
        command_to_process = 0;
    }
    __WFI();
  }
}
