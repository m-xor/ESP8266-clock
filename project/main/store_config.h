/*
 * store_config.h
 *
 *  Created on: 12 sty 2020
 *      Author: slawek
 */

#ifndef STORE_CONFIG_H_
#define STORE_CONFIG_H_

#include <stdint.h>

enum configKey {
	VAL_SSID,
	VAL_PWD,
	VAL_AUTH,
	VAL_NTP
};

void setConfigValue(enum configKey key, char *value);
char *getConfigValue(enum configKey key);
void setLastUpd(uint32_t timestamp);
uint32_t getLastUpd(void);
char *getLastUpdMsg(char *buf);


#endif /* STORE_CONFIG_H_ */
