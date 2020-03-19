/*
 * app_apsta.c
 *
 *  Created on: 4 mar 2020
 *      Author: slawek
 */

#include "app_apsta.h"
#include "esp_log.h"
#include "qep.h"
#include "project.h"
#include "qassert.h"
#include "qtasks.h"
#include "esp_event_loop.h"
#include <string.h>
#include "app_http.h"

Q_DEFINE_THIS_FILE

EventGroupHandle_t wifi_event_group;
const int WIFI_SCAN_DONE = BIT2;

TimerHandle_t xHttpdExpire;


static const char *TAG = "APSTA";

//static esp_err_t dummy_event_handler(void *ctx, system_event_t *event);
static esp_err_t event_handler(void *ctx, system_event_t *event);
static void HttpdExpireCb(TimerHandle_t xTimer);

/* ************ public functions ****************** */

void wifi_init_softap(void)
{

	wifi_event_group = xEventGroupCreate();
    esp_err_t err;

    (void)esp_event_loop_set_cb(event_handler, NULL);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = 0;
    ESP_ERROR_CHECK( (err = esp_wifi_init(&cfg)) );
    Q_ASSERT(!err);
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .ssid_len = strlen(CONFIG_ESP_WIFI_SSID),
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .max_connection = CONFIG_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
//		.sta = {
//				.ssid = ""
//		}
    };
    if (strlen(CONFIG_ESP_WIFI_PASSWORD) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK( (err = esp_wifi_set_mode(WIFI_MODE_APSTA)) );
    Q_ASSERT(!err);
    ESP_ERROR_CHECK( (err = esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config)) );
    Q_ASSERT(!err);
    ESP_ERROR_CHECK( (err = esp_wifi_start()) );
    Q_ASSERT(!err);


    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s",
    		CONFIG_ESP_WIFI_SSID, CONFIG_ESP_WIFI_PASSWORD);
}

void wifi_stop_softap(void)
{
	ESP_ERROR_CHECK(esp_wifi_disconnect());
	ESP_ERROR_CHECK(esp_wifi_deauth_sta(0));
	ESP_ERROR_CHECK(esp_wifi_stop());
	ESP_ERROR_CHECK(esp_wifi_deinit());
	vEventGroupDelete( wifi_event_group );
	(void)esp_event_loop_set_cb(dummy_event_handler, NULL);
}


esp_err_t dummy_event_handler(void *ctx, system_event_t *event)
{
	(void)event;

	ESP_LOGI(TAG, "dummy_event_handler: %d ", event->event_id);

    return ESP_OK;
}

/* ************ private functions ***************** */

static esp_err_t event_handler(void *ctx, system_event_t *event)
{

    switch(event->event_id) {

    //station connected/disconected
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "station:" MACSTR" join, AID=%d",
                 MAC2STR(event->event_info.sta_connected.mac),
                 event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station:" MACSTR " leave, AID=%d",
                 MAC2STR(event->event_info.sta_disconnected.mac),
                 event->event_info.sta_disconnected.aid);
        break;

    case SYSTEM_EVENT_AP_START :
    	ESP_LOGI(TAG, "AP started");
        /* Start the web server */
    	Q_ASSERT(0==start_webserver());
        //Connection timeout timer
        if(xHttpdExpire==NULL)
        {
        	xHttpdExpire = xTimerCreate(
        			"HttpExpireTimer",
        			pdMS_TO_TICKS(CONFIG_AP_CONN_TIMEOUT*1000),
					pdFALSE,
					NULL,
					HttpdExpireCb
					);
        	Q_ASSERT(xHttpdExpire!=NULL);
      		Q_ASSERT(pdPASS==xTimerStart(xHttpdExpire,pdMS_TO_TICKS(1000)));
        }
    	break;

    case SYSTEM_EVENT_AP_STOP :
        /* Stop the web server */
        ESP_LOGI(TAG, "AP stopped");
        stop_webserver();
        xTimerDelete(xHttpdExpire,pdMS_TO_TICKS(1000));
        xHttpdExpire = NULL;

     	break;

        // Unblocking scan ended (not caused by esp_wifi_connected())
    case SYSTEM_EVENT_SCAN_DONE:
    	ESP_LOGI(TAG, "Scan of APs done");
    	xEventGroupSetBits(wifi_event_group, WIFI_SCAN_DONE);	//let display scanned APs
    	break;

    default:
    	ESP_LOGI(TAG, "event: %d ", event->event_id);
        break;
    }
    return ESP_OK;
}


static void HttpdExpireCb(TimerHandle_t xTimer)
{

	//generuj sygnał timeouta
	QEvt e = {TIMEOUT_SIG, 0, 0};
	POST_EVENT(theClock,&e);
}
