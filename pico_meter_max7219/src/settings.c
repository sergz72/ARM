#include "settings.h"
#include "board.h"

const char* setting_names[] = {
  "cap_value_mul",
  "cap_max",
  "cap_offset",
  "mclk",
  "u0mul",
  "u1mul",
  "u2mul",
  "u0offset",
  "u1offset",
  "u2offset"
};

long long int settings[SETTINGS_COUNT];

void read_settings(void)
{
  long long int v;
  int rc = hal_read_settings();
  v = settings[SETTING_MCLK];
  if (rc || (v < 124000000 || v > 126000000))
  {
    settings[SETTING_CAP_VALUE_MUL] = DEFAULT_CAP_VALUE_MUL;
    settings[SETTING_CAP_MAX] = DEFAULT_CAP_MAX;
    settings[SETTING_CAP_OFFSET] = DEFAULT_CAP_OFFSET;
    settings[SETTING_MCLK] = DEFAULT_MCLK;
    settings[SETTING_U0MUL] = DEFAULT_SETTING_UMUL;
    settings[SETTING_U1MUL] = DEFAULT_SETTING_UMUL;
    settings[SETTING_U2MUL] = DEFAULT_SETTING_UMUL;
    settings[SETTING_U0OFFSET] = DEFAULT_SETTING_UOFFSET;
    settings[SETTING_U1OFFSET] = DEFAULT_SETTING_UOFFSET;
    settings[SETTING_U2OFFSET] = DEFAULT_SETTING_UOFFSET;
  }
}
