#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "EIE3810_TFTLCD.h"
#include "EIE3810_Lab3.h"
#include "EIE3810_LED.h"
#include "EIE3810_Key.h"
#include "EIE3810_Buzzer.h"
#include "EIE3810_USART.h"
#include "EIE3810_CLOCK.h"
#include "EIE3810_Timer.h"
#include "EIE3810_ExternalInterrupt.h"

void JOYPAD_Init(void);
void JOYPAD_Delay(u16 t);
u8 JOYPAD_Read(void);
u8 task1HeartBeat;
u8 task2HeartBeat;
u8 task3HeartBeat;

/*=================================================================================================*/
/* Implementation starts here */
#define EASY 0
#define HARD 1
#define SMALL 0
#define BIG 1
#define ROW_WELCOME 100
#define COL_WELCOME 200
#define ROW_SELECT 50
#define COL_SELECT 300
#define ROW 100
#define COL 100
#define B_Y 8
#define A_Y 792

/* variable field */
// boolean indicators
u8 LEVEL_SELECTED;      // boolean --value to indicate whether the player have selected the value
u8 BALL_SELECTED;       // boolean --value to indicate whether the player have selected the ball
u8 OBS_SELECTED;        // boolean --value to indicate whether the player choose to include obstacle in the game
u8 DIRECTION_SETTED;    // boolean --whether the direction is set
u8 GAMEEND;             // boolean --indicate whether the game ends
u8 GAMEBEGIN;           // boolean --indicate that the game begins
u8 BUZZER_ON;           // boolean --indicate that the buzzer should be toggled
u8 PAUSED;              // boolean --indicate whether the game is paused
// game global setting
u8 LEVEL;               // the level number
u8 SIZE;                // the selected ball size, 0 indicates small, 1 indicates big
u8 OBS;                 // the obstacle, 0 indicates no obstacle, 1 indicates obstacle
u8 DIRECTION;           // direction indicator
u8 ROUND;               // how many times ball hits the pads
u8 WINNER;              // winner of the game (a or b)
int DirX[8];
int DirY[8];            // direction information, read-only
u32 TIME;               // a timer to count the time past in the game
u16 COLORS[4];
u8 COLOR;               // current color index
// ball variables
int speedX_s, speedY_s; // the magnitude of the speed
int speedX, speedY;     // the speed of the ball in two directions
int ballX, ballY;       // coordinate --the ball coordinate
u8 radius;              // ball radius
u8 ballPeriod;
u16 ballColor;
// pad variables
int padX_A, padX_B;     // coordinate --x coordinates of the player pads
u8 speedPad;            // move speed of the player pad
u8 padWidth, padHeight; // width of the player pad
u16 padColorA, padColorB;
// obstacle variables
int obsX, obsY;         // coordinate --the obstacle coordinate
u8 obsWidth, obsHeight; // the width and height of the obstacle
u8 obsSpeed;            // the speed of the obstacle
int obsVelo;            // the velocity of the obstacle
u16 obsColor;

void EXTI0_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void USART1_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void MovePad(void);
void MoveBall(void);
void MoveObs(void);
u8 CheckBound_Ball(void);
u8 CheckPad_Ball(void);
u8 CheckObs_Ball(void);
u8 CheckEnd_Ball(void);
void TriggerBuzzer(void);
void DisplayTime(u16 x, u16 y);
void Welcome(void);
void SelectDifficulty(void);
void SelectBallSize(void);
void SelectObstacle(void);
void GetDirection(void);
void Game(void);
void EndScreen(void);

/*------------------------------------------------------------------*/
int main(void)
{
	/* Initializaion steps */
	EIE3810_clock_tree_init();
	EIE3810_NVIC_SetPriorityGroup(5); // Set PRIGROUP
	EIE3810_Key_Init();
	EIE3810_LED_Init();
	EIE3810_Buzzer_Init();
	EIE3810_TFTLCD_Init();
	EIE3810_Key0_EXTIInit();
	EIE3810_Key1_EXTIInit();
	EIE3810_Key2_EXTIInit();
	EIE3810_KeyUp_EXTIInit();
	DirX[0] = -9; DirX[1] = -7; DirX[2] = -5; DirX[3] = -4;
	DirX[7] = 9; DirX[6] = 7; DirX[5] = 5; DirX[4] = 4;
	DirY[0] = DirY[7] = -5; DirY[1] = DirY[6] = -7; DirY[2] = DirY[5] = -9; DirY[3] = DirY[4] = -9;
	COLORS[0] = BLACK; COLORS[1] = BLUE; COLORS[2] = GREEN; COLORS[3] = RED;
	
	/* game loop */
	while (1){
		
		/* in-game initialization */
		LEVEL_SELECTED = 0;
		BALL_SELECTED = 0;
		OBS_SELECTED = 0;
		COLOR = 0;
		PAUSED = 0;
		SIZE = 0;
		LEVEL = 0;
		OBS = 0;
		DIRECTION_SETTED = 0;
		GAMEEND = 0;
		GAMEBEGIN = 0;
		BUZZER_ON = 0;
		TIME = 0;
		WINNER = 0;
		ROUND = 0;
		Welcome();
		SelectDifficulty();
		SelectBallSize();
		SelectObstacle();
		GetDirection();
		Game();
		EndScreen();
	}
}
/*------------------------------------------------------------------*/
/*Game steps*/

void Welcome(void){
	EIE3810_TFTLCD_DrawBackground(WHITE);
	EIE3810_TFTLCD_ShowCharLine2412(ROW_WELCOME, COL_WELCOME, "Welcome to mini project!", WHITE, BLUE);\
	Delay(10000000);
	EIE3810_TFTLCD_ShowCharLine(ROW_WELCOME+24, COL_WELCOME+48, "This is the final lab", WHITE, RED);
	Delay(10000000);
	EIE3810_TFTLCD_ShowCharLine(ROW_WELCOME+24, COL_WELCOME+80, "Are you ready?", WHITE, RED);
	Delay(10000000);
	EIE3810_TFTLCD_ShowCharLine(ROW_WELCOME+24, COL_WELCOME+112, "OK. Let's start!", WHITE, RED);
	Delay(20000000);
}

void SelectDifficulty(void){
	EIE3810_TFTLCD_DrawBackground(WHITE);
	EIE3810_TFTLCD_ShowCharLine2412(ROW_SELECT-16, COL_SELECT, "Please select the difficulty level!", WHITE, RED);
	EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+48, "EASY", WHITE, BLUE);
	EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+80, "HARD", BLUE, WHITE);
	EIE3810_TFTLCD_ShowCharLine2412(ROW_SELECT-16, COL_SELECT+112, "Press KEY0 to enter!", WHITE, RED);
	
	while (!LEVEL_SELECTED){ // wait for selecting level
		;
	}
}

void GetDirection(void){
	/* recieve the direction signal from USART1 */
	EIE3810_TFTLCD_DrawBackground(WHITE);
	EIE3810_TFTLCD_ShowCharLine2412(ROW_SELECT, COL_SELECT, "Use UASRT for a random direction.", WHITE, RED);
	EIE3810_USART1_init(72, 38462); // set unique baud rate
	EIE3810_USART1_EXTIInit();
	while (!DIRECTION_SETTED){ // wait for receiving direction
		;
	}
	// print the direction
	EIE3810_TFTLCD_ShowCharLine(ROW_SELECT+16, COL_SELECT+36, "The direction is: ", WHITE, RED);
	EIE3810_TFTLCD_ShowChar(ROW_SELECT+16+8*18, COL_SELECT+36, 48+DIRECTION, WHITE, RED);
	Delay(10000000);
}

void SelectBallSize(void){
	EIE3810_TFTLCD_DrawBackground(WHITE);
	EIE3810_TFTLCD_ShowCharLine2412(ROW_SELECT-16, COL_SELECT, "Please select the ball size!", WHITE, RED);
	EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+48, "SMALL", WHITE, BLUE);
	EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+80, "BIG", BLUE, WHITE);
	EIE3810_TFTLCD_ShowCharLine2412(ROW_SELECT-16, COL_SELECT+112, "Press KEY0 to enter!", WHITE, RED);
	
	while (!BALL_SELECTED){ // wait for selecting ball
		;
	}
}

void SelectObstacle(void){
	EIE3810_TFTLCD_DrawBackground(WHITE);
	EIE3810_TFTLCD_ShowCharLine2412(ROW_SELECT-16, COL_SELECT, "Do you want obstacle in game?", WHITE, RED);
	EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+48, "NO", WHITE, BLUE);
	EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+80, "YES", BLUE, WHITE);
	EIE3810_TFTLCD_ShowCharLine2412(ROW_SELECT-16, COL_SELECT+112, "Press KEY0 to enter!", WHITE, RED);
	
	while (!OBS_SELECTED){ // wait for selecting obstacle
		;
	}
}

void Game(void){
	// 3 second count-down
	EIE3810_TFTLCD_DrawBackground(WHITE);
	EIE3810_TFTLCD_SevenSegment(212, 330, 3, RED);
	Delay(10330580);
	EIE3810_TFTLCD_DrawBackground(WHITE);
	EIE3810_TFTLCD_SevenSegment(212, 330, 2, RED);
	Delay(10330580);
	EIE3810_TFTLCD_DrawBackground(WHITE);
	EIE3810_TFTLCD_SevenSegment(212, 330, 1, RED);
	Delay(10330580);
	EIE3810_TFTLCD_DrawBackground(WHITE);
	EIE3810_TFTLCD_SevenSegment(212, 330, 0, RED);
	Delay(10330580);
	EIE3810_TFTLCD_DrawBackground(WHITE);

	// invoke ball game routine
	GAMEBEGIN = 1;
	EIE3810_SYSTICK_Init10ms();
	EIE3810_TIM4_Init(4999, 14399); // Initialize TIM3, 4Hz
	// initialize the ball
	ballColor = COLORS[0];
	speedX_s = DirX[DIRECTION];
	speedY_s = -DirY[DIRECTION];
	speedX = DirX[DIRECTION];
	speedY = DirY[DIRECTION];
	if (LEVEL == EASY) ballPeriod = 5;
	else ballPeriod = 3;
	if (SIZE == SMALL) radius = 15;
	else radius = 20;
	ballX = 240;
	ballY = 800-40-radius;
	EIE3810_TFTLCD_DrawCircle(ballX, ballY, radius, 1, ballColor);
	// initialize the obstacle
	if (OBS == 1){
		// initialize the obstacle
		obsColor = COLORS[0];
		obsWidth = 160;
		obsHeight = 20;
		obsX = 240;
		obsY = 400;
		obsSpeed = 10;
		if (DIRECTION >= 3) obsVelo = -obsSpeed;
		else obsVelo = obsSpeed;
		EIE3810_TFTLCD_FillRectangle(obsX-obsWidth/2, obsWidth, obsY-obsHeight/2, obsHeight, obsColor);
	}
	// initialize the pad
	EIE3810_TIM3_Init(4999, 71); // tim3 100 Hz
	JOYPAD_Init();
	padColorA = padColorB = COLORS[0];
	speedPad = 1;
	padHeight = 12;
	padWidth = 80;
	padX_A = padX_B = 240;
	EIE3810_TFTLCD_FillRectangle(padX_A-padWidth/2, padWidth, A_Y-padHeight/2, padHeight, padColorA);// draw pads
	EIE3810_TFTLCD_FillRectangle(padX_B-padWidth/2, padWidth, B_Y-padHeight/2, padHeight, padColorB);
	// into the ball game
	while (GAMEBEGIN && !GAMEEND){
		if (PAUSED) continue; // pause game, do nothing in the loop
		if (task2HeartBeat >= 5){
			task2HeartBeat = 1;
			// move the pad
			MovePad();
		}
		
		if (task1HeartBeat >= ballPeriod){
			task1HeartBeat = 1;
			// move the ball
			MoveBall();
		}
		
		if (task3HeartBeat >= 10){
			task3HeartBeat = 1;
			// move the obstacle
			MoveObs();
		}
		TriggerBuzzer();
		DisplayTime(50, 400);
	}
}
void EndScreen(void){
	// print winning message
	EIE3810_TFTLCD_DrawBackground(WHITE);
	Delay(10000000);
	EIE3810_TFTLCD_ShowCharLine2412(120, 388, "Player ", BLUE, YELLOW);
	if (WINNER) EIE3810_TFTLCD_ShowChar2412(192, 388, 'A', BLUE, YELLOW);
	else EIE3810_TFTLCD_ShowChar2412(192, 388, 'B', BLUE, YELLOW);
	EIE3810_TFTLCD_ShowCharLine2412(204, 388, " wins", BLUE, YELLOW);
	Delay(50000000);
}
/*------------------------------------------------------------------*/

// move the obstacle in the game
void MoveObs(void){
	EIE3810_TFTLCD_FillRectangle(obsX-obsWidth/2, obsWidth, obsY-obsHeight/2, obsHeight, WHITE);
	if (obsX <= 80){
		obsVelo = obsSpeed;
	}
	else if (obsX >= 400){
		obsVelo = -obsSpeed;
	}
	obsX += obsVelo;
	// draw obstacle
	EIE3810_TFTLCD_FillRectangle(obsX-obsWidth/2, obsWidth, obsY-obsHeight/2, obsHeight, obsColor);
}

// move the pad (player A) in the screen
void MovePad(void){
	// move the player A pad with GPIO input
	u32 pressK2 = (GPIOE->IDR>>2 & 1);
	u32 pressK0 = (GPIOE->IDR>>4 & 1);
	if (pressK2 == 0){
		u16 tempX = padX_A - speedPad*15;
		if (tempX <= 40) {
			tempX = 40;
		}
		EIE3810_TFTLCD_FillRectangle(padX_A-padWidth/2, padWidth, A_Y-padHeight/2, padHeight, WHITE);
		padX_A = tempX; // move leftwards
		// draw the player A pad
		EIE3810_TFTLCD_FillRectangle(padX_A-padWidth/2, padWidth, A_Y-padHeight/2, padHeight, padColorA);
	}
	else if (pressK0 == 0){
		u16 tempX = padX_A + speedPad*15;
		if (tempX >= 440) {
			tempX = 440;
		}
		EIE3810_TFTLCD_FillRectangle(padX_A-padWidth/2, padWidth, A_Y-padHeight/2, padHeight, WHITE);
		padX_A = tempX; // move leftwards
		// draw the player A pad
		EIE3810_TFTLCD_FillRectangle(padX_A-padWidth/2, padWidth, A_Y-padHeight/2, padHeight, padColorA);
	}
}

// move the ball in the screen
void MoveBall(void){
	BUZZER_ON = 0;
	EIE3810_TFTLCD_DrawCircle(ballX, ballY, radius, 1, WHITE); // clear ball
	// check bounds
	if (CheckBound_Ball()){
		COLOR ++;
		COLOR = COLOR % 4;
		ballColor = COLORS[COLOR];
		speedX = -speedX;
		BUZZER_ON = 1;
	}
	if (CheckPad_Ball()){
		if (ballY < 400) { // B
			COLOR ++;
			COLOR = COLOR % 4;
			ballColor = COLORS[COLOR];
			padColorB = ballColor;
			speedY = speedY_s;
		}
		else { // A
			COLOR ++;
			COLOR = COLOR % 4;
			ballColor = COLORS[COLOR];
			padColorA = ballColor;
			speedY = -speedY_s;
		}
		BUZZER_ON = 1;
		ROUND ++;
	}
	if (CheckObs_Ball()){
		if (ballY < 400) speedY = -speedY_s;
		else speedY = speedY_s;
		COLOR ++;
		COLOR = COLOR % 4;
		ballColor = COLORS[COLOR];
		obsColor = ballColor;
		BUZZER_ON = 1;
	}
	if (CheckEnd_Ball()){
		GAMEEND = 1;
		Buzzer_Toggle(1);
		if (ballY > 400) WINNER = 0; // B
		else WINNER = 1; // A
		return;
	}
	// move ball
	ballX += speedX;
	ballY += speedY;
	EIE3810_TFTLCD_DrawCircle(ballX, ballY, radius, 1, ballColor);
}

// check if the ball hit the bound (left and right)
// 0: no hit, 1: hit left or right boundary
u8 CheckBound_Ball(void){
	return (ballX >= 480-radius || ballX<=radius);
}

// check if the ball hit the pad
u8 CheckPad_Ball(void){
	int comp;
	if (ballY >= A_Y-radius-6){
		comp = ballX-padX_A;
		return (comp <= padWidth/2 && comp >= -padWidth/2);
	}
	if (ballY<=B_Y+radius+6){
		comp = ballX-padX_B;
		return (comp <= padWidth/2 && comp >= -padWidth/2);
	}
	return 0;
}

// check if the ball hit the obstacle
u8 CheckObs_Ball(void){
	int comp;
	if (ballY <= obsY+15+radius && ballY >= obsY-15-radius){
		comp = ballX - obsX;
		return (comp <= 80 && comp >= -80);
	}
	return 0;
}

// check if the ball hit the upper or lower bound
// 0: no hit, 1: hit
// 1: hit -> win
u8 CheckEnd_Ball(void){
	return (ballY <= radius+1 || ballY>=800-radius-1);
}

// trigger the buzzer if the BUZZER_ON is set
void TriggerBuzzer(void){
	if (BUZZER_ON && !GAMEEND && !PAUSED){
		Buzzer_Toggle(0);
	}
	else {
		Buzzer_Toggle(1);
	}
}

// display the time during the runtime
void DisplayTime(u16 x, u16 y){
	u32 minute, second;
	u8 min1, min0, sec1, sec0;
	if (ROUND >= 100) ROUND = 0;
	minute = TIME / 60;
	second = TIME % 60;
	min1 = minute / 10;
	min0 = minute % 10;
	sec1 = second / 10;
	sec0 = second % 10;
	EIE3810_TFTLCD_ShowChar(x, y, 48+min1, BLUE, WHITE);
	EIE3810_TFTLCD_ShowChar(x+8, y, 48+min0, BLUE, WHITE);
	EIE3810_TFTLCD_ShowCharLine(x+16, y, "min", BLUE, WHITE);
	EIE3810_TFTLCD_ShowChar(x+40, y, 48+sec1, BLUE, WHITE);
	EIE3810_TFTLCD_ShowChar(x+48, y, 48+sec0, BLUE, WHITE);
	EIE3810_TFTLCD_ShowCharLine(x+56, y, "s", BLUE, WHITE);
	EIE3810_TFTLCD_ShowCharLine(x+16, y+16, "round", BLUE, WHITE);
	EIE3810_TFTLCD_ShowChar(x+56, y+16, 48+ROUND/10, BLUE, WHITE);
	EIE3810_TFTLCD_ShowChar(x+64, y+16, 48+ROUND%10, BLUE, WHITE);
}

void EXTI0_IRQHandler(void){ // KeyUp
	if (!LEVEL_SELECTED){
		// change level (easy)
		LEVEL = EASY;
		EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+48, "EASY", WHITE, BLUE);
		EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+80, "HARD", BLUE, WHITE);
	}
	else if (!BALL_SELECTED){
		// change the ball size
		SIZE = 0; // small
		EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+48, "SMALL", WHITE, BLUE);
		EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+80, "BIG", BLUE, WHITE);
	}
	else if (!OBS_SELECTED){
		// change the obstacle setting
		OBS = 0; // no obstacle
		EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+48, "NO", WHITE, BLUE);
		EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+80, "YES", BLUE, WHITE);
	}
	EXTI->PR = 1<<0; // EXTI2: clear pending register
}
void EXTI2_IRQHandler(void){ // Key2s
	EXTI->PR = 1<<2; // EXTI2: clear pending register
}
void EXTI3_IRQHandler(void){ // Key1
	if (!LEVEL_SELECTED){
		// change level (hard)
		LEVEL = HARD;
		EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+48, "EASY", BLUE, WHITE);
		EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+80, "HARD", WHITE, BLUE);
	}
	else if (!BALL_SELECTED){
		// change the ball size
		SIZE = 1; // big
		EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+48, "SMALL", BLUE, WHITE);
		EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+80, "BIG", WHITE, BLUE);
	}
	else if (!OBS_SELECTED){
		// change the obstacle setting
		OBS = 1; // has obstacle
		EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+48, "NO", BLUE, WHITE);
		EIE3810_TFTLCD_ShowCharLine(ROW_SELECT-16, COL_SELECT+80, "YES", WHITE, BLUE);
	}
	else if (GAMEBEGIN && !GAMEEND){
		if (!PAUSED) {
			PAUSED = 1;
			Delay(8000000);
		}
		else {
			PAUSED = 0;
			Delay(8000000);
		}
	}
	EXTI->PR = 1<<3; // EXTI2: clear pending register
}
void EXTI4_IRQHandler(void){ // Key0
	// select the level in the menu screen
	if (!LEVEL_SELECTED) {
		LEVEL_SELECTED = 1;
		Delay(10000000);
	}
	else if (LEVEL_SELECTED && !BALL_SELECTED) {
		BALL_SELECTED = 1;
		Delay(10000000);
	}
	else if (LEVEL_SELECTED && BALL_SELECTED && !OBS_SELECTED) {
		OBS_SELECTED = 1;
		Delay(10000000);
	}
	EXTI->PR = 1<<4; // EXTI2: clear pending register
}

// read the direction indicator from the python program
void USART1_IRQHandler(void){
	u8 input;
	if (USART1->SR & (1<<5) && !DIRECTION_SETTED) // RXNE: read data register not empty
	{
		input = USART1->DR; // the data recieved by UASRT1
		DIRECTION = input; // set the direction indicator
		DIRECTION_SETTED = 1;
		//EIE3810_TFTLCD_ShowChar(100, 100, 48+input, BLACK, YELLOW);
	}
	
}

// use TIM3 to control the koypad
void TIM3_IRQHandler(void){
	u8 output;
	if (GAMEBEGIN && !GAMEEND){
		if (TIM3->SR & 1<<0) // update interrupt pending
		{
			output = JOYPAD_Read(); // read output
			if (output > 0){
				if (output == 1<<3) { // START button
					if (!PAUSED) PAUSED = 1; // set pause
					else PAUSED = 0; // continue
					Delay(8000000);
				}
				if (output == 1<<6 && !PAUSED) { // left(x-)
					// move the Player B pad left (x-)
					u16 tempX = padX_B - speedPad*2;
					if (tempX <= 40) {
						tempX = 40;
					}
					EIE3810_TFTLCD_FillRectangle(padX_B-padWidth/2, padWidth, B_Y-padHeight/2, padHeight, WHITE);
					padX_B = tempX; // move leftwards
					// draw the player A pad
					EIE3810_TFTLCD_FillRectangle(padX_B-padWidth/2, padWidth, B_Y-padHeight/2, padHeight, BLACK);
				}
				else if (output == 1<<7 && !PAUSED) { // right (x+)
					// move the Player B pad left (x-)
					u16 tempX = padX_B + speedPad*2;
					if (tempX >= 440) {
						tempX = 440;
					}
					EIE3810_TFTLCD_FillRectangle(padX_B-padWidth/2, padWidth, B_Y-padHeight/2, padHeight, WHITE);
					padX_B = tempX; // move leftwards
					// draw the player A pad
					EIE3810_TFTLCD_FillRectangle(padX_B-padWidth/2, padWidth, B_Y-padHeight/2, padHeight, padColorB);
				}
			}
			JOYPAD_Delay(80);
		}
	}
	TIM3->SR &= ~(1<<0); 
}

void TIM4_IRQHandler(void){
	if (!GAMEEND && GAMEBEGIN && !PAUSED){
		TIME ++;
	}
	TIM4->SR &= ~(1<<0); 
}

/*=================================================================================================*/
/* JoyPad functions*/
void JOYPAD_Init(void){
	RCC->APB2ENR |= 1<<3; // IO port B clock enable
	RCC->APB2ENR |= 1<<5; // IO port D clock enable
	GPIOB->CRH &= 0xFFFF00FF;
	GPIOB->CRH |= 0x00003800; // GPIOB10: input pull -up /down, GPIOx11: ouput, general purpose push pull
	GPIOB->ODR |= 3<<10; // port output data, 10 and 11
	GPIOD->CRL &= 0xFFFF0FFF; 
	GPIOD->CRL |= 0x00003000; // GPIOD3 ouput mode, general purpose push pull
	GPIOD->ODR |= 1<<3; // port output data, 3
}

void JOYPAD_Delay(u16 t){
	while (t--);
}

u8 JOYPAD_Read(void){
	vu8 temp=0;
	u8 t;
	GPIOB->BSRR |= 1<<11; // set ODR11 bit of PB11
	Delay(80);
	GPIOB->BSRR |= 1<<27; // reset ODR11 bit of PB11
	for (t=0; t<8; t++)
	{
		temp>>=1;
		if((((GPIOB->IDR)>>10)&0x01)==0) temp |= 0x80;
		// read input of PB10, if it is 1, set temo to 0x80
		GPIOD->BSRR |= (1<<3); // set ODR3 of PD3
		Delay(80);
		GPIOD->BSRR |= (1<<19); // set ODR3 of PD3 
		Delay(80);
	}
	return temp;
}
