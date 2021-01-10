/*
 * app.c
 *
 *  Created on: Dec 29, 2020
 *      Author: Thierry
 */
#include <stdio.h>
#include <stdbool.h>

#include "app.h"
#include "cmsis_os.h"
#include "board.h"
#include "touch.h"
#include "config.h"
#include "ugui.h"
#include "lcd.h"

#define MAX_OBJECTS	10

UG_GUI gui; // Global GUI structure

extern void Hello();

/*******************************************************************************
 * Application initialization
 ******************************************************************************/
void AppInit() {
	LcdInit();
	LcdSetOrientation(LCD_LANDSCAPE_1);
	TouchReset();
	UG_Init(&gui, LcdDrawPixel, lcd.width, lcd.height);
	UG_DriverRegister(DRIVER_DRAW_LINE, (void *) HW_DrawLine);
	UG_DriverRegister(DRIVER_FILL_FRAME, (void *) HW_FillFrame);
	UG_FontSelect(&FONT_8X12);
	UG_FillScreen(C_BLACK);
	Hello();
}

/*******************************************************************************
 * Application loop
 ******************************************************************************/
void AppLoop() {
	static bool penDown = false;
	int posX;
	int posY;
	int Res;

	// display Pen Status
	Res = TouchIsPenDown();
	if (Res) {
		// Pen is down
		if (TouchGetXYMedian(&posX, &posY)) {
			// Data is available
			penDown = true;
			UG_TouchUpdate(posX, posY, TOUCH_STATE_PRESSED);
		}
	} else if (penDown) {
		// Pen goes up
		penDown = false;
		UG_TouchUpdate(-1, -1, TOUCH_STATE_RELEASED);
	}

	UG_Update();
	osDelay(10);

}

/*******************************************************************************
 * Say Hello :-)
 ******************************************************************************/
void Window1Callback(UG_MESSAGE *msg) {
	if (msg->type == MSG_TYPE_OBJECT) {
		if (msg->id == OBJ_TYPE_BUTTON) {
			switch (msg->sub_id) {
			case BTN_ID_0: {
				// Do something !
				LedToggle(LED3);
				break;
			}
			}
		}
	}
}

void Hello() {
	UG_OBJECT objList[MAX_OBJECTS];
	UG_WINDOW window1;
	UG_BUTTON button1;
	UG_TEXTBOX textbox1;

//if (KeyTest(KEY1))
//	lcd.Calibrate();
	UG_FontSelect(&FONT_8X12);
	UG_ConsoleSetBackcolor(C_BLACK);
	UG_ConsoleSetForecolor(C_WHITE);
	UG_ConsolePutString("Salut Thierry\nComment vas-tu ?");

	UG_WindowCreate(&window1, objList, MAX_OBJECTS, Window1Callback);
	UG_WindowSetTitleText(&window1 , "uGUI Demo Window");
	UG_WindowSetTitleTextFont(&window1, &FONT_12X20);

	UG_ButtonCreate(&window1, &button1, BTN_ID_0, 10, 10, 110, 60);
	UG_ButtonSetText(&window1, BTN_ID_0, "Test");

	// Create a Textbox
	UG_TextboxCreate(&window1, &textbox1, TXB_ID_0, 120, 10, 310, 200);
	UG_TextboxSetFont(&window1, TXB_ID_0, &FONT_12X16 );
	UG_TextboxSetText(&window1, TXB_ID_0, "This is just \na very simple \nwindow to \ndemonstrate \nsome basic \n features of uGUI !");
	UG_TextboxSetForeColor(&window1, TXB_ID_0, C_BLACK);
	UG_TextboxSetAlignment(&window1, TXB_ID_0, ALIGN_CENTER);

	UG_WindowShow(&window1);
}

