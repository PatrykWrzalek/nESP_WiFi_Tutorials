#include <https.h>

static const char *TAG = "HTTPS";

httpd_uri_t hello = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = hello_get_handler,
    .user_ctx = "Hello World!"};

/******************************************************************************
 * FunctionName : connect_handler
 * Description  : callback function for the client IP assignment event.
 * Parameters   : arg - pointer to context data (handle to the web server)
 * Returns      : void
 *******************************************************************************/
void connect_handler(void *arg, esp_event_base_t event_base,
                     int32_t event_id, void *event_data)
{
    httpd_handle_t *web_server = (httpd_handle_t *)arg; // Przepisanie danych dla uchwytu
    if (*web_server == NULL)
    {
        ESP_LOGI(TAG, "Starting webserver\r\n");
        *web_server = start_webserver(); // Próba uruchomienia serwera HTTP
    }
}

/******************************************************************************
 * FunctionName : start_webserver
 * Description  : starts the web server and registers URI handlers for request processing
 * Parameters   : none
 * Returns      : httpd_handle_t - handle to the started server or NULL on error
 *******************************************************************************/
httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // Odczytanie i zapisanie standardowej konfiguracji dla webserwera
    httpd_handle_t web_server = NULL;               // Wyczyść uchwyt do serwera HTTP

    ESP_LOGI(TAG, "Starting web_server on port: '%d'\r\n", config.server_port);
    if (httpd_start(&web_server, &config) == ESP_OK) // Próba uruchomienia serwera HTTP
    {
        ESP_LOGI(TAG, "Registering URI handlers\r\n");
        esp_err_t http_reg_err = httpd_register_uri_handler(web_server, &hello); // Rejestracja obsługi URI dla metody GET (hello)
        if (http_reg_err != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed with register URI: %d\r\n", http_reg_err);
        }
    }
    else
    {
        ESP_LOGI(TAG, "Error starting web_server!\r\n");
    }

    return web_server; // Jeśli serwer się nie uruchomił, uchwyt pozostanie NULL
}

/******************************************************************************
 * FunctionName : disconnect_handler
 * Description  : callback function for the client disconnection event from AP
 * Parameters   : arg - pointer to context data (handle to the web server)
 * Returns      : void
 *******************************************************************************/
void disconnect_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    httpd_handle_t *web_server = (httpd_handle_t *)arg; // Przepisanie danych dla uchwytu
    if (*web_server)
    {
        ESP_LOGI(TAG, "Stopping webserver\r\n");
        stop_webserver(*web_server); // Wywołanie funkcji zatrzymującej serwer HTTP
        *web_server = NULL;          // Wyzerowanie danych webserwera
    }
}
/******************************************************************************
 * FunctionName : stop_webserver
 * Description  : function for stopping the webserver.
 * Parameters   : server - handler to the web server
 * Returns      : void
 *******************************************************************************/
void stop_webserver(httpd_handle_t server)
{
    if (server != NULL) // Sprawdzenie czy serwer jest uruchomiony
    {
        httpd_stop(server); // Zatrzymanie webserwera
    }
}

/******************************************************************************
 * FunctionName : hello_get_handler
 * Description  : handles GET request for URI and respond with a message from user_ctx.
 * Parameters   : req - pointer to the HTTP request structure
 * Returns      : esp_err_t - ESP_OK on success
 *******************************************************************************/
esp_err_t hello_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "New conection to web server!\r\n");

    if (req->user_ctx == NULL) // Sprawdzenie, czy user_ctx nie jest NULL
    {
        httpd_resp_send_500(req); // Wysłanie błędu 500 w razie braku user_ctx
        return ESP_FAIL;
    }

    const char *resp_str = (const char *)req->user_ctx; // Pobranie odpowiedzi z user_ctx
    httpd_resp_send(req, resp_str, strlen(resp_str));   // Wysłanie odpowiedzi do klienta

    return ESP_OK;
}