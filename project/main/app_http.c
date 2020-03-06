/*
 * app_http.c
 *
 *  Created on: 4 sty 2020
 *      Author: slawek
 */

#include <string.h>
#include "app_http.h"
#include "esp_err.h"
#include "esp_log.h"
#include "fs.h"
#include "app_json.h"
#include "esp_system.h"
#include "scan_ap.h"
#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <esp_http_server.h>
#include "store_config.h"
#include "buildstamp.h"
#include "app_apsta.h"

#define MAXDATE 30

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

/*
 * Action : set
 * DataSet:	ap
 * 		ssid=SSID
 * 		pwd=PWD
 * 		Info.timeout
 * 		Info.msg(*)
 * DataSet:	ntp
 * 		ntp=NTP SERVER URL
 * 		Info.timeout
 * 		Info.msg(*)
 *
 *
 * 	Action : get
 * 	DataSet: all
 * 		Info.timeout
 * 		Info.msg(*)
 * 		Info.currentTime
 * 		Info.lastUpd
 * 		Config.ssid
 * 		Config.auth
 * 		Config.ntp
 * 		Info.sdkVer
 * 		Info.compDate
 * 	DataSet: ap
 * 		Info.timeout
 * 		Info.msg(*)
 * 		Config.ssid
 * 		Config.auth
 * 	DataSet: ntp
 * 		Info.timeout
 * 		Info.msg(*)
 * 		Config.ntp
 * 	DataSet: apLst
 * 		Info.timeout
 * 		Info.msg(*)
 * 		APs
 */

enum dataSet {
	DATASET_ALL,
	DATASET_AP,
	DATASET_NTP,
	DATASET_APLST,
	DATASET_CLOSE,
	DATASET_INVALID
};

static const char *TAG = "HTTPD";

/* find particular header in request */
/* remember to free pointer after use */
static char *decHeader(httpd_req_t *req, char const *header)
{
	size_t buf_len;
	char *buf;
	char *res = NULL;

    buf_len = httpd_req_get_hdr_value_len(req, header);
    if (buf_len++) {
        if((buf = malloc(buf_len))) {
        	/* Copy null terminated value string into buffer */
        	if (httpd_req_get_hdr_value_str(req, header, buf, buf_len) == ESP_OK) {
        		ESP_LOGI(TAG, "Found header => %s: %s", header, buf);
        		res = buf;
        	}
        	else {
        		free(buf);
        	}
        }
    }
    return res;
}


static enum dataSet decDataSet(char *buf)
{
	enum dataSet result = DATASET_INVALID;

	//get
	if(!strcmp("all",buf))
		result = DATASET_ALL;
	else if(!strcmp("ap",buf))
		result = DATASET_AP;
	else if(!strcmp("ntp",buf))
		result = DATASET_NTP;
	else if(!strcmp("apLst",buf))
		result = DATASET_APLST;
	//set
//	else if(!strcmp("ap",buf))
//		result = DATASET_AP;
//	else if(!strcmp("ntp",buf))
//		result = DATASET_NTP;
	else if(!strcmp("close",buf))
		result = DATASET_CLOSE;

	return result;
}

/* HTTPD registered handlers */

/* handler serving files */
static esp_err_t file_handler(httpd_req_t *req)
{
	char const *data;
	int len;
	char const *type;
	const char* resp_str = (const char*) req->user_ctx;

	xTimerReset(xHttpdExpire,pdMS_TO_TICKS(50));

	if( fs_get(resp_str,&data,&len))
	{
		type = fs_type(resp_str);
		ESP_LOGI(TAG, "ctx: %s; type: %s", resp_str, type);
		httpd_resp_set_type(req, type);
		httpd_resp_send(req, (char const *)data, len);
	    return ESP_OK;
	}
	else
		return ESP_FAIL;
}


static esp_err_t json_get_handler(httpd_req_t *req)
{
	char *buf;
    enum dataSet dataSet = DATASET_INVALID;
    cJSON *root;
    char *json_out;
    esp_err_t result = ESP_OK;

    xTimerReset(xHttpdExpire,pdMS_TO_TICKS(500));

    if((buf = decHeader(req,"DataSet")))
    {
		dataSet = decDataSet(buf);
		free(buf);
	}


    if(!(root = cJSON_CreateObject()))
    {
    	ESP_LOGE(TAG, "Can't allocate JSON object");
    	return ESP_FAIL;
    }

   	setItem(root, "Info", "timeout", STR(CONFIG_AP_CONN_TIMEOUT));

	switch(dataSet)
	{
	case DATASET_ALL :
	{
		char tmp[MAXDATE];
		setItem(root, "Config", "ssid", getConfigValue(VAL_SSID));
		setItem(root, "Config", "auth", getConfigValue(VAL_AUTH));
		setItem(root, "Config", "ntp", getConfigValue(VAL_NTP));
		setItem(root, "Info", "lastUpd", getLastUpdMsg(tmp));
		setItem(root, "Info", "sdkVer", esp_get_idf_version());
		setItem(root, "Info", "compDate", BUILD_STAMP);
		break;
	}
	case DATASET_AP :
		setItem(root, "Config", "ssid", getConfigValue(VAL_SSID));
		setItem(root, "Config", "auth", getConfigValue(VAL_AUTH));
		break;
	case DATASET_NTP :
		setItem(root, "Config", "ntp", getConfigValue(VAL_NTP));
		break;
	case DATASET_APLST :
		{
			esp_err_t err;

    		if((err=initScanAp())==ESP_OK)
    		{
    			//wait 30 sec for the list
    			if(WIFI_SCAN_DONE==xEventGroupWaitBits(wifi_event_group,WIFI_SCAN_DONE,pdTRUE,pdFALSE,pdMS_TO_TICKS(30000)))
    			{

				uint16_t num;
				wifi_ap_record_t *list = getApLst(&num);

					if(list)
					{
						ESP_LOGI(TAG, "APLIST NUMER: %d", num);
						for(uint16_t i = 0; i<num; i++)
						{
							setArrayItem(root, "APs", &list[i]);
						}
						free(list);
					}
    			}
    			else
    			{
    				setItem(root, "Info", "msg", "Scannig Access Points timeout");
    				esp_wifi_scan_stop();	//Assume scan_stop function frees resources. Does it?
    			}


			}
    		else
    		{
    			ESP_LOGE(TAG,"init scan:%s", esp_err_to_name(err));
    			/* scan initialization failed, inform client */
    			setItem(root, "Info", "msg", "Scannig Access Points failed");
    		}
	}
		break;
	default:
		setItem(root, "Info", "msg", "Error at parsing GET/DataSet header");
		break;
	}

	   json_out = cJSON_Print(root);
//	   ESP_LOGI(TAG, "JSON: %s", json_out);

	   /* free all objects under root and root itself */
	   cJSON_Delete(root);


		//ESP_LOGI(TAG, "clock.json;");
		result = httpd_resp_set_type(req, "application/json");
		if(result != ESP_OK)
			ESP_LOGE(TAG,"Json header error: %s", esp_err_to_name(result));
		result = httpd_resp_send(req, (char const *)json_out, strlen(json_out));
		if(result != ESP_OK)
		{
			ESP_LOGE(TAG,"Json send error: %s", esp_err_to_name(result));
			result = ESP_FAIL;
		}

		free(json_out);

	    return result;

}

static esp_err_t json_set_handler(httpd_req_t *req)
{
	char *buf;
    enum dataSet dataSet = DATASET_INVALID;
    cJSON *root;
    char *json_out;
    esp_err_t result = ESP_OK;
    const char * req_status = HTTPD_200;

    xTimerReset(xHttpdExpire,pdMS_TO_TICKS(500));

    if((buf = decHeader(req,"DataSet"))) //function allocates memory for buf
    {
		dataSet = decDataSet(buf);
		free(buf);
	}
//    ESP_LOGI(TAG, "DataSet:%d", dataSet);


    if(!(root = cJSON_CreateObject()))
    {
    	ESP_LOGE(TAG, "Can't allocate JSON object");
    	return ESP_FAIL;
    }


    setItem(root, "Info", "timeout", STR(CONFIG_AP_CONN_TIMEOUT));

	switch(dataSet)
	{
	case DATASET_AP :
	{


		if((buf = decHeader(req,"ssid")))
		{
			ESP_LOGI(TAG, "set SSID:%s", buf); // przypisz buf
			setConfigValue(VAL_SSID, buf);
			free(buf);
		}
		else
			ESP_LOGI(TAG, "SSID untouched"); //pozostaw niezmieniony


		if((buf = decHeader(req,"pwd")))
		{
			ESP_LOGI(TAG, "set PWD:%s", buf); //przypisz buf
			setConfigValue(VAL_PWD, buf);
			free(buf);
		}
		else
		{
			setConfigValue(VAL_SSID, "");
			ESP_LOGI(TAG, "PWD cleared"); //niezmieniony
		}

		setItem(root, "Info", "msg", "OK");

		break;
	}
	case DATASET_NTP :
		if((buf = decHeader(req,"ntp")))
		{
			setConfigValue(VAL_NTP, buf);
			ESP_LOGI(TAG, "set NTP:%s", buf);
			free(buf);
		}
		else
			ESP_LOGE(TAG, "Error parsing NTP header");

		setItem(root, "Info", "msg", "OK");

		break;
	case DATASET_CLOSE :
		req_status = HTTPD_408;
		//let timer some time for complete request
		xTimerChangePeriod(xHttpdExpire,pdMS_TO_TICKS(1500), pdMS_TO_TICKS(500));
		break;
	default:
		setItem(root, "Info", "msg", "Error at parsing SET/DataSet header");
		break;
	}

	   json_out = cJSON_Print(root);

	   /* free all objects under root and root itself */
	   cJSON_Delete(root);


		//ESP_LOGI(TAG, "clock.json;");
		result = httpd_resp_set_type(req, "application/json");
		if(result != ESP_OK)
			ESP_LOGE(TAG,"Json header error: %s", esp_err_to_name(result));
//		if(dataSet == DATASET_CLOSE)
//		{
			result = httpd_resp_set_status(req, req_status);
			if(result != ESP_OK)
				ESP_LOGE(TAG,"Json resp status error: %s", esp_err_to_name(result));
//		}
		result = httpd_resp_send(req, (char const *)json_out, strlen(json_out));
		if(result != ESP_OK)
		{
			ESP_LOGE(TAG,"Json send error: %s", esp_err_to_name(result));
			result = ESP_FAIL;
		}

		free(json_out);

	    return result;

}

httpd_uri_t index_page = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = file_handler,
    .user_ctx  = "/index.html"
};

httpd_uri_t favicon_ico = {
    .uri       = "/favicon.ico",
    .method    = HTTP_GET,
    .handler   = file_handler,
    .user_ctx  = "/favicon.ico"
};

httpd_uri_t clock_javascript = {
    .uri       = "/clock.js",
    .method    = HTTP_GET,
    .handler   = file_handler,
    .user_ctx  = "/clock.js"
};

httpd_uri_t clock_css = {
    .uri       = "/clock.css",
    .method    = HTTP_GET,
    .handler   = file_handler,
    .user_ctx  = "/clock.css"
};

httpd_uri_t clock_get_json = {
    .uri       = "/clock.json",
    .method    = HTTP_GET,
    .handler   = json_get_handler,
    .user_ctx  = NULL
};

httpd_uri_t clock_set_json = {
    .uri       = "/clock.json",
    .method    = HTTP_POST,
    .handler   = json_set_handler,
    .user_ctx  = NULL
};


static httpd_handle_t server = NULL;

int start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if(server!=NULL)	//already running
    	return 0;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {

    	// Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");

        httpd_register_uri_handler(server, &index_page);
        httpd_register_uri_handler(server, &favicon_ico);
        httpd_register_uri_handler(server, &clock_javascript);
        httpd_register_uri_handler(server, &clock_css);
        httpd_register_uri_handler(server, &clock_get_json);
        httpd_register_uri_handler(server, &clock_set_json);

        return 0; /* OK */
    }

    ESP_LOGI(TAG, "Error starting server!");

    return -1;
}


void stop_webserver(void)
{
	if(server==NULL) //already stopped
		return;

    // Stop the httpd server
    httpd_stop(server);
    server = NULL;
}
