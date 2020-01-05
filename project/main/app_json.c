/*
 * app_json.c
 *
 *  Created on: 4 sty 2020
 *      Author: slawek
 */

#include  "app_json.h"

static char *getMAC(char * buf, uint8_t const bssid[6]);
static char *getPHY(char * buf, wifi_ap_record_t const *items);


//replace item if it is already created
void setItem(cJSON *root,
		char const * const objName,
		char const * const itemName,
		char const * const itemVal)
{
	cJSON *object, *item;

	if(	!(object = cJSON_GetObjectItem(root, objName)) ) //need creating
	{
		object = cJSON_CreateObject();
		cJSON_AddItemToObject(root, objName, object);
	}

	item = cJSON_CreateString(itemVal);

	if(	cJSON_HasObjectItem(object,itemName) )
	{
		cJSON_ReplaceItemInObject(object,itemName,item);
	}
	else
	{
		cJSON_AddItemToObject(object,itemName,item);
	}
}

void setArrayItem(cJSON *root,
		char const * const arrName,
		wifi_ap_record_t *items)
{
	cJSON *array, *item;
	char strbuf[33];

	if(	!(array = cJSON_GetObjectItem(root, arrName)) ) //need creating
	{
		array = cJSON_CreateArray();
		cJSON_AddItemToObject(root, "APs", array);
	}
	item = cJSON_CreateObject();
	cJSON_AddItemToArray(array, item);
	cJSON_AddItemToObject(item, "MAC", cJSON_CreateString( getMAC(strbuf,items->bssid) ));
	cJSON_AddItemToObject(item, "SSID", cJSON_CreateString((char*)items->ssid));
	cJSON_AddItemToObject(item, "Auth", cJSON_CreateString(items->authmode?"T":"N"));
	cJSON_AddItemToObject(item, "Ch", cJSON_CreateNumber(items->primary));
	cJSON_AddItemToObject(item, "RSSI", cJSON_CreateNumber(items->rssi));
	cJSON_AddItemToObject(item, "Phy", cJSON_CreateString(getPHY(strbuf,items)));
}

static char *getMAC(char * buf, uint8_t const bssid[6])
{
	uint8_t j = 0;

	for(uint8_t i=0; i< 6 ; i++)
	{
		sprintf(&buf[j],"%x:",bssid[i]);
		j += 3;
	}
	buf[--j] = '\0';

	return buf;
}

static char *getPHY(char * buf, wifi_ap_record_t const *items)
{
	char *ptr = buf;

    if(items->phy_11b)
    {
    	*buf++ = 'b';
    	*buf++ = '/';
    }
    if(items->phy_11g)
    {
    	*buf++ = 'g';
    	*buf++ = '/';
    }
    if(items->phy_11n)
    {
    	*buf++ = 'n';
    	*buf++ = '/';
    }
    if(items->phy_lr)
    {
    	*buf++ = 'L';
    	*buf++ = 'R';
    	*buf++ = '/';
    }
    if(items->wps)
    {
    	*buf++ = 'W';
    	*buf++ = 'P';
    	*buf++ = 'S';
    	*buf++ = '/';
    }

    buf[ ( ptr==buf ) ? 0 : -1 ] = '\0';

	return ptr;
}
