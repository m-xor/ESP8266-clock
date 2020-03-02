/*
 * button.c
 *
 *  Created on: 29 lut 2020
 *      Author: slawek
 */

#include "button.h"
#include "esp_log.h"


//******test httpd task
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
extern EventGroupHandle_t httpd_start_event;
extern const int HTTPD_START;
//*************

static const char *TAG = "BUTTON";

static void gpio_isr_handler(void * arg)
{
	(void)arg;

	gpio_isr_handler_remove(BUTTON_PIN);

	//sygnał
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(httpd_start_event,HTTPD_START,&xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void button_init(void)
{
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
}
