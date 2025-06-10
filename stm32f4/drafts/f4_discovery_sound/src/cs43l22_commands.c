#include "cs43l22_commands.h"
#include "board.h"
#include "cs43l22.h"
#include <shell.h>
#include <stdlib.h>
#include <string.h>

static int param2_handler(const char *command, ShellCommandParameter **cp, const struct _ShellCommandItem **item);

static int cs43l22_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static int cs43l22_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static int cs43l22_handler3(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem cs43l22_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param2_handler, cs43l22_handler1},
    {NULL, param_handler, cs43l22_handler2},
  {NULL, NULL, cs43l22_handler3}
};
static const ShellCommand cs43l22_command = {
  cs43l22_command_items,
  "cs43l22",
  "cs43l22 [get_chip_id]\r\n[power_down][power_up]\r\n[passthrough line_no]\r\n"
       "[volume passthrough|pcm|headphone|speaker|master value]\r\n[beep off|single|multiple|continuous]"
};

static int param2_handler(const char *command, ShellCommandParameter **cp, const struct _ShellCommandItem **item)
{
  ShellCommandParameter *p = *cp;
  p--;
  if (p->length == 11 && !strncmp(command + p->offset, "passthrough", 11))
    return param_handler(command, cp, item);
  if (p->length == 6 && !strncmp(command + p->offset, "volume", 6))
    return param_handler(command, cp, item);
  if (p->length == 4 && !strncmp(command + p->offset, "beep", 4))
    return param_handler(command, cp, item);
  return 1;
}

static int cs43l22_handler3(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[2]);
  if (!strcmp(argv[0], "volume"))
  {
    int rc;
    if (!strcmp(argv[1], "passthrough"))
    {
      rc = cs43l22_set_passthrough_volume(0, value);
      if (rc)
        return rc;
      return cs43l22_set_passthrough_volume(1, value);
    }
    if (!strcmp(argv[1], "pcm"))
    {
      if (!strcmp(argv[2], "off"))
      {
        rc = cs43l22_set_pcm_volume(0, 0, 1);
        if (rc)
          return rc;
        return cs43l22_set_pcm_volume(1, 0, 1);
      }
      rc = cs43l22_set_pcm_volume(0, value, 0);
      if (rc)
        return rc;
      return cs43l22_set_pcm_volume(1, value, 0);
    }
    if (!strcmp(argv[1], "headphone"))
    {
      rc = cs43l22_set_headphone_volume(0, value);
      if (rc)
        return rc;
      return cs43l22_set_headphone_volume(1, value);
    }
    if (!strcmp(argv[1], "speaker"))
    {
      rc = cs43l22_set_speaker_volume(0, value);
      if (rc)
        return rc;
      return cs43l22_set_speaker_volume(1, value);
    }
    if (!strcmp(argv[1], "master"))
    {
      rc = cs43l22_set_master_volume(0, value);
      if (rc)
        return rc;
      return cs43l22_set_master_volume(1, value);
    }
  }

  pfunc("Invalid command parameter\n");
  return 1;
}

static int cs43l22_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  if (!strcmp(argv[0], "passthrough"))
  {
    unsigned char line_no;
    int line_num = atoi(argv[1]);
    switch (line_num)
    {
      case 0:
        line_no = CS43L22_PASSTHROUGH_NONE;
        break;
      case 1:
        line_no = CS43L22_PASSTHROUGH_AIN1;
        break;
      case 2:
        line_no = CS43L22_PASSTHROUGH_AIN2;
        break;
      case 3:
        line_no = CS43L22_PASSTHROUGH_AIN3;
        break;
      case 4:
        line_no = CS43L22_PASSTHROUGH_AIN4;
        break;
      default:
        pfunc("Invalid line number\n");
        return 1;
    }
    return cs43l22_passthrough_select(line_no, 0);
  }
  if (!strcmp(argv[0], "beep"))
  {
    CS43L22_BeepToneControl control;
    control.beep_mix_disable = 0;
    control.bass_corner_frequency = CS43L22_BASS_CORNER_FREQUENCY_50HZ;
    control.treble_corner_frequency = CS43L22_TREBLE_CORNER_FREQUENCY_5KHZ;
    control.tone_control_enable = 0;
    if (!strcmp(argv[1], "off"))
      control.beep_configuration = CS43L22_BEEP_OFF;
    else if (!strcmp(argv[1], "single"))
      control.beep_configuration = CS43L22_BEEP_SINGLE;
    else if (!strcmp(argv[1], "multiple"))
      control.beep_configuration = CS43L22_BEEP_MULTIPLE;
    else if (!strcmp(argv[1], "continuous"))
      control.beep_configuration = CS43L22_BEEP_CONTINUOUS;
    else
    {
      pfunc("Invalid beep type\n");
      return 1;
    }
    return cs43l22_beep_and_tone_control(&control);
  }

  pfunc("Invalid command parameter\n");
  return 1;
}

static int cs43l22_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int rc;

  if (!strcmp(argv[0], "get_chip_id"))
  {
    unsigned char id;
    rc = cs43l22_read_id(&id);
    if (rc)
    {
      pfunc("I2C error %d\n", rc);
      return 1;
    }

    pfunc("Chip id = %02X\n", id);
    return 0;
  }
  if (!strcmp(argv[0], "power_down"))
    return cs43l22_power_control(1);
  if (!strcmp(argv[0], "power_up"))
    return cs43l22_power_control(0);

  pfunc("Invalid command parameter\n");
  return 1;
}

void register_cs43l22_commands(void)
{
  shell_register_command(&cs43l22_command);
}
