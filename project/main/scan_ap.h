/*
 * scan_ap.h
 *
 *  Created on: 4 sty 2020
 *      Author: slawek
 */

#ifndef MAIN_SCAN_AP_H_
#define MAIN_SCAN_AP_H_

#include "esp_wifi.h"

esp_err_t initScanAp(void);

wifi_ap_record_t *getApLst(uint16_t *num);

#endif /* MAIN_SCAN_AP_H_ */
