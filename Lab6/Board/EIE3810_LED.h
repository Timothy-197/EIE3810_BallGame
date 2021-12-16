#ifndef __EIE3810_LED_H
#define __EIE3810_LED_H
#include "stm32f10x.h"

// put procudure header here

void EIE3810_LED_Init(void);

void LED0_On(void);
void LED0_Off(void);
void LED1_On(void);
void LED1_Off(void);
u8 LED1_Toggle(u8 toggle);





#endif
