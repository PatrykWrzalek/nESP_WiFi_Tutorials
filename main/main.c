#include <FreeRTOS.h> // dla kontenera na Linux ("/") separato jest inny niż dla Windows'a ("\")
#include <task.h>

#include <stdio.h>
#include <string.h>
#include <gpio.h>
#include <esp_log.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

#define TAG "ESP8266"
#define HOSTNAME "ESP_Iot"
#define AP_SSID "ESP_IOT"
#define AP_PASS "ESP8266"
#define MAX_STA_CON_TO_AP 4

void gpio_init(void);

void esp_AP_init(void *arg);
void workStatus(void *arg);

void app_main(void)
{
    gpio_init();

    xTaskCreate(esp_AP_init, "ap_init", 2048, NULL, 5, NULL);
    xTaskCreate(workStatus, "Status", 1024, NULL, 1, NULL);
}

/******************************************************************************
 * FunctionName : workStatus
 * Description  : on board LED blinking every 1s for 1s.
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void workStatus(void *arg)
{
    while (true)
    {
        gpio_set_level(2, 0);
        vTaskDelay(1000 / portTICK_RATE_MS);
        gpio_set_level(2, 1);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

/******************************************************************************
 * FunctionName : gpio_init
 * Description  : initializes GPIO.
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void gpio_init(void)
{
    gpio_config_t io_conf;
    io_conf.pin_bit_mask = ((1 << 2));
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&io_conf);
}
/******************************************************************************
 * FunctionName : esp_AP_init
 * Description  : initializes the ESP8266 WiFi in Access Point (AP) mode.
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void esp_AP_init(void *arg)
{
    vTaskDelay(1000 / portTICK_RATE_MS);

    ESP_ERROR_CHECK(nvs_flash_init());                // Inicjalizacja pamięci NVS (Non-Volatile Storage)
    esp_netif_init();                                 // Inicjalizacja adaptera TCP/IP
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // Tworzenie domyślnej pętli zdarzeń

    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT(); // Konfiguracja Wi-Fi z domyślnymi ustawieniami
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));                // Inicjalizacja Wi-Fi

    wifi_config_t esp_ap_config;                         // Struktura do przechowywania konfiguracji AP
    sprintf((char *)esp_ap_config.ap.ssid, AP_SSID);     // Ustawienie SSID AP
    sprintf((char *)esp_ap_config.ap.password, AP_PASS); // Ustawienie hasła AP
    esp_ap_config.ap.ssid_len = strlen(AP_SSID);         // Ustalamy długość SSID
    esp_ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;  // Ustawiamy tryb uwierzytelniania na WPA/WPA2, jeśli hasło jest ustawione
    esp_ap_config.ap.ssid_hidden = 0;                    // Ustalamy czy sieć ma być widoczna
    esp_ap_config.ap.max_connection = MAX_STA_CON_TO_AP; // Ustalamy maksymalną liczbę połączeń
    if (strlen(AP_PASS) == 0)                            // Jeśli brak hasła
    {
        esp_ap_config.ap.authmode = WIFI_AUTH_OPEN; // Ustawiamy tryb otwarty
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));                     // Ustawienie trybu Wi-Fi na AP (Access Point)
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &esp_ap_config)); // Ustawienie konfiguracji dla AP
    ESP_ERROR_CHECK(esp_wifi_start());                                    // Uruchomienie Wi-Fi

    ESP_LOGI(TAG, "esp_AP_init finished. SSID:%s password:%s", AP_SSID, AP_PASS); // Info
    vTaskDelete(NULL);
}