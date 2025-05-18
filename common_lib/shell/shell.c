#include "board.h"
#include <shell.h>
#include <stdio.h>
#include <string.h>
#ifdef SHELL_HISTORY_SIZE
#include <fixed_queue.h>
#endif

static ShellCommand *commands[MAX_SHELL_COMMANDS];
static int registered_commands, argc;
static char argvv[MAX_SHELL_COMMAND_PARAMETERS][MAX_SHELL_COMMAND_PARAMETER_LENGTH+1], *argv[MAX_SHELL_COMMAND_PARAMETERS];
static printf_func pfunc;
static gets_func gfunc;
#ifdef SHELL_HISTORY_SIZE
static char history[SHELL_HISTORY_ITEM_LENGTH*SHELL_HISTORY_SIZE];
static Queue history_q;
static int history_offset;
#endif

int param_handler(const char *command, ShellCommandParameter **cp, const struct _ShellCommandItem **item)
{
  strncpy(argvv[argc], command + (*cp)->offset, (*cp)->length);
  argvv[argc++][(*cp)->length] = 0;
  (*cp)++;
  (*item)++;
  return 0;
}

void shell_init(printf_func _pfunc, gets_func _gfunc)
{
  int i;
  pfunc = _pfunc;
  gfunc = _gfunc;
  registered_commands = 0;
  for (i = 0; i < MAX_SHELL_COMMAND_PARAMETERS; i++)
    argv[i] = argvv[i];

#ifdef SHELL_HISTORY_SIZE
  queue_init(&history_q, SHELL_HISTORY_SIZE, SHELL_HISTORY_ITEM_LENGTH, history);
  history_offset = 0;
#endif
}

int shell_register_command(const ShellCommand *command)
{
  if (registered_commands >= MAX_SHELL_COMMANDS)
    return 1;

  commands[registered_commands++] = (ShellCommand*)command;

  return 0;
}

static int eq_handler(const char *command, const char *text, ShellCommandParameter *cp)
{
  if (strlen(text) != cp->length)
    return 1;
  return strncmp(command + cp->offset, text, cp->length);
}

#define MODE_PARAMETER 1
#define MODE_STRING 2
#define MODE_SPACE 0

static int buildCommandParameters(const char *command, ShellCommandParameter *command_parameters)
{
  int offset, length, mode, parameter_count;
  offset = length = mode = parameter_count = 0;
  while (*command)
  {
    switch (mode)
    {
      case MODE_SPACE:
        if (*command == '"')
        {
          if (parameter_count >= MAX_SHELL_COMMAND_PARAMETERS)
            return -1;
          mode = MODE_STRING;
        }
        else if (*command > ' ')
        {
          if (parameter_count >= MAX_SHELL_COMMAND_PARAMETERS)
            return -1;
          mode = MODE_PARAMETER;
          break;
        }
        command++;
        offset++;
        break;
      case MODE_PARAMETER:
        if (*command <= ' ')
        {
          if (length > MAX_SHELL_COMMAND_PARAMETER_LENGTH)
            return -1;
          command_parameters->offset = offset;
          command_parameters->length = length;
          command_parameters++;
          parameter_count++;
          offset += length;
          length = mode = 0;
          break;
        }
        length++;
        command++;
        break;
      default: // MODE_STRING
        if (*command == '"')
        {
          if (length > MAX_SHELL_COMMAND_PARAMETER_LENGTH)
            return -1;
          command_parameters->offset = offset;
          command_parameters->length = length;
          command_parameters++;
          parameter_count++;
          command++;
          offset += length;
          length = mode = 0;
          break;
        }
        length++;
        command++;
        break;
    }
  }
  switch (mode)
  {
    case MODE_SPACE:
      return parameter_count;
    case MODE_PARAMETER:
      if (length > MAX_SHELL_COMMAND_PARAMETER_LENGTH)
        return -1;
      command_parameters->offset = offset;
      command_parameters->length = length;
      return parameter_count + 1;
    default:
      return -1;
  }
}

static const char * get_help_text(ShellCommand *current_command)
{
  if (current_command->help)
    return current_command->help;
  return current_command->name;
}

int shell_execute(const char *command)
{
  int i, parameter_count;
  ShellCommand **current_command;
  const ShellCommandItem *item;
  ShellCommandParameter command_parameters[MAX_SHELL_COMMAND_PARAMETERS], *current_parameter, *last_parameter;

  if (!command)
    return 0;

  parameter_count = buildCommandParameters(command, command_parameters);
  if (!parameter_count)
    return 0;
  if (parameter_count < 0)
    return parameter_count;

  /*printf("%d\n", parameter_count);
  for (i = 0; i < parameter_count; i++)
  {
    printf("%d %d\n", command_parameters[i].offset, command_parameters[i].length);
  }*/

#ifdef SHELL_HISTORY_SIZE
  queue_push(&history_q, (void*)command);
  history_offset = 0;
#endif

  current_command = commands;

  if (!eq_handler(command, "help", command_parameters))
  {
    pfunc("usage:");
    for (i = 0; i < registered_commands; i++)
    {
      pfunc("  %s\n", get_help_text(*current_command));
      current_command++;
    }
    return 0;
  }
  last_parameter = command_parameters + parameter_count;
  for (i = 0; i < registered_commands; i++)
  {
    current_parameter = command_parameters;
    if (!eq_handler(command, (*current_command)->name, current_parameter))
    {
      if ((*current_command)->init)
        (*current_command)->init();
      current_parameter++;
      argc = 0;
      item = (*current_command)->items;
      while (current_parameter < last_parameter)
      {
        if (item->text)
        {
          if (eq_handler(command, item->text, current_parameter))
          {
            if (item->final_handler)
              return item->final_handler(pfunc, gfunc, argc, argv, (*current_command)->data);
            break;
          }
        } else if (item->handler)
        {
          if (item->handler(command, &current_parameter, &item))
          {
            if (item->final_handler)
              return item->final_handler(pfunc, gfunc, argc, argv, (*current_command)->data);
            break;
          }
          continue;
        } else
          break;
        item++;
        current_parameter++;
      }
      if (item->text || !item->final_handler || current_parameter < last_parameter)
      {
        printf("usage: %s\n", get_help_text(*current_command));
        return 1;
      }
      return item->final_handler(pfunc, gfunc, argc, argv, (*current_command)->data);
    }
    current_command++;
  }

  puts("unknown command");

  return 1;
}

const char *shell_get_prev_from_history(void)
{
#ifdef SHELL_HISTORY_SIZE
  int s = queue_size(&history_q);
  if (history_offset < s)
    return queue_peekn(&history_q, history_offset++);
  return NULL;
#else
  return NULL;
#endif
}

const char *shell_get_next_from_history(void)
{
#ifdef SHELL_HISTORY_SIZE
  if (history_offset)
  {
    history_offset--;
    return queue_peekn(&history_q, history_offset);
  }
  return NULL;
#else
  return NULL;
#endif
}
