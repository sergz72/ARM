#include "board.h"

/* Defines function pointers to be used with vector table. */
typedef void (* exc_ptr_t)(void);

void Reset_Handler(void);
void Default_Handler(void);
void main(void);

/*******************************************************************************************************************//**
 * MCU starts executing here out of reset. Main stack pointer is set up already.
 **********************************************************************************************************************/
void Reset_Handler (void)
{
    /* Initialize system using BSP. */
    SystemInit();

    /* Call user application. */
    main();

    while (1)
    {
        /* Infinite Loop. */
    }
}

/*******************************************************************************************************************//**
 * Default exception handler.
 **********************************************************************************************************************/
void Default_Handler (void)
{
    /** A error has occurred. The user will need to investigate the cause. Common problems are stack corruption
     *  or use of an invalid pointer.
     */
    while (1)
    {
        /* Infinite Loop. */
    }
}

unsigned char g_main_stack[STACK_SIZE] __attribute__((aligned(8)));
unsigned char g_heap[HEAP_SIZE] __attribute__((aligned(8)));

#define WEAK_REF_ATTRIBUTE    __attribute__((weak, alias("Default_Handler")))

void NMI_Handler(void) WEAK_REF_ATTRIBUTE;
void HardFault_Handler(void) WEAK_REF_ATTRIBUTE;
void MemManage_Handler(void) WEAK_REF_ATTRIBUTE;
void BusFault_Handler(void) WEAK_REF_ATTRIBUTE;
void UsageFault_Handler(void) WEAK_REF_ATTRIBUTE;
void SecureFault_Handler(void) WEAK_REF_ATTRIBUTE;
void SVC_Handler(void) WEAK_REF_ATTRIBUTE;
void DebugMon_Handler(void) WEAK_REF_ATTRIBUTE;
void PendSV_Handler(void) WEAK_REF_ATTRIBUTE;
void SysTick_Handler(void) WEAK_REF_ATTRIBUTE;

/* Vector table. */
const exc_ptr_t __VECTOR_TABLE[16] __attribute__((section(".fixed_vectors"))) =
{
    (exc_ptr_t)&g_main_stack[STACK_SIZE],                      /*      Initial Stack Pointer     */
    Reset_Handler,                                             /*      Reset Handler             */
    NMI_Handler,                                               /*      NMI Handler               */
    HardFault_Handler,                                         /*      Hard Fault Handler        */
    MemManage_Handler,                                         /*      MPU Fault Handler         */
    BusFault_Handler,                                          /*      Bus Fault Handler         */
    UsageFault_Handler,                                        /*      Usage Fault Handler       */
    SecureFault_Handler,                                       /*      Secure Fault Handler      */
    0,                                                         /*      Reserved                  */
    0,                                                         /*      Reserved                  */
    0,                                                         /*      Reserved                  */
    SVC_Handler,                                               /*      SVCall Handler            */
    DebugMon_Handler,                                          /*      Debug Monitor Handler     */
    0,                                                         /*      Reserved                  */
    PendSV_Handler,                                            /*      PendSV Handler            */
    SysTick_Handler,                                           /*      SysTick Handler           */
};
