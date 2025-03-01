#ifndef _NVIC_H
#define _NVIC_H

#define NVIC_PriorityGroup_0         ((uint32_t)0x700) /*!< 0 bits for pre-emption priority
                                                            8 bits for subpriority */
#define NVIC_PriorityGroup_1         ((uint32_t)0x600) /*!< 1 bit  for pre-emption priority
                                                            7 bits for subpriority */
#define NVIC_PriorityGroup_2         ((uint32_t)0x500) /*!< 2 bits for pre-emption priority
                                                            6 bits for subpriority */
#define NVIC_PriorityGroup_3         ((uint32_t)0x400) /*!< 3 bits for pre-emption priority
                                                            5 bits for subpriority */
#define NVIC_PriorityGroup_4         ((uint32_t)0x300) /*!< 4 bits for pre-emption priority
                                                            4 bits for subpriority */
#define NVIC_PriorityGroup_5         ((uint32_t)0x200) /*!< 5 bits for pre-emption priority
                                                            3 bits for subpriority */
#define NVIC_PriorityGroup_6         ((uint32_t)0x100) /*!< 6 bits for pre-emption priority
                                                            2 bits for subpriority */
#define NVIC_PriorityGroup_7         ((uint32_t)0x000) /*!< 7 bits for pre-emption priority
                                                            1 bit  for subpriority */

void NVIC_Init(unsigned int IRQChannel, unsigned int IRQChannelPreemptionPriority,
               unsigned int IRQChannelSubPriority, FunctionalState IRQChannelCmd);
void NVIC_PriorityGroupConfig(unsigned int NVIC_PriorityGroup);

#endif
