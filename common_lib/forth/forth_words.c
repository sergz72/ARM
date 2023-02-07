#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <forth_words.h>
#include <ftoa.h>
#include <itoa.h>

#define FVM_IF 0
#define FVM_DO 1
#define FVM_BEGIN 2
#define FVM_WHILE 3
#define FVM_CASE 4
#define FVM_OF 5
#define FVM_ENDOF 6

#define PUSH_CONDITION(a,b) *(--fvm_conditions_stack_pointer) = ((long long int)a << CPU_BITS) | (int)b

long long int fvm_conditions_stack[FVM_CONDITIONS_STACK_LENGTH];
long long int *fvm_conditions_stack_pointer;

int fvm_feedback_enabled;
int fvm_do_index, fvm_do_index2;
int fvm_do_variables[6];

fvm_word_handler *fvm_leave_words[3][10];
int fvm_leave_index[3];

const char* CRLF = "\r\n";

#ifdef FVM_FP_ENABLE
int itof_word_handler(fvm_data *fvm);
const forth_word itof_word = { NULL, "itof", itof_word_handler, NULL };

int ftoi_word_handler(fvm_data *fvm);
const forth_word ftoi_word = { &itof_word, "ftoi", ftoi_word_handler, NULL };

int sqrt_word_handler(fvm_data *fvm);
const forth_word sqrt_word = { &ftoi_word, "sqrt", sqrt_word_handler, NULL };

int fdiv_word_handler(fvm_data *fvm);
const forth_word fdiv_word = { &sqrt_word, "f/", fdiv_word_handler, NULL };

int fmul_word_handler(fvm_data *fvm);
const forth_word fmul_word = { &fdiv_word, "f*", fmul_word_handler, NULL };

int fminus_word_handler(fvm_data *fvm);
const forth_word fminus_word = { &fmul_word, "f-", fminus_word_handler, NULL };

int fplus_word_handler(fvm_data *fvm);
const forth_word fplus_word = { &fminus_word, "f+", fplus_word_handler, NULL };

int plus_word_handler(fvm_data *fvm);
const forth_word plus_word = { &fplus_word, "+", plus_word_handler, NULL };
#else
int plus_word_handler(fvm_data *fvm);
const forth_word plus_word = { NULL, "+", plus_word_handler, NULL };
#endif

int minus_word_handler(fvm_data *fvm);
const forth_word minus_word = { &plus_word, "-", minus_word_handler, NULL };

int start_word_handler(fvm_data *fvm);
int start_word_handler2(fvm_data *fvm);
const forth_word start_word = { &minus_word, ":", start_word_handler, start_word_handler2 };

int end_word_handler2(fvm_data *fvm);
const forth_word end_word = { &start_word, ";", NULL, end_word_handler2 };

#ifdef FVM_FP_ENABLE
int femit_word_handler(fvm_data *fvm);
const forth_word femit_word = { &end_word, "f.", femit_word_handler, NULL };

int emit_word_handler(fvm_data *fvm);
const forth_word emit_word = { &femit_word, ".", emit_word_handler, NULL };
#else
int emit_word_handler(fvm_data *fvm);
const forth_word emit_word = { &end_word, ".", emit_word_handler, NULL };
#endif

int constant_word_handler(fvm_data *fvm);
const forth_word constant_word = { &emit_word, "constant", constant_word_handler, NULL };

int drop_word_handler(fvm_data *fvm);
const forth_word drop_word = { &constant_word, "drop", drop_word_handler, NULL };

int dup2_word_handler(fvm_data *fvm);
const forth_word dup2_word = { &drop_word, "2dup", dup2_word_handler, NULL };

int dup_word_handler(fvm_data *fvm);
const forth_word dup_word = { &dup2_word, "dup", dup_word_handler, NULL };

int get_xt_word_handler(fvm_data *fvm);
int get_xt_word_handler2(fvm_data *fvm);
const forth_word get_xt_word = { &dup_word, "'", get_xt_word_handler, get_xt_word_handler2 };

int mul_word_handler(fvm_data *fvm);
const forth_word mul_word = { &get_xt_word, "*", mul_word_handler, NULL };

int div_word_handler(fvm_data *fvm);
const forth_word div_word = { &mul_word, "/", div_word_handler, NULL };

int onep_word_handler(fvm_data *fvm);
const forth_word onep_word = { &div_word, "1+", onep_word_handler, NULL };

int ones_word_handler(fvm_data *fvm);
const forth_word ones_word = { &onep_word, "1-", ones_word_handler, NULL };

int twop_word_handler(fvm_data *fvm);
const forth_word twop_word = { &ones_word, "2+", twop_word_handler, NULL };

int twos_word_handler(fvm_data *fvm);
const forth_word twos_word = { &twop_word, "2-", twos_word_handler, NULL };

int twom_word_handler(fvm_data *fvm);
const forth_word twom_word = { &twos_word, "2*", twom_word_handler, NULL };

int twod_word_handler(fvm_data *fvm);
const forth_word twod_word = { &twom_word, "2/", twod_word_handler, NULL };

int drop2_word_handler(fvm_data *fvm);
const forth_word drop2_word = { &twod_word, "2drop", drop2_word_handler, NULL };

int swap_word_handler(fvm_data *fvm);
const forth_word swap_word = { &drop2_word, "swap", swap_word_handler, NULL };

int swap2_word_handler(fvm_data *fvm);
const forth_word swap2_word = { &swap_word, "2swap", swap2_word_handler, NULL };

int eq0_word_handler(fvm_data *fvm);
const forth_word eq0_word = { &swap2_word, "0=", eq0_word_handler, NULL };

int ne0_word_handler(fvm_data *fvm);
const forth_word ne0_word = { &eq0_word, "0<>", ne0_word_handler, NULL };

int gt0_word_handler(fvm_data *fvm);
const forth_word gt0_word = { &ne0_word, "0>", gt0_word_handler, NULL };

int lt0_word_handler(fvm_data *fvm);
const forth_word lt0_word = { &gt0_word, "0<", lt0_word_handler, NULL };

int if_word_handler(fvm_data *fvm);
int if_word_handler2(fvm_data *fvm);
const forth_word if_word = { &lt0_word, "if", if_word_handler, if_word_handler2 };

int else_word_handler(fvm_data *fvm);
int else_word_handler2(fvm_data *fvm);
const forth_word else_word = { &if_word, "else", else_word_handler, else_word_handler2 };

int then_word_handler2(fvm_data *fvm);
const forth_word then_word = { &else_word, "then", NULL, then_word_handler2 };

int do_word_handler(fvm_data *fvm);
int do_word_handler2(fvm_data *fvm);
const forth_word do_word = { &then_word, "do", do_word_handler, do_word_handler2 };

int loop_word_handler(fvm_data *fvm);
int loop_word_handler2(fvm_data *fvm);
const forth_word loop_word = { &do_word, "loop", loop_word_handler, loop_word_handler2 };

int ploop_word_handler(fvm_data *fvm);
int ploop_word_handler2(fvm_data *fvm);
const forth_word ploop_word = { &loop_word, "+loop", ploop_word_handler, ploop_word_handler2 };

int repeat_word_handler(fvm_data *fvm);
int repeat_word_handler2(fvm_data *fvm);
const forth_word repeat_word = { &ploop_word, "repeat", repeat_word_handler, repeat_word_handler2 };

int until_word_handler(fvm_data *fvm);
int until_word_handler2(fvm_data *fvm);
const forth_word until_word = { &repeat_word, "until", until_word_handler, until_word_handler2 };

int eq_word_handler(fvm_data *fvm);
const forth_word eq_word = { &until_word, "=", eq_word_handler, NULL };

int ne_word_handler(fvm_data *fvm);
const forth_word ne_word = { &eq_word, "<>", ne_word_handler, NULL };

int lt_word_handler(fvm_data *fvm);
const forth_word lt_word = { &ne_word, "<", lt_word_handler, NULL };

int gt_word_handler(fvm_data *fvm);
const forth_word gt_word = { &lt_word, ">", gt_word_handler, NULL };

int le_word_handler(fvm_data *fvm);
const forth_word le_word = { &gt_word, "<=", le_word_handler, NULL };

int ge_word_handler(fvm_data *fvm);
const forth_word ge_word = { &le_word, ">=", ge_word_handler, NULL };

int le0_word_handler(fvm_data *fvm);
const forth_word le0_word = { &ge_word, "0<=", le0_word_handler, NULL };

int ge0_word_handler(fvm_data *fvm);
const forth_word ge0_word = { &le0_word, "0>=", ge0_word_handler, NULL };

int begin_word_handler2(fvm_data *fvm);
const forth_word begin_word = { &ge0_word, "begin", NULL, begin_word_handler2 };

int over_word_handler(fvm_data *fvm);
const forth_word over_word = { &begin_word, "over", over_word_handler, NULL };

int leave_word_handler(fvm_data *fvm);
int leave_word_handler2(fvm_data *fvm);
const forth_word leave_word = { &over_word, "leave", leave_word_handler, leave_word_handler2 };

int rot_word_handler(fvm_data *fvm);
const forth_word rot_word = { &leave_word, "rot", rot_word_handler, NULL };

int variable_word_handler(fvm_data *fvm);
const forth_word variable_word = { &rot_word, "variable", variable_word_handler, NULL };

int and_word_handler(fvm_data *fvm);
const forth_word and_word = { &variable_word, "and", and_word_handler, NULL };

int or_word_handler(fvm_data *fvm);
const forth_word or_word = { &and_word, "or", or_word_handler, NULL };

int xor_word_handler(fvm_data *fvm);
const forth_word xor_word = { &or_word, "xor", xor_word_handler, NULL };

int depth_word_handler(fvm_data *fvm);
const forth_word depth_word = { &xor_word, "depth", depth_word_handler, NULL };

int execute_word_handler(fvm_data *fvm);
const forth_word execute_word = { &depth_word, "execute", execute_word_handler, NULL };

int nip_word_handler(fvm_data *fvm);
const forth_word nip_word = { &execute_word, "nip", nip_word_handler, NULL };

int pick_word_handler(fvm_data *fvm);
const forth_word pick_word = { &nip_word, "pick", pick_word_handler, NULL };

int mrot_word_handler(fvm_data *fvm);
const forth_word mrot_word = { &pick_word, "-rot", mrot_word_handler, NULL };

int tuck_word_handler(fvm_data *fvm);
const forth_word tuck_word = { &mrot_word, "tuck", tuck_word_handler, NULL };

int dup0_word_handler(fvm_data *fvm);
const forth_word dup0_word = { &tuck_word, "?dup", dup0_word_handler, NULL };

int qstack_word_handler(fvm_data *fvm);
const forth_word qstack_word = { &dup0_word, "?stack", qstack_word_handler, NULL };

int words_word_handler(fvm_data *fvm);
const forth_word words_word = { &qstack_word, "words", words_word_handler, NULL };

int forget_word_handler(fvm_data *fvm);
const forth_word forget_word = { &words_word, "forget", forget_word_handler, NULL };

int mod_word_handler(fvm_data *fvm);
const forth_word mod_word = { &forget_word, "mod", mod_word_handler, NULL };

int roll_word_handler(fvm_data *fvm);
const forth_word roll_word = { &mod_word, "roll", roll_word_handler, NULL };

int mroll_word_handler(fvm_data *fvm);
const forth_word mroll_word = { &roll_word, "-roll", mroll_word_handler, NULL };

int while_word_handler2(fvm_data *fvm);
const forth_word while_word = { &mroll_word, "while", until_word_handler, while_word_handler2 };

int abs_word_handler(fvm_data *fvm);
const forth_word abs_word = { &while_word, "abs", abs_word_handler, NULL };

int invert_word_handler(fvm_data *fvm);
const forth_word invert_word = { &abs_word, "invert", invert_word_handler, NULL };

int lshift_word_handler(fvm_data *fvm);
const forth_word lshift_word = { &invert_word, "lshift", lshift_word_handler, NULL };

int max_word_handler(fvm_data *fvm);
const forth_word max_word = { &lshift_word, "max", max_word_handler, NULL };

int min_word_handler(fvm_data *fvm);
const forth_word min_word = { &max_word, "min", min_word_handler, NULL };

#ifdef FVM_FP_ENABLE
int fnegate_word_handler(fvm_data *fvm);
const forth_word fnegate_word = { &min_word, "fnegate", fnegate_word_handler, NULL };

int negate_word_handler(fvm_data *fvm);
const forth_word negate_word = { &fnegate_word, "negate", negate_word_handler, NULL };
#else
int negate_word_handler(fvm_data *fvm);
const forth_word negate_word = { &min_word, "negate", negate_word_handler, NULL };
#endif

int rshift_word_handler(fvm_data *fvm);
const forth_word rshift_word = { &negate_word, "rshift", rshift_word_handler, NULL };

int unloop_word_handler(fvm_data *fvm);
const forth_word unloop_word = { &rshift_word, "unloop", unloop_word_handler, NULL };

int within_word_handler(fvm_data *fvm);
const forth_word within_word = { &unloop_word, "within", within_word_handler, NULL };

int case_word_handler2(fvm_data *fvm);
const forth_word case_word = { &within_word, "case", NULL, case_word_handler2 };

int of_word_handler(fvm_data *fvm);
int of_word_handler2(fvm_data *fvm);
const forth_word of_word = { &case_word, "of", of_word_handler, of_word_handler2 };

int endof_word_handler(fvm_data *fvm);
int endof_word_handler2(fvm_data *fvm);
const forth_word endof_word = { &of_word, "endof", endof_word_handler, endof_word_handler2 };

int endcase_word_handler(fvm_data *fvm);
int endcase_word_handler2(fvm_data *fvm);
const forth_word endcase_word = { &endof_word, "endcase", NULL, endcase_word_handler2 };

int again_word_handler(fvm_data *fvm);
int again_word_handler2(fvm_data *fvm);
const forth_word again_word = { &endcase_word, "again", again_word_handler, again_word_handler2 };

int rangeof_word_handler(fvm_data *fvm);
int rangeof_word_handler2(fvm_data *fvm);
const forth_word rangeof_word = { &again_word, "rangeof", rangeof_word_handler, rangeof_word_handler2 };

int feedback_on_word_handler(fvm_data *fvm);
const forth_word feedback_on_word = { &rangeof_word, "feedback_on", feedback_on_word_handler, NULL };

int feedback_off_word_handler(fvm_data *fvm);
const forth_word feedback_off_word = { &feedback_on_word, "feedback_off", feedback_off_word_handler, NULL };

int cr_word_handler(fvm_data *fvm);
const forth_word cr_word = { &feedback_off_word, "cr", cr_word_handler, NULL };

int lf_word_handler(fvm_data *fvm);
const forth_word lf_word = { &cr_word, "lf", lf_word_handler, NULL };

int crlf_word_handler(fvm_data *fvm);
const forth_word crlf_word = { &lf_word, "crlf", crlf_word_handler, NULL };

int i_word_handler(fvm_data *fvm);
const forth_word i_word = { &crlf_word, "i", i_word_handler, NULL };

int j_word_handler(fvm_data *fvm);
const forth_word j_word = { &i_word, "j", j_word_handler, NULL };

int k_word_handler(fvm_data *fvm);
const forth_word k_word = { &j_word, "k", k_word_handler, NULL };

int dotr_word_handler(fvm_data *fvm);
const forth_word dotr_word = { &k_word, ".r", dotr_word_handler, NULL };

int reset_word_handler(fvm_data *fvm);
const forth_word reset_word = { &dotr_word, "reset", reset_word_handler, NULL };

int reset_stack_word_handler(fvm_data *fvm);
const forth_word reset_stack_word = { &reset_word, "reset_stack", reset_stack_word_handler, NULL };

int fvm_ret_handler(fvm_data *fvm);
const forth_word exit_word = { &reset_stack_word, "exit", fvm_ret_handler, NULL };

int s_quote_word_handler(fvm_data *fvm);
int s_quote_word_handler2(fvm_data *fvm);
const forth_word s_quote_word = { &exit_word, "s\"", s_quote_word_handler, s_quote_word_handler2 };

int point_quote_word_handler(fvm_data *fvm);
int point_quote_word_handler2(fvm_data *fvm);
const forth_word point_quote_word = { &s_quote_word, ".\"", point_quote_word_handler, point_quote_word_handler2 };

int s_point_word_handler(fvm_data *fvm);
const forth_word s_point_word = { &point_quote_word, "s.", s_point_word_handler, NULL };

int store_word_handler(fvm_data *fvm);
const forth_word store_word = { &s_point_word, "!", store_word_handler, NULL };

int fetch_word_handler(fvm_data *fvm);
const forth_word fetch_word = { &store_word, "@", fetch_word_handler, NULL };

int array_word_handler(fvm_data *fvm);
const forth_word array_word = { &fetch_word, "array", array_word_handler, NULL };

int dict_free_word_handler(fvm_data *fvm);
const forth_word dict_free_word = { &array_word, "dict_free", dict_free_word_handler, NULL };

int var_set_word_handler(fvm_data *fvm);
int do2_word_handler(fvm_data *fvm);
int plit_word_handler(fvm_data *fvm);

#ifdef FVM_FP_ENABLE
int fplus_word_handler(fvm_data *fvm)
{
  float v;
  
  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;
  v = *((float*)fvm->fvm_data_stack_pointer);
  fvm->fvm_data_stack_pointer++;
  *((float*)fvm->fvm_data_stack_pointer) += v;
  
  return FVM_TRUE;
}
#endif

int plus_word_handler(fvm_data *fvm)
{
  int v;
  
  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;
  v = *fvm->fvm_data_stack_pointer++;
  *fvm->fvm_data_stack_pointer += v;
  
  return FVM_TRUE;
}

#ifdef FVM_FP_ENABLE
int fminus_word_handler(fvm_data *fvm)
{
  float v;
  
  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;
  v = *((float*)fvm->fvm_data_stack_pointer);
  fvm->fvm_data_stack_pointer++;
  *((float*)fvm->fvm_data_stack_pointer) -= v;
  
  return FVM_TRUE;
}

int fmul_word_handler(fvm_data *fvm)
{
  float v;
  
  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;
  v = *((float*)fvm->fvm_data_stack_pointer);
  fvm->fvm_data_stack_pointer++;
  *((float*)fvm->fvm_data_stack_pointer) *= v;
  
  return FVM_TRUE;
}

int fdiv_word_handler(fvm_data *fvm)
{
  float v;
  
  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;
  v = *((float*)fvm->fvm_data_stack_pointer);
  fvm->fvm_data_stack_pointer++;
  if (v == 0)
  {
    fvm_puts("Division by zero.\r\n");
    return FVM_FALSE;
  }
  *((float*)fvm->fvm_data_stack_pointer) /= v;
  
  return FVM_TRUE;
}

int sqrt_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  *((float*)fvm->fvm_data_stack_pointer) = sqrt(*((float*)fvm->fvm_data_stack_pointer));
  
  return FVM_TRUE;
}

int itof_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  *((float*)fvm->fvm_data_stack_pointer) = *fvm->fvm_data_stack_pointer;
  
  return FVM_TRUE;
}

int ftoi_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  *fvm->fvm_data_stack_pointer = (int)*((float*)fvm->fvm_data_stack_pointer);
  
  return FVM_TRUE;
}
#endif

int minus_word_handler(fvm_data *fvm)
{
  int v;
  
  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;
  v = *fvm->fvm_data_stack_pointer++;
  *fvm->fvm_data_stack_pointer -= v;
  
  return FVM_TRUE;
}

int start_new_word(int get_next_word)
{
  char *p, *p2;
  short l;
  int md;

  if (get_next_word && !fvm_get_next_word(fvm_word_name, FVM_WORD_NAME_LENGTH, FVM_TRUE))
    return FVM_FALSE;

  if (fvm_word_within_dictionary(fvm_last_word))
    fvm_new_word = (forth_word*)((char*)fvm_last_word + fvm_last_word->props.dict.word_length);
  else
    fvm_new_word = (forth_word*)fvm_dictionary;
  
  p = &fvm_new_word->props.dict.name;
  p2 = fvm_word_name;
  l = 0;
  while (*p2)
  {
    *p++ = *p2++;
    l++;
  }
  *p++ = 0;
  l++;
  
  md = (int)p & (CPU_BITS/8-1);
  if (md)
  {
    md = (CPU_BITS/8) - md;
    p += md;
    l += md;
  }

  fvm_new_word->props.dict.name_length = l;

  fvm_word_code = (fvm_word_handler*)p;

  return FVM_TRUE;
}

int start_word_handler(fvm_data *fvm)
{
  if (!start_new_word(FVM_TRUE))
    return FVM_FALSE;

  fvm_conditions_stack_pointer = &fvm_conditions_stack[FVM_CONDITIONS_STACK_LENGTH];

  fvm_do_index2 = -1;
  fvm_leave_index[0] = 0;
  fvm_leave_index[1] = 0;
  fvm_leave_index[2] = 0;

  fvm_mode = FVM_MODE_COMPILE;

  return FVM_TRUE;
}

void Unexpected(const forth_word *fw)
{
  fvm_puts("Unexpected ");
  fvm_puts(fw->props.non_dict.name);
  fvm_puts(CRLF);
}

int start_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    Unexpected(&start_word);
    return FVM_FALSE;
  }

  return FVM_TRUE;
}

void end_new_word(void)
{
  *fvm_word_code++ = fvm_ret_handler;
  fvm_new_word->pred = fvm_last_word;
  fvm_new_word->props.dict.word_length = (char*)fvm_word_code - (char*)fvm_new_word;
  fvm_last_word = fvm_new_word;
}

int end_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    fvm_mode = FVM_MODE_INTERPRET;

    if (fvm_conditions_stack_pointer == &fvm_conditions_stack[FVM_CONDITIONS_STACK_LENGTH])
      end_new_word();
    else
    {
      fvm_puts("Condition stack is not empty.\r\n");
      return FVM_FALSE;
    }

    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

void emit(int v, int l, int sp)
{
  static char temp[40];
  char *temp_p = &temp[20], *temp_p2;
  int ltemp;

  _itoa_s(v, temp_p, 20, 10);
  ltemp = strlen(temp_p);
  l -= ltemp;
  temp_p2 = temp_p + ltemp;
  while (l-- > 0)
  {
	  *--temp_p = ' ';
	  ltemp++;
  }
  if (sp)
  {
	  *temp_p2 = ' ';
	  ltemp++;
  }
  fvm_put_string_proc(temp_p, ltemp);
}

#ifdef FVM_FP_ENABLE
void femit(float v, int l, int sp)
{
  static char temp[50];
  char *temp_p = &temp[20], *temp_p2;
  int ltemp;

  if (ftoa(v, temp_p, 30))
    strcpy_s(temp_p, 10, "finv");
  ltemp = strlen(temp_p);
  l -= ltemp;
  temp_p2 = temp_p + ltemp;
  while (l-- > 0)
  {
    *--temp_p = ' ';
    ltemp++;
  }
  if (sp)
  {
    *temp_p2 = ' ';
    ltemp++;
  }
  fvm_put_string_proc(temp_p, ltemp);
}

int femit_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  femit(*((float*)fvm->fvm_data_stack_pointer), 0, FVM_TRUE);
  fvm->fvm_data_stack_pointer++;

  return FVM_TRUE;
}
#endif

int emit_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  emit(*fvm->fvm_data_stack_pointer++, 0, FVM_TRUE);

  return FVM_TRUE;
}

int dotr_word_handler(fvm_data *fvm)
{
  int l;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  l = *fvm->fvm_data_stack_pointer++;

  emit(*fvm->fvm_data_stack_pointer++, l, FVM_FALSE);

  return FVM_TRUE;
}

int constant_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  if (!start_new_word(FVM_TRUE))
    return FVM_FALSE;

  *fvm_word_code++ = lit_word_handler;
  *fvm_word_code++ = (fvm_word_handler)*fvm->fvm_data_stack_pointer++;

  end_new_word();

  return FVM_TRUE;
}

int drop_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;
  fvm->fvm_data_stack_pointer++;
  return FVM_TRUE;
}

int dup_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;
  v = *fvm->fvm_data_stack_pointer;
  *(--fvm->fvm_data_stack_pointer) = v;
  return FVM_TRUE;
}

int dup2_word_handler(fvm_data *fvm)
{
  int v, v2;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;
  v = *fvm->fvm_data_stack_pointer++;
  v2 = *fvm->fvm_data_stack_pointer--;
  *(--fvm->fvm_data_stack_pointer) = v2;
  *(--fvm->fvm_data_stack_pointer) = v;
  return FVM_TRUE;
}

int get_xt_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    *fvm_word_code++ = get_xt_word_handler;
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int get_xt_word_handler(fvm_data *fvm)
{
  fvm->fvm_current_word++;
  if (*fvm->fvm_current_word != fvm_call_handler)
  {
    *(--fvm->fvm_data_stack_pointer) = (int)*((fvm_word_handler**)fvm->fvm_current_word);
    *(--fvm->fvm_data_stack_pointer) = 0;
  }
  else
  {
    *(--fvm->fvm_data_stack_pointer) = (int)*((fvm_word_handler**)(fvm->fvm_current_word+1));
    *(--fvm->fvm_data_stack_pointer) = (int)*((fvm_word_handler**)fvm->fvm_current_word++);
  }

  return FVM_TRUE;
}

int mul_word_handler(fvm_data *fvm)
{
  int v;
  
  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;
  v = *fvm->fvm_data_stack_pointer++;
  *fvm->fvm_data_stack_pointer *= v;
  
  return FVM_TRUE;
}

int div_word_handler(fvm_data *fvm)
{
  int v;
  
  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;
  v = *fvm->fvm_data_stack_pointer++;
  if (v == 0)
  {
    fvm_puts("Division by zero.\r\n");
    return FVM_FALSE;
  }
  *fvm->fvm_data_stack_pointer /= v;
  
  return FVM_TRUE;
}

int onep_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;
  *fvm->fvm_data_stack_pointer += 1;
  return FVM_TRUE;
}

int ones_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;
  *fvm->fvm_data_stack_pointer -= 1;
  return FVM_TRUE;
}

int twop_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;
  *fvm->fvm_data_stack_pointer += 2;
  return FVM_TRUE;
}

int twos_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;
  *fvm->fvm_data_stack_pointer -= 1;
  return FVM_TRUE;
}

int twom_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;
  *fvm->fvm_data_stack_pointer *= 2;
  return FVM_TRUE;
}

int twod_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;
  *fvm->fvm_data_stack_pointer /= 2;
  return FVM_TRUE;
}

int drop2_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;
  fvm->fvm_data_stack_pointer += 2;
  return FVM_TRUE;
}

int swap_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;
  v = *fvm->fvm_data_stack_pointer;
  *fvm->fvm_data_stack_pointer = fvm->fvm_data_stack_pointer[1];
  fvm->fvm_data_stack_pointer[1] = v;
  return FVM_TRUE;
}

int swap2_word_handler(fvm_data *fvm)
{
//2swap ( x1 x2 x3 x4 -- x3 x4 x1 x2 )
//Поменять местами две пары объектов на вершине стека.
  int v;

  if (fvm_data_stack_underflow2(fvm, 4))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer;
  *fvm->fvm_data_stack_pointer = fvm->fvm_data_stack_pointer[2];
  fvm->fvm_data_stack_pointer[2] = v;
  v = fvm->fvm_data_stack_pointer[1];
  fvm->fvm_data_stack_pointer[1] = fvm->fvm_data_stack_pointer[3];
  fvm->fvm_data_stack_pointer[3] = v;

  return FVM_TRUE;
}

int eq0_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;
  
  *fvm->fvm_data_stack_pointer = *fvm->fvm_data_stack_pointer == 0;

  return FVM_TRUE;
}

int ne0_word_handler(fvm_data *fvm)
{
  *fvm->fvm_data_stack_pointer = *fvm->fvm_data_stack_pointer != 0;

  return FVM_TRUE;
}

int gt0_word_handler(fvm_data *fvm)
{
  *fvm->fvm_data_stack_pointer = *fvm->fvm_data_stack_pointer > 0;

  return FVM_TRUE;
}

int lt0_word_handler(fvm_data *fvm)
{
  *fvm->fvm_data_stack_pointer = *fvm->fvm_data_stack_pointer < 0;

  return FVM_TRUE;
}

int if_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    *fvm_word_code++ = if_word_handler;
    PUSH_CONDITION(FVM_IF, fvm_word_code);
    fvm_word_code++;
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int if_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  fvm->fvm_current_word++;

  if (!*fvm->fvm_data_stack_pointer++)
    fvm->fvm_current_word = *((fvm_word_handler**)fvm->fvm_current_word);

  return FVM_TRUE;
}

int fvm_conditions_stack_underflow(const forth_word *fw)
{
  int result = fvm_conditions_stack_pointer >= &fvm_conditions_stack[FVM_CONDITIONS_STACK_LENGTH];
  if (result)
    Unexpected(fw);
  return result;
}

fvm_word_handler *pcond;

int resolve_condition(int condition_type, const forth_word *fw)
{
  long long int cond;

  if (fvm_conditions_stack_underflow(fw))
    return FVM_FALSE;
  cond = *fvm_conditions_stack_pointer++;
  if ((cond >> CPU_BITS) != condition_type)
  {
    Unexpected(fw);
    return FVM_FALSE;
  }

  pcond = (fvm_word_handler*)(cond & 0xFFFFFFFF);
    
  return FVM_TRUE;
}

int else_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    if (!resolve_condition(FVM_IF, &else_word))
      return FVM_FALSE;

    *((int*)pcond) = (int)(fvm_word_code + 1);

    *fvm_word_code++ = else_word_handler;
    PUSH_CONDITION(FVM_IF, fvm_word_code);
    fvm_word_code++;
    
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int else_word_handler(fvm_data *fvm)
{
  fvm->fvm_current_word = *((fvm_word_handler**)(fvm->fvm_current_word + 1));
  return FVM_TRUE;
}

int then_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    if (!resolve_condition(FVM_IF, &then_word))
      return FVM_FALSE;

  *((int*)pcond) = (int)(fvm_word_code - 1);

    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int do_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    *fvm_word_code++ = do_word_handler;
    *fvm_word_code++ = do2_word_handler;
    PUSH_CONDITION(FVM_DO, fvm_word_code);
    fvm_word_code++;
    fvm_do_index2++;
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int do_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  fvm_do_index += 2;
  fvm_do_variables[fvm_do_index] = *fvm->fvm_data_stack_pointer++;
  fvm_do_variables[fvm_do_index+1] = *fvm->fvm_data_stack_pointer++;

  return FVM_TRUE;
}

int loop_word_handler_(const forth_word *fw)
{
  int idx;

  if (fvm_mode == FVM_MODE_COMPILE)
  {
    if (!resolve_condition(FVM_DO, fw))
      return FVM_FALSE;

    *((int*)pcond) = (int)(fvm_word_code + 1);

    *fvm_word_code++ = fw->props.non_dict.handler;
    *fvm_word_code = (fvm_word_handler)(pcond - 2);
    
    idx = fvm_leave_index[fvm_do_index2];
    fvm_leave_index[fvm_do_index2] = 0;
    while (idx)
      *fvm_leave_words[fvm_do_index2][--idx] = (fvm_word_handler)fvm_word_code;
    fvm_word_code++;
    fvm_do_index2--;

    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int loop_word_handler2(fvm_data *fvm)
{
  return loop_word_handler_(&loop_word);
}

int loop_word_handler(fvm_data *fvm)
{
  fvm_do_variables[fvm_do_index]++;

  fvm->fvm_current_word = *((fvm_word_handler**)(fvm->fvm_current_word + 1));

  return FVM_TRUE;
}

int ploop_word_handler2(fvm_data *fvm)
{
  return loop_word_handler_(&ploop_word);
}

int ploop_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  fvm_do_variables[fvm_do_index] += *fvm->fvm_data_stack_pointer++;

  fvm->fvm_current_word = *((fvm_word_handler**)(fvm->fvm_current_word + 1));

  return FVM_TRUE;
}

int repeat_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {

    if (!resolve_condition(FVM_WHILE, &repeat_word))
      return FVM_FALSE;

    *((int*)pcond) = (int)(fvm_word_code + 1);

    if (!resolve_condition(FVM_BEGIN, &repeat_word))
      return FVM_FALSE;

    *fvm_word_code++ = repeat_word_handler;
    *fvm_word_code++ = (fvm_word_handler)(pcond - 1);
    
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int repeat_word_handler(fvm_data *fvm)
{
  fvm->fvm_current_word = *((fvm_word_handler**)(fvm->fvm_current_word + 1));
  return FVM_TRUE;
}

int until_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {

    if (!resolve_condition(FVM_BEGIN, &until_word))
      return FVM_FALSE;

    *fvm_word_code++ = until_word_handler;
    *fvm_word_code++ = (fvm_word_handler)(pcond - 1);
    
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int until_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  fvm->fvm_current_word++;

  if (!*fvm->fvm_data_stack_pointer++)
    fvm->fvm_current_word = *((fvm_word_handler**)fvm->fvm_current_word);

  return FVM_TRUE;
}

int eq_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;

  *fvm->fvm_data_stack_pointer = *fvm->fvm_data_stack_pointer == v;

  return FVM_TRUE;
}

int ne_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;

  *fvm->fvm_data_stack_pointer = *fvm->fvm_data_stack_pointer != v;

  return FVM_TRUE;
}

int lt_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;

  *fvm->fvm_data_stack_pointer = *fvm->fvm_data_stack_pointer < v;

  return FVM_TRUE;
}

int gt_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;

  *fvm->fvm_data_stack_pointer = *fvm->fvm_data_stack_pointer > v;

  return FVM_TRUE;
}

int le_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;

  *fvm->fvm_data_stack_pointer = *fvm->fvm_data_stack_pointer <= v;

  return FVM_TRUE;
}

int ge_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;

  *fvm->fvm_data_stack_pointer = *fvm->fvm_data_stack_pointer >= v;

  return FVM_TRUE;
}

int le0_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;
  
  *fvm->fvm_data_stack_pointer = *fvm->fvm_data_stack_pointer <= 0;

  return FVM_TRUE;
}

int ge0_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;
  
  *fvm->fvm_data_stack_pointer = *fvm->fvm_data_stack_pointer >= 0;

  return FVM_TRUE;
}

int begin_word_handler2(fvm_data *fvm)
{
//begin
//Начало цикла begin ... again или begin ... until или begin ... while ... repeat
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    PUSH_CONDITION(FVM_BEGIN, fvm_word_code);
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int over_word_handler(fvm_data *fvm)
{
//over ( x1 x2 -- x1 x2 x1 )
//Положить копию x1 на стек.
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = fvm->fvm_data_stack_pointer[1];
  *(--fvm->fvm_data_stack_pointer) = v;

  return FVM_TRUE;
}

int leave_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    *fvm_word_code++ = leave_word_handler;
    fvm_leave_words[fvm_do_index2][fvm_leave_index[fvm_do_index2]++] = fvm_word_code++;
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int leave_word_handler(fvm_data *fvm)
{
//leave
//Снять текущие  параметры цикла  и продолжить выполнение  за пределами
//текущего цикла  (после LOOP или +LOOP).
  fvm_do_index -= 2;
  fvm->fvm_current_word = *((fvm_word_handler**)(fvm->fvm_current_word + 1));
  return FVM_TRUE;
}

int rot_word_handler(fvm_data *fvm)
{
//rot  ( x1 x2 x3 -- x2 x3 x1 )
//Повернуть верхние три ячейки на вершине стека.
  int v1, v2, v3;

  if (fvm_data_stack_underflow2(fvm, 3))
    return FVM_FALSE;
  
  v3 = *fvm->fvm_data_stack_pointer++;
  v2 = *fvm->fvm_data_stack_pointer++;
  v1 = *fvm->fvm_data_stack_pointer++;

  *(--fvm->fvm_data_stack_pointer) = v2;
  *(--fvm->fvm_data_stack_pointer) = v3;
  *(--fvm->fvm_data_stack_pointer) = v1;

  return FVM_TRUE;
}

int variable_word_handler(fvm_data *fvm)
{
  fvm_word_handler *fw2;
  char *p;
  
  if (!start_new_word(FVM_TRUE))
    return FVM_FALSE;

  p = fvm_word_name;

  while (*p)
    p++;

  *p = '@';
  p[1] = 0;

  *fvm_word_code++ = lit_word_handler;
  fw2 = fvm_word_code;
  fvm_word_code++;

  end_new_word();

  *p = '!';

  if (!start_new_word(FVM_FALSE))
    return FVM_FALSE;

  *fvm_word_code++ = var_set_word_handler;
  *fvm_word_code++ = (fvm_word_handler)fw2;

  end_new_word();

  return FVM_TRUE;
}

int and_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;

  *fvm->fvm_data_stack_pointer &= v;

  return FVM_TRUE;
}

int or_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;

  *fvm->fvm_data_stack_pointer |= v;

  return FVM_TRUE;
}

int xor_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;

  *fvm->fvm_data_stack_pointer ^= v;

  return FVM_TRUE;
}

int depth_word_handler(fvm_data *fvm)
{
  int v;

  v = (&fvm->fvm_data_stack[FVM_DATA_STACK_LENGTH]) - fvm->fvm_data_stack_pointer;

  *(--fvm->fvm_data_stack_pointer) = v;

  return FVM_TRUE;
}

int execute_word_handler(fvm_data *fvm)
{
  fvm_word_handler *wh, wh2;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  if (*fvm->fvm_data_stack_pointer++) // call
  {
    wh = fvm->fvm_current_word;
    fvm->fvm_current_word = (fvm_word_handler*)*fvm->fvm_data_stack_pointer++;
    while (fvm->fvm_current_word)
    {
      if (!(*fvm->fvm_current_word)(fvm))
        return FVM_FALSE;
      fvm->fvm_current_word++;
    }
    fvm->fvm_current_word = wh;
    return FVM_TRUE;
  }
  else
  {
    wh2 = (fvm_word_handler)*fvm->fvm_data_stack_pointer++;
    return wh2(fvm);
  }
}

int nip_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;
  *fvm->fvm_data_stack_pointer = v;

  return FVM_TRUE;
}

int pick_word_handler(fvm_data *fvm)
{
  int n;

  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  n = *fvm->fvm_data_stack_pointer;

  if (fvm_data_stack_underflow2(fvm, n+2))
    return FVM_FALSE;

  *fvm->fvm_data_stack_pointer = fvm->fvm_data_stack_pointer[n+1];

  return FVM_TRUE;
}

int mrot_word_handler(fvm_data *fvm)
{
//-rot ( x1 x2 x3 -- x3 x1 x2 )
//Повернуть верхние три ячейки на вершине стека.
  int v1, v2, v3;

  if (fvm_data_stack_underflow2(fvm, 3))
    return FVM_FALSE;
  
  v3 = *fvm->fvm_data_stack_pointer++;
  v2 = *fvm->fvm_data_stack_pointer++;
  v1 = *fvm->fvm_data_stack_pointer++;

  *(--fvm->fvm_data_stack_pointer) = v3;
  *(--fvm->fvm_data_stack_pointer) = v1;
  *(--fvm->fvm_data_stack_pointer) = v2;

  return FVM_TRUE;
}

int tuck_word_handler(fvm_data *fvm)
{
//tuck ( x1 x2 -- x2 x1 x2 )
//Копировать первую (верхнюю) ячейку стека ниже второй.
  int v1, v2;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v2 = *fvm->fvm_data_stack_pointer++;
  v1 = *fvm->fvm_data_stack_pointer++;

  *(--fvm->fvm_data_stack_pointer) = v2;
  *(--fvm->fvm_data_stack_pointer) = v1;
  *(--fvm->fvm_data_stack_pointer) = v2;

  return FVM_TRUE;
}

int dup0_word_handler(fvm_data *fvm)
{
//?dup ( x -- 0 | x x )
//Повторить на стеке x, если x не равно нулю.
  int v;

  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer;
  if (v)
    *(--fvm->fvm_data_stack_pointer) = v;

  return FVM_TRUE;
}

int qstack_word_handler(fvm_data *fvm)
{
//?stack
//Выводит сообщение о содержимом стека
  int *p = fvm->fvm_data_stack_pointer;

  while (p < &fvm->fvm_data_stack[FVM_DATA_STACK_LENGTH])
    emit(*p++, 0, FVM_TRUE);

  return FVM_TRUE;
}

int words_word_handler(fvm_data *fvm)
{
//выводит на экран список созданных слов
  const forth_word *p = fvm_last_word;

  while ((char*)p >= fvm_dictionary && (char*)p < fvm_dictionary + FVM_DICTIONARY_LENGTH)
  {
    fvm_puts(&p->props.dict.name);
    fvm_puts(CRLF);
    p = p->pred;
  }

  return FVM_TRUE;
}

int forget_word_handler(fvm_data *fvm)
{
//forget word_name
//Удаляет из текущего словаря слово word_name и все слова, добавленные
//в словарь после word_name. Неопределенная ситуация существует,
//если name не может быть найдено.

  const forth_word *p;

  if (!fvm_get_next_word(fvm_word_name, FVM_WORD_NAME_LENGTH, FVM_FALSE))
    fvm_last_word = fvm_last_word_initial;
  else
  {
    p = fvm_last_word;
    while (fvm_word_within_dictionary(p))
    {
      if (!strcmp(&p->props.dict.name, fvm_word_name))
      {
        fvm_last_word = p->pred;
        return FVM_TRUE;
      }
      p = p->pred;
    }

    fvm_puts("Word not found.\r\n");

    return FVM_FALSE;
  }

  return FVM_TRUE;
}

int mod_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;

  *fvm->fvm_data_stack_pointer %= v;

  return FVM_TRUE;
}

int roll_word_handler(fvm_data *fvm)
{
//roll ( xu xu-1 ... x0 u -- xu-1 ... x0 xu )
//Снять u. Провернуть u+1 величин на вершине стека. Hеопределённое состоя-
//ние возникает, если перед  исполнением стек содержит меньше  чем u+2
//величин.
  int u, xu;

  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  u = *fvm->fvm_data_stack_pointer++;

  if (u > 0)
  {
    if (fvm_data_stack_underflow2(fvm, u+1))
      return FVM_FALSE;

    xu = fvm->fvm_data_stack_pointer[u];

    while (u)
    {
      fvm->fvm_data_stack_pointer[u] = fvm->fvm_data_stack_pointer[u-1];
      u--;
    }

    *fvm->fvm_data_stack_pointer = xu;
  }

  return FVM_TRUE;
}

int mroll_word_handler(fvm_data *fvm)
{
//-roll ( xu xu-1 ... x0 u -- x0 xu xu-1 ... x1 )
//Снять со стека u. Провернуть u+1 величин на вершине стека.
//Hеопределённое состояние возникает, если перед  исполнением стек
//содержит меньше  чем u+2 величин.
  int i, u, x0;

  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  u = *fvm->fvm_data_stack_pointer++;

  if (u > 0)
  {
    if (fvm_data_stack_underflow2(fvm, u+1))
      return FVM_FALSE;

    x0 = *fvm->fvm_data_stack_pointer;

    for (i = 0; i < u; i++)
      fvm->fvm_data_stack_pointer[i] = fvm->fvm_data_stack_pointer[i+1];

    fvm->fvm_data_stack_pointer[u] = x0;
  }

  return FVM_TRUE;
}

int while_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    *fvm_word_code++ = until_word_handler;
    PUSH_CONDITION(FVM_WHILE, fvm_word_code);
    fvm_word_code++;
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int abs_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  *fvm->fvm_data_stack_pointer = abs(*fvm->fvm_data_stack_pointer);

  return FVM_TRUE;
}

int invert_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  *fvm->fvm_data_stack_pointer = ~*fvm->fvm_data_stack_pointer;

  return FVM_TRUE;
}

int lshift_word_handler(fvm_data *fvm)
{
//lshift ( x1 u -- x2 )
//Произвести логический сдвиг x1 влево на u бит и вернуть результат x2.
//В освобождаемые младшие биты поместить нуль.

  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;

  *fvm->fvm_data_stack_pointer <<= v;

  return FVM_TRUE;
}

int max_word_handler(fvm_data *fvm)
{
  int v, v2;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;
  v2 = *fvm->fvm_data_stack_pointer;

  *fvm->fvm_data_stack_pointer = v > v2 ? v : v2;

  return FVM_TRUE;
}

int min_word_handler(fvm_data *fvm)
{
  int v, v2;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;
  v2 = *fvm->fvm_data_stack_pointer;

  *fvm->fvm_data_stack_pointer = v < v2 ? v : v2;

  return FVM_TRUE;
}

#ifdef FVM_FP_ENABLE
int fnegate_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  *((float*)fvm->fvm_data_stack_pointer) = -*((float*)fvm->fvm_data_stack_pointer);

  return FVM_TRUE;
}
#endif

int negate_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  *fvm->fvm_data_stack_pointer = -*fvm->fvm_data_stack_pointer;

  return FVM_TRUE;
}

int rshift_word_handler(fvm_data *fvm)
{
//rshift ( x1 u -- x2 )
//Произвести логический сдвиг x1 вправо на u бит и вернуть результат x2. В
//освобождаемые старшие биты поместить нуль.

  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;

  *fvm->fvm_data_stack_pointer >>= v;

  return FVM_TRUE;
}

int unloop_word_handler(fvm_data *fvm)
{
//unloop
//Снять с стека текущие параметры цикла
  return FVM_TRUE;
}

int within_word_handler(fvm_data *fvm)
{
//WITHIN     ( n1 low high -- f1 ) \ f1=true if ((n1 >= low) & (n1 < high))

  int high, low, n1;

  if (fvm_data_stack_underflow2(fvm, 3))
    return FVM_FALSE;

  high = *fvm->fvm_data_stack_pointer++;
  low = *fvm->fvm_data_stack_pointer++;
  n1 = *fvm->fvm_data_stack_pointer;

  *fvm->fvm_data_stack_pointer = (n1 >= low) && (n1 < high);

  return FVM_TRUE;
}

int case_word_handler2(fvm_data *fvm)
{
//case
//Начало структуры  CASE ... OF  ...  ENDOF ... ENDCASE.
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    PUSH_CONDITION(FVM_CASE, fvm_word_code);
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int of_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    *fvm_word_code++ = of_word_handler;
    PUSH_CONDITION(FVM_OF, fvm_word_code);
    fvm_word_code++;
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int of_word_handler(fvm_data *fvm)
{
  int v;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  v = *fvm->fvm_data_stack_pointer++;

  fvm->fvm_current_word++;

  if (v != *fvm->fvm_data_stack_pointer)
    fvm->fvm_current_word = *((fvm_word_handler**)fvm->fvm_current_word);

  return FVM_TRUE;
}

int endof_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    if (!resolve_condition(FVM_OF, &endof_word))
      return FVM_FALSE;

    *((int*)pcond) = (int)(fvm_word_code + 1);

    *fvm_word_code++ = endof_word_handler;
    PUSH_CONDITION(FVM_ENDOF, fvm_word_code);
    fvm_word_code++;
    
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int endof_word_handler(fvm_data *fvm)
{
  fvm->fvm_current_word = *((fvm_word_handler**)(fvm->fvm_current_word + 1));
  return FVM_TRUE;
}

int endcase_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    while ((*fvm_conditions_stack_pointer >> CPU_BITS) == FVM_ENDOF)
    {
      if (!resolve_condition(FVM_ENDOF, &endcase_word))
        return FVM_FALSE;
      *((int*)pcond) = (int)(fvm_word_code - 1);
    }

    if (fvm_conditions_stack_underflow(&endcase_word))
      return FVM_FALSE;

    if ((*fvm_conditions_stack_pointer++ >> CPU_BITS) != FVM_CASE)
    {
      Unexpected(&endcase_word);
      return FVM_FALSE;
    }

    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int again_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {

    if (!resolve_condition(FVM_BEGIN, &again_word))
      return FVM_FALSE;

    *fvm_word_code++ = again_word_handler;
    *fvm_word_code++ = (fvm_word_handler)(pcond - 1);
    
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int again_word_handler(fvm_data *fvm)
{
//again
//Используется для организации бесконечных циклов begin ... again
  fvm->fvm_current_word = *((fvm_word_handler**)(fvm->fvm_current_word + 1));
  return FVM_TRUE;
}

int rangeof_word_handler2(fvm_data *fvm)
{
  if (fvm_mode == FVM_MODE_COMPILE)
  {
    *fvm_word_code++ = rangeof_word_handler;
    PUSH_CONDITION(FVM_OF, fvm_word_code);
    fvm_word_code++;
    return FVM_TRUE;
  }
  else
    return FVM_COMPILE_TIME_WORD;
}

int rangeof_word_handler(fvm_data *fvm)
{
  int v1, v2, n;

  if (fvm_data_stack_underflow2(fvm, 3))
    return FVM_FALSE;

  v2 = *fvm->fvm_data_stack_pointer++;
  v1 = *fvm->fvm_data_stack_pointer++;
  n = *fvm->fvm_data_stack_pointer;

  fvm->fvm_current_word++;

  if (v1 <= n && n <= v2)
    fvm->fvm_current_word = *((fvm_word_handler**)fvm->fvm_current_word);

  return FVM_TRUE;
}

int feedback_on_word_handler(fvm_data *fvm)
{
  fvm_feedback_enabled = FVM_TRUE;
  return FVM_TRUE;
}

int feedback_off_word_handler(fvm_data *fvm)
{
  fvm_feedback_enabled = FVM_FALSE;
  return FVM_TRUE;
}

int dict_free_word_handler(fvm_data *fvm)
{
  char *p;

  if (fvm_word_within_dictionary(fvm_last_word))
    p = (char*)fvm_last_word + fvm_last_word->props.dict.word_length;
  else
    p = fvm_dictionary;

  emit(fvm_dictionary + FVM_DICTIONARY_LENGTH - p, 0, FVM_TRUE);

  return FVM_TRUE;
}

int var_set_word_handler(fvm_data *fvm)
{
  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  fvm->fvm_current_word++;

  *((int*)*fvm->fvm_current_word) = *fvm->fvm_data_stack_pointer++;

  return FVM_TRUE;
}

int do2_word_handler(fvm_data *fvm)
{
  fvm->fvm_current_word++;

  if (fvm_do_variables[fvm_do_index] >= fvm_do_variables[fvm_do_index+1])
  {
    fvm_do_index -= 2;
    fvm->fvm_current_word = *((fvm_word_handler**)fvm->fvm_current_word);
  }

  return FVM_TRUE;
}

int cr_word_handler(fvm_data *fvm)
{
  fvm_put_string_proc("\r", 1);

  return FVM_TRUE;
}

int lf_word_handler(fvm_data *fvm)
{
  fvm_put_string_proc("\n", 1);

  return FVM_TRUE;
}

int crlf_word_handler(fvm_data *fvm)
{
  fvm_put_string_proc("\r\n", 2);

  return FVM_TRUE;
}

int i_word_handler(fvm_data *fvm)
{
  *(--fvm->fvm_data_stack_pointer) = fvm_do_variables[0];

  return FVM_TRUE;
}

int j_word_handler(fvm_data *fvm)
{
  *(--fvm->fvm_data_stack_pointer) = fvm_do_variables[2];

  return FVM_TRUE;
}

int k_word_handler(fvm_data *fvm)
{
  *(--fvm->fvm_data_stack_pointer) = fvm_do_variables[4];

  return FVM_TRUE;
}

int reset_word_handler(fvm_data *fvm)
{
  fvm_reinit(fvm);
#ifdef FVM_USER_PROC
  FVM_USER_RESET_PROC();
#endif
  return FVM_TRUE;
}

int reset_stack_word_handler(fvm_data *fvm)
{
  fvm->fvm_data_stack_pointer = &fvm->fvm_data_stack[FVM_DATA_STACK_LENGTH];

  return FVM_TRUE;
}

int fvm_get_string(void (*handler)(const char*, unsigned int))
{
  char c;

  if (*fvm_command_buffer_current++)
  {
    while ((c = *fvm_command_buffer_current++) != 0)
    {
      switch (c)
      {
        case '\\':
          c = *fvm_command_buffer_current++;
          switch (c)
          {
            case 'r': handler("\r", 1); break;
            case 'n': handler("\n", 1); break;
            case 't': handler("\t", 1); break;
            case '\\': handler("\\", 1); break;
            default:
              fvm_puts("Invalid special character definition.\r\n");
              return FVM_FALSE;
          }
          break;
        case '"':
          return FVM_TRUE;
        default:
          handler(&c, 1);
      }
    }
  }

  fvm_puts("Unterminated string constant.\r\n");

  return FVM_FALSE;
}

char *char_p;

void string_save_handler(const char *s, unsigned int n)
{
  while (n--)
  {
    *char_p++ = *s++;
  }
}

int s_quote_word_handler2(fvm_data *fvm)
{
  int i;

  if (fvm_mode == FVM_MODE_COMPILE)
  {
    *fvm_word_code++ = s_quote_word_handler;
    char_p = (char*)fvm_word_code;
    if (!fvm_get_string(string_save_handler))
      return FVM_FALSE;
    *char_p++ = 0;
    i = ((int)char_p) & (CPU_BITS/8-1);
    if (i)
      char_p += (CPU_BITS/8) - i;
    fvm_word_code = (fvm_word_handler*)char_p;
  }
  else
    return FVM_COMPILE_TIME_WORD;

  return FVM_TRUE;
}

int s_quote_word_handler(fvm_data *fvm)
{
  int i;

  fvm->fvm_current_word++;

  *(--fvm->fvm_data_stack_pointer) = (int)fvm->fvm_current_word;

  char_p = (char*)fvm->fvm_current_word;
  while (*char_p++);
  
  i = ((int)char_p) & (CPU_BITS/8-1);
  if (i)
    char_p -= i;

  fvm->fvm_current_word = (fvm_word_handler*)char_p;

  return FVM_TRUE;
}

int point_quote_word_handler2(fvm_data *fvm)
{
  int i;

  if (fvm_mode == FVM_MODE_COMPILE)
  {
    *fvm_word_code++ = point_quote_word_handler;
    char_p = (char*)fvm_word_code;
    if (!fvm_get_string(string_save_handler))
      return FVM_FALSE;
    *char_p++ = 0;
    i = ((int)char_p) & (CPU_BITS/8-1);
    if (i)
      char_p += (CPU_BITS/8) - i;
    fvm_word_code = (fvm_word_handler*)char_p;
  }
  else
  {
    if (!fvm_get_string(fvm_put_string_proc))
      return FVM_ERROR;
    return FVM_FALSE;
  }

  return FVM_TRUE;
}

int point_quote_word_handler(fvm_data *fvm)
{
  int i;

  char_p = (char*)(fvm->fvm_current_word + 1);
  i = strlen(char_p);
  fvm_put_string_proc(char_p, i);
  
  char_p += i + 1;

  i = ((int)char_p) & (CPU_BITS/8-1);
  if (i)
    char_p -= i;

  fvm->fvm_current_word = (fvm_word_handler*)char_p;
  
  return FVM_TRUE;
}

int s_point_word_handler(fvm_data *fvm)
{
  int l;

  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  char_p = (char*)*fvm->fvm_data_stack_pointer++;
  l = strlen(char_p);
  fvm_put_string_proc(char_p, l);

  return FVM_TRUE;
}

int store_word_handler(fvm_data *fvm)
{
// !
//( x a-addr -- )
// Сохраняет x в a-addr .    
  int *addr;

  if (fvm_data_stack_underflow2(fvm, 2))
    return FVM_FALSE;

  addr  = (int*)*fvm->fvm_data_stack_pointer++;
  *addr = *fvm->fvm_data_stack_pointer++;

  return FVM_TRUE;
}

int fetch_word_handler(fvm_data *fvm)
{
// @
// ( a-addr -- x )
// x - значение, сохраненное в a-addr.
  int *addr;

  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  addr  = (int*)*fvm->fvm_data_stack_pointer;
  *fvm->fvm_data_stack_pointer = *addr;

  return FVM_TRUE;
}

int array_word_handler(fvm_data *fvm)
{
  int size;

  if (fvm_data_stack_underflow(fvm))
    return FVM_FALSE;

  size = *fvm->fvm_data_stack_pointer++;

  if (!start_new_word(FVM_TRUE))
    return FVM_FALSE;

  *fvm_word_code++ = plit_word_handler;
  fvm_word_code += size;

  end_new_word();

  return FVM_TRUE;
}

int plit_word_handler(fvm_data *fvm)
{
  *(--fvm->fvm_data_stack_pointer) = (int)(fvm->fvm_current_word + 1);

  fvm->fvm_current_word = (fvm_word_handler*)-(CPU_BITS/8);

  return FVM_TRUE;
}

int fvm_ret_handler(fvm_data *fvm)
{
  fvm->fvm_current_word = (fvm_word_handler*)-(CPU_BITS/8);

  return FVM_TRUE;
}
