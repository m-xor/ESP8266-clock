/*
 * store_config.c
 *
 *  Created on: 12 sty 2020
 *      Author: slawek
 */

#include "store_config.h"

/* MOCK */
#include <string.h>

#define MAX_SSID 15
#define MAX_PWD 64
#define MAX_NTP 32

static char ssid[MAX_SSID+1];
static char pwd[MAX_PWD+1];
static char ntp[MAX_NTP];
static char const * const auth[] = { "T", "N" };

void setConfigValue(enum configKey key, char *value)
{
	switch(key)
	{
	case VAL_SSID:
		strcpy(ssid,value);	//beware of buffer overflow
		break;
	case VAL_PWD:
		strcpy(pwd, value);
		break;
	case VAL_NTP:
		strcpy(ntp, value);
		break;
	default:
		break;
	}
}

char *getConfigValue(enum configKey key)
{
	char *ret = 0;

	switch(key)
	{
	case VAL_SSID:
		ret = ssid;
		break;
	case VAL_PWD:
		ret = pwd;
		break;
	case VAL_AUTH:
		ret = (char *)auth[*pwd?1:0];
		break;
	case VAL_NTP:
		ret = ntp;
		break;
	default:
		break;
	}

	return ret;
}

void setLastUpd(uint32_t timestamp)
{

}

uint32_t getLastUpd(void)
{
	return 0;
}

char *getLastUpdMsg(char *buf)
{
	uint32_t timestamp;

	if(!(timestamp=getLastUpd()))
		return "20.01.2020 19:50:23 CET";
	else
		return "N/A";
}
