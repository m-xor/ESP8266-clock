/*
 * app_http.c
 *
 *  Created on: 4 sty 2020
 *      Author: slawek
 */

#include "app_http.h"
#include "esp_err.h"
#include "esp_log.h"
#include "fs.h"
#include "app_json.h"
#include "esp_system.h"


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


    if((buf = decHeader(req,"DataSet")))
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

   	setItem(root, "Info", "timeout", CONFIG_AP_CONN_TIMEOUT);

	switch(dataSet)
	{
	case DATASET_ALL :
		setItem(root, "Config", "ssid", "Gateway To NTP Server");
		setItem(root, "Config", "auth", "true");
		setItem(root, "Config", "ntp", "ntp.pool.org");
		setItem(root, "Info", "currentTime", __TIME__);
		setItem(root, "Info", "lastUpd", __TIME__);

		setItem(root, "Info", "sdkVer", esp_get_idf_version());
		setItem(root, "Info", "compDate", __DATE__ ", " __TIME__);
		break;
	case DATASET_AP :
		setItem(root, "Config", "ssid", "Gateway To NTP Server");
		setItem(root, "Config", "auth", "true");
		break;
	case DATASET_NTP :
		setItem(root, "Config", "ntp", "ntp.pool.org");
		break;
	case DATASET_APLST :
		{
			cJSON *array, *item;
		array = cJSON_CreateArray();
		cJSON_AddItemToObject(root, "APs", array);
		//first AP
		item = cJSON_CreateObject();
		cJSON_AddItemToArray(array, item);
			cJSON_AddItemToObject(item, "MAC", cJSON_CreateString("00:11:22:33:44:55:66"));
			cJSON_AddItemToObject(item, "SSID", cJSON_CreateString("Fiu Fiu Fiu"));
			cJSON_AddItemToObject(item, "Auth", cJSON_CreateString("T"));
			cJSON_AddItemToObject(item, "Ch", cJSON_CreateNumber(1));
			cJSON_AddItemToObject(item, "RSSI", cJSON_CreateNumber(-20));
			cJSON_AddItemToObject(item, "Phy", cJSON_CreateString("b/g/n"));
		//second
			item = cJSON_CreateObject();
			cJSON_AddItemToArray(array, item);
					cJSON_AddItemToObject(item, "MAC", cJSON_CreateString("66:55:44:33:33:22:11"));
					cJSON_AddItemToObject(item, "SSID", cJSON_CreateString("TraLaLa"));
					cJSON_AddItemToObject(item, "Auth", cJSON_CreateString("N"));
					cJSON_AddItemToObject(item, "Ch", cJSON_CreateString("4"));
					cJSON_AddItemToObject(item, "RSSI", cJSON_CreateString("-30"));
					cJSON_AddItemToObject(item, "Phy", cJSON_CreateString("b/g"));
					//third
							item = cJSON_CreateObject();
							cJSON_AddItemToArray(array, item);
									cJSON_AddItemToObject(item, "MAC", cJSON_CreateString("00:11:44:33:33:11:00"));
									cJSON_AddItemToObject(item, "SSID", cJSON_CreateString("Bum Cyk Cyk"));
									cJSON_AddItemToObject(item, "Auth", cJSON_CreateString("T"));
									cJSON_AddItemToObject(item, "Ch", cJSON_CreateString("5"));
									cJSON_AddItemToObject(item, "RSSI", cJSON_CreateString("-30"));
									cJSON_AddItemToObject(item, "Phy", cJSON_CreateString("b/g/n/WPS"));
	}
		break;
	default:
		setItem(root, "Info", "msg", "Error at parsing GET/DataSet header");
		break;
	}


	   json_out = cJSON_Print(root);

	   /* free all objects under root and root itself */
	   cJSON_Delete(root);


		ESP_LOGI(TAG, "clock.json;");
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


    if((buf = decHeader(req,"DataSet")))
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

    if(dataSet != DATASET_CLOSE)
    	setItem(root, "Info", "timeout", CONFIG_AP_CONN_TIMEOUT);

	switch(dataSet)
	{
	case DATASET_AP :
		if((buf = decHeader(req,"ssid")))
		{
			ESP_LOGI(TAG, "set SSID:%s", buf); // przypisz buf
			free(buf);
		}
		else
			ESP_LOGI(TAG, "SSID untouched"); //pozostaw niezmieniony


		if((buf = decHeader(req,"pwd")))
		{
			ESP_LOGI(TAG, "set PWD:%s", buf); //przypisz buf
			free(buf);
		}
		else
			ESP_LOGI(TAG, "PWD cleared"); //niezmieniony

		setItem(root, "Info", "msg", "OK");

		break;
	case DATASET_NTP :
		if((buf = decHeader(req,"ntp")))
		{
			ESP_LOGI(TAG, "set NTP:%s", buf);
			free(buf);
		}
		else
			ESP_LOGE(TAG, "Error parsing NTP header");

		setItem(root, "Info", "msg", "OK");

		break;
	case DATASET_CLOSE :
		//signal to app CLOSE
		break;
	default:
		setItem(root, "Info", "msg", "Error at parsing SET/DataSet header");
		break;
	}

	   json_out = cJSON_Print(root);

	   /* free all objects under root and root itself */
	   cJSON_Delete(root);


		ESP_LOGI(TAG, "clock.json;");
		result = httpd_resp_set_type(req, "application/json");
		if(result != ESP_OK)
			ESP_LOGE(TAG,"Json header error: %s", esp_err_to_name(result));
		if(dataSet == DATASET_CLOSE)
		{
			result = httpd_resp_set_status(req, HTTPD_408);
			if(result != ESP_OK)
				ESP_LOGE(TAG,"Json 408 status error: %s", esp_err_to_name(result));
		}
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


httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

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

        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}
