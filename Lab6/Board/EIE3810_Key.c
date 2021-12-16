#include "stm32f10x.h"
#include "EIE3810_KEY.h"

// put your procedure and code here
void EIE3810_Key_Init()
{
	RCC->APB2ENR|=1<<2;
	RCC->APB2ENR|=1<<6;
  // Init the PE4: Key0 input
	GPIOE->CRL &=0xFFF0FFFF;
	GPIOE->CRL |=0x00080000; // Set the PE4: input with pull-up/ pull-down
	GPIOE->ODR |=0x00000010; // Set the PE4: pull-up

  // Init the PE3: Key1 input
	GPIOE->CRL &=0xFFFF0FFF;
	GPIOE->CRL |=0x00008000; // Set the PE3: input with pull-up/ pull-down
	GPIOE->ODR |=0x00000008; // Set the PE3: pull-up
	
  // Init the PE2: Key2 input
	GPIOE->CRL &=0xFFFFF0FF;
	GPIOE->CRL |=0x00000800; // Set the PE2: input with pull-up/ pull-down
	GPIOE->ODR |=0x00000004; // Set the PE2: pull-up
    
  // Init the PA0: Key-up input
	GPIOA->CRL &=0xFFFFFFF0;
	GPIOA->CRL |=0x00000008; // Set the PA0: input with pull-up/ pull-down
	GPIOA->ODR &=0xFFFFFFFE; // Set the PA0: pull-down  
}

u32 KEY_UP_Read()
{
	return (GPIOA->IDR>>0 & 1);
}

u32 KEY1_Read()
{
	if ((GPIOE->IDR>>3 & 1)==0) return 1;
	else return 0;
}

u32 KEY2_Read()
{
	if ((GPIOE->IDR>>2 & 1)==0) return 1;
	else return 0;
}
