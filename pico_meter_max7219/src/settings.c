#include "settings.h"
#include "board.h"

const char* setting_names[] = {
  "cap_value_mul",
  "cap_max",
  "cap_offset",
  "mclk",
  "u0mul",
  "u1mul",
  "u2mul"
};

long long int settings[SETTINGS_COUNT];

void read_settings(void)
{
  if (hal_read_settings())
  {
    settings[SETTING_CAP_VALUE_MUL] = DEFAULT_CAP_VALUE_MUL;
    settings[SETTING_CAP_MAX] = DEFAULT_CAP_MAX;
    settings[SETTING_CAP_OFFSET] = DEFAULT_CAP_OFFSET;
    settings[SETTING_MCLK] = DEFAULT_MCLK;
    settings[SETTING_U0MUL] = DEFAULT_SETTING_U0MUL;
    settings[SETTING_U1MUL] = DEFAULT_SETTING_U1MUL;
    settings[SETTING_U2MUL] = DEFAULT_SETTING_U2MUL;
  }
}
