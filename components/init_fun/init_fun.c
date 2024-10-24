#include "init_fun.h"

// Funkcja callback do obsługi zdarzeń WiFi
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    switch (event_id) // Sprawdź rodzaj zdarzenia
    {
    case WIFI_EVENT_AP_STACONNECTED: // Gdy nowe urządzenie (klient) połączy się z ESP działającym jako AP
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;  // Rzutowanie danych
        ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid); // Wypisz adres MAC klienta oraz jego AID (identyfikator)
        break;
    }
    case WIFI_EVENT_AP_STADISCONNECTED: // Gdy urządzenie (klient) rozłączy się z ESP działającym jako AP
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data; // Rzutowanie danych
        ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);     // Wypisz adres MAC klienta oraz jego AID
        break;
    }
    default:
        break; // Jeżeli zdarzenie nie jest rozpoznane, nic nie rób
    }
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
void esp_AP_init(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());                // Inicjalizacja pamięci NVS (Non-Volatile Storage)
    esp_netif_init();                                 // Inicjalizacja adaptera TCP/IP
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // Tworzenie domyślnej pętli zdarzeń

    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();                                          // Konfiguracja Wi-Fi z domyślnymi ustawieniami
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));                                                         // Inicjalizacja Wi-Fi
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL)); // Rejestarcja funkcja callback do obsługi zdarzeń WiFi

    wifi_config_t esp_ap_config;                         // Struktura do przechowywania konfiguracji AP
    sprintf((char *)esp_ap_config.ap.ssid, AP_SSID);     // Ustawienie SSID AP
    sprintf((char *)esp_ap_config.ap.password, AP_PASS); // Ustawienie hasła AP
    esp_ap_config.ap.ssid_len = strlen(AP_SSID);         // Ustalamy długość SSID
    esp_ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;  // Ustawiamy tryb uwierzytelniania na WPA/WPA2, jeśli hasło jest ustawione
    esp_ap_config.ap.ssid_hidden = 0;                    // Ustalamy czy sieć ma być widoczna
    esp_ap_config.ap.max_connection = MAX_STA_CON_TO_AP; // Ustalamy maksymalną liczbę połączeń
    if ((8 > strlen(AP_PASS)) || (strlen(AP_PASS) > 63)) // Niepoprawna ilość znaków dla hasła (wymóg standardu WPA/WPA2-PSK od 8 do 63 znaków)
    {
        ESP_LOGI(TAG, "Wrong AP_PASS len: %d\r\n", strlen(AP_PASS)); // Info for debug
        esp_ap_config.ap.authmode = WIFI_AUTH_OPEN;                  // Ustawiamy tryb otwarty
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));                     // Ustawienie trybu Wi-Fi na AP (Access Point)
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &esp_ap_config)); // Ustawienie konfiguracji dla AP
    ESP_ERROR_CHECK(esp_wifi_start());                                    // Uruchomienie Wi-Fi

    ESP_LOGI(TAG, "esp_AP_init finished. SSID:%s password:%s\r\n", AP_SSID, AP_PASS); // Info for debug
}