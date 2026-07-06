#include <board.h>
#include <nvic.h>

#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000)

/**
  * @brief  Initializes the NVIC peripheral according to the specified
  *         parameters
  * @retval None
  */
void NVIC_Init(unsigned int IRQChannel, unsigned int IRQChannelPreemptionPriority,
               unsigned int IRQChannelSubPriority, FunctionalState IRQChannelCmd)
{
  unsigned int bits, ipr;

  if (IRQChannelCmd != DISABLE)
  {
    /* Compute the Corresponding IRQ Priority --------------------------------*/
    bits = ((SCB->AIRCR & 0x700) >> 8) + 1;
    ipr = (IRQChannelPreemptionPriority << bits) | (IRQChannelSubPriority & ((1 << bits) - 1));

    NVIC->IPR[IRQChannel] = (unsigned char)ipr;
    
    /* Enable the Selected IRQ Channels --------------------------------------*/
    NVIC->ISER[IRQChannel >> 0x05] = 1 << (IRQChannel & 0x1F);
  }
  else
    /* Disable the Selected IRQ Channels -------------------------------------*/
    NVIC->ICER[IRQChannel >> 0x05] = 1 << (IRQChannel & 0x1F);
}

/**
  * @brief  Configures the priority grouping: pre-emption priority and subpriority.
  * @param  NVIC_PriorityGroup: specifies the priority grouping bits length. 
  *   This parameter can be one of the following values:
  *     @arg NVIC_PriorityGroup_0: 0 bits for pre-emption priority
  *                                4 bits for subpriority
  *     @arg NVIC_PriorityGroup_1: 1 bits for pre-emption priority
  *                                3 bits for subpriority
  *     @arg NVIC_PriorityGroup_2: 2 bits for pre-emption priority
  *                                2 bits for subpriority
  *     @arg NVIC_PriorityGroup_3: 3 bits for pre-emption priority
  *                                1 bits for subpriority
  *     @arg NVIC_PriorityGroup_4: 4 bits for pre-emption priority
  *                                0 bits for subpriority
  * @retval None
  */
void NVIC_PriorityGroupConfig(unsigned int NVIC_PriorityGroup)
{
  /* Set the PRIGROUP[10:8] bits according to NVIC_PriorityGroup value */
  SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup;
}
