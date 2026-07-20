#include "board.h"
#include "security_commands.h"
#include <shell.h>
#include <trng.h>
#include <stdlib.h>

#define MAX_WORDS 32

static unsigned int words[MAX_WORDS];

static int trng_generate_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem trng_generate_command_items[] = {
  {nullptr, param_handler, nullptr},
  {nullptr, nullptr, trng_generate_handler},
  {nullptr, nullptr, nullptr}
};
static const ShellCommand trng_generate_command = {
  trng_generate_command_items,
  "trng_generate",
  "trng_generate num_words",
  nullptr,
  nullptr
};

static int trng_generate_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int num_words = atoi(argv[0]);
  if (num_words <= 0 || num_words > MAX_WORDS)
    return 1;
  trng_generate(words, num_words);
  for (int i = 0; i < num_words; i++)
    pfunc("0x%08X ", words[i]);
  pfunc("\n");
  return 0;
}

void register_security_commands(void)
{
  shell_register_command(&trng_generate_command);
}
