#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#define SETTING_CAP_VALUE_MUL 0
#define SETTING_CAP_MAX       1
#define SETTING_CAP_OFFSET    2
#define SETTING_MCLK          3
#define SETTING_U0MUL         4
#define SETTING_U1MUL         5
#define SETTING_U2MUL         6
#define SETTINGS_COUNT        7

#define SETTINGS_SIZE (sizeof(long long int) * SETTINGS_COUNT)

extern long long int settings[];
extern const char* setting_names[];

void read_settings(void);
int save_settings(void);
int hal_read_settings(void);

#endif
