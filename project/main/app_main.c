/* Simple WiFi Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "rom/ets_sys.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include <esp_http_server.h>
//#include "fs.h"
#include "app_http.h"
#include "button.h"


/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
//#define EXAMPLE_ESP_WIFI_MODE_AP   CONFIG_ESP_WIFI_MODE_AP //TRUE:AP FALSE:STA
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_MAX_STA_CONN       CONFIG_MAX_STA_CONN

/* FreeRTOS event group to signal when we are connected*/
EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
//const int WIFI_CONNECTED_BIT = BIT0;
//const int WIFI_NOT_CONNECTED_BIT = BIT1;
const int WIFI_SCAN_DONE = BIT2;

static const char *TAG = "MAIN";

TimerHandle_t xHttpdExpire;

static void HttpdExpireCb(TimerHandle_t xTimer)
{
	esp_wifi_disconnect();
	esp_wifi_deauth_sta(0);
	esp_wifi_stop();
	esp_wifi_deinit();
	vEventGroupDelete( wifi_event_group );
	button_reg_isr(TAG);
}

//todo: separate event handler for AP+sta and sole STA mode
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	httpd_handle_t *server = (httpd_handle_t *) ctx;

    /* For accessing reason codes in case of disconnection */
    system_event_info_t *info = &event->event_info;
    
    switch(event->event_id) {

    //station connected/disconected
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
 //       xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR" join, AID=%d",
                 MAC2STR(event->event_info.sta_connected.mac),
                 event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d",
                 MAC2STR(event->event_info.sta_disconnected.mac),
                 event->event_info.sta_disconnected.aid);
        break;

    //Connecting as station in AP+STA is not to occur, following 2 events for debug purpose only
    case SYSTEM_EVENT_STA_CONNECTED:
    	/* We succeeded to try connecting choosen AP */
//        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "Connection accepted");
    	break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    	/* We didn't succeed to try connecting choosen AP */
    	//send reason to http server
        ESP_LOGI(TAG, "Connection not accepted or disconnection");
//        xEventGroupSetBits(wifi_event_group, WIFI_NOT_CONNECTED_BIT);

        ESP_LOGE(TAG, "Disconnect reason : %d", info->disconnected.reason);
//        if (info->disconnected.reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
//            /*Switch to 802.11 bgn mode */
//            esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCAL_11B | WIFI_PROTOCAL_11G | WIFI_PROTOCAL_11N);
//        }
        break;

    case SYSTEM_EVENT_AP_START :
    	ESP_LOGI(TAG, "ap start");
        /* Start the web server */
        if (*server == NULL) {
            *server = start_webserver();
            ESP_LOGI(TAG, "Httpd started");
        }
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
        	if(xHttpdExpire)
        	{
        		ESP_LOGI(TAG, "Timer created");
        		if(pdFALSE==xTimerStart(xHttpdExpire,pdMS_TO_TICKS(1000)))
        			ESP_LOGI(TAG, "Http Timer hasn't started!");
        	}
        }
    	break;
    case SYSTEM_EVENT_AP_STOP :
        /* Stop the web server */
        if (*server) {
            stop_webserver(*server);
            *server = NULL;
            ESP_LOGI(TAG, "Httpd stopped");
        }
        xTimerDelete(xHttpdExpire,pdMS_TO_TICKS(CONFIG_AP_CONN_TIMEOUT));
    	break;

        // Unblocking scan ended (not caused by esp_wifi_connected())
    case SYSTEM_EVENT_SCAN_DONE:
    	ESP_LOGI(TAG, "SCAN DONE");
    	xEventGroupSetBits(wifi_event_group, WIFI_SCAN_DONE);	//let display scanned APs
    	break;

    default:
    	ESP_LOGI(TAG, "event: %d ", event->event_id);
        break;
    }
    return ESP_OK;
}

void wifi_init_softap(void *arg)
{
    wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
//		.sta = {
//				.ssid = ""
//		}
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());


    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

//void wifi_init_sta()
//{
//    wifi_event_group = xEventGroupCreate();
//
//    tcpip_adapter_init();
//    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );
//
//    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
//    wifi_config_t wifi_config = {
//        .sta = {
//            .ssid = EXAMPLE_ESP_WIFI_SSID,
//            .password = EXAMPLE_ESP_WIFI_PASS
//        },
//    };
//
//    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
//    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
//    ESP_ERROR_CHECK(esp_wifi_start() );
//
//    ESP_LOGI(TAG, "wifi_init_sta finished.");
//    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
//             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
//}



void app_main()
{
	button_init();

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

//#if EXAMPLE_ESP_WIFI_MODE_AP
    static httpd_handle_t server = NULL;
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP+STA");
    wifi_init_softap(&server);
//#else
//    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
//    wifi_init_sta();
//#endif /*EXAMPLE_ESP_WIFI_MODE_AP*/


}
