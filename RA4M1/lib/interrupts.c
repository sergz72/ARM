#include "board.h"
#include <interrupts.h>

void ApplicationInterruptHandler1(void);
void ApplicationInterruptHandler2(void);
void ApplicationInterruptHandler3(void);
void ApplicationInterruptHandler4(void);
void ApplicationInterruptHandler5(void);
void ApplicationInterruptHandler6(void);
void ApplicationInterruptHandler7(void);
void ApplicationInterruptHandler8(void);
void ApplicationInterruptHandler9(void);
void ApplicationInterruptHandler10(void);
void ApplicationInterruptHandler11(void);
void ApplicationInterruptHandler12(void);
void ApplicationInterruptHandler13(void);
void ApplicationInterruptHandler14(void);
void ApplicationInterruptHandler15(void);
void ApplicationInterruptHandler16(void);

const interrupt_handler_t g_vector_table[16] __attribute__((section(".application_vectors"))) =
{
  ApplicationInterruptHandler1,
  ApplicationInterruptHandler2,
  ApplicationInterruptHandler3,
  ApplicationInterruptHandler4,
  ApplicationInterruptHandler5,
  ApplicationInterruptHandler6,
  ApplicationInterruptHandler7,
  ApplicationInterruptHandler8,
  ApplicationInterruptHandler9,
  ApplicationInterruptHandler10,
  ApplicationInterruptHandler11,
  ApplicationInterruptHandler12,
  ApplicationInterruptHandler13,
  ApplicationInterruptHandler14,
  ApplicationInterruptHandler15,
  ApplicationInterruptHandler16
};

static interrupt_handler_t h_vector_table[16];

void ApplicationInterruptHandler1(void)
{
  h_vector_table[0]();
}

void ApplicationInterruptHandler2(void)
{
  h_vector_table[1]();
}

void ApplicationInterruptHandler3(void)
{
  h_vector_table[2]();
}

void ApplicationInterruptHandler4(void)
{
  h_vector_table[3]();
}

void ApplicationInterruptHandler5(void)
{
  h_vector_table[4]();
}

void ApplicationInterruptHandler6(void)
{
  h_vector_table[5]();
}

void ApplicationInterruptHandler7(void)
{
  h_vector_table[6]();
}

void ApplicationInterruptHandler8(void)
{
  h_vector_table[7]();
}

void ApplicationInterruptHandler9(void)
{
  h_vector_table[8]();
}

void ApplicationInterruptHandler10(void)
{
  h_vector_table[9]();
}

void ApplicationInterruptHandler11(void)
{
  h_vector_table[10]();
}

void ApplicationInterruptHandler12(void)
{
  h_vector_table[11]();
}

void ApplicationInterruptHandler13(void)
{
  h_vector_table[12]();
}

void ApplicationInterruptHandler14(void)
{
  h_vector_table[13]();
}

void ApplicationInterruptHandler15(void)
{
  h_vector_table[14]();
}

void ApplicationInterruptHandler16(void)
{
  h_vector_table[15]();
}

static unsigned int application_interrupt_idx = 0;

unsigned int RegisterApplicationInterrupt(interrupt_handler_t handler, enum e_elc_event_ra4m1 interrupt_type)
{
  h_vector_table[application_interrupt_idx] = handler;
  R_ICU->IELSR[application_interrupt_idx] = interrupt_type;

  return application_interrupt_idx++;
}
