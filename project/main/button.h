/*
 * button.h
 *
 *  Created on: 29 lut 2020
 *      Author: slawek
 */

#ifndef MAIN_BUTTON_H_
#define MAIN_BUTTON_H_

#include "driver/gpio.h"

#define BUTTON_PIN	GPIO_NUM_5


extern void button_init(void);
extern void button_reg_isr(const char *tag);

#endif /* MAIN_BUTTON_H_ */
