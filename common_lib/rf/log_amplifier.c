#include <log_amplifier.h>

int log_amplifier_mv_to_db(int amplifier_type, int mv)
{
  switch (amplifier_type)
  {
    case LOG_AMPLIFIER_AD8307:
      return (mv - 2250) / 25;
    default:
      return 0;
  }
}

