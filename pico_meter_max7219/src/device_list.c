#include <stddef.h>
#include "device_list.h"
#include "dev_ina226.h"
#include "dev_si5351.h"
#include "dev_mcp3421.h"
#include "dev_freq_pwm.h"
#include "dev_cap_meter.h"

const char dds_menu[] = "tfA ";

const Device devices[] = {
    {
        .name = "cap_meter",
        .short_name = "fp",
        .initializer = cap_meter_initializer,
        .data_collector = cap_meter_data_collector,
        .ui_init_handler = NULL,
        .ui_data_handler = cap_meter_ui_handler,
        .ui_keyboard_handler = NULL,
        .print_config = cap_meter_print_config,
        .set_config = cap_meter_set_config,
        .set_config_parameter_count = 2,
        .set_config_help = "offset R",
        .menu = NULL
    },
    {
        .name = "freq_pwm",
        .short_name = "fp",
        .initializer = freq_pwm_initializer,
        .data_collector = freq_pwm_data_collector,
        .ui_init_handler = NULL,
        .ui_data_handler = freq_pwm_ui_handler,
        .ui_keyboard_handler = NULL,
        .print_config = freq_pwm_print_config,
        .set_config = freq_pwm_set_config,
        .set_config_parameter_count = 1,
        .set_config_help = "cpu_freq",
        .menu = NULL
    },
    {
        .name = "ina226",
        .short_name = "in",
        .initializer = ina226_initializer,
        .data_collector = ina226_data_collector,
        .ui_init_handler = NULL,
        .ui_data_handler = ina226_ui_handler,
        .ui_keyboard_handler = NULL,
        .print_config = NULL,
        .set_config = NULL,
        .set_config_parameter_count = 0,
        .set_config_help = NULL,
        .menu = NULL
    },
    {
        .name = "mcp3421",
        .short_name = "mc",
        .initializer = mcp3421_initializer,
        .data_collector = mcp3421_data_collector,
        .ui_init_handler = NULL,
        .ui_data_handler = mcp3421_ui_handler,
        .ui_keyboard_handler = NULL,
        .print_config = msp3421_print_config,
        .set_config = msp3421_set_config,
        .set_config_parameter_count = 2,
        .set_config_help = "coef offset",
        .menu = NULL
    },
    {
        .name = "si5351",
        .short_name = "si",
        .initializer = si5351_initializer,
        .data_collector = NULL,
        .ui_init_handler = si5351_ui_init_handler,
        .ui_data_handler = NULL,
        .ui_keyboard_handler = si5351_ui_keyboard_handler,
        .print_config = NULL,
        .set_config = NULL,
        .set_config_parameter_count = 0,
        .set_config_help = NULL,
        .menu = dds_menu
    }
};
