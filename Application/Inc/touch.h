/**
 ******************************************************************************
 * File Name          : TOUCH.hpp
 * Description        : touch screen management for the XPT2046
 ******************************************************************************
 */

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef INC_TOUCH_HPP_
#define INC_TOUCH_HPP_

#include <stdbool.h>

/* Constants ----------------------------------------------------------------*/

/* Definitions --------------------------------------------------------------*/
typedef struct {
	int orientation;
	// screen
	int widthLCD;
	int heightLCD;
	// touch
	int xOffsetTouch;
	int yOffsetTouch;
	int widthTouch;
	int heightTouch;
} TouchScreen;



/* Functions ----------------------------------------------------------------*/
	void TouchReset(void);
	void TouchSetOrientation(int w, int h, int o);
	void TouchGetXY(int *x, int *y);
	bool TouchGetXYMedian(int *x, int *y);
	bool TouchIsPenDown();
	void TouchWaitPenUp();
	void TouchWaitPenDown();

	//int _TouchGet(uint8_t address);
	//int _TouchFastMedian(int *samples);
	//bool _TouchCorrection(int *x, int *y);



/* Variables ----------------------------------------------------------------*/
extern TouchScreen touch;


#endif /* INC_TOUCH_HPP_ */
