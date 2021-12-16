#ifndef __EIE3810_KEY_H
#define __EIE3810_KEY_H
#include "stm32f10x.h"

// put procudure header here
void EIE3810_Key_Init(void);

u32 KEY_UP_Read(void);
u32 KEY1_Read(void);
u32 KEY2_Read(void);




#endif
