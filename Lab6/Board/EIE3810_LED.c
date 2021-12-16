#include "stm32f10x.h"
#include "EIE3810_LED.h"


void EIE3810_LED_Init()
{
	RCC->APB2ENR|=1<<3;
	RCC->APB2ENR|=1<<6;
  // Init the PB5: LED0 output
	GPIOB->CRL &=0xFF0FFFFF;
	GPIOB->CRL |=0x00300000; // Set the PB5: output push-pull mode
	GPIOB->BSRR = 1<<5;

  // Init the PE5: LED1 output
	GPIOE->CRL &=0xFF0FFFFF;
	GPIOE->CRL |=0x00300000; // Set the PE5: output push-pull mode
	GPIOE->BSRR = 1<<5;
}

void LED0_On()
{
	GPIOB->BRR = 1<<5; // Reset
}

void LED0_Off()
{
	GPIOB->BSRR = 1<<5; // Set
}

void LED1_On()
{
	GPIOE->BRR = 1<<5; // Reset
}

void LED1_Off()
{
	GPIOE->BSRR = 1<<5; // Set
}

u8 LED1_Toggle(u8 toggle)
{
	if (toggle == 0)
	{
		LED1_On();
		return 1;
	}
	else
	{
		LED1_Off();
		return 0;
	}
}
