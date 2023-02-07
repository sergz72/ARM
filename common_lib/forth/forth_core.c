#include <math.h>
#include <string.h>
#include <forth_core.h>

char *fvm_command_buffer_current;

char fvm_dictionary[FVM_DICTIONARY_LENGTH];

forth_word *fvm_new_word;
char fvm_word_name[FVM_WORD_NAME_LENGTH];
fvm_word_handler *fvm_word_code;

const forth_word *fvm_last_word, *fvm_last_word_initial;
int fvm_mode;
fvm_put_string_handler fvm_put_string_proc;

extern int fvm_do_index;

void fvm_init_data(fvm_data *fvm)
{
  fvm->fvm_data_stack_pointer = &fvm->fvm_data_stack[FVM_DATA_STACK_LENGTH];
}

void fvm_reinit(fvm_data *fvm)
{
  fvm_init_data(fvm);
  fvm_last_word = fvm_last_word_initial;
  fvm_mode = FVM_MODE_INTERPRET;
  fvm_feedback_enabled = FVM_TRUE;
}

void fvm_init(fvm_data *fvm, fvm_put_string_handler put_string_func, const forth_word *_last_word)
{
  fvm_last_word_initial = _last_word;
  fvm_put_string_proc = put_string_func;
  fvm_reinit(fvm);
}

void fvm_nputs(const char *c, int l)
{
  int len = 0;
  while (*c && l--)
    len++;
  fvm_put_string_proc(c, len);
}

void fvm_puts(const char *c)
{
  fvm_put_string_proc(c, strlen(c));
}

int fvm_word_within_dictionary(const forth_word *w)
{
  return (char*)w >= fvm_dictionary && (char*)w < fvm_dictionary + FVM_DICTIONARY_LENGTH;
}

fvm_word_handler *fvm_get_code(const forth_word *fw)
{
  char *p = (char*)&fw->props.dict.name;
  p += fw->props.dict.name_length;
  return (fvm_word_handler*)p;
}

int fvm_call_handler(fvm_data *fvm)
{
  fvm_word_handler *h = fvm->fvm_current_word + 1;

  fvm->fvm_current_word = *((fvm_word_handler**)h);

  while (fvm->fvm_current_word)
  {
    if (!(*fvm->fvm_current_word)(fvm))
      return FVM_FALSE;
    fvm->fvm_current_word++;
  }

  fvm->fvm_current_word = h;

  return FVM_TRUE;
}

int exec_forth_word(fvm_data *fvm, const forth_word *fw, int within)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    if (!within && fw->props.non_dict.chandler)
      return fw->props.non_dict.chandler(fvm);
    if (within)
    {
      *fvm_word_code++ = fvm_call_handler;
      *fvm_word_code++ = (fvm_word_handler)fvm_get_code(fw); 
    }
    else
      *fvm_word_code++ = fw->props.non_dict.handler; 
    return FVM_TRUE;
  }

  if (!within && fw->props.non_dict.chandler)
  {
    switch (fw->props.non_dict.chandler(fvm))
    {
      case FVM_COMPILE_TIME_WORD:
        fvm_puts("Compile time word: ");
        fvm_puts(fw->props.non_dict.name);
        fvm_puts("\r\n");
        return FVM_FALSE;
      case FVM_ERROR:
        return FVM_FALSE;
      case FVM_FALSE:
        return FVM_TRUE;
    }
  }

  if (within)
  {
    fvm->fvm_current_word = fvm_get_code(fw);
    while (fvm->fvm_current_word)
    {
      if (!(*fvm->fvm_current_word)(fvm))
        return FVM_FALSE;
      fvm->fvm_current_word++;
    }
    return FVM_TRUE;
  }
  else
    return fw->props.non_dict.handler(fvm);
}

int exec_forth_word_pipelined(fvm_data *fvm, fvm_word_handler *fw, int _continue)
{
  if (!_continue)
    fvm->fvm_current_word = fw;
  if (!fvm->fvm_current_word)
    return FVM_END;
  if (!(*fvm->fvm_current_word)(fvm))
    return FVM_FALSE;
  fvm->fvm_current_word++;
  return FVM_TRUE;
}

#ifdef FVM_FP_ENABLE
int try_interpret_number(char *p1, int *number, float *fnumber);

int processE(char *p1, float *fnumber)
{
  int n;
  float f;
  if (try_interpret_number(p1, &n, &f) != FVM_INTEGER)
    return FVM_FALSE;
  *fnumber *= pow(10.0, n);
  return FVM_TRUE;
}

int try_interpret_number(char *p1, int *number, float *fnumber)
{
  int negate = 0, real = 0;
  int base, code;
  int result = 0;
  float fresult = 0, f10 = 0.1;
  char c;

  if (p1 >= fvm_command_buffer_current)
    return FVM_FALSE;

  c = *p1++;

  switch (c)
  {
    case '-':
      negate = 1;
      base = 10;
      break;
    case '0':
      base = 8;
      break;
    case '.':
      base = 10;
      real = 1; // floating point
      break;
    default:
      if (c < '1' || c > '9')
        return FVM_FALSE;
      base = 10;
      result = c - '0';
      break;
  }

  if (p1 == fvm_command_buffer_current)
    if (base != 8 && !result)
      return FVM_FALSE;
    else
    {
      *number = negate ? -(int)result : (int)result;
      return FVM_INTEGER;
    }

  c = *p1++;

  if (base == 8 && (c == 'x' || c == 'X'))
    base = 16;
  else if (base == 8 && (c == 'b' || c == 'B'))
    base = 2;
  else if ((base == 10 || base == 8) && c == '.' && !real)
  {
    base = 10;
    real = 1;
    fresult = result;
  }
  else if (base == 10 && (c == 'E' || c == 'e') && !real && !negate)
  {
    *fnumber = result;
    if (!processE(p1, fnumber))
      return FVM_FALSE;
    return FVM_FLOAT;
  }
  else
  {
    if (c < '0' || c >= '0' + base)
      return FVM_FALSE;
    else
    {
      if (real)
      {
        fresult = f10 * (c - '0');
        f10 /= 10;
      }
      else
        result = result * base + c - '0';
    }
  }

  while (p1 < fvm_command_buffer_current)
  {
    c = *p1++;

    if (base == 10 && c == '.' && !real)
    {
      real = 1;
      fresult = result;
    }
    else if (base == 10 && (c == 'E' || c == 'e'))
    {
      if (!real)
        *fnumber = negate ? -result : result;
      else
        *fnumber = negate ? -fresult : fresult;
      if (!processE(p1, fnumber))
        return FVM_FALSE;
      return FVM_FLOAT;
    }
    else if (base > 10 && ((c >= 'a' && c <= 'z') || (c >= 'A' || c <= 'Z')))
    {
      if (c >= 'a')
        code = c + 10 - 'a';
      else
        code = c + 10 - 'A';
      if (code >= base)
        return FVM_FALSE;
      else
        result = result * base + code;
    }
    else
    {
      if (c < '0' || c >= '0' + (base > 10 ? 10 : base))
        return FVM_FALSE;
      else
      {
        if (!real)
          result = result * base + c - '0';
        else
        {
          fresult += f10 * (c - '0');
          f10 /= 10;
        }
      }
    }
  }

  if (real)
  {
    *fnumber = negate ? -fresult : fresult;
    return FVM_FLOAT;
  }

  *number = negate ? -(int)result : (int)result;

  return FVM_INTEGER;
}
#else
int try_interpret_number(char *p1, int *number)
{
  int negate = 0;
  int base, code;
  int result = 0;
  char c;

  if (p1 >= fvm_command_buffer_current)
    return FVM_FALSE;

  c = *p1++;

  switch (c)
  {
  case '-':
    negate = 1;
    base = 10;
    break;
  case '0':
    base = 8;
    break;
  default:
    if (c < '1' || c > '9')
      return FVM_FALSE;
    base = 10;
    result = c - '0';
    break;
  }

  if (p1 == fvm_command_buffer_current)
  if (base != 8 && !result)
    return FVM_FALSE;
  else
  {
    *number = negate ? -(int) result : (int) result;
    return FVM_INTEGER;
  }

  c = *p1++;

  if (base == 8 && (c == 'x' || c == 'X'))
    base = 16;
  else if (base == 8 && (c == 'b' || c == 'B'))
    base = 2;
  else
  {
    if (c < '0' || c >= '0' + base)
      return FVM_FALSE;
    else
      result = result * base + c - '0';
  }

  while (p1 < fvm_command_buffer_current)
  {
    c = *p1++;

    if (base > 10 && (c >= 'a' || c >= 'A'))
    {
      if (c >= 'a')
        code = c + 10 - 'a';
      else
        code = c + 10 - 'A';
      if (code >= base)
        return FVM_FALSE;
      else
        result = result * base + code;
    }
    else
    {
      if (c < '0' || c >= '0' + (base > 10 ? 10 : base))
        return FVM_FALSE;
      else
        result = result * base + c - '0';
    }
  }

  *number = negate ? -(int) result : (int) result;

  return FVM_INTEGER;
}
#endif

int try_interpret_char(char *p1, int *ch)
{
  if (p1 + 2 >= fvm_command_buffer_current || *p1 != '\'')
    return FVM_FALSE;

  p1++;

  if (*p1 == '\\')
  {
    p1++;
    switch (*p1++)
    {
      case 'r': *ch = '\r'; break;
      case 'n': *ch = '\n'; break;
      case 't': *ch = '\t'; break;
      case 'b': *ch = '\b'; break;
      default: return FVM_FALSE;
    }
  }
  else
    *ch = *p1++;

  if (*p1++ != '\'')
    return FVM_FALSE;
  if (p1 != fvm_command_buffer_current)
    return FVM_FALSE;

  return FVM_TRUE;
}

int exec_word(fvm_data *fvm, char *p1)
{
  const forth_word *w = fvm_last_word;
  const char *n;
  char c, *p;
  float_h fh;
  int rc;

  while (w)
  {
    fh.i = fvm_word_within_dictionary(w);
    if (fh.i)
      n = &w->props.dict.name;
    else
      n = w->props.non_dict.name;

    p = p1;
    while (p < fvm_command_buffer_current)
    {
      c = *n;
      if (!c)
        break;
      if (c != *p)
        break;
      n++;
      p++;
    }
    if (!(*n) && p == fvm_command_buffer_current)
      return exec_forth_word(fvm, w, fh.i);
    w = w->pred;
  }

#ifdef FVM_FP_ENABLE
  rc = try_interpret_number(p1, &fh.i, &fh.f);
#else
  rc = try_interpret_number(p1, &fh.i);
#endif
  if (rc)
  {
    if (fvm_mode == FVM_MODE_INTERPRET)
      *(--fvm->fvm_data_stack_pointer) = fh.i;
    else
    {
      *fvm_word_code++ = lit_word_handler;
      *fvm_word_code++ = fh.wh;
    }

    return FVM_TRUE;
  }

  rc = try_interpret_char(p1, &fh.i);
  if (rc)
  {
    if (fvm_mode == FVM_MODE_INTERPRET)
      *(--fvm->fvm_data_stack_pointer) = fh.i;
    else
    {
      *fvm_word_code++ = lit_word_handler;
      *fvm_word_code++ = fh.wh;
    }

    return FVM_TRUE;
  }

  fvm_puts("Word not found: ");
  fvm_nputs(p1, fvm_command_buffer_current - p1);
  fvm_puts("\r\n");

  fvm_mode = FVM_MODE_INTERPRET;

  return FVM_FALSE;
}

void fvm_process_string(fvm_data *fvm, char *p)
{
  char *w;
  int OK = FVM_TRUE;

  fvm_do_index = -2;

  fvm_command_buffer_current = p;
  w = NULL;
  while (*fvm_command_buffer_current)
  {
    if (*fvm_command_buffer_current == '/' && fvm_command_buffer_current[1] == '/' && !w)
    {
      fvm_command_buffer_current += 2;
      while (*fvm_command_buffer_current && *fvm_command_buffer_current != '\r')
        fvm_command_buffer_current++;
      continue;
    }
    else if (*fvm_command_buffer_current > 0x20)
    {
      if (!w)
        w = fvm_command_buffer_current;
    }
    else
    {
      if (w)
      {
        if (!exec_word(fvm, w))
        {
          OK = FVM_FALSE;
          break;
        }
        w = NULL;
        if (!*fvm_command_buffer_current)
          break;
      }
    }
    fvm_command_buffer_current++;
  }

  if (w && OK)
    OK = exec_word(fvm, w);

  if (OK && fvm_feedback_enabled)
    fvm_puts("OK\r\n");
}

int fvm_data_stack_underflow(fvm_data *fvm)
{
  int result = fvm->fvm_data_stack_pointer >= &fvm->fvm_data_stack[FVM_DATA_STACK_LENGTH];
  if (result)
    fvm_puts("Data stack underflow.\r\n");
  return result;
}

int fvm_data_stack_underflow2(fvm_data *fvm, int n)
{
  int result = fvm->fvm_data_stack_pointer + n > &fvm->fvm_data_stack[FVM_DATA_STACK_LENGTH];
  if (result)
    fvm_puts("Data stack underflow.\r\n");
  return result;
}

int lit_word_handler(fvm_data *fvm)
{
  fvm->fvm_current_word++;
  *(--fvm->fvm_data_stack_pointer) = *((int*)fvm->fvm_current_word);
  return FVM_TRUE;
}

int fvm_get_next_word(char *p, int cnt, int put_error_msg)
{
  int cnt2 = 0;

  while (*fvm_command_buffer_current)
  {
    if (*fvm_command_buffer_current > 0x20)
    {
      if (cnt2 < cnt)
      {
        *p++ = *fvm_command_buffer_current;
        cnt2++;
      }
    }
    else
    {
      if (cnt2)
      {
        *p = 0;
        return cnt2;
      }
    }
    fvm_command_buffer_current++;
  }

  *p = 0;

  if (!cnt2 && put_error_msg)
    fvm_puts("Name expected.\r\n");

  return cnt2;
}
