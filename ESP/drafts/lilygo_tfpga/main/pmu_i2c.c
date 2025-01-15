#include <string.h>
#include "driver/i2c_master.h"
#include "board.h"

#ifdef PMU_I2C_MASTER_NUM
i2c_master_bus_handle_t bus_handle;

esp_err_t pmu_i2c_init(void)
{
    i2c_master_bus_config_t i2c_bus_config;
    memset(&i2c_bus_config, 0, sizeof(i2c_bus_config));
    i2c_bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_bus_config.i2c_port = PMU_I2C_MASTER_NUM;
    i2c_bus_config.scl_io_num = PMU_IIC_SCL;
    i2c_bus_config.sda_io_num = PMU_IIC_SDA;
    i2c_bus_config.glitch_ignore_cnt = 7;
    i2c_bus_config.flags.enable_internal_pullup = true;
    return i2c_new_master_bus(&i2c_bus_config, &bus_handle);
}
#endif
