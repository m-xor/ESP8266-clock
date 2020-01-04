/*
 * app_http.h
 *
 *  Created on: 4 sty 2020
 *      Author: slawek
 */

#ifndef MAIN_APP_HTTP_H_
#define MAIN_APP_HTTP_H_

#include <esp_http_server.h>

httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t server);

#endif /* MAIN_APP_HTTP_H_ */
