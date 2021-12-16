#include "stm32f10x.h"
#include "EIE3810_Clock.h"

void EIE3810_clock_tree_init(void)
{
	u8 PLL=7;
	u8 temp=0;
	RCC->CR |= 0x00010000; //HSE clock enable
	while(!((RCC->CR>>17)&0x1));//wait until the clock is ready
	RCC->CFGR &= 0xFFFDFFFF; //HSE clock not divide
	RCC->CFGR |= 1<<16; //HSE oscillator selected as PLL input clock
	RCC->CFGR |= PLL<<18; //input clock * 9
	RCC->CR |=0x01000000;//PLL ON
	while(!(RCC->CR>>25));//wait until the PLL unlocked
	RCC->CFGR &=0xFFFFFFFE;
	RCC->CFGR |=0x00000002;//PLL set as the system clock
	while(temp !=0x02) //check if sws=[1:0], i.e. if the PLL used as system clock
	{
		temp=RCC->CFGR>>2;
		temp &= 0x03; //temp = sws
	}	
	RCC->CFGR &= 0xFFFFFC0F;//sysclock not divided
	RCC->CFGR |= 0x00000400;//APB1: HCLK divided by 2
	RCC->CFGR &= 0xFFFFC7FF;//APB2: HCLK not divided
	FLASH->ACR = 0x32;//Set FLASH with 2 wait states
	RCC->APB1ENR |= 1<<17; //APB1: USART2 enable
	RCC->APB2ENR |= 1<<14; //APB2: USART1 enable
}
