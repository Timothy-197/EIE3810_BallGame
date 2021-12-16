#ifndef __EIE3810_EXTERNALINTERRUPT_H
#define __EIE3810_EXTERNALINTERRUPT_H
#include "stm32f10x.h"

void EIE3810_Key0_EXTIInit(void);
void EIE3810_Key1_EXTIInit(void);
void EIE3810_Key2_EXTIInit(void);
void EIE3810_KeyUp_EXTIInit(void);
void EIE3810_NVIC_SetPriorityGroup(u8 prigroup);
void EIE3810_USART1_EXTIInit(void);

#endif
