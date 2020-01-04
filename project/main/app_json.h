/*
 * app_json.h
 *
 *  Created on: 4 sty 2020
 *      Author: slawek
 */

#ifndef MAIN_APP_JSON_H_
#define MAIN_APP_JSON_H_

#include "cJSON.h"

struct arrItem {
	char *mac;
	char *ssid;
	char auth;
	int ch;
	int rssi;
	char *phy;
};

/*
 * Add ItemName of itemVal into objName object and attach it to root
 * objName is created if doesn't exist
 * itemName is created or replaced if exists
 * root must exist
 */
void setItem(cJSON *root,
		char const * const objName,
		char const * const itemName,
		char const * const itemVal);

void setArrayItem(cJSON *root,
		char const * const arrName,
		struct arrItem *item);

#endif /* MAIN_APP_JSON_H_ */
