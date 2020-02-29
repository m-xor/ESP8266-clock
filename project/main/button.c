/*
 * button.c
 *
 *  Created on: 29 lut 2020
 *      Author: slawek
 */

#include "button.h"
#include "esp_log.h"

static const char *TAG = "BUTTON";

static void gpio_isr_handler(void * arg)
{
	(void)arg;

	gpio_isr_handler_remove(BUTTON_PIN);

    //debug
    gpio_set_level(GPIO_NUM_2,1);

	//sygnał
}

void button_init(void)
{
	//debug LED
	const gpio_config_t xGpioLEDConf = {
		.pin_bit_mask = BIT(GPIO_NUM_2),
		.mode = GPIO_MODE_OUTPUT_OD,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE

	};
	gpio_config(&xGpioLEDConf);
	gpio_set_level(GPIO_NUM_2,1);
	//*************
	const gpio_config_t xGpioButtonConf = {
		.pin_bit_mask = BIT(BUTTON_PIN),
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_POSEDGE
	};

	if( gpio_config(&xGpioButtonConf) != ESP_OK )
	{
		ESP_LOGE(TAG, "GPIO config error");
	}

    //install gpio isr service
    if(ESP_OK != gpio_install_isr_service(0))
    	ESP_LOGE(TAG, "ISR service install error");

    button_reg_isr(TAG);

}

void button_reg_isr(const char *tag)
{
    //hook isr handler for specific gpio pin
    if(ESP_OK != gpio_isr_handler_add(BUTTON_PIN, gpio_isr_handler, (void *)0))
    	ESP_LOGE(tag, "Isr handler add error");

    //debug
    gpio_set_level(GPIO_NUM_2,0);
}
