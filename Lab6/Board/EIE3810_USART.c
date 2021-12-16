#include "stm32f10x.h"
#include "EIE3810_USART.h"

void EIE3810_USART2_init(u32 pclk1, u32 baudrate) // pclk: input clock to the peripheral
{
	//USART2
	float temp;
	u16 mantissa;
	u16 fraction;
	temp=(float) (pclk1*1000000)/(baudrate*16); // Tx/Rx baud
	mantissa=temp;
	fraction=(temp-mantissa)*16;
	mantissa<<=4;
	mantissa+=fraction;
	RCC->APB2ENR |= 1<<2; //enable GPIOA
	GPIOA->CRL &= 0xFFFF00FF; //
	GPIOA->CRL |= 0x00008B00; //PA2:output, push-pill; PA3:input, pull-up/pull down
	RCC->APB1RSTR |= 1<<17; //Reset USART2
	RCC->APB1RSTR &= ~(1<<17); //no effect, set the value back to 0
	USART2->BRR=mantissa;//set the baud rate
	USART2->CR1=0x2008; //transmitter is enabled + word length is 9 bit(1 start, 8 data) + disable parity
}

void EIE3810_USART1_init(u32 pclk1, u32 baudrate) // pclk: input clock to the peripheral
{
	//USART2
	float temp;
	u16 mantissa;
	u16 fraction;
	temp=(float) (pclk1*1000000)/(baudrate*16); // Tx/Rx baud
	mantissa=temp;
	fraction=(temp-mantissa)*16;
	mantissa<<=4;
	mantissa+=fraction;
	RCC->APB2ENR |= 1<<2; //enable GPIOA
	GPIOA->CRH &= 0xFFFFF00F; //
	GPIOA->CRH |= 0x000008B0; //PA9(TX):output, push-pill; PA10(RX):input, pull-up/pull down
	RCC->APB2RSTR |= 1<<14; //Reset USART1
	RCC->APB2RSTR &= ~(1<<14); //no effect, set the value back to 0
	USART1->BRR=mantissa;//set the baud rate
	USART1->CR1=0x2008; //transmitter is enabled + word length is 10 bit(1 start, 9 data) + disable parity
	USART1->CR1|=0x24; // set: RXNE interrupt enable and reciever enable
}

void USART_print(u8 USARTport, char* st)
{
	u8 i=0;
	
	while (st[i] != 0x00)
	{
		if (USARTport == 1) USART1->DR = st[i];
		if (USARTport == 2) USART2->DR = st[i];
		
		if (USARTport == 1) 
			while(!(((USART1->SR)>>7)&0x1));
		else if (USARTport == 2)
			while(!(((USART2->SR)>>7)&0x1));
		else return;
		
		while(!(((USART1->SR)>>7)&0x1));
		if (i== 255) break;
		i++;
	}
}
