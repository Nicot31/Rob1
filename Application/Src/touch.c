/**
 ******************************************************************************
 * File Name          : TOUCH.c
 * Description        : This file provides code for the touch screen
 * 						Touch Screen Driver : XPT2046
 ******************************************************************************
 */
/* Includes -----------------------------------------------------------------*/
#include "main.h"
#include "touch.h"
#include "lcd.h"
#include "config.h"
#include "board.h"

/* Constants ----------------------------------------------------------------*/

#define XPT2046_READ_X 		0xD0
#define XPT2046_READ_Y 		0x90

#define XPT2046_HOR_RES     320
#define XPT2046_VER_RES     240
#define XPT2046_MIN       	200
#define XPT2046_MAX	       	3800

#define SORT(a,b) { if ((a)>(b)) SWAP((a),(b)); }
#define SWAP(a,b) { int temp=(a);(a)=(b);(b)=temp; }

/* Global Variables ---------------------------------------------------------*/
extern SPI_HandleTypeDef hspi2;
TouchScreen touch;


/* Functions ------------------------------------------------------------------*/
void TouchReset() {
	uint8_t Cmd;
	uint8_t Data;

	// Reset
	Cmd = 0x80;
	HAL_SPI_TransmitReceive(&hspi2, &Cmd, &Data, 1, 1000);
	Cmd = 0x00;
	HAL_SPI_TransmitReceive(&hspi2, &Cmd, &Data, 1, 1000);
	Cmd = 0x00;
	HAL_SPI_TransmitReceive(&hspi2, &Cmd, &Data, 1, 1000);
	HAL_Delay(5);

	// read configuration
	touch.xOffsetTouch = ConfigRead16(TOUCH_MIN, POS16_0);
	touch.yOffsetTouch = ConfigRead16(TOUCH_MIN, POS16_1);
	touch.widthTouch = ConfigRead16(TOUCH_SIZE, POS16_0);
	touch.heightTouch = ConfigRead16(TOUCH_SIZE, POS16_1);
}

/*---------------------------------------------------------------------------*/
void TouchSetOrientation(int w, int h, int o) {

	touch.orientation = o;
	touch.widthLCD = w;
	touch.heightLCD = h;
}

/*---------------------------------------------------------------------------*/
int _TouchGet(uint8_t address) {
	int val;
	uint8_t Cmd = 0x00;
	uint8_t Data;

	// Reset
	HAL_SPI_TransmitReceive(&hspi2, &address, &Data, 1, 1000);
	HAL_SPI_TransmitReceive(&hspi2, &Cmd, &Data, 1, 1000);
	val = (uint16_t) Data << 8;
	HAL_SPI_TransmitReceive(&hspi2, &Cmd, &Data, 1, 1000);
	val |= Data;
	val >>= 3;
	return val;
}

/*---------------------------------------------------------------------------*/
void TouchGetXY(int *x, int *y) {

	*x = _TouchGet(XPT2046_READ_X);
	*y = _TouchGet(XPT2046_READ_Y);

	if ((touch.orientation == LCD_LANDSCAPE_1) || (touch.orientation == LCD_LANDSCAPE_2)) {
		SWAP(*x, *y);
	}
}

/*---------------------------------------------------------------------------*/
int _TouchFastMedian(int *samples) {

	// do a fast median selection (reference code available on internet). This code basically
	// avoids sorting the entire samples array

	SORT(samples[0], samples[5]);
	SORT(samples[0], samples[3]);
	SORT(samples[1], samples[6]);
	SORT(samples[2], samples[4]);
	SORT(samples[0], samples[1]);
	SORT(samples[3], samples[5]);
	SORT(samples[2], samples[6]);
	SORT(samples[2], samples[3]);
	SORT(samples[3], samples[6]);
	SORT(samples[4], samples[5]);
	SORT(samples[1], samples[4]);
	SORT(samples[1], samples[3]);
	SORT(samples[3], samples[4]);

	return samples[3];
}

/*---------------------------------------------------------------------------*/
bool _TouchCorrection(int *x, int *y) {
	*x -= touch.xOffsetTouch;
	*y -= touch.yOffsetTouch;

	*x = ((*x) * touch.widthLCD) / touch.widthTouch;
	*y = ((*y) * touch.heightLCD) / touch.heightTouch;

	switch (touch.orientation) {
	case LCD_PORTRAIT_1:
		*y = touch.heightLCD - *y;
		break;
	case LCD_PORTRAIT_2:
		*x = touch.widthLCD - *x;
		break;
	case LCD_LANDSCAPE_1:
		*x = touch.widthLCD - *x;
		*y = touch.heightLCD - *y;
		break;
	case LCD_LANDSCAPE_2:
		break;
	}

	if ((*x < 0) || (*y < 0) || (*x > touch.widthLCD) || (*y > touch.heightLCD))
		return false;
	return true;
}

/*---------------------------------------------------------------------------*/
bool TouchGetXYMedian(int *x, int *y) {
	int i;
	int tab_x[7], tab_y[7];

	for (i = 0; i < 7; i++) {
		TouchGetXY(&tab_x[i], &tab_y[i]);
	}

	*x = _TouchFastMedian(tab_x);
	*y = _TouchFastMedian(tab_y);
	return _TouchCorrection(x, y);
}

/*---------------------------------------------------------------------------*/
bool TouchIsPenDown() {
	return (HAL_GPIO_ReadPin(PEN_INT_GPIO_Port, PEN_INT_Pin) == GPIO_PIN_RESET);
}

/*---------------------------------------------------------------------------*/
void TouchWaitPenUp() {
	while (TouchIsPenDown())
		;
}

/*---------------------------------------------------------------------------*/
void TouchWaitPenDown() {
	while (!TouchIsPenDown())
		;
}
