#include "stm32f10x.h"
#include "EIE3810_ExternalInterrupt.h"


void EIE3810_Key0_EXTIInit(void)
{
	RCC->APB2ENR |= 1<<6;
	GPIOE->CRL &=0xFFF0FFFF;
	GPIOE->CRL |=0x00080000; // Set the PE4: input with pull-up/ pull-down
	GPIOE->ODR |=0x00000010; // Set the PE4: pull-up
	RCC->APB2ENR |= 0x01; // AFIO enable
	AFIO->EXTICR[1] &= 0xFFFFFFF0;
	AFIO->EXTICR[1] |= 0x00000004; // EXTI4: config PE[X] pin
	EXTI->IMR |= 1<<4; // mask bits except bit line 4 (PE4: key0)
	EXTI->FTSR |= 1<<4; // falling edge enabled, PE4
	NVIC->IP[10] = 0x65; // EXTI4: interrupt priorty: 0x65 01100101
	NVIC->ISER[0] |= (1<<10); // enable interrupt EXTI2
}

void EIE3810_Key1_EXTIInit(void)
{
	RCC->APB2ENR |= 1<<6;
	GPIOE->CRL &=0xFFFF0FFF;
	GPIOE->CRL |=0x00008000; // Set the PE3: input with pull-up/ pull-down
	GPIOE->ODR |=0x00000008; // Set the PE3: pull-up
	RCC->APB2ENR |= 0x01; // AFIO enable
	AFIO->EXTICR[0] &= 0xFFFF0FFF;
	AFIO->EXTICR[0] |= 0x00004000; // EXTI3: config PE[X] pin
	EXTI->IMR |= 1<<3; // mask bits except bit line 3 (PE3: key1)
	EXTI->FTSR |= 1<<3; // falling edge enabled, PE3
	NVIC->IP[9] = 0x65; // EXTI: interrupt priorty: 0x65 01100101
	NVIC->ISER[0] |= (1<<9); // enable interrupt EXTI2
}

void EIE3810_Key2_EXTIInit(void)
{
	RCC->APB2ENR |= 1<<6;
	GPIOE->CRL &= 0xFFFFF0FF;
	GPIOE->CRL |= 0x00000800;
	GPIOE->ODR |= 1<<2; // SET KEY2
	RCC->APB2ENR |= 0x01; // AFIO enable
	AFIO->EXTICR[0] &= 0xFFFFF0FF;
	AFIO->EXTICR[0] |= 0x00000400; // EXTI4: config PE[X] pin
	EXTI->IMR |= 1<<2; // mask bits except bit line 2 (PE2: key2)
	EXTI->FTSR |= 1<<2; // falling edge enabled, PE2
	NVIC->IP[8] = 0x65; // EXTI2: interrupt priorty: 0x65 01100101
	NVIC->ISER[0] |= (1<<8); // enable interrupt EXTI2
}

void EIE3810_KeyUp_EXTIInit(void)
{
	RCC->APB2ENR |= 1<<2;
	GPIOA->CRL &=0xFFFFFFF0;
	GPIOA->CRL |=0x00000008; // Set the PA0: input with pull-up/ pull-down
	GPIOA->ODR &=0xFFFFFFFE; // Set the PA0: pull-down  
	RCC->APB2ENR |= 0x01; // AFIO enable
	AFIO->EXTICR[0] &= 0xFFFFFFF0;
	AFIO->EXTICR[0] |= 0x00000000; // EXTI0: config PA[X] pin
	EXTI->IMR |= 1<<0; // mask bits except bit line 0 (PA0: keyUp)
	EXTI->FTSR |= 1<<0; // falling edge enable: PA0
	NVIC->IP[6] = 0x35; // EXTI0: interrupt priority: 0x75
	NVIC->ISER[0] |= (1<<6); // enable interrupt EXTI0
}

void EIE3810_USART1_EXTIInit(void)
{
	NVIC->IP[37] = 0x65; // set interrupt priority 0x65
	NVIC->ISER[1] |= 1<<5; // enable interrupt USART1
}

void EIE3810_NVIC_SetPriorityGroup(u8 prigroup)
{
	u32 temp1, temp2;
	temp2 = prigroup & 0x00000007;
	temp2 <<= 8;
	temp1 = SCB->AIRCR;
	temp1 &= 0x0000F8FF;
	temp1 |= 0x05FA0000;
	temp1 |= temp2;
	SCB->AIRCR = temp1;
}
