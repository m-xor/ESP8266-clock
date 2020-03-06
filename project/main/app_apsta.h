/*
 * app_apsta.h
 *
 *  Created on: 4 mar 2020
 *      Author: slawek
 */

#ifndef MAIN_APP_APSTA_H_
#define MAIN_APP_APSTA_H_

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "esp_wifi.h"


/* To detect end of scanning access points */
extern EventGroupHandle_t wifi_event_group;
extern const int WIFI_SCAN_DONE;

/* httpd idle state timeout */
extern TimerHandle_t xHttpdExpire;



extern void wifi_init_softap(void);
extern void wifi_stop_softap(void);
extern esp_err_t dummy_event_handler(void *ctx, system_event_t *event);

#endif /* MAIN_APP_APSTA_H_ */
