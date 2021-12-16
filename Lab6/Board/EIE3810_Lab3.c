#include "stm32f10x.h"
#include "EIE3810_Lab3.h"
#include "EIE3810_TFTLCD.h"

void Exp1()
{
	int i;
	// draw 5 lines
	for (i=0; i<20; i++)
	{
		EIE3810_TFTLCD_DrawDot(10+i, 10, BLACK);
	}
	for (i=0; i<20; i++)
	{
		EIE3810_TFTLCD_DrawDot(10+i, 20, WHITE);
	}
	for (i=0; i<20; i++)
	{
		EIE3810_TFTLCD_DrawDot(10+i, 30, GREEN);
	}
	for (i=0; i<20; i++)
	{
		EIE3810_TFTLCD_DrawDot(10+i, 40, RED);
	}
	for (i=0; i<20; i++)
	{
		EIE3810_TFTLCD_DrawDot(10+i, 50, BLUE);
	}
}

void Exp2()
{
	// draw a rectangle
	EIE3810_TFTLCD_FillRectangle(100, 100, 100, 100, YELLOW);
}

void Exp3()
{
	// count down
	int count;
	count = 10;
	while (1)
	{
		if (count == 0) count = 10;
		count--;
		EIE3810_TFTLCD_SevenSegment(212, 330, count, RED);
		Delay(10330580);
		EIE3810_TFTLCD_SevenSegment(212, 330, count, WHITE);
	}
}

void Exp4()
{
	// print the student id
	EIE3810_TFTLCD_ShowChar(212, 500, 49, BLACK, YELLOW);
	EIE3810_TFTLCD_ShowChar(220, 500, 49, BLACK, YELLOW);
	EIE3810_TFTLCD_ShowChar(228, 500, 57, BLACK, YELLOW);
	EIE3810_TFTLCD_ShowChar(236, 500, 48, BLACK, YELLOW);
	EIE3810_TFTLCD_ShowChar(244, 500, 49, BLACK, YELLOW);
	EIE3810_TFTLCD_ShowChar(252, 500, 48, BLACK, YELLOW);
	EIE3810_TFTLCD_ShowChar(260, 500, 49, BLACK, YELLOW);
	EIE3810_TFTLCD_ShowChar(268, 500, 48, BLACK, YELLOW);
	EIE3810_TFTLCD_ShowChar(276, 500, 52, BLACK, YELLOW);
}

void Delay(u32 count)
{
	u32 i;
	for (i=0;i<count;i++);
}
