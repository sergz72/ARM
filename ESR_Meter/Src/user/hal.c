#include <board.h>
#include <main.h>

extern I2C_HandleTypeDef hi2c1;

int SSD1306_I2C_Write(int num_bytes, unsigned char control_byte, unsigned char *buffer)
{
  return HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDRESS, control_byte, I2C_MEMADD_SIZE_8BIT, buffer, num_bytes, 0xFFFFF) != HAL_OK;
}

