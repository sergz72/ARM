#include <stddef.h>
#include "device_list.h"
#include "dev_ina226.h"
#include "dev_si5351.h"
#include "dev_mcp3421.h"
#include "dev_freq_pwm.h"
#include "dev_cap_meter.h"

const Device devices[] = {
    {
        .name = "cap_meter",
        .short_name = "CA",
        .initializer = cap_meter_initializer,
        .data_collector = NULL,
        .ui_init_handler = cap_meter_ui_init_handler,
        .ui_data_handler = cap_meter_ui_handler,
        .ui_keyboard_handler = cap_meter_ui_keyboard_handler,
        .print_config = cap_meter_print_config,
        .set_config = cap_meter_set_config,
        .set_config_parameter_count = 2,
        .set_config_help = "offset R",
    },
    {
        .name = "freq_pwm",
        .short_name = "FP",
        .initializer = freq_pwm_initializer,
        .data_collector = NULL,
        .ui_init_handler = freq_pwm_ui_init_handler,
        .ui_data_handler = freq_pwm_ui_handler,
        .ui_keyboard_handler = freq_pwm_ui_keyboard_handler,
        .print_config = freq_pwm_print_config,
        .set_config = freq_pwm_set_config,
        .set_config_parameter_count = 1,
        .set_config_help = "cpu_freq",
    },
    {
        .name = "ina226",
        .short_name = "In",
        .initializer = ina226_initializer,
        .data_collector = ina226_data_collector,
        .ui_init_handler = ina226_ui_init_handler,
        .ui_data_handler = ina226_ui_handler,
        .ui_keyboard_handler = NULL,
        .print_config = NULL,
        .set_config = NULL,
        .set_config_parameter_count = 0,
        .set_config_help = NULL,
    },
    {
        .name = "mcp3421",
        .short_name = "MC",
        .initializer = mcp3421_initializer,
        .data_collector = mcp3421_data_collector,
        .ui_init_handler = NULL,
        .ui_data_handler = mcp3421_ui_handler,
        .ui_keyboard_handler = NULL,
        .print_config = msp3421_print_config,
        .set_config = msp3421_set_config,
        .set_config_parameter_count = 2,
        .set_config_help = "coef offset",
    },
    {
        .name = "si5351",
        .short_name = "SI",
        .initializer = si5351_initializer,
        .data_collector = NULL,
        .ui_init_handler = si5351_ui_init_handler,
        .ui_data_handler = NULL,
        .ui_keyboard_handler = si5351_ui_keyboard_handler,
        .print_config = NULL,
        .set_config = NULL,
        .set_config_parameter_count = 0,
        .set_config_help = NULL,
    }
};