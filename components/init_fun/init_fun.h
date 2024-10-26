#ifndef __INIT_FUN_H__
#define __INIT_FUN_H__

#include <main.h>

#define TAG "ESP8266"
#define HOSTNAME "ESP_Iot"
#define AP_SSID "ESP_IOT"
#define AP_PASS "MyESP8266"
#define MAX_STA_CON_TO_AP 5

void gpio_init(void);
void esp_AP_init(void);
esp_err_t change_tcpip_dhcp();

#endif