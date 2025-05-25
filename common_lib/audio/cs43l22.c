#include "cs43l22.h"

int cs43l22_read_id(unsigned char *id)
{
  return cs43l22_read(1, id);
}

int cs43l22_power_control(int power_down)
{
  return cs43l22_write(2, power_down ? 0x01 : 0x9E);
}

int cs43l22_power_control2(unsigned char headphone_a_command, unsigned char headphone_b_command,
                            unsigned char speaker_a_command, unsigned char speaker_b_command)
{
  return cs43l22_write(4, speaker_a_command | (speaker_b_command << 2) | (headphone_a_command << 4) |
                        (headphone_b_command << 6));
}

int cs43l22_clock_control(const CS43L22_ClockControl *control)
{
  unsigned char value = control->speed | control->ratio;
  if (control->autodetect)
    value |= 0x80;
  if (control->group_32k)
    value |= 0x10;
  if (control->video_clock)
    value |= 8;
  if (control->mclkdiv2)
    value |= 1;
  return cs43l22_write(5, value);
}

int cs43l22_interface_control(const CS43L22_InterfaceControl *control)
{
  unsigned char value = control->dac_interface_format | control->audio_word_length;
  if (control->master_mode)
    value |= 0x80;
  if (control->sclk_inverted)
    value |= 0x40;
  if (control->dsp_enabled)
    value |= 0x10;
  int rc = cs43l22_write(6, value);
  if (rc)
    return rc;
  value = control->sclk_equals_mclk ? 0x40 : 0;
  if (control->speaker_switch_invert)
    value |= 8;
  return cs43l22_write(7, value);
}

int cs43l22_passthrough_select(unsigned char input_no, int gang_control)
{
  int rc = cs43l22_write(8, input_no);
  if (rc)
    return rc;
  rc = cs43l22_write(9, input_no);
  if (rc)
    return rc;
  return cs43l22_write(12, gang_control ? 0x80 : 0);
}

int cs43l22_analog_soft_ramp_zero_cross(int soft_ramp_a, int soft_ramp_b, int zero_cross_a, int zero_cross_b)
{
  unsigned char value = zero_cross_a ? 1 : 0;
  if (zero_cross_b)
    value |= 2;
  if (soft_ramp_a)
    value |= 4;
  if (soft_ramp_b)
    value |= 8;
  return cs43l22_write(10, value);
}

int cs43l22_playback_control(const CS43L22_PlaybackControl *control)
{
  unsigned char value = control->headphone_gain << 5;
  if (control->playback_volume_b_eq_a)
    value |= 0x10;
  if (control->pcm_inverted[1])
    value |= 8;
  if (control->pcm_inverted[0])
    value |= 4;
  if (control->mute_inverted[1])
    value |= 2;
  if (control->mute_inverted[0])
    value |= 1;
  int rc = cs43l22_write(13, value);
  if (rc)
    return rc;
  value = control->headphone_mute[1] ? 0x80 : 0;
  if (control->headphone_mute[0])
    value |= 0x40;
  if (control->speaker_mute[1])
    value |= 0x20;
  if (control->speaker_mute[0])
    value |= 0x10;
  if (control->speaker_volume_b_eq_a)
    value |= 8;
  if (control->speaker_channel_swap)
    value |= 4;
  if (control->speaker_mono)
    value |= 2;
  if (control->speaker_mute_50_50)
    value |= 1;
  return cs43l22_write(15, value);
}

int cs43l22_misc_control(const CS43L22_MiscControl *control)
{
  unsigned char value = control->passthrough_analog_enable[1] ? 0x80 : 0;
  if (control->passthrough_analog_enable[0])
    value |= 0x40;
  if (control->passthrough_analog_mute[1])
    value |= 0x20;
  if (control->passthrough_analog_mute[0])
    value |= 0x10;
  if (control->freeze_registers)
    value |= 8;
  if (control->deemphasis_enable)
    value |= 4;
  if (control->digital_soft_ramp_enable)
    value |= 2;
  if (control->digital_zero_cross_enable)
    value |= 1;
  return cs43l22_write(14, value);
}

int cs43l22_set_passthrough_volume(int channel, int dbx10)
{
  return cs43l22_write(channel ? 0x15 : 0x14, (unsigned char)(dbx10 / 5));
}

int cs43l22_set_pcm_volume(int channel, int dbx10, int pcm_mute)
{
  dbx10 /= 5;
  if (dbx10 > 24)
    dbx10 = 24;
  else if (dbx10 < -103)
    dbx10 = -103;
  unsigned char value = dbx10;
  if (pcm_mute)
    value |= 0x80;
  return cs43l22_write(channel ? 0x1B : 0x1A, value);
}

int cs43l22_set_beep_frequency_and_on_time(unsigned int frequency, unsigned int on_time)
{
  unsigned char value;
  if (frequency < 261)
    value = 0;
  else if (frequency < 522)
    value = 0x10;
  else if (frequency < 586)
    value = 0x20;
  else if (frequency < 667)
    value = 0x30;
  else if (frequency < 706)
    value = 0x40;
  else if (frequency < 775)
    value = 0x50;
  else if (frequency < 889)
    value = 0x60;
  else if (frequency <= 1000)
    value = 0x70;
  else if (frequency < 1044)
    value = 0x80;
  else if (frequency <= 1200)
    value = 0x90;
  else if (frequency < 1334)
    value = 0xA0;
  else if (frequency < 1412)
    value = 0xB0;
  else if (frequency <= 1600)
    value = 0xC0;
  else if (frequency < 1715)
    value = 0xD0;
  else if (frequency <= 2000)
    value = 0xE0;
  else
    value = 0xF0;
  if (on_time > 86)
  {
    if (on_time <= 430)
      value |= 1;
    else if (on_time <= 780)
      value |= 2;
    else if (on_time <= 1200)
      value |= 3;
    else if (on_time <= 1500)
      value |= 4;
    else if (on_time <= 1800)
      value |= 5;
    else if (on_time <= 2200)
      value |= 6;
    else if (on_time <= 2500)
      value |= 7;
    else if (on_time <= 2800)
      value |= 8;
    else if (on_time <= 3200)
      value |= 9;
    else if (on_time <= 3500)
      value |= 10;
    else if (on_time <= 3800)
      value |= 11;
    else if (on_time <= 4200)
      value |= 12;
    else if (on_time <= 4500)
      value |= 13;
    else if (on_time <= 4800)
      value |= 14;
    else
      value |= 15;
  }
  return cs43l22_write(0x1C, value);
}

int cs43l22_set_beep_volume_and_off_time(int volume_db, unsigned int off_time)
{
  unsigned char value;
  if (off_time <= 1230)
    value = 0;
  else if (off_time <= 2580)
    value = 0x20;
  else if (off_time <= 3900)
    value = 0x40;
  else if (off_time <= 5200)
    value = 0x60;
  else if (off_time <= 6600)
    value = 0x80;
  else if (off_time <= 8050)
    value = 0xA0;
  else if (off_time <= 9350)
    value = 0xC0;
  else
    value = 0xE0;
  if (volume_db > 6)
    volume_db = 6;
  else if (volume_db < -56)
    volume_db = -56;
  volume_db /= 2;
  volume_db += 6;
  value |= (unsigned char)volume_db;
  return cs43l22_write(0x1C, value);
}

int cs43l22_beep_and_tone_control(const CS43L22_BeepToneControl *control)
{
  unsigned char value = (control->beep_configuration << 6) | (control->treble_corner_frequency << 3) |
    (control->bass_corner_frequency << 1);
  if (control->beep_mix_disable)
    value |= 0x20;
  if (control->tone_control_enable)
    value |= 1;
  return cs43l22_write(0x1E, value);
}

static int transform_gain(int gain_dbx10)
{
  if (gain_dbx10 > 120)
    gain_dbx10 = 120;
  else if (gain_dbx10 < -105)
    gain_dbx10 = -105;
  return (120 - gain_dbx10) / 15;
}

int cs43l22_set_treble_bass_gain(int treble_gain_dbx10, int bass_gain_dbx10)
{
  int g1 = transform_gain(treble_gain_dbx10);
  int g2 = transform_gain(bass_gain_dbx10);
  unsigned char value = (unsigned char)(g1 << 4) | (unsigned char)g2;
  return cs43l22_write(0x1E, value);
}

int cs43l22_set_master_volume(int channel, int dbx10)
{
  if (dbx10 > 120)
    dbx10 = 120;
  else if (dbx10 < -1020)
    dbx10 = -1020;
  dbx10 /= 5;
  return cs43l22_write(channel ? 0x21: 0x20, (unsigned char)dbx10);
}

static int set_volume(unsigned char reg_no, int dbx10)
{
  if (dbx10 > 0)
    dbx10 = 0;
  else if (dbx10 < -960)
    dbx10 = -960;
  dbx10 /= 5;
  return cs43l22_write(reg_no, (unsigned char)dbx10);
}

int cs43l22_set_headphone_volume(int channel, int dbx10)
{
  return set_volume(channel ? 0x23 : 0x22, dbx10);
}

int cs43l22_set_speaker_volume(int channel, int dbx10)
{
  return set_volume(channel ? 0x25 : 0x24, dbx10);
}

int cs43l22_set_pcm_channel_swap(unsigned char swap_a, unsigned char swap_b)
{
  return cs43l22_write(0x26, (swap_a << 6) | (swap_b << 4));
}
