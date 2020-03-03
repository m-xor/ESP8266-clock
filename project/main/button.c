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
#include "freertos/timers.h"

static const char *TAG = "BUTTON";

static void buttonCb( TimerHandle_t xTimer);

//static void gpio_isr_handler(void * arg)
//{
//	(void)arg;
//
//	gpio_isr_handler_remove(BUTTON_PIN);
//
//	//sygnał
//    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//    xEventGroupSetBitsFromISR(httpd_start_event,HTTPD_START,&xHigherPriorityTaskWoken);
//    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
//}

void button_init(void)
{
//	const gpio_config_t xGpioButtonConf = {
//		.pin_bit_mask = BIT(BUTTON_PIN),
//		.mode = GPIO_MODE_INPUT,
//		.pull_up_en = GPIO_PULLUP_ENABLE,
//		.pull_down_en = GPIO_PULLDOWN_DISABLE,
//		.intr_type = GPIO_INTR_POSEDGE
//	};
//
//	if( gpio_config(&xGpioButtonConf) != ESP_OK )
//	{
//		ESP_LOGE(TAG, "GPIO config error");
//	}
//
//    //install gpio isr service
//    if(ESP_OK != gpio_install_isr_service(0))
//    	ESP_LOGE(TAG, "ISR service install error");
//
//    button_reg_isr(TAG);


    //---- wersja z buttonem przemiatanym w tasku timera
	const gpio_config_t xGpioButtonConf = {
		.pin_bit_mask = BIT(BUTTON_PIN),
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
    	.intr_type = GPIO_INTR_DISABLE
    };

	if( gpio_config(&xGpioButtonConf) != ESP_OK )
	{
		ESP_LOGE(TAG, "GPIO config error");
	}

	TimerHandle_t xTimerBtn = xTimerCreate( "Timer buttona",
							pdMS_TO_TICKS(10),
							pdTRUE,
							NULL,
							buttonCb );
	if( xTimerBtn == NULL )
	{
		ESP_LOGE(TAG, "Timer buttona nie utworzony");
		return;
	}

	if( xTimerStart( xTimerBtn, pdMS_TO_TICKS(1000) ) == pdFALSE )
	{
		ESP_LOGE(TAG, "Timer nie wystartował");
	}
}

//void button_reg_isr(const char *tag)
//{
//    //hook isr handler for specific gpio pin
//    if(ESP_OK != gpio_isr_handler_add(BUTTON_PIN, gpio_isr_handler, (void *)0))
//    	ESP_LOGE(tag, "Isr handler add error");
//}

static void buttonCb( TimerHandle_t xTimer)
{
//	/* state of the button debouncing, see below */
//	static struct ButtonsDebouncing {
//      uint16_t depressed;
//      uint16_t previous;
//	} buttons = { BIT(BUTTON_PIN), BIT(BUTTON_PIN) };
//	uint16_t current;
//	uint16_t tmp;
//
//    /* get state of the user button */
//    /* Perform the debouncing of buttons. The algorithm for debouncing
//    * adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
//    * and Michael Barr, page 71.
//    */
//    current = (uint16_t)gpio_get_level(BUTTON_PIN); /* read Port with the state of Button */
//    tmp = buttons.depressed; /* save the debounced depressed buttons */
//    buttons.depressed |= (buttons.previous & current); /* set depressed */
//    buttons.depressed &= (buttons.previous | current); /* clear released */
//    buttons.previous   = current; /* update the history */
//    tmp ^= buttons.depressed;     /* changed debounced depressed */
//    if ((tmp & (1<<BUTTON_PIN)) != 0U) {  /* debounced Button state changed? */
//        if ((buttons.depressed & (1<<BUTTON_PIN)) == 0U) { /* is Button depressed? */
//        	xEventGroupSetBits(httpd_start_event,HTTPD_START); /* send signal */
//        }
////        else {            /* the button is released */
//
////        }
//
//	}

	//https://www.embedded.com/my-favorite-software-debouncers/
    static uint16_t State = 0; // Current debounce status

    State = (State<<1) | !gpio_get_level(BUTTON_PIN) | 0xe000;
    if(State==0xf000)
    {
    	//debounced edge is detected
    	xEventGroupSetBits(httpd_start_event,HTTPD_START); /* send signal */
    }
}

