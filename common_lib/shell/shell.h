#ifndef FLASH_FS_SHELL_SHELL_H
#define FLASH_FS_SHELL_SHELL_H

#define SHELL_UP_KEY   0x1F
#define SHELL_DOWN_KEY 0x1E

typedef struct {
  unsigned char offset, length;
} ShellCommandParameter;

typedef int (*printf_func)(const char *, ...);
typedef char * (*gets_func)(void);

typedef struct _ShellCommandItem {
  const char *text;
  int (*handler)(const char *command, ShellCommandParameter **cp, const struct _ShellCommandItem **item);
  int (*final_handler)(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
} ShellCommandItem;

typedef struct {
  const ShellCommandItem *items;
  const char *name, *help;
  void (*init)(void);
  void *data;
} ShellCommand;

int param_handler(const char *command, ShellCommandParameter **cp, const struct _ShellCommandItem **item);

void shell_init(printf_func pfunc, gets_func _gfunc);
int shell_register_command(const ShellCommand *command);
int shell_execute(const char *command);
const char *shell_get_prev_from_history(void);
const char *shell_get_next_from_history(void);

#endif //FLASH_FS_SHELL_SHELL_H
