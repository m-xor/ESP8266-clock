/*
 * app_json.c
 *
 *  Created on: 4 sty 2020
 *      Author: slawek
 */

#include  "app_json.h"

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
