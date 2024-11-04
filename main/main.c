#include <main.h> // dla kontenera na Linux ("/") separato jest inny niż dla Windows'a ("\")
#include <init_fun.h>
#include <tekst.h> // Wygenerowany plik z zawartością tekst.txt

void system_init(void *arg);
void workStatus(void *arg);
void spiffs_test(void *arg);

void spiffs_info(void);
void spiffs_folder_in_workspace(void);

/******************************************************************************
 * FunctionName : app_main
 * Description  : entry of app application, init app function here.
 * Users can use tasks with priorities from 1 to 15
 * (priority of the freeRTOS timer is 2).
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void app_main(void)
{
    xTaskCreate(system_init, "Init_system", 2048, NULL, 9, NULL);

    xTaskCreate(spiffs_test, "SPIFFS", 2048, NULL, 3, NULL);
    xTaskCreate(workStatus, "Status", 1024, NULL, 1, NULL);
}

/******************************************************************************
 * FunctionName : system_init
 * Description  : main initializes task.
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void system_init(void *arg)
{
    gpio_init();
    spiffs_init();

    vTaskDelay(5000 / portTICK_RATE_MS);
    esp_AP_init();

    uint16_t siz_dat = uxTaskGetStackHighWaterMark(NULL); // Info for debug
    ESP_LOGI(TAG, "Task2 stack: %d\r\n", siz_dat);
    vTaskDelete(NULL);
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
 * FunctionName : spiffs_test
 * Description  : test task for SPIFFS.
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void spiffs_test(void *arg)
{
    vTaskDelay(10000 / portTICK_RATE_MS);

    spiffs_info();

    ESP_LOGI(TAG, "Opening file...\r\n");
    FILE *f = fopen("/spiffs/hello.txt", "w"); // Otwarcie pliku do zapisu "w" - writte (jeżeli takiego nie ma to utworzenie)
    if (f == NULL)                             // Błąd otwierania pliku
    {
        ESP_LOGE(TAG, "Failed to open file for writing\r\n"); // Błąd otwierania pliku do zapisu
        return;
    }
    fprintf(f, "Hello World!");        // Zapisanie tekstu do pliku
    fclose(f);                         // Zamknięcie pliku
    ESP_LOGI(TAG, "File written\r\n"); // Informacja o zakończeniu zapisu

    struct stat st;
    if (stat("/spiffs/foo.txt", &st) == 0) // Sprawdzenie, czy plik docelowy istnieje przed zmianą nazwy
    {
        // Usunięcie istniejącego pliku
        unlink("/spiffs/foo.txt");
    }

    ESP_LOGI(TAG, "Renaming file..\r\n");
    if (rename("/spiffs/hello.txt", "/spiffs/foo.txt") != 0) // Zmiana nazwy pierwotnego pliku
    {
        ESP_LOGE(TAG, "Rename failed\r\n"); // Błąd zmiany nazwy pliku
        return;
    }
    else
    {
        ESP_LOGI(TAG, "Successfully renamed file from: >> /spiffs/hello.txt << to: >> /spiffs/foo.txt <<\r\n");
    }

    ESP_LOGI(TAG, "Reading file...\r\n");
    f = fopen("/spiffs/foo.txt", "r"); // Otwarcie pliku do odczytu "r" - read
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading\r\n"); // Błąd otwierania pliku do odczytu
        return;
    }
    char line[64];
    fgets(line, sizeof(line), f); // Odczytanie linii z pliku
    ESP_LOGI(TAG, "Read from file: < %s >\r\n", line);
    fclose(f); // Zamknięcie pliku

    spiffs_info();

    ESP_LOGI(TAG, "Removing file..\r\n");
    if (remove("/spiffs/foo.txt") != 0) // Usunięcie pliku
    {
        ESP_LOGE(TAG, "Remove failed\r\n"); // Błąd usuwania pliku
        return;
    }
    else
    {
        ESP_LOGI(TAG, "Successfully removed file: >> /spiffs/foo.txt <<\r\n");
    }

    spiffs_folder_in_workspace();

    spiffs_info();
    spiffs_deinit();
    vTaskDelete(NULL);
}

/******************************************************************************
 * FunctionName : spiffs_info
 * Description  : SPIFFS info function.
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void spiffs_info(void)
{
    size_t total = 0, used = 0;
    esp_err_t ERR = esp_spiffs_info(NULL, &total, &used); // Pobieranie informacji o partycji SPIFFS
    if (ERR != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)\r\n", esp_err_to_name(ERR)); // Błąd podczas pobierania informacji o partycji
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d\r\n", total, used); // Wyświetlenie całkowitej i użytej wielkości partycji
    }
}

/******************************************************************************
 * FunctionName : spiffs_folder_in_workspace
 * Description  : write & read tekst.txt to SPIFFS.
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void spiffs_folder_in_workspace(void)
{
    ESP_LOGI(TAG, "Reading file from workspace folder...\r\n");

    ESP_LOGI(TAG, "Opening file...\r\n");
    FILE *f = fopen("/spiffs/tekst.txt", "w"); // Otwarcie pliku do zapisu "w" - writte (jeżeli takiego nie ma to utworzenie)
    if (f == NULL)                             // Błąd otwierania pliku
    {
        ESP_LOGE(TAG, "Failed to open file for writing\r\n"); // Błąd otwierania pliku do zapisu
        return;
    }
    fprintf(f, tekst_txt);             // Zapisanie ciągu znaków uzyskanego z pliku tekst.txt do pliku SPIFFS
    fclose(f);                         // Zamknięcie pliku
    ESP_LOGI(TAG, "File written\r\n"); // Informacja o zakończeniu zapisu

    ESP_LOGI(TAG, "Reading file...\r\n");
    f = fopen("/spiffs/tekst.txt", "r"); // Otwarcie pliku do odczytu "r" - read
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading\r\n"); // Błąd otwierania pliku do odczytu
        return;
    }
    char line[256];
    fgets(line, sizeof(line), f); // Odczytanie linii z pliku
    ESP_LOGI(TAG, "Read from file: < %s >\r\n", line);

    uint16_t siz_dat = uxTaskGetStackHighWaterMark(NULL); // Info for debug
    ESP_LOGI(TAG, "SPIFFS task stack: %d\r\n", siz_dat);
    fclose(f); // Zamknięcie pliku
}