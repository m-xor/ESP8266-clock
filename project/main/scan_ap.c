/*
 * scan_ap.c
 *
 *  Created on: 4 sty 2020
 *      Author: slawek
 */


#include "scan_ap.h"
#include "esp_err.h"
#include "esp_log.h"
#include <stdbool.h>
#include <stdlib.h>

static const char *TAG = "SCAN";

esp_err_t initScanAp(void)
{
	wifi_scan_config_t scanconf;
	esp_err_t res;

	scanconf.ssid = (uint8_t*)0;
	scanconf.bssid = (uint8_t*)0;
	scanconf.channel = 0;
	scanconf.show_hidden = true;
	scanconf.scan_type = WIFI_SCAN_TYPE_ACTIVE;
	scanconf.scan_time.active.min = 120;
	scanconf.scan_time.active.min = 400;
//    	scanconf.scan_type = WIFI_SCAN_TYPE_PASSIVE;
//		scanconf.scan_time.passive = 1000;

	if((res = esp_wifi_scan_start(&scanconf, false)) != ESP_OK)
	{
		ESP_LOGE(TAG,"init scan:%s", esp_err_to_name(res));
	}
	return res;
}


wifi_ap_record_t *getApLst(uint16_t *num)
{
	wifi_ap_record_t *apList = 0;
	uint16_t number;
	esp_err_t err;


	if((err = esp_wifi_scan_get_ap_num(&number)) != ESP_OK)
	{
		ESP_LOGE(TAG,"get num:%s", esp_err_to_name(err));
		return apList;
	}
	apList = malloc(number*sizeof(*apList));
	if(!apList)
	{
		ESP_LOGE(TAG,"AP list alloc failed");
		/* free records allocated when scanning */
		number = 0;
		esp_wifi_scan_get_ap_records(&number, apList);

		return 0;
	}
	if((err = esp_wifi_scan_get_ap_records(&number, apList)) != ESP_OK)
	{
		free(apList);
		ESP_LOGE(TAG,"get records:%s", esp_err_to_name(err));
		return 0;
	}

	*num = number;

	return apList;
}
