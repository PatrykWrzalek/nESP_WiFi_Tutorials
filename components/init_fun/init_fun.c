#include <init_fun.h>

// Funkcja callback do obsługi zdarzeń WiFi
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    switch (event_id) // Sprawdź rodzaj zdarzenia
    {
    case WIFI_EVENT_AP_START: // Gdy modułu wifi w trybie AP wystartował
    {
        ESP_LOGI(TAG, "ESP started in AP Mode\r\n");
        break;
    }
    case WIFI_EVENT_AP_STACONNECTED: // Gdy nowe urządzenie (klient) połączy się z ESP działającym jako AP
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;      // Rzutowanie danych
        ESP_LOGI(TAG, "station " MACSTR " join, AID=%d\r\n", MAC2STR(event->mac), event->aid); // Wypisz adres MAC klienta oraz jego AID (identyfikator)
        break;
    }
    case WIFI_EVENT_AP_STADISCONNECTED: // Gdy urządzenie (klient) rozłączy się z ESP działającym jako AP
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data; // Rzutowanie danych
        ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d\r\n", MAC2STR(event->mac), event->aid); // Wypisz adres MAC klienta oraz jego AID
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
    ESP_ERROR_CHECK(esp_netif_init());                // Inicjalizacja adaptera TCP/IP
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // Tworzenie domyślnej pętli zdarzeń

    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();                                          // Konfiguracja Wi-Fi z domyślnymi ustawieniami
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));                                                         // Inicjalizacja Wi-Fi
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL)); // Rejestarcja funkcja callback do obsługi zdarzeń WiFi
    // ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ANY_IP, &event_handler, NULL));     // Rejestarcja funkcja callback do obsługi zdarzeń IP

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

    custom_tcpip_dhcp();                                                              // Ustawienie własnej konfiguracji TCP/IP & DHCP
    ESP_LOGI(TAG, "esp_AP_init finished. SSID:%s password:%s\r\n", AP_SSID, AP_PASS); // Info for debug
}

/******************************************************************************
 * FunctionName : custom_tcpip_dhcp
 * Description : function that changes TCP/IP and DHCP settings. If applying the custom
 *               settings fails after multiple attempts, it restores the default values.
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void custom_tcpip_dhcp(void)
{
    ESP_LOGI(TAG, "Starting custom TCP/IP & DHCP configuration...\r\n");
    esp_err_t dhcps_stop_err = tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP); // Wyłączenie serwera DHCP przed jego konfiguracją
    if (dhcps_stop_err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to stop DHCP server: %d\r\n", dhcps_stop_err);
    }
    else
    {
        tcpip_adapter_ip_info_t ip_info_backup = {0};                    // Konfiguracja IP (default)
        dhcps_lease_t dhcp_lease_backup = {0};                           // Konfiguracja DHCP (default)
        tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ip_info_backup); // Odczytanie konfiguracji IP
        tcpip_adapter_dhcps_option(TCPIP_ADAPTER_OP_GET, TCPIP_ADAPTER_REQUESTED_IP_ADDRESS,
                                   &dhcp_lease_backup, sizeof(dhcps_lease_t)); // Odczytanie konfiguracji DHCP

        for (uint8_t attempt = 1; attempt <= 3; attempt++) // 3 próby ustawienia własnej konfiguracji TCP/IP & DHCP
        {
            esp_err_t change_tcpip_dhcp_attempt = change_tcpip_dhcp(); // Próba zmiany konfiguracji TCP/IP & DHCP
            if (change_tcpip_dhcp_attempt == ESP_OK)
                return;
            else if (attempt == 3)
            {
                tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ip_info_backup); // Ustawienie konfiguracji IP (default)
                tcpip_adapter_dhcps_option(TCPIP_ADAPTER_OP_SET, TCPIP_ADAPTER_REQUESTED_IP_ADDRESS,
                                           &dhcp_lease_backup, sizeof(dhcps_lease_t)); // Ustawienie konfiguracji DHCP (default)
                tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP);                        // Ponowne uruchomienie serwera DHCP
            }
        }
    }
}

/******************************************************************************
 * FunctionName : change_tcpip_dhcp
 * Description  : function that change TCP/IP & DHCP settings.
 * Parameters   : none
 * Returns      : - ESP_OK on success
 *                - one of the errors from esp_event on failure
 *******************************************************************************/
esp_err_t change_tcpip_dhcp()
{
    tcpip_adapter_ip_info_t ip_info = {0};        // Konfiguracja IP
    IP4_ADDR(&ip_info.ip, 192, 168, 1, 1);        // Nowy adres IP dla AP
    IP4_ADDR(&ip_info.gw, 192, 168, 1, 1);        // Brama
    IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0); // Maska podsieci

    esp_err_t ERR = tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ip_info); // Ustawienie konfiguracji IP
    if (ERR != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set IP configuration: %d\r\n", ERR);
        return ERR;
    }

    dhcps_lease_t dhcp_lease = {0};                   // Konfiguracja DHCP
    dhcp_lease.enable = true;                         // Zezwolenie na leasing IP
    IP4_ADDR(&dhcp_lease.start_ip, 192, 168, 1, 100); // Startowy adres IP dla DHCP
    IP4_ADDR(&dhcp_lease.end_ip, 192, 168, 1, 150);   // Końcowy adres IP dla DHCP

    ERR = tcpip_adapter_dhcps_option(TCPIP_ADAPTER_OP_SET, TCPIP_ADAPTER_REQUESTED_IP_ADDRESS,
                                     &dhcp_lease, sizeof(dhcps_lease_t)); // Ustawienie konfiguracji DHCP
    if (ERR != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure DHCP lease range: %d\r\n", ERR);
        return ERR;
    }

    ERR = tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP); // Ponowne uruchomienie serwera DHCP
    if (ERR != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start DHCP server: %d\r\n", ERR);
        return ERR;
    }

    ESP_LOGI(TAG, "Custom TCP/IP & DHCP configuration completed successfully.\r\n");
    uint8_t my_MAC[6]; // Tablica na adress MAC
    // uint8_t *my_MAC = (uint8_t *)malloc(6);      // Dynamiczna alokacja pamięci na adres MAC
    esp_err_t my_MAC_err = esp_base_mac_addr_get(my_MAC); // Odczytanie jaki adres MAC ma ESP
    if (my_MAC_err == ESP_OK)
    {
        ESP_LOGI(TAG, "MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                 my_MAC[0], my_MAC[1], my_MAC[2],
                 my_MAC[3], my_MAC[4], my_MAC[5]);
    }
    else
    {
        my_MAC_err = esp_efuse_mac_get_default(my_MAC); // Odczytanie jaki adres MAC ma ESP w EFUSE
        if (my_MAC_err == ESP_OK)
        {
            ESP_LOGI(TAG, "MAC Address in EFUSE: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                     my_MAC[0], my_MAC[1], my_MAC[2],
                     my_MAC[3], my_MAC[4], my_MAC[5]);
        }
    }
    // free(my_MAC);    // Zwolnienie (dynamicznie za alokowanej) pamięci na adress MAC
    return ESP_OK;
}