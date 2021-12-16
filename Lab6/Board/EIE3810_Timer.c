#include "stm32f10x.h"
#include "EIE3810_Timer.h"

void EIE3810_SYSTICK_Init10ms(void) // if we need to change the frequency, remember to reset the load register
{
	// SYSTICK
	SysTick->CTRL = 0; // Clear SysTick->CTRL setting
	SysTick->LOAD = 90000; // reload value 9*e4, now that the frquency = 9MHx/90000 = 100 Hz =>10ms period clock
	SysTick->CTRL = 0x3; // control abd status register, use clock source 0
	
	// CLKSOURCE=0: FCLK/8
	// CLKSOURCE=1: FCLK
	// CLKSOURCE=0 is synchronized and better than CLKSOURCE=1
	// P92 of RM0008
}

void EIE3810_TIM3_Init(u16 arr, u16 psc)
{
	// TIM3
	RCC->APB1ENR |= 1<<1; // TIM3EN: enable TIM3
	TIM3->ARR = arr; 
	// set prescaler value, The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1).
	TIM3->PSC = psc; 
	// set prescaler value, ARR is the value to be loaded in the actual auto-reload register.
	TIM3->DIER |= 1<<0; // update interrupt enable
	TIM3->CR1 |= 0x01; // counter enable + UEV disable
	/* the bug occurs here, the TIM3 is interrupt 29 */
	NVIC->IP[29] = 0X45; // set interrupt priority of TIM3
	NVIC->ISER[0] = (1<<29); // enable interrupt TIM3
}

void EIE3810_TIM4_Init(u16 arr, u16 psc)
{
	// TIM3
	RCC->APB1ENR |= 1<<2; // TIM3EN: enable TIM3
	TIM4->ARR = arr; 
	// set prescaler value, The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1).
	TIM4->PSC = psc; 
	// set prescaler value, ARR is the value to be loaded in the actual auto-reload register.
	TIM4->DIER |= 1<<0; // update interrupt enable
	TIM4->CR1 |= 0x01; // counter enable + UEV disable
	NVIC->IP[30] = 0X45; // set interrupt priority of USART3
	NVIC->ISER[0] = (1<<30); // enable interrupt TIM3
}

void EIE3810_TIM3_PWMInit(u16 arr, u16 psc)
{
	RCC->APB2ENR |= 1<<3; // IO port B enable
	GPIOB->CRL &= 0xFF0FFFFF;
	GPIOB->CRL |= 0X00B00000; // PB5 output mode, alternative push-pull
	RCC->APB2ENR |= 1<<0; // AFIOEN: Alternative funtion IO clock enable
	AFIO->MAPR &= 0xFFFFF3FF;
	AFIO->MAPR |= 1<<11; // TIM3_REMAP: partial remap, remap TIM3_CH2 to the PB5
	RCC->APB1ENR |= 1<<1; // TIM3 enable
	TIM3->ARR = arr;
	TIM3->PSC = psc; // set prescaler and reload value
	TIM3->CCMR1 |= 7<<12; // set TIM3 output compare 2 mode
	TIM3->CCMR1 |= 1<<11; // output compare 2 preload enable
	TIM3->CCER |= 1<<4; // Capture/compare 2 output enable
	TIM3->CR1 = 0x0080; // TXE interrupt enable
	TIM3->CR1 |= 1<<0; // break character will be transimitted
}
