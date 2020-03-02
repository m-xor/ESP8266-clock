/*
 * display.h
 *
 *  Created on: 1 mar 2020
 *      Author: slawek
 */

#ifndef MAIN_DISPLAY_H_
#define MAIN_DISPLAY_H_

enum {
	DISPLAY_UPPER_0,
	DISPLAY_UPPER_1,
	DISPLAY_UPPER_2,
	DISPLAY_UPPER_3,
	DISPLAY_UPPER_4,
	DISPLAY_LOWER_0,
	DISPLAY_LOWER_1,
	DISPLAY_LOWER_2,
	DISPLAY_LOWER_3,
	DISPLAY_LOWER_4,
	DISPLAY_COLON,
};

enum {
	DISPLAY_HTTP_ACTIVE = DISPLAY_LOWER_0,
	DISPLAY_NO_CONNECTION = DISPLAY_LOWER_4
};

enum {
	DISPLAY_LED_ON,
	DISPLAY_LED_OFF
};

void display_init(void);
void display_led(int led, int level);

#endif /* MAIN_DISPLAY_H_ */
