#ifndef _FORTH_CORE_H
#define _FORTH_CORE_H

#include <board.h>

#define FVM_FLOAT 2
#define FVM_INTEGER 1
#define FVM_MODE_INTERPRET 0
#define FVM_MODE_COMPILE 1
#define FVM_TRUE 1
#define FVM_FALSE 0
#define FVM_COMPILE_TIME_WORD 2
#define FVM_ERROR 3
#define FVM_END 4

struct _forth_word;
struct _fvm_data;

typedef int (*fvm_word_handler)(struct _fvm_data *fvm);
typedef void (*fvm_put_string_handler)(const char *, unsigned int);

typedef struct _forth_word {
  const struct _forth_word *pred;
  union
  {
    struct {
      const char *name;
      fvm_word_handler handler, chandler;
    } non_dict;
    struct {
      short name_length;
      short word_length;
      char name;
    } dict;
  } props;
} forth_word;

typedef union {
  fvm_word_handler wh;
#ifdef FVM_FP_ENABLE
  float f;
#endif
  int i;
} float_h;

typedef struct _fvm_data {
  int fvm_data_stack[FVM_DATA_STACK_LENGTH];
  int *fvm_data_stack_pointer;
  fvm_word_handler *fvm_current_word;
} fvm_data;

extern char *fvm_command_buffer_current;

extern forth_word *fvm_new_word;
extern char fvm_word_name[];
extern fvm_word_handler *fvm_word_code;

extern const forth_word *fvm_last_word;
extern const forth_word *fvm_last_word_initial;

extern int fvm_mode;

extern fvm_put_string_handler fvm_put_string_proc;

extern char fvm_dictionary[];

int lit_word_handler(fvm_data *fvm);

extern int fvm_feedback_enabled;

void fvm_init(fvm_data *fvm, fvm_put_string_handler, const forth_word *);
void fvm_nputs(const char*, int);
void fvm_puts(const char*);
void fvm_process_string(fvm_data*, char*);
int fvm_data_stack_underflow(fvm_data *fvm);
int fvm_data_stack_underflow2(fvm_data *fvm, int);
int fvm_get_next_word(char*, int, int);
void fvm_reinit(fvm_data *fvm);
int fvm_word_within_dictionary(const forth_word *w);
int fvm_call_handler(fvm_data *fvm);
void fvm_init_data(fvm_data *fvm);
int exec_forth_word_pipelined(fvm_data *fvm, fvm_word_handler *fw, int _continue);

#endif
