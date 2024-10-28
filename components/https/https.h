#ifndef __HTTPS_H__
#define __HTTPS_H__

#include <main.h>

extern httpd_handle_t *web_server;

httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t server);

esp_err_t hello_get_handler(httpd_req_t *req);

void connect_handler(void *arg, esp_event_base_t event_base,
                     int32_t event_id, void *event_data);
void disconnect_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data);

#endif