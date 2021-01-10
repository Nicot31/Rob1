/**
 ******************************************************************************
 * File Name          : LCD.c
 * Description        : This file provides code for the LCD driver
 * 						TFT LCD Driver : ILI9341
 ******************************************************************************
 */
/* Includes -----------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "lcd.h"
#include "touch.h"
//#include "config.hpp"
//#include "util.hpp"

/* Constants ----------------------------------------------------------------*/
// Address
#define LCD_CMD      (*((volatile uint16_t *) 0x60000000))
#define LCD_PARAM    (*((volatile uint16_t *) 0x60080000))

// Regulative Command Set
#define LCD_NOP                 0x00
#define LCD_RESET				0x01
#define LCD_R_DISP_INFO         0x04
#define LCD_R_DISP_STATUS       0x09
#define LCD_R_DSIP_POWER_MODE   0x0A
#define LCD_R_DISP_MADCTL       0x0B
#define LCD_R_PIXEL_FORMAT      0x0C
#define LCD_R_IMAGE_FORMAT      0x0D
#define LCD_R_DISP_SIGNAL_MODE  0x0E
#define LCD_R_SELF_DIAG_RESULT  0X0F
#define LCD_ENTER_SLEEP_MODE    0x10
#define LCD_SLEEP_OUT			0x11
#define LCD_PARTIAL_MODE_ON     0x12
#define LCD_NORMAL_DISP_ON      0x13
#define LCD_DISP_INVERSION_OFF  0x20
#define LCD_DISP_INVERSION_ON   0x21
#define LCD_GAMMA_SET			0x26
#define LCD_DISP_OFF			0x28
#define LCD_DISP_ON 			0x29
#define LCD_COLUMN_ADDR_SET		0x2A
#define LCD_PAGE_ADDR_SET		0x2B
#define LCD_MEMORY_WRITE        0x2C
#define LCD_COLOR_SET           0x2D
#define LCD_MEMRY_READ          0x2E
#define LCD_PARTIAL_AREA        0x30
#define LCD_VERT_SCROLL_DEF     0x33
#define LCD_TEARING_LINE_OFF    0x34
#define LCD_TEARING_LINE_ON     0x35
#define LCD_MEMORY_ACCESS_CTRL  0x36
#define LCD_VERT_SCROLL_ADR     0x37
#define LCD_IDLE_MODE_OFF       0x38
#define LCD_IDLE_MODE_ON        0x39
#define LCD_PIXEL_FORMAT		0x3A
#define LCD_W_MEM_CONTINUE      0x3C
#define LCD_R_MEM_CONTINUE      0x3E
#define LCD_SET_TEAR_SCANLINE   0x44
#define LCD_GET_SCANLINE        0x45
#define LCD_W_DISP_BRIGHTNESS	0x51
#define LCD_R_DISP_BRIGHTNESS   0x52
#define LCD_W_CTRL_DISP         0x53
#define LCD_R_CTRL_DISP         0x54
#define LCD_W_ADAPT_BRIGHT      0x55
#define LCD_R_ADAPT_BRIGHT      0x56
#define LCD_W_CBAC_MIN_BRIGHT   0x5E
#define LCD_R_CBAC_MIN_BRIGHT   0x5F
#define LCD_R_ID1               0xDA
#define LCD_R_D2                0xDB
#define LCD_R_D3                0xDC

// Extended Command Set
#define LCD_RGB_SIGNAL_CTRL     0xB0
#define LCD_FRAME_CTRL_NORMAL   0xB1
#define LCD_FRAME_CTRL_IDLE     0xB2
#define LCD_FRAME_CTRL_PARTIAL  0xB3
#define LCD_DISP_INV_CTRL       0xB4
#define LCD_BLANK_PORCH_CTRL    0xB5
#define LCD_DISP_FCT_CTRL       0xB6
#define LCD_ENTER_MODE_SET      0xB7
#define LCD_BACKLIGHT_CTRL_1    0xB8
#define LCD_BACKLIGHT_CTRL_2    0xB9
#define LCD_BACKLIGHT_CTRL_3    0xBA
#define LCD_BACKLIGHT_CTRL_4    0xBB
#define LCD_BACKLIGHT_CTRL_5    0xBC
#define LCD_BACKLIGHT_CTRL_6    0xBD
#define LCD_BACKLIGHT_CTRL_7    0xBE
#define LCD_BACKLIGHT_CTRL_8    0xBF
#define LCD_POWER_CTRL_1        0xC0
#define LCD_POWER_CTRL_2        0xC1
#define LCD_VCOM_CTRL_1         0xC5
#define LCD_VCOM_CTRL_2         0xC7
#define LCD_NV_MEM_W            0xD0
#define LCD_NV_MEM_PROT_KEY     0xD1
#define LCD_NV_MEM_STATUS_R     0xD2
#define LCD_R_ID4               0xD3
#define LCD_POS_GAMMA_CORRECT   0xE0
#define LCD_NEG_GAMMA_CORRECT   0xE1
#define LCD_DIGIT_GAMMA_CTRL_1  0xE2
#define LCD_DIGIT_GAMMA_CTRL_2  0xE3
#define LCD_INTERFACE_CTRL      0xF6

#define SWAP(a,b) { int temp=(a);(a)=(b);(b)=temp; }

/* Global Variables ---------------------------------------------------------*/

LcdScreen lcd;

/*---------------------------------------------------------------------------*/
void LcdInit() {
	LCD_CMD = LCD_RESET;
	HAL_Delay(120);       // wait 500ms

	LcdSleepOff();
	HAL_Delay(120);

	LCD_CMD = LCD_PIXEL_FORMAT;
	LCD_PARAM = 0x55;   // 16bits / pixel

	LcdSetOrientation(LCD_LANDSCAPE_1);
	//SetFont(Font_7x10);

	LcdDisplayOn();
}

/*---------------------------------------------------------------------------*/
void LcdSetOrientation(int orientation) {
	uint16_t Param;

	switch (orientation) {
	case LCD_PORTRAIT_1:
		lcd.orientation = LCD_PORTRAIT_1;
		lcd.height = LCD_WIDTH;
		lcd.width = LCD_HEIGHT;
		Param = 0x88;
		break;
	case LCD_PORTRAIT_2:
		lcd.orientation = LCD_PORTRAIT_2;
		lcd.height = LCD_WIDTH;
		lcd.width = LCD_HEIGHT;
		Param = 0x58;
		break;
	case LCD_LANDSCAPE_1:
		lcd.orientation = LCD_LANDSCAPE_1;
		lcd.height = LCD_HEIGHT;
		lcd.width = LCD_WIDTH;
		Param = 0xE8;
		break;
	case LCD_LANDSCAPE_2:
		lcd.orientation = LCD_LANDSCAPE_2;
		lcd.height = LCD_HEIGHT;
		lcd.width = LCD_WIDTH;
		Param = 0x28;
		break;
	default:
		return;
	}
	LCD_CMD = LCD_MEMORY_ACCESS_CTRL;
	LCD_PARAM = Param;
	TouchSetOrientation(lcd.width, lcd.height, lcd.orientation);
}



/*---------------------------------------------------------------------------*/
void LcdSleepOn() {
	LCD_CMD = LCD_ENTER_SLEEP_MODE;
}

void LcdSleepOff() {
	LCD_CMD = LCD_SLEEP_OUT;
}

void LcdDisplayOn() {
	LCD_CMD = LCD_DISP_ON;
}

void LcdDisplayOff() {
	LCD_CMD = LCD_DISP_OFF;
}

void LcdIdleModeOn() {
	LCD_CMD = LCD_IDLE_MODE_ON;
}

void LcdIdleModeOff() {
	LCD_CMD = LCD_IDLE_MODE_OFF;
}

void LcdBrightness(int val) {
	LCD_CMD = LCD_W_DISP_BRIGHTNESS;
	LCD_PARAM = (uint16_t) val & 0xFF;
}


/*---------------------------------------------------------------------------*/
int LcdComposeColor(uint8_t red, uint8_t green, uint8_t blue) {
	uint16_t co;
	// rgb16 = red5 green6 blue5
	co = (red >> 3) << 11 | (green >> 2) << 5 | (blue >> 3);
	return (int) co;
}

/*---------------------------------------------------------------------------*/
void LcdWriteMem(uint16_t *mem, int size) {
	LCD_CMD = LCD_MEMORY_WRITE;
	while (size--) {
		LCD_PARAM = *mem++;
	}
}


/*---------------------------------------------------------------------------*/
void LcdSetFrame(int x1, int y1, int x2, int y2) {
	LCD_CMD = LCD_COLUMN_ADDR_SET;
	LCD_PARAM = (uint16_t) x1 >> 8;
	LCD_PARAM = (uint16_t) x1 & 0xFF;
	LCD_PARAM = (uint16_t) x2 >> 8;
	LCD_PARAM = (uint16_t) x2 & 0xFF;
	LCD_CMD = LCD_PAGE_ADDR_SET;
	LCD_PARAM = (uint16_t) y1 >> 8;
	LCD_PARAM = (uint16_t) y1 & 0xFF;
	LCD_PARAM = (uint16_t) y2 >> 8;
	LCD_PARAM = (uint16_t) y2 & 0xFF;
}

void LcdSetCursor(int x, int y) {
	LcdSetFrame(x, y, x, y);
}

/*---------------------------------------------------------------------------*/
/*  DRAW																  	 */
/*---------------------------------------------------------------------------*/
void LcdDrawPixel(UG_S16 x, UG_S16 y, UG_COLOR color) {
	LcdSetFrame(x, y, x, y);
	LCD_CMD = LCD_MEMORY_WRITE;
	LCD_PARAM = (uint16_t) color;
}


/*---------------------------------------------------------------------------*/
UG_RESULT HW_DrawLine(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c) {
	if (y1 == y2) {
		_LcdHLine(x1, x2, y1, c);
		return UG_RESULT_OK;
	} else if (x1 == x2) {
		_LcdVLine(x1, y1, y2, c);
		return UG_RESULT_OK;
	}
	return UG_RESULT_FAIL;
}

void _LcdHLine(int x1, int x2, int y, int color) {
	if (x2 < x1)
		SWAP(x1, x2);
	LcdSetFrame(x1, y, x2, y);
	LCD_CMD = LCD_MEMORY_WRITE;
	while (x2-- >= x1) {
		LCD_PARAM = (uint16_t) color;
	}
}

void _LcdVLine(int x, int y1, int y2, int color) {
	if (y2 < y1)
		SWAP(y1, y2);
	LcdSetFrame(x, y1, x, y2);
	LCD_CMD = LCD_MEMORY_WRITE;
	while (y2-- >= y1) {
		LCD_PARAM = (uint16_t) color;
	}
}



/*---------------------------------------------------------------------------*/
UG_RESULT HW_FillFrame (UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c) {
	uint32_t size;
	LcdSetFrame(x1, y1, x2, y2);
	size = (x2 - x1 + 1) * (y2 - y1 + 1);
	LCD_CMD = LCD_MEMORY_WRITE;
	while (size--) {
		LCD_PARAM = (uint16_t) c;
	}
	return UG_RESULT_OK;
}

#if 0
/*---------------------------------------------------------------------------*/
void LcdScreen::Rect(int x1, int y1, int x2, int y2, int color) {
	HLine(x1, x2, y1, color);
	HLine(x1, x2, y2, color);
	VLine(x1, y1, y2, color);
	VLine(x2, y1, y2, color);
}

void LcdScreen::Rect(Point &p1, Point &p2, int color) {
	Rect(p1.x, p1.y, p2.x, p2.y, color);
}

void LcdScreen::Rect(Rectangle &rect, int color) {
	Rect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y, color);
}



void LcdScreen::FillRect(Point &p1, Point &p2, int color) {
	FillRect(p1.x, p1.y, p2.x, p2.y, color);
}

void LcdScreen::FillRect(Rectangle &rect, int color) {
	FillRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y, color);
}

/*---------------------------------------------------------------------------*/
void LcdScreen::FillScreen(int color) {
	FillRect(0, 0, width, height, color);
}
#endif
#if 0
/*---------------------------------------------------------------------------*/
/*  CALIBRATE																 */
/*---------------------------------------------------------------------------*/
void _LcdDrawCrossHair(Point &p, int color) {
	Rect(p.x - 10, p.y - 10, p.x + 10, p.y + 10, color);
	HLine(p.x - 5, p.x + 5, p.y, color);
	VLine(p.x, p.y - 5, p.y + 5, color);
}

bool _LcdReadTestPoint(int *px, int *py) {
	int cpt = 0;
	int tx, sumX = 0;
	int ty, sumY = 0;

	lcd.SetFontColor(COLOR_WHITE);
	TextPutsCenterXY("     >   PRESS   <     ");
	touch.WaitPenDown();
	TextPutsCenterXY("     =>  HOLD!  <=     ");
	while ((touch.IsPenDown()) && (cpt < 5000)) {
		touch.GetXY(&tx, &ty);
		sumX += tx;
		sumY += ty;
		cpt++;
	}
	if (cpt < 5000)
		return false;
	p.x = sumX / 5000;
	p->y = sumY / 5000;
	TextPutsCenterXY("     ==> DONE! <==     ");
	return true;
}

void LcdScreen::Calibrate() {
	Point point[8];
	Point res[8];
	Point mesMin, mesMax;
	int txMin, tyMin;		// Touch min
	int tWidth, tHeight;	// Touch size
	bool ok;
	Config config;

	Orientation(LCD_LANDSCAPE_1);
	SetFont(Font_7x10);
	FillScreen(COLOR_BLACK);
	/*--------------------*/
	/* 0       	3      	5 */
	/*					  */
	/* 1			   	6 */
	/*					  */
	/* 2		4		7 */
	/*--------------------*/
	point[0] = Point(10, 10);
	point[1] = Point(10, height / 2);
	point[2] = Point(10, height - 10);
	point[3] = Point(width / 2, 10);
	point[4] = Point(width / 2, height - 10);
	point[5] = Point(width - 10, 10);
	point[6] = Point(width - 10, height / 2);
	point[7] = Point(width - 10, height - 10);

// for each point
	for (int i = 0; i < 8; i++) {
		// draw all cross
		for (int j = 0; j < 8; j++) {
			DrawCrossHair(point[j],
					(i == j) ? COLOR_WHITE : COLOR_GRAY);
		}
		// loop if error
		do {
			ok = ReadTestPoint(&res[i]);
			if (!ok) {
				SetFontColor(COLOR_RED);
				TextPutsCenterXY("ERROR! (touch to retry)");
				SetFontColor(COLOR_WHITE);
				DrawCrossHair(point[i], COLOR_RED);
				touch.WaitPenDown();
				DrawCrossHair(point[i], COLOR_WHITE);
				touch.WaitPenUp();
			}
		} while (!ok);
		DrawCrossHair(point[i], COLOR_GREEN);
		touch.WaitPenUp();
	}
	FillScreen(COLOR_BLACK);

// Compute
	mesMin = Point((res[0].x + res[1].x + res[2].x) / 3,
			(res[0].y + res[3].y + res[5].y) / 3);
	mesMax = Point((res[5].x + res[6].x + res[7].x) / 3,
			(res[2].y + res[4].y + res[7].y) / 3);
	if (mesMin.x > mesMax.x)
		SWAP(mesMin.x, mesMax.x);
	if (mesMin.y > mesMax.y)
		SWAP(mesMin.y, mesMax.y);

	txMin = mesMin.x - (10 * (mesMax.x - mesMin.x)) / (width - 20);
	tyMin = mesMin.y - (10 * (mesMax.y - mesMin.y)) / (height - 20);

	tWidth = ((mesMax.x - mesMin.x) * width) / (width - 20);
	tHeight = ((mesMax.y - mesMin.y) * height) / (height - 20);

// store configuration
	config.Write16(TOUCH_MIN, POS16_0, (uint16_t) txMin);
	config.Write16(TOUCH_MIN, POS16_1, (uint16_t) tyMin);
	config.Write16(TOUCH_SIZE, POS16_0, (uint16_t) tWidth);
	config.Write16(TOUCH_SIZE, POS16_1, (uint16_t) tHeight);

	touch.Reset();
}
#endif
