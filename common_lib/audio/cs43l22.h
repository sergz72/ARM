#ifndef CS43L22_H
#define CS43L22_H

#define CS43L22_CHIP_ID 0xE0
#define CS43L22_CHIP_ID_MASK 0xF8
#define CS43L22_REVISION_MASK 7

#define CS43L22_POWER_CONTROL_ON_WHEN_PIN_6_LO 0
#define CS43L22_POWER_CONTROL_ON_WHEN_PIN_6_HI 1
#define CS43L22_POWER_CONTROL_ALWAYS_ON 2
#define CS43L22_POWER_CONTROL_ALWAYS_OFF 3

#define CS43L22_DOUBLE_SPEED_MODE  0
#define CS43L22_SINGLE_SPEED_MODE  0x20
#define CS43L22_HALF_SPEED_MODE    0x40
#define CS43L22_QUARTER_SPEED_MODE 0x60

#define CS43L22_MCLK_RATIO_128 0
#define CS43L22_MCLK_RATIO_125 2
#define CS43L22_MCLK_RATIO_132 4
#define CS43L22_MCLK_RATIO_136 6

#define CS43L22_DAC_INTERFACE_FORMAT_LEFT_JUSTIFIED  0
#define CS43L22_DAC_INTERFACE_FORMAT_I2S             4
#define CS43L22_DAC_INTERFACE_FORMAT_RIGHT_JUSTIFIED 8

#define CS43L22_AUDIO_WORD_LENGTH_DSP_32 0
#define CS43L22_AUDIO_WORD_LENGTH_DSP_24 1
#define CS43L22_AUDIO_WORD_LENGTH_DSP_20 2
#define CS43L22_AUDIO_WORD_LENGTH_DSP_16 3

#define CS43L22_AUDIO_WORD_LENGTH_RIGHT_JUSTIFIED_24 0
#define CS43L22_AUDIO_WORD_LENGTH_RIGHT_JUSTIFIED_20 1
#define CS43L22_AUDIO_WORD_LENGTH_RIGHT_JUSTIFIED_18 2
#define CS43L22_AUDIO_WORD_LENGTH_RIGHT_JUSTIFIED_16 3

#define CS43L22_PASSTHROUGH_NONE 0
#define CS43L22_PASSTHROUGH_AIN1 1
#define CS43L22_PASSTHROUGH_AIN2 2
#define CS43L22_PASSTHROUGH_AIN3 4
#define CS43L22_PASSTHROUGH_AIN4 8

#define CS43L22_CHANNEL_A 0
#define CS43L22_CHANNEL_B 1

#define CS43L22_HEADPHONE_GAIN_MIN 0
#define CS43L22_HEADPHONE_GAIN_MAX 7

#define CS43L22_BEEP_OFF        0
#define CS43L22_BEEP_SINGLE     0x40
#define CS43L22_BEEP_MULTIPLE   0x80
#define CS43L22_BEEP_CONTINUOUS 0xC0

#define CS43L22_TREBLE_CORNER_FREQUENCY_5KHZ  0
#define CS43L22_TREBLE_CORNER_FREQUENCY_7KHZ  8
#define CS43L22_TREBLE_CORNER_FREQUENCY_10KHZ 0x10
#define CS43L22_TREBLE_CORNER_FREQUENCY_15KHZ 0x18

#define CS43L22_BASS_CORNER_FREQUENCY_50HZ   0
#define CS43L22_BASS_CORNER_FREQUENCY_100HZ  2
#define CS43L22_BASS_CORNER_FREQUENCY_200HZ  4
#define CS43L22_BASS_CORNER_FREQUENCY_250HZ  6

#define CS43L22_PCM_CHANNEL_SWAP_OFF  0
#define CS43L22_PCM_CHANNEL_SWAP_MONO 1
#define CS43L22_PCM_CHANNEL_SWAP_ON   3

typedef struct
{
  int autodetect;
  int group_32k;
  int video_clock;
  int mclkdiv2;
  unsigned char speed;
  unsigned char ratio;
} CS43L22_ClockControl;

typedef struct
{
  int master_mode;
  int sclk_inverted;
  int dsp_enabled;
  int sclk_equals_mclk;
  int speaker_switch_invert;
  unsigned char dac_interface_format;
  unsigned char audio_word_length;
} CS43L22_InterfaceControl;

typedef struct
{
  int playback_volume_b_eq_a;
  int speaker_volume_b_eq_a;
  int pcm_inverted[2];
  int mute_inverted[2];
  int headphone_mute[2];
  int speaker_mute[2];
  int speaker_channel_swap;
  int speaker_mono;
  int speaker_mute_50_50;
  unsigned char headphone_gain;
} CS43L22_PlaybackControl;

typedef struct
{
  int passthrough_analog_enable[2];
  int passthrough_analog_mute[2];
  int freeze_registers;
  int deemphasis_enable;
  int digital_soft_ramp_enable;
  int digital_zero_cross_enable;
} CS43L22_MiscControl;

typedef struct
{
  int beep_mix_disable;
  int tone_control_enable;
  unsigned char beep_configuration;
  unsigned char treble_corner_frequency;
  unsigned char bass_corner_frequency;
} CS43L22_BeepToneControl;

int cs43l22_read(unsigned char register_number, unsigned char *value); //should be defined in hal.c
int cs43l22_write(unsigned char register_number, unsigned char value); //should be defined in hal.c

int cs43l22_read_id(unsigned char *id);
int cs43l22_power_control(int power_down);
int cs43l22_power_control2(unsigned char headphone_a_command, unsigned char headphone_b_command,
                            unsigned char speaker_a_command, unsigned char speaker_b_command);
int cs43l22_clock_control(const CS43L22_ClockControl *control);
int cs43l22_interface_control(const CS43L22_InterfaceControl *control);
int cs43l22_passthrough_select(unsigned char input_no, int gang_control);
int cs43l22_analog_soft_ramp_zero_cross(int soft_ramp_a, int soft_ramp_b, int zero_cross_a, int zero_cross_b);
int cs43l22_playback_control(const CS43L22_PlaybackControl *control);
int cs43l22_misc_control(const CS43L22_MiscControl *control);
int cs43l22_set_passthrough_volume(int channel, int dbx10);
int cs43l22_set_pcm_volume(int channel, int dbx10, int pcm_mute);
int cs43l22_set_beep_frequency_and_on_time(unsigned int frequency, unsigned int on_time);
int cs43l22_set_beep_volume_and_off_time(int volume_db, unsigned int off_time);
int cs43l22_beep_and_tone_control(const CS43L22_BeepToneControl *control);
int cs43l22_set_treble_bass_gain(int treble_gain_dbx10, int bass_gain_dbx10);
int cs43l22_set_master_volume(int channel, int dbx10);
int cs43l22_set_headphone_volume(int channel, int dbx10);
int cs43l22_set_speaker_volume(int channel, int dbx10);
int cs43l22_set_pcm_channel_swap(unsigned char swap_a, unsigned char swap_b);

#endif
