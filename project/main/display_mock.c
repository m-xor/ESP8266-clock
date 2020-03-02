/*
 * display.c
 *
 *  Created on: 1 mar 2020
 *      Author: slawek
 */


#include "display.h"
#include "driver/gpio.h"

void display_init(void)
{
	const gpio_config_t xGpioLEDConf = {
		.pin_bit_mask = BIT(GPIO_NUM_2),
		.mode = GPIO_MODE_OUTPUT_OD,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE
	};
	gpio_config(&xGpioLEDConf);
	gpio_set_level(GPIO_NUM_2,DISPLAY_LED_OFF);
}

void display_led(int led, int level)
{
	if(DISPLAY_HTTP_ACTIVE==led)
	{
	    gpio_set_level(GPIO_NUM_2,level);
	}
}
